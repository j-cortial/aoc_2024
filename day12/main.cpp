#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <optional>
#include <print>
#include <ranges>
#include <set>
#include <utility>
#include <vector>

using Idx = std::int32_t;
using Crop = char;

struct Loc {
  Idx row;
  Idx col;

  auto operator<=>(const Loc& other) const = default;
};

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
  std::uint64_t perimeter_length;

  auto price() const { return tiles.size() * perimeter_length; }
};

auto compute_region(const Field& field, Loc start) {
  std::set<Loc> tiles{start};
  std::uint64_t perimeter_length{};

  std::vector<Loc> front{start};
  const auto region_crop = *field[start];

  while (!front.empty()) {
    const auto loc = front.back();
    front.pop_back();
    for (const Loc& m : moves) {
      const Loc neighbor{.row = loc.row + m.row, .col = loc.col + m.col};
      if (!tiles.contains(neighbor)) {
        const auto crop = field[neighbor];
        if (crop == region_crop) {
          tiles.insert(neighbor);
          front.push_back(neighbor);
        } else {
          perimeter_length += std::uint64_t{1};
        }
      }
    }
  }
  return Region{.tiles = tiles, .perimeter_length = perimeter_length};
}

auto solve_part1(const Field& input) {
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
    price += region.price();

    done.merge(region.tiles);
    while (it != all_tiles.end() && done.contains(*it)) {
      ++it;
    }
  }

  return price;
}

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
