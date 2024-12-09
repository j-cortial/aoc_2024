#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iterator>
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

  auto operator<=>(const Loc&) const = default;
};

using Tile = char;

auto make_antennas(auto&& rng) {
  std::map<char, std::vector<Loc>> result;

  for (const auto& [kind, loc] : rng) {
    result[kind].push_back(loc);
  }

  return result;
}

auto antinode_loc(const Loc& first, const Loc& second) {
  return Loc{.row = (Idx(2) * second.row) - first.row, .col = (Idx(2) * second.col) - first.col};
}

class City {
 public:
  auto row_count() const { return row_count_; }
  auto col_count() const { return col_count_; }

  auto is_inside(const Loc& loc) const {
    const auto [row, col] = loc;
    return (row >= 0 && row < row_count_ && col >= 0 && col < col_count_);
  }

  auto antinodes() const {
    auto result_with_duplicates = std::ranges::common_view(std::ranges::views::join(
        antennas_ | std::ranges::views::values |
        std::ranges::views::transform([this](const auto& locs) {
          return std::ranges::views::cartesian_product(locs, locs) |
                 std::ranges::views::filter(
                     [](const auto& p) { return std::get<0>(p) != std::get<1>(p); }) |
                 std::ranges::views::transform(
                     [](const auto& p) { return antinode_loc(std::get<0>(p), std::get<1>(p)); }) |
                 std::ranges::views::filter([&](const auto& loc) { return is_inside(loc); });
        })));
    return std::set<Loc>{result_with_duplicates.begin(), result_with_duplicates.end()};
  }

  City(auto&& antennas, Idx row_count, Idx col_count)
      : antennas_{make_antennas(antennas)}, row_count_{row_count}, col_count_{col_count} {}

 private:
  std::map<char, std::vector<Loc>> antennas_;
  Idx row_count_;
  Idx col_count_;
};

auto parse_input(std::istream&& in) {
  const std::vector<char> data{std::istreambuf_iterator{in}, {}};

  const auto row_count = Idx(std::ranges::count(data, '\n'));
  const auto col_count = Idx(std::ranges::distance(
      std::ranges::views::take_while(data, [](const char c) { return c != '\n'; })));
  auto antennas = std::ranges::views::common(std::ranges::views::join(
      std::ranges::views::enumerate(std::ranges::views::split(data, '\n')) |
      std::ranges::views::transform([&](const auto& p) {
        const auto& [i, r] = p;
        return std::ranges::views::enumerate(r) |
               std::ranges::views::filter([&](const auto& p) { return std::get<1>(p) != '.'; }) |
               std::ranges::views::transform([i](const auto& p) {
                 const auto& [j, tile] = p;
                 return std::make_pair(tile, Loc{.row = Idx(i), .col = Idx(j)});
               });
      })));

  return City{std::move(antennas), row_count, col_count};
}

auto solve_part1(const auto& input) { return input.antinodes().size(); }

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
