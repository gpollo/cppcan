#ifndef INCLUDE_CAN_DATABASE_DBC_SPEED_HPP
#define INCLUDE_CAN_DATABASE_DBC_SPEED_HPP

#include "lexy/callback/integer.hpp"
#include "lexy/dsl.hpp"
#include "lexy/dsl/ascii.hpp"
#include "lexy/dsl/integer.hpp"
#include "lexy/dsl/literal.hpp"
#include "lexy/dsl/option.hpp"

#include "can/database/dbc/ast/speed.hpp"
#include "lexy/dsl/peek.hpp"

namespace can::database::dbc::grammar {

struct speed {
    static constexpr auto rule_prefix = LEXY_LIT("BS_");

    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    static constexpr auto rule_colon = lexy::dsl::colon;

    static constexpr auto rule_speed = lexy::dsl::opt(lexy::dsl::integer<unsigned int>(lexy::dsl::digits<>));

    static constexpr auto rule_eol = lexy::dsl::eol;

    static constexpr auto rule_content =
        rule_prefix + rule_whitespace + rule_colon + rule_whitespace + rule_speed + rule_eol;

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >> rule_content;

    static constexpr auto value = lexy::as_integer<ast::speed>;
};

} /* namespace can::database::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_SPEED_HPP */