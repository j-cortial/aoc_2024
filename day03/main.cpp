#include <charconv>
#include <cstdint>
#include <fstream>
#include <numeric>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>

using std::operator""sv;

using Int = std::int64_t;

struct Mul {
  Int left;
  Int right;

  auto eval() const { return left * right; }
};

auto parse_input(std::istream&& in) {
  std::vector<Mul> result;

  std::string data(std::istreambuf_iterator<char>{in}, {});
  const std::regex re("mul\\([0-9]{1,3},[0-9]{1,3}\\)");
  std::regex_iterator it{data.cbegin(), data.cend(), re};

  for (decltype(it) last; it != last; ++it) {
    const auto match = it->str();
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
    result.emplace_back(left, right);
  }

  return result;
}

auto solve_part1(const auto& input) {
  return std::transform_reduce(input.cbegin(), input.cend(), Int{}, std::plus<>{},
                               [](const auto& m) { return m.eval(); });
}

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
