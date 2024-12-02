#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

using Int = std::int64_t;

auto parse_input(std::istream&& in) {
  std::array<std::vector<Int>, 2> result;
  std::string line;
  while (std::getline(in, line)) {
    std::istringstream stream{line};
    Int buffer;  // NOLINT
    stream >> buffer;
    result[0].push_back(buffer);
    stream >> buffer;
    result[1].push_back(buffer);
  }
  return result;
}

auto solve_part1(const auto& input) {
  auto data = input;
  for (auto& list : data) {
    std::ranges::sort(list);
  }
  return std::ranges::fold_left(std::ranges::views::zip(data[0], data[1]), Int{},
                                [](const auto acc, const auto& elem) {
                                  return acc + std::abs(std::get<0>(elem) - std::get<1>(elem));
                                });
}

auto solve_part2(const auto& input) {
  const auto& left = input[0];
  auto right = input[1];
  std::ranges::sort(right);
  return std::ranges::fold_left(left, Int{}, [&right](const auto acc, const auto& elem) {
    return acc + (elem * std::distance(std::ranges::lower_bound(right, elem),
                                       std::ranges::upper_bound(right, elem)));
  });
}

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #1 is {}", answer2);
}
