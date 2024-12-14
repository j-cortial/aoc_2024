#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iterator>
#include <optional>
#include <print>
#include <ranges>
#include <set>
#include <utility>
#include <vector>

using Idx = std::int32_t;

struct Loc {
  Idx row;
  Idx col;

  auto operator<=>(const Loc& other) const = default;
};

using Height = std::uint8_t;

class Terrain {
 public:
  auto row_count() const { return row_count_; }
  auto col_count() const { return col_count_; }

  auto operator[](Loc loc) const -> std::optional<Height> {
    const auto& [row, col] = loc;
    if (row < 0 || row >= row_count_ || col < 0 || col >= col_count_) {
      return std::nullopt;
    }
    const decltype(tiles_)::size_type entry = (row * row_count_) + col;
    return {tiles_[entry]};
  }

  Terrain(std::vector<Height> tiles, Idx row_count, Idx col_count)
      : tiles_{std::move(tiles)}, row_count_{row_count}, col_count_{col_count} {
    assert(std::cmp_equal(row_count_ * col_count_, tiles_.size()));
  }

 private:
  std::vector<Height> tiles_;
  Idx row_count_;
  Idx col_count_;
};

auto parse_input(std::istream&& in) {
  const std::vector<char> data{std::istreambuf_iterator{in}, {}};

  const auto row_count = Idx(std::ranges::count(data, '\n'));
  const auto col_count = Idx(std::ranges::distance(
      std::ranges::views::take_while(data, [](const char c) { return c != '\n'; })));
  auto heights = std::ranges::views::filter(data, [](const char c) { return c != '\n'; }) |
                 std::ranges::views::transform([](const char c) { return Height(c - '0'); }) |
                 std::ranges::to<std::vector>();

  return Terrain{std::move(heights), row_count, col_count};
}

constexpr std::array<Loc, 4> moves{
    {{.row = -1, .col = 0}, {.row = 1, .col = 0}, {.row = 0, .col = -1}, {.row = 0, .col = 1}}};

auto trailhead_score_impl(const Terrain& terrain, Loc current_loc, Height current_height,
                          std::set<Loc>& trailends) {
  if (terrain[current_loc] != current_height) {
    return;
  }

  if (current_height == Height{9}) {
    trailends.insert(current_loc);
    return;
  }

  std::ranges::for_each(moves, [&](const auto& m) {
    const Loc candidate{.row = current_loc.row + m.row, .col = current_loc.col + m.col};
    trailhead_score_impl(terrain, candidate, current_height + Height{1}, trailends);
  });
}

auto trailhead_score(const Terrain& terrain, Loc start) {
  std::set<Loc> trailends;
  trailhead_score_impl(terrain, start, {}, trailends);
  return trailends.size();
}

auto solve_part1(const auto& input) {
  return std::ranges::fold_left(
      std::ranges::views::cartesian_product(std::ranges::views::iota(Idx{}, input.row_count()),
                                            std::ranges::views::iota(Idx{}, input.col_count())) |
          std::ranges::views::transform([&](const auto& p) {
            return trailhead_score(input, Loc{.row = std::get<0>(p), .col = std::get<1>(p)});
          }),
      std::size_t{}, std::plus<>{});
}

auto trail_count_impl(const Terrain& terrain, Loc current_loc, Height current_height) {
  if (terrain[current_loc] != current_height) {
    return 0UZ;
  }

  if (current_height == Height{9}) {
    return 1UZ;
  }

  return std::ranges::fold_left(
      std::ranges::views::transform(
          moves,
          [&](const auto& m) {
            const Loc candidate{.row = current_loc.row + m.row, .col = current_loc.col + m.col};
            return trail_count_impl(terrain, candidate, current_height + Height{1});
          }),
      0UZ, std::plus<>{});
}

auto trail_count(const Terrain& terrain, Loc start) { return trail_count_impl(terrain, start, {}); }

auto solve_part2(const auto& input) {
  return std::ranges::fold_left(
      std::ranges::views::cartesian_product(std::ranges::views::iota(Idx{}, input.row_count()),
                                            std::ranges::views::iota(Idx{}, input.col_count())) |
          std::ranges::views::transform([&](const auto& p) {
            return trail_count(input, Loc{.row = std::get<0>(p), .col = std::get<1>(p)});
          }),
      std::size_t{}, std::plus<>{});
}

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
