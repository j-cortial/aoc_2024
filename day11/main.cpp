#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <forward_list>
#include <fstream>
#include <functional>
#include <iterator>
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

auto blink(std::forward_list<Int>& stones) {
  for (auto it = stones.begin(); it != stones.end(); ++it) {
    if (*it == Int{0}) {
      *it = Int{1};
    } else if (const auto succ = split(*it); succ.has_value()) {
      *it = succ->first;
      stones.insert_after(it, succ->second);
      ++it;
    } else {
      *it *= Int{2024};
    }
  }
}

auto solve_part1(const auto& input) {
  std::forward_list<Int> stones{input.cbegin(), input.cend()};
  for (std::size_t i{}; i != 25UZ; ++i) {
    blink(stones);
  }
  return std::ranges::distance(stones);
}

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
