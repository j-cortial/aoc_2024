#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <ios>
#include <istream>
#include <print>
#include <ranges>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

using Height = std::uint8_t;

using Profile = std::array<Height, 5>;

struct Input {
  std::vector<Profile> locks;
  std::vector<Profile> keys;
};

auto parse_input(std::istream&& in) {
  const auto content =
      std::views::istream<char>(in >> std::noskipws) | std::ranges::to<std::vector>();

  Input result;

  auto schemes = std::views::split(content, "\n\n"sv) |
                 std::views::filter([](const auto& line) { return !std::ranges::empty(line); });

  for (auto scheme : schemes) {
    auto lines = std::views::split(scheme, '\n') | std::views::drop(1) | std::views::take(5);
    const Profile p = std::ranges::fold_left(lines, Profile{}, [](Profile&& acc, const auto& line) {
      for (auto&& [sum, row] : std::views::zip(acc, line)) {
        if (row == '#') {
          ++sum;
        }
      }
      return acc;
    });
    if (*std::ranges::begin(scheme) == '#') {
      result.locks.push_back(p);
    } else {
      result.keys.push_back(p);
    }
  }

  return result;
}

auto solve_part1(const auto& input) {
  return std::ranges::count_if(
      std::views::cartesian_product(std::views::all(input.locks), std::views::all(input.keys)),
      [](const auto& p) {
        const auto& [lock, key] = p;
        return std::ranges::all_of(std::views::zip(lock, key), [](const auto& p) {
          return std::get<0>(p) + std::get<1>(p) <= Height(5);
        });
      });
}

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
}
