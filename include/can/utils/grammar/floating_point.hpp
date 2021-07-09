#ifndef INCLUDE_CAN_UTILS_GRAMMAR_FLOATING_POINT_HPP
#define INCLUDE_CAN_UTILS_GRAMMAR_FLOATING_POINT_HPP

#include <string>

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
 * This rule parses floating-point string without exponent value.
 */
struct floating_point : lexy::token_production {
    static constexpr auto rule_integer =
        lexy::dsl::identifier(lexy::dsl::ascii::digit / lexy::dsl::lit_c<'-'>, lexy::dsl::ascii::digit);
    static constexpr auto rule_dot      = lexy::dsl::opt(lexy::dsl::lit_c<'.'>);
    static constexpr auto rule_decimals = lexy::dsl::opt(lexy::dsl::digits<>);

    static constexpr auto rule = lexy::dsl::peek(lexy::dsl::lit_c<'-'> / lexy::dsl::digit<>) >>
                                 lexy::dsl::capture(rule_integer + rule_dot + rule_decimals);

    static constexpr auto value = lexy::callback<float>(
        [](auto c, auto /* u */) { return std::stof(std::string(c.begin(), c.end())); },
        [](auto c, auto /* u */, lexy::nullopt /* u */) { return std::stof(std::string(c.begin(), c.end())); },
        [](auto c, auto /* u */, lexy::nullopt /* u */, lexy::nullopt /* u */) {
            return std::stof(std::string(c.begin(), c.end()));
        });
};

} /* namespace can::utils::grammar */

#endif /* INCLUDE_CAN_UTILS_GRAMMAR_FLOATING_POINT_HPP */