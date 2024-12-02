#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iterator>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <utility>
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

auto is_safe(auto&& report) {
  auto deltas = std::ranges::views::pairwise_transform(report, std::minus<>{});
  const auto head = *deltas.cbegin();
  return std::ranges::all_of(deltas, [&](const auto& value) {
    return std::abs(value) <= 3 && std::abs(value) >= 1 &&
           std::signbit(value) == std::signbit(head);
  });
}

auto solve_part1(const auto& input) {
  return std::ranges::count_if(input, [](const auto& report) { return is_safe(report); });
}

auto solve_part2(const auto& input) {
  return std::ranges::count_if(input, [](const auto& report) {
    return is_safe(report) ||
           std::ranges::any_of(std::ranges::views::iota(0UZ, report.size()), [&](const auto idx) {
             return is_safe(
                 std::ranges::views::enumerate(report) |
                 std::ranges::views::filter(
                     [&](const auto& p) { return std::cmp_not_equal(std::get<0>(p), idx); }) |
                 std::ranges::views::transform([](const auto& p) { return std::get<1>(p); }));
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
