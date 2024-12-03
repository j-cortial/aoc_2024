#include <algorithm>
#include <charconv>
#include <cstdint>
#include <iterator>
#include <fstream>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

using std::operator""sv;

using Int = std::int64_t;

struct Mul {
  Int left;
  Int right;

  auto eval() const { return left * right; }
};

using Cmd = std::variant<Mul, bool>;

auto parse_input(std::istream&& in) {
  std::vector<Cmd> result;

  std::string data(std::istreambuf_iterator<char>{in}, {});
  const std::regex re(R"(mul\([0-9]{1,3},[0-9]{1,3}\)|do\(\)|don't\(\))");
  std::regex_iterator it{data.cbegin(), data.cend(), re};

  for (decltype(it) last; it != last; ++it) {
    const auto match = it->str();
    if (match[0] == 'd') {
      result.emplace_back(match.size() == 4);
    } else {
      const auto data = match.substr(4, match.size() - 5);
      auto terms =
          std::ranges::views::split(data, ","sv) | std::ranges::views::transform([](const auto& s) {
            Int value;  // NOLINT
            std::string_view v(s);
            std::from_chars(v.begin(), v.end(), value);
            return value;
          });
      auto it = terms.begin();
      const auto left = *it++;
      const auto right = *it++;
      result.emplace_back(Mul{.left = left, .right = right});
    }
  }

  return result;
}

struct Part1Visitor {
  constexpr auto operator()(const Mul& mul) const { return acc + mul.eval(); }
  constexpr auto operator()(bool /*active*/) const { return acc; }

  Int acc;
};

auto solve_part1(const auto& input) {
  return std::ranges::fold_left(input, Int{}, [](const auto& acc, const auto& cmd) {
    return std::visit(Part1Visitor{acc}, cmd);
  });
}

struct Part2Visitor {
  constexpr auto operator()(const Mul& mul) const {
    return std::make_pair(acc.first + (acc.second ? mul.eval() : Int{}), acc.second);
  }
  constexpr auto operator()(bool active) const { return std::make_pair(acc.first, active); }

  std::pair<Int, bool> acc;
};

auto solve_part2(const auto& input) {
  return std::ranges::fold_left(
             input, std::pair{Int{}, true},
             [](const auto& acc, const auto& cmd) { return std::visit(Part2Visitor{acc}, cmd); })
      .first;
}

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
