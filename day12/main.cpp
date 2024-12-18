#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <map>
#include <optional>
#include <print>
#include <ranges>
#include <set>
#include <span>
#include <utility>
#include <vector>

using Idx = std::int32_t;
using Crop = char;

struct Loc {
  Idx row;
  Idx col;

  auto operator<=>(const Loc& other) const = default;
};

auto operator+(const Loc& left, const Loc& right) {
  return Loc{.row = left.row + right.row, .col = left.col + right.col};
}

auto operator-(const Loc& left, const Loc& right) {
  return Loc{.row = left.row - right.row, .col = left.col - right.col};
}

class Field {
 public:
  auto row_count() const { return row_count_; }
  auto col_count() const { return col_count_; }

  auto operator[](Loc loc) const -> std::optional<Crop> {
    const auto& [row, col] = loc;
    if (row < 0 || row >= row_count_ || col < 0 || col >= col_count_) {
      return std::nullopt;
    }
    const decltype(tiles_)::size_type entry = (row * row_count_) + col;
    return {tiles_[entry]};
  }

  Field(std::vector<Crop> tiles, Idx row_count, Idx col_count)
      : tiles_{std::move(tiles)}, row_count_{row_count}, col_count_{col_count} {
    assert(std::cmp_equal(row_count_ * col_count_, tiles_.size()));
  }

 private:
  std::vector<Crop> tiles_;
  Idx row_count_;
  Idx col_count_;
};

auto parse_input(std::istream&& in) {
  const std::vector<char> data{std::istreambuf_iterator{in}, {}};

  const auto row_count = Idx(std::ranges::count(data, '\n'));
  const auto col_count = Idx(std::ranges::distance(
      std::ranges::views::take_while(data, [](const char c) { return c != '\n'; })));
  auto crops = std::ranges::views::filter(data, [](const char c) { return c != '\n'; }) |
               std::ranges::to<std::vector>();

  return Field{std::move(crops), row_count, col_count};
}

constexpr std::array<Loc, 4> moves{
    {{.row = -1, .col = 0}, {.row = 1, .col = 0}, {.row = 0, .col = -1}, {.row = 0, .col = 1}}};

struct Region {
  std::set<Loc> tiles;
  std::map<Loc, std::vector<Loc>> inner_perimeter;
  std::map<Loc, std::vector<Loc>> outer_perimeter;

  auto price() const;
  auto discount_price() const;
};

auto Region::price() const {
  return std::ranges::fold_left(std::ranges::views::values(inner_perimeter), 0UZ,
                                [](const auto acc, const auto& x) { return acc + x.size(); }) *
         tiles.size();
}

auto Region::discount_price() const {
  const auto are_opposite = [](const Loc& a, const Loc& b) {
    const Loc delta = a - b;
    return std::max(std::abs(delta.row), std::abs(delta.col)) == Idx{2};
  };
  const auto corner_count = [&](std::span<const Loc> boundaries) {
    if (boundaries.size() == 4UZ) {
      return 4UZ;
    }
    if (boundaries.size() == 2UZ && are_opposite(boundaries[0], boundaries[1])) {
      return 0UZ;
    }
    return boundaries.size() - 1UZ;
  };

  const auto false_corner_count = [&](const std::pair<const Loc, std::vector<Loc>>& entry,
                                      const std::map<Loc, std::vector<Loc>>& dual) {
    return std::ranges::count_if(
        std::ranges::views::cartesian_product(std::ranges::views::all(entry.second),
                                              std::ranges::views::all(entry.second)),
        [&](const auto& p) {
          if (std::get<0>(p) >= std::get<1>(p) || are_opposite(std::get<0>(p), std::get<1>(p))) {
            return false;
          }
          const Loc diagonal = std::get<0>(p) + (std::get<1>(p) - entry.first);
          return std::ranges::contains(dual.find(std::get<0>(p))->second, diagonal) &&
                 std::ranges::contains(dual.find(std::get<1>(p))->second, diagonal);
        });
  };

  return (std::ranges::fold_left(
              std::ranges::views::values(inner_perimeter), 0UZ,
              [&](const auto acc, const auto& x) { return acc + corner_count(x); }) +
          std::ranges::fold_left(
              std::ranges::views::values(outer_perimeter), 0UZ,
              [&](const auto acc, const auto& x) { return acc + corner_count(x); }) -
          std::ranges::fold_left(std::ranges::views::all(outer_perimeter), 0UZ,
                                 [&](const auto acc, const auto& x) {
                                   return acc + false_corner_count(x, inner_perimeter);
                                 })) *
         tiles.size();
}

auto compute_region(const Field& field, Loc start) {
  Region result{.tiles = {start}, .inner_perimeter = {}, .outer_perimeter = {}};

  std::vector<Loc> front{start};
  const auto region_crop = *field[start];

  while (!front.empty()) {
    const auto loc = front.back();
    front.pop_back();
    for (const Loc& m : moves) {
      const Loc neighbor = loc + m;
      if (!result.tiles.contains(neighbor)) {
        if (field[neighbor] == region_crop) {
          result.tiles.insert(neighbor);
          front.push_back(neighbor);
        } else {
          result.inner_perimeter[loc].push_back(neighbor);
          result.outer_perimeter[neighbor].push_back(loc);
        }
      }
    }
  }
  return result;
}

template <auto cost_function>
auto solve(const Field& input) {
  std::uint64_t price{};
  std::set<Loc> done;

  auto all_tiles =
      std::ranges::views::cartesian_product(std::ranges::views::iota(Idx{}, input.row_count()),
                                            std::ranges::views::iota(Idx{}, input.col_count())) |
      std::ranges::views::transform(
          [&](const auto& p) { return Loc{.row = std::get<0>(p), .col = std::get<1>(p)}; });

  auto it = all_tiles.begin();
  while (it != all_tiles.end()) {
    auto region = compute_region(input, *it++);
    price += (region.*cost_function)();

    done.merge(region.tiles);
    while (it != all_tiles.end() && done.contains(*it)) {
      ++it;
    }
  }

  return price;
}

auto solve_part1(const auto& input) { return solve<&Region::price>(input); }
auto solve_part2(const auto& input) { return solve<&Region::discount_price>(input); }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
