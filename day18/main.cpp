#include <cstdint>
#include <fstream>
#include <ios>
#include <print>
#include <ranges>
#include <string>
#include <vector>

using Idx = std::int_fast16_t;

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

auto parse_input(std::istream&& in) {
  return std::views::istream<char>(in >> std::noskipws) | std::views::lazy_split('\n') |
         std::views::filter(
             [](const auto& line) { return std::ranges::begin(line) != std::ranges::end(line); }) |
         std::views::transform([](const auto& line) {
           auto coords =
               std::views::lazy_split(line, ',') | std::views::transform([](const auto& token) {
                 auto str = token | std::ranges::to<std::string>();
                 return std::stol(str);
               });
           auto coords_it = std::ranges::begin(coords);
           const Idx row = *coords_it;
           ++coords_it;
           const Idx col = *coords_it;
           return Loc{.row = row, .col = col};
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
