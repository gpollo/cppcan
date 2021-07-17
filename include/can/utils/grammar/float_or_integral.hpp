#ifndef INCLUDE_CAN_UTILS_GRAMMAR_FLOAT_OR_INTEGRAL_HPP
#define INCLUDE_CAN_UTILS_GRAMMAR_FLOAT_OR_INTEGRAL_HPP

#include <string>
#include <variant>

#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"
#include "lexy/dsl/ascii.hpp"
#include "lexy/dsl/capture.hpp"
#include "lexy/dsl/digit.hpp"
#include "lexy/dsl/identifier.hpp"
#include "lexy/dsl/integer.hpp"
#include "lexy/dsl/literal.hpp"
#include "lexy/dsl/loop.hpp"
#include "lexy/dsl/option.hpp"
#include "lexy/dsl/production.hpp"
#include "lexy/dsl/punctuator.hpp"
#include "lexy/dsl/sign.hpp"

namespace can::utils::grammar {

/*
 * This rule parses floating-point or an integral string.
 */
struct float_or_integral : lexy::token_production {
    static constexpr auto rule_integer =
        lexy::dsl::identifier(lexy::dsl::ascii::digit / lexy::dsl::lit_c<'-'>, lexy::dsl::ascii::digit);
    static constexpr auto rule_dot      = lexy::dsl::opt(lexy::dsl::lit_c<'.'>);
    static constexpr auto rule_decimals = lexy::dsl::opt(lexy::dsl::digits<>);

    static constexpr auto rule = lexy::dsl::peek(lexy::dsl::lit_c<'-'> / lexy::dsl::digit<>) >>
                                 lexy::dsl::capture(rule_integer + rule_dot + rule_decimals);

    using result = std::variant<float, uint64_t, int64_t>;

    static constexpr auto value = lexy::callback<result>(
        [](auto c, auto /* u */) {
            auto input = std::string(c.begin(), c.end());
            return result((float)std::stof(input));
        },
        [](auto c, auto /* u */, lexy::nullopt /* u */) {
            auto input = std::string(c.begin(), c.end());
            if (c[0] != '-') {
                return result((uint64_t)std::stoi(input));
            }
            return result((int64_t)std::stoi(input));
        },
        [](auto c, auto /* u */, lexy::nullopt /* u */, lexy::nullopt /* u */) {
            auto input = std::string(c.begin(), c.end());
            if (c[0] != '-') {
                return result((uint64_t)std::stoi(input));
            }
            return result((int64_t)std::stoi(input));
        });
};

} /* namespace can::utils::grammar */

#endif /* INCLUDE_CAN_UTILS_GRAMMAR_FLOAT_OR_INTEGRAL_HPP */