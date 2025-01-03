#include <cstdint>
#include <fstream>
#include <istream>
#include <print>
#include <ranges>
#include <vector>

using Int = std::uint64_t;

auto parse_input(std::istream&& in) {
  return std::views::istream<Int>(in) | std::ranges::to<std::vector>();
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
