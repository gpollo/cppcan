#ifndef INCLUDE_CAN_DATABASE_DBC_GRAMMAR_MESSAGE_HPP
#define INCLUDE_CAN_DATABASE_DBC_GRAMMAR_MESSAGE_HPP

#include "lexy/callback/base.hpp"
#include "lexy/callback/object.hpp"

#include "can/database/dbc/ast/message.hpp"
#include "can/database/dbc/grammar/signal.hpp"
#include "lexy/dsl/peek.hpp"

namespace can::database::dbc::grammar {

struct message {
    static constexpr auto rule_prefix = LEXY_LIT("BO_");

    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    static constexpr auto rule_identifier = lexy::dsl::integer<unsigned int>(lexy::dsl::digits<>);

    static constexpr auto rule_name = lexy::dsl::p<can::utils::grammar::c_identifier>;

    static constexpr auto rule_colon = lexy::dsl::colon;

    static constexpr auto rule_byte_count = lexy::dsl::integer<unsigned short>(lexy::dsl::digits<>);

    static constexpr auto rule_node = lexy::dsl::p<can::utils::grammar::c_identifier>;

    static constexpr auto rule_eol = lexy::dsl::eol;

    struct signals {
        static constexpr auto rule = lexy::dsl::list(lexy::dsl::p<signal>);

        static constexpr auto value = lexy::as_list<std::vector<ast::signal>>;
    };

    static constexpr auto rule_signals = lexy::dsl::opt(lexy::dsl::p<signals>);

    static constexpr auto rule_content = rule_prefix + rule_whitespace + rule_identifier + rule_whitespace + rule_name +
                                         rule_whitespace + rule_colon + rule_whitespace + rule_byte_count +
                                         rule_whitespace + rule_node + rule_eol + rule_signals;

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >> rule_content;

    static constexpr auto value = lexy::construct<ast::message>;
};

} /* namespace can::database::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_GRAMMAR_MESSAGE_HPP */