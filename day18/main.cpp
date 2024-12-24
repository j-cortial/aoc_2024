#include <array>
#include <cmath>
#include <cstdint>
#include <format>
#include <fstream>
#include <functional>
#include <ios>
#include <iterator>
#include <limits>
#include <optional>
#include <print>
#include <queue>
#include <ranges>
#include <set>
#include <span>
#include <string>
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

auto manhattan_length(const Loc& loc) { return std::abs(loc.row) + std::abs(loc.col); }

auto parse_input(std::istream&& in) {
  return std::views::istream<char>(in >> std::noskipws) | std::views::lazy_split('\n') |
         std::views::filter(
             [](const auto& line) { return std::ranges::begin(line) != std::ranges::end(line); }) |
         std::views::transform([](const auto& line) {
           auto coords =
               std::views::lazy_split(line, ',') | std::views::transform([](const auto& token) {
                 auto str = token | std::ranges::to<std::string>();
                 return std::stol(str);
               });
           auto coords_it = std::ranges::begin(coords);
           const Idx row = *coords_it;
           ++coords_it;
           const Idx col = *coords_it;
           return Loc{.row = row, .col = col};
         }) |
         std::ranges::to<std::vector>();
}

class Memory {
 public:
  Memory(Loc lower_right, std::span<const Loc> bad_blocks)
      : lower_right_(lower_right), bad_blocks_(bad_blocks.begin(), bad_blocks.end()) {}

  auto block_is_open(Loc loc) const {
    return loc.row >= Idx{} && loc.col >= Idx{} && loc.row <= lower_right_.row &&
           loc.col <= lower_right_.col && !bad_blocks_.contains(loc);
  }

 private:
  Loc lower_right_;
  std::set<Loc> bad_blocks_;
};

constexpr std::array<Loc, 4> moves{
    {{.row = 0, .col = 1}, {.row = -1, .col = 0}, {.row = 0, .col = -1}, {.row = 1, .col = 0}}};

using Cost = std::uint32_t;

struct Candidate {
  Cost heuristic_cost;
  Cost cost;
  Loc loc;

  auto operator<=>(const Candidate&) const = default;
};

auto solve_astar(std::span<const Loc> bad_blocks, Idx size) -> std::optional<Cost> {
  const Loc exit{.row = size, .col = size};
  const Memory memory{exit, bad_blocks};

  const Loc start{.row = Idx{}, .col = Idx{}};
  std::set<Loc> visited;
  visited.insert(start);
  std::priority_queue<Candidate, std::vector<Candidate>, std::greater<>> candidates;
  candidates.emplace(manhattan_length(exit), Cost{}, start);

  while (!candidates.empty()) {
    const auto [_, cost, loc] = candidates.top();
    if (loc == exit) {
      return {cost};
    }
    candidates.pop();

    for (const auto& move : moves) {
      const auto target = loc + move;
      if (memory.block_is_open(target) && !visited.contains(target)) {
        const Cost estimated_cost = cost + Cost{1} + manhattan_length(exit - target);
        candidates.emplace(estimated_cost, cost + Cost{1}, target);
        visited.insert(target);
      }
    }
  }

  return std::nullopt;
}

auto solve_part1(const auto& input) {
  const auto result = solve_astar({input.cbegin(), std::next(input.cbegin(), 1024)}, Idx{70});
  return result.has_value() ? *result : std::numeric_limits<Cost>::max();
}

auto solve_part2(const auto& input) {
  const auto start_index = 1024UZ;
  const auto index =
      start_index +
      std::ranges::distance(
          std::views::iota(start_index + 1UZ) | std::views::take_while([&input](const auto count) {
            return solve_astar({input.cbegin(), std::next(input.cbegin(), count)}, Idx{70})
                .has_value();
          }));
  const auto loc = input[index];
  return std::format("{},{}", loc.row, loc.col);
}

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
