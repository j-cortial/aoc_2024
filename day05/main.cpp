#include <charconv>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

using namespace std::string_literals;
using namespace std::string_view_literals;

using Page = std::uint8_t;

struct Rule {
  Page before;
  Page after;
};

using Update = std::vector<Page>;

auto parse_input(std::istream&& in) -> std::pair<std::vector<Rule>, std::vector<Update>> {
  const std::vector<char> data{std::istreambuf_iterator{in}, {}};
  auto sections = std::ranges::views::lazy_split(data, "\n\n"sv) |
                  std::ranges::views::transform([](const auto& section) {
                    return section | std::ranges::views::lazy_split('\n');
                  });

  auto it = sections.begin();

  auto rules = *it++ | std::ranges::views::transform([](const auto& line) {
    auto pages =
        std::ranges::views::split(line, '|') | std::ranges::views::transform([](const auto& page) {
          const auto token = std::ranges::to<std::vector>(page);
          Page result;  // NOLINT
          std::from_chars(token.data(), std::next(token.data(), token.size()), result);
          return result;
        });
    return Rule{*pages.begin(), *std::next(pages.begin())};
  }) | std::ranges::to<std::vector>();

  auto updates = *it++ | std::ranges::views::transform([](const auto& line) {
    auto pages =
        std::ranges::views::split(line, ',') | std::ranges::views::transform([](const auto& page) {
          const auto token = std::ranges::to<std::vector>(page);
          Page result;  // NOLINT
          std::from_chars(token.data(), std::next(token.data(), token.size()), result);
          return result;
        });
    return pages | std::ranges::to<std::vector>();
  }) | std::ranges::to<std::vector>();

  return {std::move(rules), std::move(updates)};
}

auto solve_part1(const auto& input) { return 0; }

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
