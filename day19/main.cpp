#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iterator>
#include <map>
#include <optional>
#include <print>
#include <ranges>
#include <span>
#include <string_view>
#include <utility>
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

auto operator<<(std::ostream& out, const Color c) -> auto& {
  const char ch = [&]() {
    switch (c) {
      case Color::white:
        return 'w';
      case Color::blue:
        return 'u';
      case Color::black:
        return 'b';
      case Color::red:
        return 'r';
      case Color::green:
        return 'g';
    }
    std::unreachable();
  }();
  return out << ch;
}

using Pattern = std::vector<Color>;
using Design = std::vector<Color>;

auto operator<<(std::ostream& out, const Design& design) -> auto& {
  for (const Color c : design) {
    out << c;
  }
  return out;
}

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

auto possible_arrangements(const std::span<const Pattern> patterns, const Design& design) {
  std::map<Design::const_iterator, std::uint64_t> prefixes{{design.begin(), std::uint64_t(1)}};

  for (auto it = design.begin(); it != design.end(); ++it) {
    decltype(prefixes) next_prefixes;

    for (const auto& [prefix_begin, count] : prefixes) {
      const std::span<const Color> candidate{prefix_begin, std::next(it)};
      for (const auto& pattern : patterns) {
        const auto [it_p, it_c] = std::ranges::mismatch(pattern, candidate);
        if (it_c == candidate.end()) {
          if (it_p == pattern.end()) {
            next_prefixes[std::next(it)] += count;
          } else {
            next_prefixes[prefix_begin] = count;
          }
        }
      }
    }

    if (next_prefixes.empty()) {
      return std::uint64_t{};
    }

    prefixes = next_prefixes;
  }

  const auto result = prefixes.find(design.end());
  return result != prefixes.end() ? result->second : std::uint64_t{};
}

auto is_design_possible(const std::span<const Pattern> patterns, const Design& design) {
  return possible_arrangements(patterns, design) > std::uint64_t{};
}

auto solve_part1(const auto& input) {
  return std::ranges::count_if(input.designs, [&](const Design& design) {
    return is_design_possible(input.patterns, design);
  });
}

auto solve_part2(const auto& input) {
  return std::ranges::fold_left(input.designs, std::uint64_t{},
                                [&](const auto acc, const Design& design) {
                                  return acc + possible_arrangements(input.patterns, design);
                                });
}

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
