#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <ios>
#include <istream>
#include <map>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <utility>
#include <vector>

using Computer = std::string;
using Link = std::pair<Computer, Computer>;

auto parse_input(std::istream&& in) {
  return std::views::istream<char>(in >> std::noskipws) | std::views::lazy_split('\n') |
         std::views::filter(
             [](const auto& line) { return std::ranges::begin(line) != std::ranges::end(line); }) |
         std::views::transform([](const auto& line) {
           auto computers =
               std::views::lazy_split(line, '-') | std::views::transform([](const auto& token) {
                 return token | std::ranges::to<Computer>();
               });
           auto computer_it = std::ranges::begin(computers);
           Computer first = *computer_it;
           ++computer_it;
           Computer second = *computer_it;
           ++computer_it;
           return Link{std::move(first), std::move(second)};
         }) |
         std::ranges::to<std::vector>();
}

auto compute_neighborhoods(const std::span<const Link> links) {
  std::multimap<Computer, Computer> result;

  for (const Link& link : links) {
    const auto it_pair = result.equal_range(link.first);
    if (!std::ranges::contains(it_pair.first, it_pair.second, link.second,
                               [](const auto& p) { return p.second; })) {
      result.emplace_hint(it_pair.second, link.first, link.second);
      result.emplace(link.second, link.first);
    }
  }

  return result;
}

auto compute_triplets(const std::span<const Link> links) {
  std::set<std::array<Computer, 3>> result;

  auto neighborhoods = compute_neighborhoods(links);

  auto computers =
      std::views::keys(neighborhoods) | std::views::chunk_by(std::equal_to<>{}) |
      std::views::transform([](const auto& chunk) { return *std::ranges::begin(chunk); });
  for (const Computer& computer : computers) {
    const auto n_it = neighborhoods.equal_range(computer);
    auto neighbors = std::views::values(std::ranges::subrange(n_it.first, n_it.second));
    for (const Computer& neighbor : neighbors) {
      const auto t_it = neighborhoods.equal_range(neighbor);
      auto third_parties = std::views::values(std::ranges::subrange(t_it.first, t_it.second)) |
                           std::views::filter([&](const Computer& third_party) {
                             return std::ranges::contains(neighbors, third_party);
                           });
      for (const Computer& third_party : third_parties) {
        std::array<Computer, 3> triplet{computer, neighbor, third_party};
        std::ranges::sort(triplet);
        result.insert(std::move(triplet));
      }
    }
  }

  return result;
}

auto solve_part1(const auto& input) {
  const auto triplets = compute_triplets(input);
  return std::ranges::count_if(triplets, [](const auto& triplet) {
    return std::ranges::any_of(triplet, [](const Computer& member) {
      return member.starts_with('t');
    });
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
