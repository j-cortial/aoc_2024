#include <cassert>
#include <cstdint>
#include <fstream>
#include <ios>
#include <optional>
#include <print>
#include <ranges>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

enum class Color : std::uint8_t {
  white,
  blue,
  black,
  red,
  green,
};

auto parse_color(const char c) -> std::optional<Color> {
  switch (c) {
    case 'w':
      return Color::white;
    case 'u':
      return Color::blue;
    case 'b':
      return Color::black;
    case 'r':
      return Color::red;
    case 'g':
      return Color::green;
    default:
      return std::nullopt;
  }
}

using Pattern = std::vector<Color>;
using Design = std::vector<Color>;

struct Input {
  std::vector<Pattern> patterns;
  std::vector<Design> designs;
};

auto parse_input(std::istream&& in) {
  const auto content =
      std::views::istream<char>(in >> std::noskipws) | std::ranges::to<std::vector>();
  auto sections = std::views::split(content, "\n\n"sv);
  auto section_it = std::ranges::begin(sections);

  Input result;

  auto parse_color_range = [](auto&& rng) {
    return std::views::transform(rng,
                                 [](const char c) {
                                   const auto maybe_color = parse_color(c);
                                   assert(maybe_color.has_value());
                                   return *maybe_color;
                                 }) |
           std::ranges::to<std::vector>();
  };

  auto patterns = *section_it++;
  result.patterns = std::views::split(patterns, ", "sv) | std::views::transform(parse_color_range) |
                    std::ranges::to<std::vector>();

  auto designs = *section_it++;
  result.designs = std::views::split(designs, '\n') |
                   std::views::filter([](const auto& line) { return !std::ranges::empty(line); }) |
                   std::views::transform(parse_color_range) | std::ranges::to<std::vector>();

  return result;
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
