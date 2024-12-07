#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <print>
#include <ranges>
#include <set>
#include <utility>
#include <vector>

using Idx = std::int32_t;

struct Loc {
  Idx row;
  Idx col;

  auto operator<=>(const Loc&) const = default;
};

using Obstacles = std::set<Loc>;

using Tile = bool;

struct Guard {
  Loc loc;
  Loc dir;

  auto operator<=>(const Guard&) const = default;
};

class Room {
 public:
  auto row_count() const { return row_count_; }
  auto col_count() const { return col_count_; }

  auto operator[](Loc loc) const -> std::optional<Tile> {
    const auto [row, col] = loc;
    if (row < 0 || row >= row_count_ || col < 0 || col >= col_count_) {
      return std::nullopt;
    }
    return {obstacles_.contains(loc)};
  }

  Room(Obstacles obstacles, Idx row_count, Idx col_count)
      : obstacles_{std::move(obstacles)}, row_count_{row_count}, col_count_{col_count} {}

  Room(Room other, Loc new_obstacle) : Room(std::move(other)) { obstacles_.insert(new_obstacle); }

 private:
  Obstacles obstacles_;
  Idx row_count_;
  Idx col_count_;
};

auto parse_input(std::istream&& in) {
  const std::vector<char> data{std::istreambuf_iterator{in}, {}};

  const auto row_count = Idx(std::ranges::count(data, '\n'));
  const auto col_count = Idx(std::ranges::distance(
      std::ranges::views::take_while(data, [](const char c) { return c != '\n'; })));

  auto obstacles = std::ranges::views::common(std::ranges::views::join(
      std::ranges::views::enumerate(std::ranges::views::split(data, '\n')) |
      std::ranges::views::transform([&](const auto& p) {
        const auto& [i, r] = p;
        return std::ranges::views::enumerate(r) |
               std::ranges::views::filter([&](const auto& p) { return std::get<1>(p) == '#'; }) |
               std::ranges::views::transform(
                   [i](const auto& p) { return Loc{.row = Idx(i), .col = Idx(std::get<0>(p))}; });
      })));

  auto guard =
      (*std::ranges::views::filter(
            std::ranges::views::join(
                std::ranges::views::enumerate(std::ranges::views::split(data, '\n')) |
                std::ranges::views::transform([&](const auto& p) {
                  const auto& [i, r] = p;
                  return std::ranges::views::enumerate(r) |
                         std::ranges::views::transform([i](const auto& p) {
                           return std::make_pair(std::get<1>(p),
                                                 Loc{.row = Idx(i), .col = Idx(std::get<0>(p))});
                         });
                })),
            [](const auto& p) { return std::get<0>(p) == '^'; })
            .begin())
          .second;

  return std::make_pair(Room{Obstacles(obstacles.begin(), obstacles.end()), row_count, col_count},
                        guard);
}

auto turn_right(const Loc& dir) { return Loc{.row = dir.col, .col = -dir.row}; }

auto patrol(const Room& room, Guard guard) {
  std::set<Loc> result{};
  result.insert(guard.loc);

  for (;;) {
    const auto candidate =
        Loc{.row = guard.loc.row + guard.dir.row, .col = guard.loc.col + guard.dir.col};
    const auto tile = room[candidate];
    if (!tile) {
      break;
    }
    if (*tile) {
      guard.dir = turn_right(guard.dir);
    } else {
      guard.loc = candidate;
      result.insert(candidate);
    }
  }
  return result;
}

auto solve_part1(const auto& input) {
  return patrol(input.first, {input.second, Loc{.row = -1, .col = 0}}).size();
}

auto loops(const Room& room, Guard guard) {
  std::set<Guard> patrol{{guard}};
  for (;;) {
    const auto candidate =
        Loc{.row = guard.loc.row + guard.dir.row, .col = guard.loc.col + guard.dir.col};
    if (patrol.contains(Guard{.loc = candidate, .dir = guard.dir})) {
      return true;
    }
    const auto tile = room[candidate];
    if (!tile) {
      return false;
    }
    if (*tile) {
      guard.dir = turn_right(guard.dir);
    } else {
      guard.loc = candidate;
      patrol.insert(guard);
    }
  }
}

auto solve_part2(const auto& input) {
  const auto& [room, start] = input;
  const auto& guard = Guard{start, Loc{.row = -1, .col = 0}};
  auto candidates = patrol(input.first, guard);
  candidates.erase(start);
  return std::ranges::count_if(
      candidates, [&](const Loc& new_obstacle) { return loops(Room{room, new_obstacle}, guard); });
}

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
