#ifndef INCLUDE_CAN_DATABASE_DBC_REQUIREMENTS_HPP
#define INCLUDE_CAN_DATABASE_DBC_REQUIREMENTS_HPP

#include "lexy/callback.hpp"
#include "lexy/callback/string.hpp"
#include "lexy/dsl/ascii.hpp"
#include "lexy/dsl/capture.hpp"
#include "lexy/dsl/identifier.hpp"
#include "lexy/dsl/list.hpp"
#include "lexy/dsl/literal.hpp"
#include "lexy/dsl/loop.hpp"
#include "lexy/dsl/newline.hpp"
#include "lexy/dsl/peek.hpp"
#include "lexy/dsl/production.hpp"
#include "lexy/dsl/punctuator.hpp"
#include "lexy/dsl/whitespace.hpp"

#include "can/database/dbc/ast/requirements.hpp"

namespace can::database::dbc::grammar {

struct requirements {
    static constexpr auto rule_prefix = LEXY_LIT("NS_");

    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    static constexpr auto rule_colon = lexy::dsl::colon;

    static constexpr auto rule_eol = lexy::dsl::eol;

    static constexpr auto rule_first_line = rule_prefix + rule_whitespace + rule_colon + rule_whitespace + rule_eol;

    struct rule_identifier {
        static constexpr auto rule_lead_char = lexy::dsl::ascii::upper;

        static constexpr auto rule_trailing_char = lexy::dsl::ascii::upper / lexy::dsl::lit_c<'_'>;

        static constexpr auto rule_name = lexy::dsl::identifier(rule_lead_char, rule_trailing_char);

        static constexpr auto rule = lexy::dsl::ascii::blank >> (rule_whitespace + rule_name + rule_whitespace);

        static constexpr auto value = lexy::as_string<std::string>;
    };

    static constexpr auto rule_trailing_sep = lexy::dsl::trailing_sep(rule_eol);

    static constexpr auto rule_identifiers = lexy::dsl::list(lexy::dsl::p<rule_identifier>, rule_trailing_sep);

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >> (rule_first_line + rule_identifiers);

    static constexpr auto value = lexy::as_list<ast::requirements>;
};

} /* namespace can::database::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_REQUIREMENTS_HPP */