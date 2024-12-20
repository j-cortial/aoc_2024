#include <cstdint>
#include <fstream>
#include <ios>
#include <print>
#include <ranges>
#include <string>
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
             const auto row = *coords_it++;
             const auto col = *coords_it++;
             return Loc{row, col};
           };
           const auto pos = parse(*section_it++);
           const auto vel = parse(*section_it++);
           return Robot{pos, vel};
         }) |
         std::ranges::to<std::vector>();
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
