#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iterator>
#include <optional>
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
  auto output() -> std::vector<Int>;
  auto next_output() -> std::optional<Int>;

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

  using IpValue = decltype(program_)::size_type;
  IpValue ip_{};
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

auto Computer::next_output() -> std::optional<Int> {
  while (ip_ < program_.size()) {
    assert(ip_ + IpValue{1} != program_.size());
    assert(program_[ip_] <= Int{7});
    const auto op = static_cast<Operator>(program_[ip_]);
    const auto value = operand_value(op, program_[ip_ + IpValue{1}]);

    switch (op) {
      case Operator::adv:
        (*this)[Register::a] >>= value;
        ip_ += IpValue{2};
        break;
      case Operator::bxl:
        (*this)[Register::b] ^= value;
        ip_ += IpValue{2};
        break;
      case Operator::bst:
        (*this)[Register::b] = value % RegisterValue{8};
        ip_ += IpValue{2};
        break;
      case Operator::jnz:
        if (std::as_const(*this)[Register::a] == RegisterValue{0}) {
          ip_ += IpValue{2};
        } else {
          ip_ = IpValue{value};
        }
        break;
      case Operator::bxc:
        (*this)[Register::b] ^= std::as_const(*this)[Register::c];
        ip_ += IpValue{2};
        break;
      case Operator::out: {
        const Int result = value % RegisterValue{8};
        ip_ += IpValue{2};
        return {result};
      }
      case Operator::bdv:
        (*this)[Register::b] = std::as_const(*this)[Register::a] >> value;
        ip_ += IpValue{2};
        break;
      case Operator::cdv:
        (*this)[Register::c] = std::as_const(*this)[Register::a] >> value;
        ip_ += IpValue{2};
        break;
    }
  }

  return std::nullopt;
}

auto Computer::output() -> std::vector<Int> {
  std::vector<Int> result;

  for (auto maybe_output = next_output(); maybe_output.has_value(); maybe_output = next_output()) {
    result.push_back(*maybe_output);
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

class ComputerOutput {
 public:
  explicit ComputerOutput(Computer& computer) : computer_(&computer) { ++(*this); }

  auto operator++() -> ComputerOutput& {
    output_ = computer_->next_output();
    return *this;
  }
  auto operator++(int) {
    const auto result = *this;
    ++(*this);
    return result;
  }
  auto operator*() { return *output_; }
  auto operator==(const std::default_sentinel_t /*sentinel*/) const {
    return !output_.has_value();
  };
  auto operator!=(const std::default_sentinel_t sentinel) const { return !(*this == sentinel); }

 private:
  Computer* computer_;
  std::optional<Int> output_;
};

template <>
constexpr bool std::disable_sized_sentinel_for<ComputerOutput, std::default_sentinel_t> = true;

auto solve_part2(const Input& input) {
  return *std::ranges::find_if(std::views::iota(RegisterValue{0}), [&input](const RegisterValue v) {
    auto register_values = input.register_values;
    register_values[std::to_underlying(Register::a)] = v;
    Computer computer{register_values, input.program};

    ComputerOutput it{computer};
    auto jt{begin(input.program)};
    while (it != std::default_sentinel && jt != end(input.program)) {
      if (*it != *jt) {
        return false;
      }
      ++it;
      ++jt;
    }
    return it == std::default_sentinel && jt == end(input.program);
  });
}

auto main() -> int {
  const auto input = parse_input(std::ifstream{"input.txt"});
  const auto answer1 = solve_part1(input);
  std::println("The answer to part #1 is {}", answer1);
  const auto answer2 = solve_part2(input);
  std::println("The answer to part #2 is {}", answer2);
}
