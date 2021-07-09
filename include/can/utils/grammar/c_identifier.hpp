#ifndef INCLUDE_CAN_UTILS_GRAMMAR_C_IDENTIFIER_HPP
#define INCLUDE_CAN_UTILS_GRAMMAR_C_IDENTIFIER_HPP

#include <string>

#include "lexy/callback.hpp"
#include "lexy/dsl/ascii.hpp"
#include "lexy/dsl/identifier.hpp"
#include "lexy/dsl/literal.hpp"
#include "lexy/dsl/production.hpp"

namespace can::utils::grammar {

struct c_identifier : lexy::transparent_production {
    static constexpr auto rule_lead_char = lexy::dsl::ascii::alpha / lexy::dsl::lit_c<'_'>;

    static constexpr auto rule_trailing_char = lexy::dsl::ascii::alnum / lexy::dsl::lit_c<'_'>;

    static constexpr auto rule = lexy::dsl::identifier(rule_lead_char, rule_trailing_char);

    static constexpr auto value = lexy::as_string<std::string>;
};

} /* namespace can::utils::grammar */

#endif /* INCLUDE_CAN_UTILS_GRAMMAR_C_IDENTIFIER_HPP */