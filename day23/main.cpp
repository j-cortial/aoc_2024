#include <fstream>
#include <ios>
#include <print>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

using Computer = std::string;
using Link = std::pair<Computer, Computer>;

auto parse_input(std::istream&& in) {
  return std::views::istream<char>(in >> std::noskipws) | std::views::lazy_split('\n') |
         std::views::filter(
             [](const auto& line) { return std::ranges::begin(line) != std::ranges::end(line); }) |
         std::views::transform([](const auto& line) {
           auto computers =
               std::views::lazy_split(line, '-') | std::views::transform([](const auto& token) {
                 return token | std::ranges::to<Computer>();
               });
           auto computer_it = std::ranges::begin(computers);
           Computer first = *computer_it;
           ++computer_it;
           Computer second = *computer_it;
           ++computer_it;
           return Link{std::move(first), std::move(second)};
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
