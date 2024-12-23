#include <array>
#include <cstdint>
#include <fstream>
#include <functional>
#include <ios>
#include <limits>
#include <print>
#include <queue>
#include <ranges>
#include <set>
#include <utility>
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
    {{.row = 0, .col = 1}, {.row = -1, .col = 0}, {.row = 0, .col = -1}, {.row = 1, .col = 0}}};

enum class Direction : std::uint8_t {
  east,
  north,
  west,
  south,
};

auto turn_left(Direction dir) { return static_cast<Direction>((std::to_underlying(dir) + 1) % 4); }

auto turn_right(Direction dir) { return static_cast<Direction>((std::to_underlying(dir) + 3) % 4); }

struct State {
  Loc loc;
  Direction dir;

  auto operator<=>(const State&) const = default;
};

using Cost = std::uint64_t;

auto transitions(const std::set<Loc>& open_tiles, const State& state) {
  std::vector<std::pair<Cost, State>> result;
  const auto next_tile = state.loc + moves[std::to_underlying(state.dir)];
  if (open_tiles.contains(next_tile)) {
    result.emplace_back(Cost{1}, State{.loc{next_tile}, .dir{state.dir}});
  }
  result.emplace_back(Cost{1000}, State{.loc = state.loc, .dir = turn_left(state.dir)});
  result.emplace_back(Cost{1000}, State{.loc = state.loc, .dir = turn_right(state.dir)});
  return result;
}

auto solve_part1(const auto& input) {
  std::set<State> explored;

  using Element = std::pair<Cost, State>;
  std::priority_queue<Element, std::vector<Element>, std::greater<>> front;
  front.emplace(Cost{}, State{input.start, Direction::east});

  while (!front.empty()) {
    const auto [cost, state] = front.top();
    front.pop();
    if (state.loc == input.finish) {
      return cost;
    }
    explored.insert(state);
    for (const auto& [candidate_cost, candidate_state] : transitions(input.open_tiles, state)) {
      if (!explored.contains(candidate_state)) {
        front.emplace(cost + candidate_cost, candidate_state);
      }
    }
  }

  return std::numeric_limits<Cost>::max();
}

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
