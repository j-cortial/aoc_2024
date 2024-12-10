#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <optional>
#include <print>
#include <ranges>
#include <utility>
#include <vector>

using Idx = std::int32_t;
using Height = std::uint8_t;

class Terrain {
 public:
  auto row_count() const { return row_count_; }
  auto col_count() const { return col_count_; }

  auto operator[](Idx row, Idx col) const -> std::optional<Height> {
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
                 std::ranges::views::transform([](const char c) { return Height(c - '\0'); }) |
                 std::ranges::to<std::vector>();

  return Terrain{std::move(heights), row_count, col_count};
}

auto solve_part1(const auto& input) { return 0; }

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
