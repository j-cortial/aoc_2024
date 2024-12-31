#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <map>
#include <print>
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

auto find_fair_path(const Input& input) {
  std::vector<Loc> result;
  result.reserve(input.open_tiles.size());
  result.push_back(input.start);

  assert(input.start != input.finish);
  result.push_back(input.start + *std::ranges::find_if(moves, [&](const auto& m) {
                     return input.open_tiles.contains(input.start + m);
                   }));

  while (result.back() != input.finish) {
    const Loc& current = result.back();
    const Loc& previous = *std::next(result.crbegin());
    result.push_back(current + *std::ranges::find_if(moves, [&](const auto& m) {
                       const Loc candidate = current + m;
                       return candidate != previous && input.open_tiles.contains(candidate);
                     }));
  }

  return result;
}

using Cost = std::int32_t;

struct Cheat {
  Loc wall;
  Loc exit;

  auto operator<=>(const Cheat&) const = default;
};

auto compute_cheat_gains(const Input& input) {
  const auto fair_path = find_fair_path(input);
  assert(fair_path.size() == input.open_tiles.size());

  const auto costs = std::views::enumerate(fair_path) | std::views::transform([](const auto& p) {
                       return std::make_pair(std::get<1>(p), Cost(std::get<0>(p)));
                     }) |
                     std::ranges::to<std::map>();

  std::map<Cheat, Cost> result;

  for (const auto [entry_cost, entry] : std::views::enumerate(fair_path)) {
    for (const Loc m : moves) {
      const Loc wall = entry + m;
      if (!input.open_tiles.contains(wall)) {
        const Loc exit = wall + m;
        if (const auto it = costs.find(exit); it != costs.end()) {
          const Cost gain = it->second - (Cost(entry_cost) + Cost{2});
          if (gain > Cost{}) {
            result.emplace(Cheat{.wall = wall, .exit = exit}, gain);
          }
        }
      }
    }
  }

  return result;
}

auto solve_part1(const auto& input) {
  const auto fair_path = find_fair_path(input);
  assert(fair_path.size() == input.open_tiles.size());

  const auto cheat_gains = compute_cheat_gains(input);

  constexpr Cost bound{100};

  return std::ranges::count_if(std::views::values(cheat_gains),
                               [](const Cost gain) { return gain >= bound; });
}

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
