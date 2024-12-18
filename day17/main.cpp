#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iterator>
#include <print>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace std::string_view_literals;

enum class Register : std::uint8_t { a, b, c };

enum class Operator : std::uint8_t {
  adv,
  bxl,
  bst,
  jnz,
  bxc,
  out,
  bdv,
  cdv,
};

using RegisterValue = std::uint64_t;

using Int = std::uint8_t;

struct Input {  // NOLINT(cppcoreguidelines-pro-type-member-init)
  std::array<RegisterValue, 3> register_values;
  std::vector<Int> program;
};

auto parse_input(std::istream&& in) {
  Input result;

  const auto content =
      std::views::istream<char>(in >> std::noskipws) | std::ranges::to<std::vector>();
  auto sections = std::views::split(content, "\n\n"sv);
  auto section_it = sections.begin();

  {
    auto register_section = *section_it++;
    auto register_values =
        register_section | std::views::split('\n') | std::views::transform([](const auto& line) {
          const auto token = std::views::join(line | std::views::split(' ') | std::views::drop(2) |
                                              std::views::take(1)) |
                             std::ranges::to<std::string>();
          return std::stoul(token);
        });
    std::ranges::copy(register_values | std::views::take(3), begin(result.register_values));
  }

  {
    auto program_section = *section_it++;
    auto program = std::views::join(program_section | std::views::split(' ') | std::views::drop(1) |
                                    std::views::take(1)) |
                   std::views::lazy_split(',') | std::views::transform([](const auto& t) {
                     const auto token = t | std::ranges::to<std::string>();
                     return std::stoul(token);
                   });
    std::ranges::copy(program, std::back_inserter(result.program));
  }

  return result;
}

class Computer {
 public:
  auto output();

  Computer(std::span<const RegisterValue, 3> register_values, std::span<const Int> program)
      : register_values_{register_values[0], register_values[1], register_values[2]},
        program_{begin(program), end(program)} {}

 private:
  auto operand_value(Operator op, Int operand) const;
  auto combo_value(Int operand) const;
  static auto literal_value(Int operand) { return RegisterValue{operand}; }

  auto operator[](Register r) const;
  auto operator[](Register r) -> decltype(auto);

  std::array<RegisterValue, 3> register_values_;
  std::vector<Int> program_;
};

auto Computer::operator[](Register r) const { return register_values_[std::to_underlying(r)]; }

auto Computer::operator[](Register r) -> decltype(auto) {
  return register_values_[std::to_underlying(r)];
}

auto Computer::combo_value(Int operand) const {
  assert(operand < Int{7});
  switch (operand) {
    case Int{0}:
      return RegisterValue{0};
    case Int{1}:
      return RegisterValue{1};
    case Int{2}:
      return RegisterValue{2};
    case Int{3}:
      return RegisterValue{3};
    case Int{4}:
      return std::as_const(*this)[Register::a];
    case Int{5}:
      return std::as_const(*this)[Register::b];
    case Int{6}:
      return std::as_const(*this)[Register::c];
    default:
      std::unreachable();
  }
}

auto Computer::operand_value(Operator op, Int operand) const {
  switch (op) {
    case Operator::adv:
    case Operator::bst:
    case Operator::out:
    case Operator::bdv:
    case Operator::cdv:
      return combo_value(operand);
    case Operator::bxl:
    case Operator::jnz:
    case Operator::bxc:
      return literal_value(operand);
  }
  std::unreachable();
}

auto Computer::output() {
  std::vector<Int> result;

  using IpValue = decltype(program_)::size_type;
  IpValue ip{};

  while (ip < program_.size()) {
    assert(ip + IpValue{1} != program_.size());
    assert(program_[ip] <= Int{7});
    const auto op = static_cast<Operator>(program_[ip]);
    const auto value = operand_value(op, program_[ip + IpValue{1}]);

    switch (op) {
      case Operator::adv:
        (*this)[Register::a] >>= value;
        ip += IpValue{2};
        break;
      case Operator::bxl:
        (*this)[Register::b] ^= value;
        ip += IpValue{2};
        break;
      case Operator::bst:
        (*this)[Register::b] = value % RegisterValue{8};
        ip += IpValue{2};
        break;
      case Operator::jnz:
        if (std::as_const(*this)[Register::a] == RegisterValue{0}) {
          ip += IpValue{2};
        } else {
          ip = IpValue{value};
        }
        break;
      case Operator::bxc:
        (*this)[Register::b] ^= std::as_const(*this)[Register::c];
        ip += IpValue{2};
        break;
      case Operator::out:
        result.push_back(value % RegisterValue{8});
        ip += IpValue{2};
        break;
      case Operator::bdv:
        (*this)[Register::b] = std::as_const(*this)[Register::a] >> value;
        ip += IpValue{2};
        break;
      case Operator::cdv:
        (*this)[Register::c] = std::as_const(*this)[Register::a] >> value;
        ip += IpValue{2};
        break;
    }
  }

  return result;
}

auto solve_part1(const Input& input) {
  return std::views::join_with(
             std::views::transform(Computer{input.register_values, input.program}.output(),
                                   [](const Int i) { return std::to_string(i); }),
             ',') |
         std::ranges::to<std::string>();
}

auto solve_part2(const Input& input) { return 0; }

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
