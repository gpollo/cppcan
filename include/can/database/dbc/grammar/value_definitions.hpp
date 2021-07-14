#ifndef INCLUDE_CAN_DATABASE_DBC_GRAMMAR_VALUE_DEFINITIONS_HPP
#define INCLUDE_CAN_DATABASE_DBC_GRAMMAR_VALUE_DEFINITIONS_HPP

#include "can/database/dbc/ast/value_definitions.hpp"
#include "can/database/dbc/grammar/value_table_definition.hpp"

namespace can::database::dbc::grammar {

struct value_definitions {
    static constexpr auto rule_prefix = LEXY_LIT("VAL_");

    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    static constexpr auto rule_message = lexy::dsl::integer<unsigned int>(lexy::dsl::digits<>);

    static constexpr auto rule_signal = lexy::dsl::p<utils::grammar::c_identifier>;

    struct content {
        static constexpr auto rule_entries = value_table_definition::rule_entries;

        static constexpr auto rule_table = lexy::dsl::p<utils::grammar::c_identifier>;

        static constexpr auto rule =
            lexy::dsl::peek(lexy::dsl::digit<>) >> rule_entries | lexy::dsl::else_ >> rule_table;

        static constexpr auto value = lexy::construct<std::variant<ast::value_definitions::entries, std::string>>;
    };

    static constexpr auto rule_content = lexy::dsl::p<content>;

    static constexpr auto rule_semicolon = lexy::dsl::semicolon;

    static constexpr auto rule_eol = lexy::dsl::eol;

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >>
                                 (rule_prefix + rule_whitespace + rule_message + rule_whitespace + rule_signal +
                                  rule_whitespace + rule_content + rule_whitespace + rule_semicolon + rule_whitespace +
                                  rule_eol);

    static constexpr auto value = lexy::construct<ast::value_definitions>;
};

} /* namespace can::database::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_GRAMMAR_VALUE_DEFINITIONS_HPP */