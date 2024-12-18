#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iterator>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

enum class Register : std::uint8_t { a, b, c };

enum class Operator : std::uint8_t {
  adv,
  bxl,
  bst,
  jnz,
  bxc,
  out,
  bdv,
  cdv,
};

using RegisterValue = std::uint64_t;

using Int = std::uint8_t;

struct Input {  // NOLINT(cppcoreguidelines-pro-type-member-init)
  std::array<RegisterValue, 3> register_values;
  std::vector<Int> program;
};

auto parse_input(std::istream&& in) {
  const auto content =
      std::views::istream<char>(in >> std::noskipws) | std::ranges::to<std::vector>();

  Input result;

  auto sections = std::views::split(content, "\n\n"sv);
  auto section_it = sections.begin();

  {
    auto register_section = *section_it++;
    auto register_values =
        register_section | std::views::split('\n') | std::views::transform([](const auto& line) {
          const auto token = std::views::join(line | std::views::split(' ') | std::views::drop(2) |
                                              std::views::take(1)) |
                             std::ranges::to<std::string>();
          return std::stoul(token);
        });
    std::ranges::copy(register_values | std::views::take(3), begin(result.register_values));
  }

  {
    auto program_section = *section_it++;
    auto program = std::views::join(program_section | std::views::split(' ') | std::views::drop(1) |
                                    std::views::take(1)) |
                   std::views::lazy_split(',') | std::views::transform([](const auto& t) {
                     const auto token = t | std::ranges::to<std::string>();
                     return std::stoul(token);
                   });
    std::ranges::copy(program, std::back_inserter(result.program));
  }

  return result;
}

auto solve_part1(const Input& input) { return 0; }

auto solve_part2(const Input& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
