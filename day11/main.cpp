#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <map>
#include <optional>
#include <print>
#include <ranges>
#include <utility>
#include <vector>

using Int = std::uint64_t;

auto parse_input(std::istream&& in) {
  return std::ranges::views::istream<Int>(in) | std::ranges::to<std::vector>();
}

auto split(Int i) -> std::optional<std::pair<Int, Int>> {
  Int bound{9};
  std::size_t digits{1};
  while (i > bound) {
    bound = ((bound + Int{1}) * Int{10}) - Int{1};
    digits += 1UZ;
  }
  if (digits % 2 != 0) {
    return std::nullopt;
  }
  const auto divisor = std::ranges::fold_left(std::ranges::views::repeat(Int{10}, digits / 2),
                                              Int{1}, std::multiplies<>{});
  return {{i / divisor, i % divisor}};
}

auto blink(const std::map<Int, std::size_t>& stones) {
  std::map<Int, std::size_t> result;
  for (const auto& [i, count] : stones) {
    if (i == Int{0}) {
      result[Int{1}] += count;
    } else if (const auto succ = split(i); succ.has_value()) {
      result[succ->first] += count;
      result[succ->second] += count;
    } else {
      result[i * Int{2024}] += count;
    }
  }
  return result;
}

auto solve(const auto& input, std::size_t blinks) {
  std::map<Int, std::size_t> stones;
  for (const auto i : input) {
    stones[i] += 1UZ;
  }
  for (std::size_t i{}; i != blinks; ++i) {
    stones = blink(stones);
  }
  return std::ranges::fold_left(std::ranges::views::values(stones), std::size_t{}, std::plus<>{});
}

auto solve_part1(const auto& input) { return solve(input, 25UZ); }
auto solve_part2(const auto& input) { return solve(input, 75UZ); }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
