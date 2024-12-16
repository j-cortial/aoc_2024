#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <ios>
#include <print>
#include <ranges>
#include <set>
#include <string_view>
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

constexpr std::array<Loc, 4> moves{
    {{.row = -1, .col = 0}, {.row = 1, .col = 0}, {.row = 0, .col = -1}, {.row = 0, .col = 1}}};

enum class Instruction : std::uint8_t {
  up,
  down,
  left,
  right,
};

struct Input {  // NOLINT(cppcoreguidelines-pro-type-member-init)
  std::set<Loc> walls;
  std::set<Loc> crates;
  Loc robot;
  std::vector<Instruction> instructions;
};

auto parse_input(std::istream&& in) {
  Input result;
  auto& [walls, crates, robot, instructions] = result;

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
        walls.insert(loc);
        break;
      case 'O':
        crates.insert(loc);
        break;
      case '@':
        robot = loc;
        break;
      case '.':
        break;
      default:
        std::unreachable();
    }
  }

  auto instruction_section = *section++;

  for (const char c :
       instruction_section | std::views::filter([](const char c) { return c != '\n'; })) {
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

auto gps_coordinates(Loc loc) { return (Idx{100} * loc.row) + loc.col; }

auto solve_part1(const auto& input) {
  const auto& [walls, initial_crates, robot_start, instructions] = input;

  auto crates = initial_crates;
  auto robot = robot_start;

  for (const Loc move : std::views::transform(
           instructions, [](const auto i) { return moves[std::to_underlying(i)]; })) {
    const auto target = robot + move;
    auto space = target;
    std::vector<std::set<Loc>::iterator> line;
    for (auto it = crates.find(space); it != crates.end();
         space = space + move, it = crates.find(space)) {
      line.push_back(it);
    }
    if (!walls.contains(space)) {
      if (!line.empty()) {
        crates.insert(*line.back() + move);
        crates.erase(line.front());
      }
      robot = target;
    }
  }
  return std::ranges::fold_left(
      crates, 0UZ, [](const auto acc, const Loc& x) { return acc + gps_coordinates(x); });
}

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
