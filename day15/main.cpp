#include <array>
#include <cstdint>
#include <fstream>
#include <ios>
#include <print>
#include <ranges>
#include <set>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

using namespace std::string_view_literals;

using Idx = std::int32_t;

struct Loc {
  Idx row;
  Idx col;

  auto operator<=>(const Loc& other) const = default;
};

auto operator+(const Loc& left, const Loc& right) {
  return Loc{.row = left.row + right.row, .col = left.col + right.col};
}

auto operator-(const Loc& left, const Loc& right) {
  return Loc{.row = left.row - right.row, .col = left.col - right.col};
}

struct Warehouse {
  std::set<Loc> walls;
  std::set<Loc> crates;
};

constexpr std::array<Loc, 4> moves{
    {{.row = -1, .col = 0}, {.row = 1, .col = 0}, {.row = 0, .col = -1}, {.row = 0, .col = 1}}};

enum class Instruction : std::uint8_t {
  up,
  down,
  left,
  right,
};

auto parse_input(std::istream&& in) {
  std::tuple<Warehouse, Loc, std::vector<Instruction>> result;
  auto& [warehouse, robot_start, instructions] = result;

  const auto content =
      std::views::istream<char>(in >> std::noskipws) | std::ranges::to<std::vector>();
  auto sections = std::views::split(content, "\n\n"sv);

  auto section = sections.begin();
  auto warehouse_section = *section++;

  auto tiles = std::views::join(std::views::enumerate(std::views::split(warehouse_section, '\n')) |
                                std::views::transform([](const auto& p) {
                                  const auto& [i, row] = p;
                                  return std::views::enumerate(row) |
                                         std::views::transform([i](const auto& p) {
                                           const auto& [j, tile] = p;
                                           return std::make_pair(Loc(i, j), tile);
                                         });
                                }));

  for (const auto [loc, tile] : tiles) {
    switch (tile) {
      case '#':
        warehouse.walls.insert(loc);
        break;
      case 'O':
        warehouse.crates.insert(loc);
        break;
      case '@':
        robot_start = loc;
        break;
      case '.':
        break;
      default:
        std::unreachable();
    }
  }

  auto instruction_section = *section++;

  for (const char c : instruction_section) {
    const auto i = [](const char c) {
      switch (c) {
        case '^':
          return Instruction::up;
        case 'v':
          return Instruction::down;
        case '<':
          return Instruction::left;
        case '>':
          return Instruction::right;
        default:
          std::unreachable();
      }
    }(c);
    instructions.push_back(i);
  }

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
