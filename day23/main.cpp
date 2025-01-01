#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <functional>
#include <ios>
#include <istream>
#include <iterator>
#include <map>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <utility>
#include <vector>

using Computer = std::string;
using Link = std::array<Computer, 2>;

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

using Neighborhoods = std::multimap<Computer, Computer>;

auto compute_neighborhoods(const std::span<const Link> links) {
  Neighborhoods result;

  for (const Link& link : links) {
    const auto it_pair = result.equal_range(link[0]);
    if (!std::ranges::contains(it_pair.first, it_pair.second, link[1],
                               [](const auto& p) { return p.second; })) {
      result.emplace_hint(it_pair.second, link[0], link[1]);
      result.emplace(link[1], link[0]);
    }
  }

  return result;
}

auto get_neighbors(const Neighborhoods& neighborhoods, const Computer& computer) {
  const auto n_it = neighborhoods.equal_range(computer);
  return std::views::values(std::ranges::subrange(n_it.first, n_it.second));
}

using Party = std::vector<Computer>;  // Sorted

auto compute_larger_parties(const Neighborhoods& neighborhoods,
                            const std::ranges::range auto& parties) {
  std::set<Party> result;

  for (const Party& party : parties) {
    auto candidates = get_neighbors(neighborhoods, party.front()) |
                      std::views::filter([&party](const auto& candidate) {
                        return !std::ranges::binary_search(party, candidate);
                      }) |
                      std::ranges::to<std::set>();
    for (const Computer& other_computer : std::views::drop(party, 1)) {
      auto other_candidates =
          get_neighbors(neighborhoods, other_computer) | std::ranges::to<std::set>();
      decltype(candidates) new_candidates;
      std::ranges::set_intersection(candidates, other_candidates,
                                    std::inserter(new_candidates, new_candidates.begin()));
      candidates = std::move(new_candidates);
    }
    for (const Computer& laureate : candidates) {
      Party larger_party;
      larger_party.reserve(party.size() + 1UZ);

      const auto it = std::ranges::lower_bound(party, laureate);
      larger_party.assign(party.begin(), it);
      larger_party.push_back(laureate);
      larger_party.insert(larger_party.end(), it, party.end());

      result.insert(std::move(larger_party));
    }
  }

  return result;
}

auto compute_pairs(const Neighborhoods& neighborhoods) {
  std::set<Party> result;

  auto computers =
      std::views::keys(neighborhoods) | std::views::chunk_by(std::equal_to<>{}) |
      std::views::transform([](const auto& chunk) { return *std::ranges::begin(chunk); });

  for (const Computer& computer : computers) {
    auto neighbors = get_neighbors(neighborhoods, computer);
    for (const Computer& neighbor : neighbors) {
      std::vector pair{computer, neighbor};
      if (pair[0] > pair[1]) {
        std::swap(pair[0], pair[1]);
      }
      result.insert(std::move(pair));
    }
  }

  return result;
}
auto compute_triplets(const std::span<const Link> links) {
  const auto neighborhoods = compute_neighborhoods(links);
  const auto pairs = compute_pairs(neighborhoods);
  return compute_larger_parties(neighborhoods, pairs);
}

auto solve_part1(const auto& input) {
  const auto triplets = compute_triplets(input);
  return std::ranges::count_if(triplets, [](const auto& triplet) {
    return std::ranges::any_of(triplet,
                               [](const Computer& member) { return member.starts_with('t'); });
  });
}

auto solve_part2(const auto& input) {
  const auto neighborhoods = compute_neighborhoods(input);

  auto parties = compute_pairs(neighborhoods);
  while (parties.size() > 1UZ) {
    parties = compute_larger_parties(neighborhoods, parties);
  }

  assert(parties.size() == 1UZ);
  return std::views::join_with(*parties.begin(), ',') | std::ranges::to<std::string>();
}

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
