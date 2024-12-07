#include <algorithm>
#include <cassert>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iterator>
#include <print>
#include <ranges>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

using namespace std::string_view_literals;

template <typename Numeric, typename Range>
auto parse(Range&& rng) {
  const auto token = std::forward<Range>(rng) | std::ranges::to<std::vector>();
  Numeric result;  // NOLINT
  const std::from_chars_result status =
      std::from_chars(token.data(), std::next(token.data(), std::ptrdiff_t(token.size())), result);
  assert(status.ec == std::errc{});
  return result;
}

using Int = std::uint64_t;

struct Equation {
  Int result;
  std::vector<Int> terms;
};

auto parse_input(std::istream&& in) {
  std::vector<char> data{std::istreambuf_iterator<char>(in), {}};

  return std::ranges::views::split(data, '\n') |
         std::ranges::views::filter([](const auto& line) { return !line.empty(); }) |
         std::ranges::views::transform([](const auto& line) {
           auto sections = std::ranges::views::split(line, ": "sv);
           auto it = sections.begin();
           const auto result = parse<Int>(*it++);
           auto terms = *it++ | std::ranges::views::split(' ') |
                        std::ranges::views::transform([](const auto& token) {
                          auto r = parse<Int>(token);
                          return r;
                        }) |
                        std::ranges::to<std::vector>();
           return Equation{result, std::move(terms)};
         }) |
         std::ranges::to<std::vector>();
}

enum class Operator : std::uint8_t {
  plus,
  mult,
};

auto apply(const Operator& op, const Int& left, const Int& right) {
  switch (op) {
    case Operator::plus:
      return left + right;
    case Operator::mult:
      return left * right;
  }
  return Int{};
}

auto value(auto&& terms, auto&& operators) {
  const Int init = *terms.begin();
  return std::ranges::fold_left(
      std::ranges::views::zip(std::ranges::views::drop(terms, 1), operators), init,
      [](const auto& acc, const auto& x) {
        const auto [term, op] = x;
        return apply(op, acc, term);
      });
}

auto combination(const std::size_t size, const std::size_t rank) {
  std::vector<Operator> result;
  result.reserve(size);
  for (std::size_t i{}; i != size; ++i) {
    result.push_back(Operator((rank >> i) & 1UZ));
  }
  return result;
}

auto is_valid(const Equation& eq) {
  const auto term_count = eq.terms.size();
  assert(term_count > 0);
  const auto op_count = term_count - 1UZ;
  return std::ranges::any_of(std::ranges::views::iota(0UZ, 1UZ << op_count),
                             [op_count, &eq](const auto& rank) {
                               return eq.result == value(eq.terms, combination(op_count, rank));
                             });
}

auto solve_part1(const auto& input) {
  return std::ranges::fold_left(
      std::ranges::views::filter(input, is_valid) |
          std::ranges::views::transform([](const Equation& eq) { return eq.result; }),
      Int{}, std::plus<>{});
}

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
