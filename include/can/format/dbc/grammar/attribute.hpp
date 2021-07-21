#ifndef INCLUDE_CAN_DATABASE_DBC_GRAMMAR_ATTRIBUTE_HPP
#define INCLUDE_CAN_DATABASE_DBC_GRAMMAR_ATTRIBUTE_HPP

#include <variant>

#include "lexy/callback/base.hpp"
#include "lexy/callback/object.hpp"
#include "lexy/callback/string.hpp"
#include "lexy/dsl/ascii.hpp"
#include "lexy/dsl/branch.hpp"
#include "lexy/dsl/capture.hpp"
#include "lexy/dsl/delimited.hpp"
#include "lexy/dsl/digit.hpp"
#include "lexy/dsl/integer.hpp"
#include "lexy/dsl/literal.hpp"
#include "lexy/dsl/loop.hpp"
#include "lexy/dsl/newline.hpp"
#include "lexy/dsl/option.hpp"
#include "lexy/dsl/peek.hpp"
#include "lexy/dsl/production.hpp"
#include "lexy/dsl/punctuator.hpp"
#include "lexy/dsl/sign.hpp"
#include "lexy/dsl/whitespace.hpp"

#include "can/format/dbc/ast/database_attribute.hpp"
#include "can/format/dbc/ast/message_attribute.hpp"
#include "can/format/dbc/ast/node_attribute.hpp"
#include "can/format/dbc/ast/signal_attribute.hpp"
#include "can/utils/grammar/c_identifier.hpp"

namespace can::format::dbc::grammar {

struct attribute {
    static constexpr auto rule_prefix = LEXY_LIT("BA_");

    struct quoted {
        static constexpr auto rule_chars = lexy::dsl::ascii::alpha_digit_underscore / lexy::dsl::ascii::space;

        static constexpr auto rule = lexy::dsl::quoted(rule_chars);

        static constexpr auto value = lexy::as_string<std::string>;
    };

    static constexpr auto rule_name = lexy::dsl::p<quoted>;

    struct text {
        struct number {
            static constexpr auto rule_sign_or_digit = lexy::dsl::ascii::digit / lexy::dsl::lit_c<'-'>;

            static constexpr auto rule_whole = rule_sign_or_digit + lexy::dsl::while_(lexy::dsl::ascii::digit);

            static constexpr auto rule_dot = lexy::dsl::lit_c<'.'>;

            static constexpr auto rule_fraction = lexy::dsl::peek(rule_dot) >>
                                                  (rule_dot + lexy::dsl::while_(lexy::dsl::ascii::digit));

            static constexpr auto rule_fraction_opt = lexy::dsl::if_(rule_fraction);

            static constexpr auto rule_content = lexy::dsl::capture(rule_whole + rule_fraction_opt);

            static constexpr auto rule = lexy::dsl::peek(rule_sign_or_digit) >> rule_content;

            static constexpr auto value = lexy::as_string<std::string>;
        };

        struct string {
            static constexpr auto rule_quote = lexy::dsl::lit_c<'"'>;

            static constexpr auto rule = lexy::dsl::peek(rule_quote) >> lexy::dsl::p<quoted>;

            static constexpr auto value = lexy::as_string<std::string>;
        };

        static constexpr auto rule = lexy::dsl::p<number> | lexy::dsl::p<string>;

        static constexpr auto value = lexy::as_string<std::string>;
    };

    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    struct node {
        static constexpr auto rule_prefix = LEXY_LIT("BU_");

        static constexpr auto rule_node = lexy::dsl::p<utils::grammar::c_identifier>;

        static constexpr auto rule = lexy::dsl::peek(rule_prefix) >> (rule_prefix + rule_whitespace + rule_node);
    };

    struct message {
        static constexpr auto rule_prefix = LEXY_LIT("BO_");

        static constexpr auto rule_message = lexy::dsl::integer<unsigned int>(lexy::dsl::digits<>);

        static constexpr auto rule = lexy::dsl::peek(rule_prefix) >>
                                     (rule_prefix + rule_whitespace + rule_message + rule_whitespace);
    };

    struct signal {
        static constexpr auto rule_prefix = LEXY_LIT("SG_");

        static constexpr auto rule_message = lexy::dsl::integer<unsigned int>(lexy::dsl::digits<>);

        static constexpr auto rule_signal = lexy::dsl::p<utils::grammar::c_identifier>;

        static constexpr auto rule = lexy::dsl::peek(rule_prefix) >>
                                     (rule_prefix + rule_whitespace + rule_message + rule_whitespace + rule_signal);
    };

    static constexpr auto rule_choices = lexy::dsl::if_(node::rule | message::rule | signal::rule);

    static constexpr auto rule_text = lexy::dsl::p<text>;

    static constexpr auto rule_semicolon = lexy::dsl::semicolon;

    static constexpr auto rule_eol = lexy::dsl::eol;

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >>
                                 (rule_prefix + rule_whitespace + rule_name + rule_whitespace + rule_choices +
                                  rule_whitespace + rule_text + rule_whitespace + rule_semicolon + rule_whitespace +
                                  rule_eol);

    using node_attr   = ast::node_attribute;
    using msg_attr    = ast::message_attribute;
    using sig_attr    = ast::signal_attribute;
    using db_attr     = ast::database_attribute;
    using output_type = std::variant<node_attr, msg_attr, sig_attr, db_attr>;

    static constexpr auto value = lexy::callback<output_type>(
        [](std::string name, std::string text) { return db_attr(name, text); },
        [](std::string name, std::string node, std::string text) { return node_attr(name, node, text); },
        [](std::string name, unsigned int message, std::string text) { return msg_attr(name, message, text); },
        [](std::string name, unsigned int message, std::string signal, std::string text) {
            return sig_attr(name, message, signal, text);
        });
};

} /* namespace can::format::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_GRAMMAR_ATTRIBUTE_HPP */