#include <cstdint>
#include <fstream>
#include <istream>
#include <numeric>
#include <print>
#include <ranges>
#include <vector>

using Int = std::uint64_t;

auto parse_input(std::istream&& in) {
  return std::views::istream<Int>(in) | std::ranges::to<std::vector>();
}

auto mix(const Int a, const Int b) { return a ^ b; }

auto prune(const Int a) { return a % Int{16777216}; }

auto next_secret_number(const Int a) {
  const Int step1 = prune(mix(a, a * Int{64}));
  const Int step2 = prune(mix(step1, step1 / Int{32}));
  const Int step3 = prune(mix(step2, step2 * Int{2048}));
  return step3;
}

auto solve_part1(const auto& input) {
  auto secrets = input;
  for (auto _ : std::views::iota(0, 2000)) {
    for (auto& s : secrets) {
      s = next_secret_number(s);
    }
  }
  return std::reduce(std::ranges::begin(secrets), std::ranges::end(secrets), Int{});
}

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
