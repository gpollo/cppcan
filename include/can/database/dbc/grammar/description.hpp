#ifndef INCLUDE_CAN_DATABASE_DBC_GRAMMAR_DESCRIPTION_HPP
#define INCLUDE_CAN_DATABASE_DBC_GRAMMAR_DESCRIPTION_HPP

#include <variant>

#include "lexy/callback/string.hpp"
#include "lexy/dsl/ascii.hpp"
#include "lexy/dsl/branch.hpp"
#include "lexy/dsl/delimited.hpp"
#include "lexy/dsl/digit.hpp"
#include "lexy/dsl/integer.hpp"
#include "lexy/dsl/literal.hpp"
#include "lexy/dsl/newline.hpp"
#include "lexy/dsl/peek.hpp"
#include "lexy/dsl/production.hpp"
#include "lexy/dsl/whitespace.hpp"

#include "can/database/dbc/ast/database_description.hpp"
#include "can/database/dbc/ast/message_description.hpp"
#include "can/database/dbc/ast/node_description.hpp"
#include "can/database/dbc/ast/signal_description.hpp"
#include "can/utils/grammar/c_identifier.hpp"

namespace can::database::dbc::grammar {

struct description {
    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    struct desc {
        static constexpr auto rule_chars = lexy::dsl::ascii::alpha_digit_underscore / lexy::dsl::ascii::space;

        static constexpr auto rule = lexy::dsl::quoted(rule_chars);

        static constexpr auto value = lexy::as_string<std::string>;
    };

    static constexpr auto rule_desc = lexy::dsl::p<desc>;

    struct node {
        static constexpr auto rule_prefix = LEXY_LIT("BU_");

        static constexpr auto rule_node = lexy::dsl::p<utils::grammar::c_identifier>;

        static constexpr auto rule = lexy::dsl::peek(rule_prefix) >>
                                     (rule_prefix + rule_whitespace + rule_node + rule_whitespace + rule_desc);

        static constexpr auto value = lexy::construct<ast::node_description>;
    };

    struct message {
        static constexpr auto rule_prefix = LEXY_LIT("BO_");

        static constexpr auto rule_message = lexy::dsl::integer<unsigned int>(lexy::dsl::digits<>);

        static constexpr auto rule = lexy::dsl::peek(rule_prefix) >>
                                     (rule_prefix + rule_whitespace + rule_message + rule_whitespace + rule_desc);

        static constexpr auto value = lexy::construct<ast::message_description>;
    };

    struct signal {
        static constexpr auto rule_prefix = LEXY_LIT("SG_");

        static constexpr auto rule_message = lexy::dsl::integer<unsigned int>(lexy::dsl::digits<>);

        static constexpr auto rule_signal = lexy::dsl::p<utils::grammar::c_identifier>;

        static constexpr auto rule = lexy::dsl::peek(rule_prefix) >>
                                     (rule_prefix + rule_whitespace + rule_message + rule_whitespace + rule_signal +
                                      rule_whitespace + rule_desc);

        static constexpr auto value = lexy::construct<ast::signal_description>;
    };

    struct database {
        static constexpr auto rule = rule_desc;

        static constexpr auto value = lexy::construct<ast::database_description>;
    };

    static constexpr auto rule_prefix = LEXY_LIT("CM_");

    static constexpr auto rule_semicolon = lexy::dsl::lit_c<';'>;

    static constexpr auto rule_eol = lexy::dsl::eol;

    static constexpr auto rule_content = rule_prefix + rule_whitespace +
                                         (lexy::dsl::p<node> | lexy::dsl::p<message> | lexy::dsl::p<signal> |
                                          lexy::dsl::else_ >> lexy::dsl::p<database>)+rule_whitespace +
                                         rule_semicolon + rule_whitespace + rule_eol;

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >> rule_content;

    static constexpr auto value = lexy::forward<std::variant<ast::node_description, ast::message_description,
                                                             ast::signal_description, ast::database_description>>;
};

} /* namespace can::database::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_GRAMMAR_DESCRIPTION_HPP */
