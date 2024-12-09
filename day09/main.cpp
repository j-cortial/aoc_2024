#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <limits>
#include <numeric>
#include <print>
#include <ranges>
#include <span>
#include <vector>

using Id = std::uint16_t;
using Byte = std::uint8_t;

auto parse_input(std::istream&& in) {
  return std::ranges::views::istream<char>(in) |
         std::ranges::views::take_while([](const char c) { return c != '\n'; }) |
         std::ranges::views::transform([](const char c) { return Byte(c - '0'); }) |
         std::ranges::to<std::vector>();
}

auto checksum(std::span<const Id> filesystem) {
  return std::ranges::fold_left(std::ranges::views::enumerate(filesystem), 0UZ,
                                [](const std::size_t acc, const auto& x) {
                                  const auto& [index, id] = x;
                                  return acc + (index * std::size_t(id));
                                });
}

constexpr auto sentinel = std::numeric_limits<Id>::max();

auto disk_from_map(std::span<const Byte> filemap) {
  std::vector<Id> result;
  const auto size = std::reduce(filemap.cbegin(), filemap.cend(), 0UZ);
  result.reserve(size);
  Id id{};
  for (auto it = filemap.cbegin(); it != filemap.cend(); ++it) {
    {
      auto range = std::ranges::views::repeat(id++) | std::ranges::views::take(*it);
      result.insert(result.end(), range.begin(), range.end());
    }
    if (++it == filemap.cend()) {
      break;
    }
    {
      auto range = std::ranges::views::repeat(sentinel) | std::ranges::views::take(*it);
      result.insert(result.end(), range.begin(), range.end());
    }
  }
  return result;
}

auto compact(std::vector<Id>& filesystem) {
  auto sink = std::ranges::find(filesystem, sentinel);
  auto source = std::ranges::find_if(std::ranges::views::reverse(filesystem),
                                     [](const Id id) { return id != sentinel; });

  while (sink != source.base()) {
    std::iter_swap(sink, source);
    sink = std::ranges::find(std::ranges::subrange(sink, filesystem.end()), sentinel);
    source = std::ranges::find_if(std::ranges::subrange(source, filesystem.rend()),
                                  [](const Id id) { return id != sentinel; });
  }

  return sink;
}

auto solve_part1(const auto& input) {
  auto filesystem = disk_from_map(input);
  const auto end = compact(filesystem);
  return checksum({filesystem.begin(), end});
}

auto solve_part2(const auto& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
