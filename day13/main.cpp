#include <algorithm>
#include <cassert>
#include <cctype>
#include <charconv>
#include <fstream>
#include <ios>
#include <optional>
#include <print>
#include <ranges>
#include <string_view>
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

struct Game {
  Loc button_a;
  Loc button_b;
  Loc target;
};

template <typename Numeric, typename Range>
auto parse(Range&& rng) {
  const auto token = std::forward<Range>(rng) | std::ranges::to<std::vector>();
  Numeric result;  // NOLINT
  [[maybe_unused]] const std::from_chars_result status =
      std::from_chars(token.data(), std::next(token.data(), std::ptrdiff_t(token.size())), result);
  assert(status.ec == std::errc{});
  return result;
}

auto parse_input(std::istream&& in) {
  const auto content =
      std::views::istream<char>(in >> std::noskipws) | std::ranges::to<std::vector>();

  return std::views::split(content, "\n\n"sv) | std::views::transform([](const auto& block) {
           auto locs =
               std::views::split(block, '\n') | std::views::transform([](const auto& line) {
                 auto coords = std::views::join(
                     std::views::split(line, ": "sv) | std::views::drop(1) |
                     std::views::transform([](const auto& part) {
                       return std::views::split(part, ',') |
                              std::views::transform([](const auto& statement) {
                                return parse<Idx>(std::views::drop_while(
                                    statement, [](const char c) { return !std::isdigit(c); }));
                              });
                     }));
                 auto coords_it = std::ranges::begin(coords);
                 const Idx row = *coords_it;
                 ++coords_it;
                 const Idx col = *coords_it;
                 return Loc{.row = row, .col = col};
               });
           auto locs_it = std::ranges::begin(locs);
           const Loc button_a = *locs_it++;
           const Loc button_b = *locs_it++;
           const Loc target = *locs_it++;
           return Game{.button_a = button_a, .button_b = button_b, .target = target};
         }) |
         std::ranges::to<std::vector>();
}

auto button_a_dominates(const Game& game) {
  return game.button_a.row >= 3 * game.button_b.row && game.button_a.col >= 3 * game.button_b.col;
}

using PushCount = std::int32_t;
using TokenCount = std::int32_t;

auto operator*(Idx factor, const Loc& base) {
  return Loc{.row = base.row * factor, .col = base.col * factor};
}

auto operator/(const Loc& left, const Loc& right) {
  return std::min(left.row / right.row, left.col / right.col);
}

auto least_tokens_to_win(const Game& game) -> std::optional<TokenCount> {
  assert(!button_a_dominates(game));
  for (PushCount b_pushes = game.target / game.button_b; b_pushes >= PushCount{}; --b_pushes) {
    const auto remainder = game.target - (b_pushes * game.button_b);
    const PushCount a_pushes = remainder / game.button_a;
    if (remainder == a_pushes * game.button_a) {
      return TokenCount{(3 * a_pushes) + b_pushes};
    }
  }
  return std::nullopt;
}

auto solve_part1(const auto& input) {
  return std::ranges::fold_left(input, TokenCount{}, [](const TokenCount acc, const Game& game) {
    const auto maybe_tokens = least_tokens_to_win(game);
    return acc + (maybe_tokens.has_value() ? *maybe_tokens : TokenCount{});
  });
}

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
