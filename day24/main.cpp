#include <array>
#include <cassert>
#include <fstream>
#include <ios>
#include <istream>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

using Wire = std::string;

struct WireStatus {
  Wire wire;
  bool value;
};

using Op = bool (*)(bool, bool);

auto apply(const Op& op, const bool left, const bool right) { return (*op)(left, right); };

auto parse_op(std::string_view str) -> Op {
  if (str == "AND"sv) {
    return [](bool left, bool right) { return left && right; };
  }
  if (str == "OR"sv) {
    return [](bool left, bool right) { return left || right; };
  }
  if (str == "XOR"sv) {
    return [](bool left, bool right) { return left != right; };
  }
  return nullptr;
}

struct Gate {
  std::array<Wire, 2> inputs;
  Op op;
  Wire output;
};

struct Input {
  std::vector<WireStatus> wire_statuses;
  std::vector<Gate> gates;
};

auto parse_input(std::istream&& in) {
  const auto content =
      std::views::istream<char>(in >> std::noskipws) | std::ranges::to<std::vector>();

  auto sections = std::views::split(content, "\n\n"sv);
  auto section_it = std::ranges::begin(sections);

  auto wire_statuses =
      std::views::split(*section_it++, '\n') |
      std::views::filter([](const auto& line) { return !std::ranges::empty(line); }) |
      std::views::transform([](const auto& line) {
        auto tokens = std::views::split(line, ": "sv);
        auto token_it = std::ranges::begin(tokens);
        auto wire = *token_it++;
        auto value = *token_it++ | std::views::transform([](const char c) { return c == '1'; });
        return WireStatus{wire | std::ranges::to<std::string>(), *std::ranges::begin(value)};
      });

  auto gates = std::views::split(*section_it++, '\n') |
               std::views::filter([](const auto& line) { return !std::ranges::empty(line); }) |
               std::views::transform([](const auto& line) {
                 auto parts = std::views::split(line, " -> "sv);
                 auto part_it = std::ranges::begin(parts);

                 auto tokens = std::views::split(*part_it++, ' ');
                 auto token_it = std::ranges::begin(tokens);
                 auto left_input = *token_it++;
                 auto op = parse_op(*token_it++ | std::ranges::to<std::string>());
                 assert(bool(op));
                 auto right_input = *token_it++;

                 auto output = *part_it++;

                 return Gate{{left_input | std::ranges::to<std::string>(),
                              right_input | std::ranges::to<std::string>()},
                             op,
                             output | std::ranges::to<std::string>()};
               });

  return Input{.wire_statuses = wire_statuses | std::ranges::to<std::vector>(),
               .gates = gates | std::ranges::to<std::vector>()};
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
