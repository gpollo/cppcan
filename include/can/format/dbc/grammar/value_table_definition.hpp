#ifndef INCLUDE_CAN_DATABASE_DBC_GRAMMAR_VALUE_TABLE_DEFINITION_HPP
#define INCLUDE_CAN_DATABASE_DBC_GRAMMAR_VALUE_TABLE_DEFINITION_HPP

#include <string>

#include "lexy/callback/integer.hpp"
#include "lexy/callback/object.hpp"
#include "lexy/callback/string.hpp"
#include "lexy/dsl/ascii.hpp"
#include "lexy/dsl/capture.hpp"
#include "lexy/dsl/delimited.hpp"
#include "lexy/dsl/integer.hpp"
#include "lexy/dsl/list.hpp"
#include "lexy/dsl/literal.hpp"
#include "lexy/dsl/loop.hpp"
#include "lexy/dsl/newline.hpp"
#include "lexy/dsl/peek.hpp"
#include "lexy/dsl/production.hpp"
#include "lexy/dsl/punctuator.hpp"
#include "lexy/dsl/separator.hpp"
#include "lexy/dsl/whitespace.hpp"

#include "can/format/dbc/ast/value_table_definition.hpp"
#include "can/utils/grammar/c_identifier.hpp"

namespace can::format::dbc::grammar {

struct value_table_definition {
    static constexpr auto rule_prefix = LEXY_LIT("VAL_TABLE_");

    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    static constexpr auto rule_name = lexy::dsl::p<utils::grammar::c_identifier>;

    struct entry {
        static constexpr auto rule_number = lexy::dsl::integer<unsigned int>(lexy::dsl::digits<>);

        struct quoted {
            static constexpr auto rule_chars = lexy::dsl::ascii::alpha_digit_underscore / lexy::dsl::ascii::space;

            static constexpr auto rule = lexy::dsl::quoted(rule_chars);

            static constexpr auto value = lexy::as_string<std::string>;
        };

        static constexpr auto rule_quoted = lexy::dsl::p<quoted>;

        static constexpr auto rule = lexy::dsl::peek(lexy::dsl::digit<>) >>
                                     (rule_number + rule_whitespace + rule_quoted);

        static constexpr auto value = lexy::construct<ast::value_table_definition::entry>;
    };

    static constexpr auto rule_entry = lexy::dsl::p<entry>;

    struct entries {
        static constexpr auto rule =
            lexy::dsl::list(rule_entry, lexy::dsl::sep(lexy::dsl::while_one(lexy::dsl::lit_c<' '>)));

        static constexpr auto value = lexy::as_list<ast::value_table_definition::entries>;
    };

    static constexpr auto rule_entries = lexy::dsl::p<entries>;

    static constexpr auto rule_semicolon = lexy::dsl::semicolon;

    static constexpr auto rule_eol = lexy::dsl::eol;

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >>
                                 (rule_prefix + rule_whitespace + rule_name + rule_whitespace + rule_entries +
                                  rule_whitespace + rule_semicolon + rule_whitespace + rule_eol);

    static constexpr auto value = lexy::construct<ast::value_table_definition>;
};

} /* namespace can::format::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_GRAMMAR_VALUE_TABLE_DEFINITION_HPP */