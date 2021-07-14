#ifndef INCLUDE_CAN_DATABASE_DBC_VERSION_HPP
#define INCLUDE_CAN_DATABASE_DBC_VERSION_HPP

#include "lexy/callback/forward.hpp"
#include "lexy/dsl.hpp"
#include "lexy/dsl/ascii.hpp"
#include "lexy/dsl/capture.hpp"
#include "lexy/dsl/delimited.hpp"
#include "lexy/dsl/literal.hpp"

#include "can/database/dbc/ast/version.hpp"

namespace can::database::dbc::grammar {

struct version {
    static constexpr auto rule_prefix = LEXY_LIT("VERSION");

    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    static constexpr auto rule_version =
        lexy::dsl::lit_c<'"'> +
        lexy::dsl::capture(lexy::dsl::while_(lexy::dsl::ascii::alpha_digit_underscore / lexy::dsl::ascii::blank)) +
        lexy::dsl::lit_c<'"'>;

    static constexpr auto rule_eol = lexy::dsl::eol;

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >>
                                 (rule_prefix + rule_whitespace + rule_version + rule_whitespace + rule_eol);

    static constexpr auto value = lexy::forward<ast::version>;
};

} /* namespace can::database::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_VERSION_HPP */