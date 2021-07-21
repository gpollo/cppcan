#ifndef INCLUDE_CAN_DATABASE_DBC_GRAMMAR_ATTRIBUTE_DEFINITION_DEFAULT_HPP
#define INCLUDE_CAN_DATABASE_DBC_GRAMMAR_ATTRIBUTE_DEFINITION_DEFAULT_HPP

#include "can/format/dbc/ast/attribute_definition_default.hpp"
#include "can/format/dbc/grammar/attribute.hpp"

namespace can::format::dbc::grammar {

struct attribute_definition_default {
    static constexpr auto rule_prefix = LEXY_LIT("BA_DEF_DEF_");

    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    struct name {
        static constexpr auto rule_chars = lexy::dsl::ascii::alpha_digit_underscore / lexy::dsl::ascii::space;

        static constexpr auto rule = lexy::dsl::quoted(rule_chars);

        static constexpr auto value = lexy::as_string<std::string>;
    };

    static constexpr auto rule_name = lexy::dsl::p<name>;

    static constexpr auto rule_text = lexy::dsl::p<attribute::text>;

    static constexpr auto rule_semicolon = lexy::dsl::semicolon;

    static constexpr auto rule_eol = lexy::dsl::eol;

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >>
                                 (rule_prefix + rule_whitespace + rule_name + rule_whitespace + rule_text +
                                  rule_whitespace + rule_semicolon + rule_whitespace + rule_eol);

    static constexpr auto value = lexy::construct<ast::attribute_definition_default>;
};

} /* namespace can::format::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_GRAMMAR_ATTRIBUTE_DEFINITION_DEFAULT_HPP */