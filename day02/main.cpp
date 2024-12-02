#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <numeric>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

using Int = std::int64_t;

auto parse_input(std::istream&& in) {
  std::vector<std::vector<Int>> result;
  std::string line;
  while (std::getline(in, line)) {
    result.emplace_back();
    std::istringstream stream{line};
    std::copy(std::istream_iterator<Int>{stream}, {}, std::back_inserter(result.back()));
  }
  return result;
}

auto is_safe(const auto& report) {
  std::vector<Int> differences;
  differences.reserve(report.size());
  std::adjacent_difference(report.cbegin(), report.cend(), std::back_inserter(differences));
  const std::span<const Int> deltas{std::next(differences.cbegin()), differences.cend()};
  return std::ranges::all_of(deltas, [&](const auto& value) {
    return std::abs(value) <= 3 && std::abs(value) >= 1 &&
           std::signbit(value) == std::signbit(deltas[0]);
  });
}

auto solve_part1(const auto& input) {
  return std::ranges::count_if(input, [](const auto& report) { return is_safe(report); });
}

auto solve_part2(const auto& input) {
  return std::ranges::count_if(input, [](const auto& report) {
    return is_safe(report) ||
           std::ranges::any_of(std::ranges::views::iota(0UZ, report.size()), [&](const auto idx) {
             std::vector<Int> truncated_report;
             truncated_report.reserve(report.size() - 1UZ);
             std::copy(report.cbegin(), std::next(report.cbegin(), idx),
                       std::back_inserter(truncated_report));
             std::copy(std::next(report.cbegin(), idx + 1UZ), report.cend(),
                       std::back_inserter(truncated_report));
             return is_safe(truncated_report);
           });
  });
}

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #1 is {}", answer2);
}
