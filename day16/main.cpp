#include <array>
#include <cstdint>
#include <fstream>
#include <ios>
#include <print>
#include <ranges>
#include <set>
#include <vector>

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

struct Input {
  std::set<Loc> open_tiles;
  Loc start;
  Loc finish;
};

auto parse_input(std::istream&& in) {
  const auto content =
      std::views::istream<char>(in >> std::noskipws) | std::ranges::to<std::vector>();
  Loc start;   // NOLINT(cppcoreguidelines-pro-type-member-init)
  Loc finish;  // NOLINT(cppcoreguidelines-pro-type-member-init)
  auto open_tiles = std::views::common(std::views::join(
      std::views::enumerate(std::views::split(content, '\n') |
                            std::views::filter([](const auto& line) { return !empty(line); })) |
      std::views::transform([&start, &finish](const auto& p) {
        const auto& [row, line] = p;
        return std::views::enumerate(line) |
               std::views::filter([](const auto& q) { return std::get<1>(q) != '#'; }) |
               std::views::transform([row, &start, &finish](const auto& q) {
                 const auto& [col, c] = q;
                 const Loc loc{.row = Idx(row), .col = Idx(col)};
                 if (c == 'S') {
                   start = loc;
                 }
                 if (c == 'E') {
                   finish = loc;
                 }
                 return loc;
               });
      })));
  return Input{.open_tiles{begin(open_tiles), end(open_tiles)}, .start{start}, .finish{finish}};
}

constexpr std::array<Loc, 4> moves{
    {{.row = -1, .col = 0}, {.row = 1, .col = 0}, {.row = 0, .col = -1}, {.row = 0, .col = 1}}};

enum class Instruction : std::uint8_t {
  up,
  down,
  left,
  right,
};

auto solve_part1(const auto& input) { return 0; }

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
