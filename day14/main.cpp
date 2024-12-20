#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <functional>
#include <ios>
#include <optional>
#include <print>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

using Idx = std::int32_t;

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

struct Robot {
  Loc pos;
  Loc vel;
};

auto parse_input(std::istream&& in) {
  const auto content =
      std::ranges::istream_view<char>{in >> std::noskipws} | std::ranges::to<std::string>();
  return std::views::split(content, '\n') |
         std::views::filter([](const auto& line) { return !empty(line); }) |
         std::views::transform([](const auto& line) {
           auto sections = line | std::views::lazy_split(' ');
           auto section_it = std::ranges::begin(sections);
           const auto parse = [](const auto& section) {
             auto coords = section | std::views::drop(2) | std::views::lazy_split(',') |
                           std::views::transform([](const auto& token) {
                             return std::stoi(token | std::ranges::to<std::string>());
                           });
             auto coords_it = std::ranges::begin(coords);
             const auto col = *coords_it++;
             const auto row = *coords_it++;
             return Loc{row, col};
           };
           const auto pos = parse(*section_it++);
           const auto vel = parse(*section_it++);
           return Robot{pos, vel};
         }) |
         std::ranges::to<std::vector>();
}

class Area {
 public:
  explicit Area(Loc lower_right) : lower_right_(lower_right) {}

  auto tile(Loc target) const;

  auto quadrant(Loc tile) const -> std::optional<std::uint_fast8_t>;

 private:
  Loc lower_right_;
};

auto Area::tile(Loc target) const {
  const auto row = (target.row + lower_right_.row) % lower_right_.row;
  const auto col = (target.col + lower_right_.col) % lower_right_.col;
  return Loc{.row = row, .col = col};
}

auto Area::quadrant(Loc tile) const -> std::optional<std::uint_fast8_t> {
  if (tile.row == (lower_right_.row / Idx{2}) || tile.col == (lower_right_.col / Idx{2})) {
    return std::nullopt;
  }

  return {std::uint_fast8_t(tile.row > (lower_right_.row / Idx{2})) +
          (std::uint_fast8_t(tile.col > (lower_right_.col / Idx{2})) * std::uint_fast8_t(2))};
}

const auto transform_filter = []<typename R, typename P>(R&& r, P&& p) {
  return std::views::transform(std::forward<R>(r), std::forward<P>(p)) |
         std::views::filter([](const auto& maybe) { return bool(maybe); }) |
         std::views::transform([](const auto& maybe) { return *maybe; });
};

auto solve_part1(const auto& input) {
  const Area area{{.row = 103, .col = 101}};
  auto robots = input;
  for (auto i = 0; i != 100; ++i) {
    for (auto& robot : robots) {
      robot.pos = area.tile(robot.pos + robot.vel);
    }
  }

  std::array<std::size_t, 4> counts{};
  for (const auto quadrant :
       transform_filter(robots, [&area](const Robot& robot) { return area.quadrant(robot.pos); })) {
    ++counts[quadrant];
  }

  return std::ranges::fold_left(counts, 1UZ, std::multiplies<>{});
}

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
