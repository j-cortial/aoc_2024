#include <fstream>
#include <iostream>

auto parse_input(std::istream&& in) {
  return 0;
}

auto solve_part1(const auto& input) {
  return 0;
}

auto solve_part2(const auto& input) {
  return 0;
}

auto main() -> int {
  const auto input = parse_input(std::ifstream{"day21.txt"});
  const auto answer1 = solve_part1(input);
  std::cout << "The answer to part #1 is " << answer1 << std::endl;
  const auto answer2 = solve_part2(input);
  std::cout << "The answer to part #2 is " << answer2 << std::endl;
}
