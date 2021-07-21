#ifndef INCLUDE_CAN_DATABASE_DBC_GRAMMAR_ATTRIBUTE_DEFINITION_HPP
#define INCLUDE_CAN_DATABASE_DBC_GRAMMAR_ATTRIBUTE_DEFINITION_HPP

#include <optional>

#include "lexy/callback/base.hpp"
#include "lexy/callback/container.hpp"
#include "lexy/callback/integer.hpp"
#include "lexy/callback/object.hpp"
#include "lexy/dsl/branch.hpp"
#include "lexy/dsl/capture.hpp"
#include "lexy/dsl/delimited.hpp"
#include "lexy/dsl/integer.hpp"
#include "lexy/dsl/literal.hpp"
#include "lexy/dsl/option.hpp"
#include "lexy/dsl/peek.hpp"
#include "lexy/dsl/punctuator.hpp"
#include "lexy/dsl/sign.hpp"
#include "lexy/dsl/whitespace.hpp"

#include "can/format/dbc/ast/attribute_definition.hpp"
#include "can/utils/grammar/floating_point.hpp"

namespace can::format::dbc::grammar {

struct attribute_definition {
    static constexpr auto rule_prefix = LEXY_LIT("BA_DEF_");

    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    static constexpr auto rule_quote = lexy::dsl::peek(lexy::dsl::lit_c<'"'>);

    struct quoted {
        static constexpr auto rule_chars = lexy::dsl::ascii::alpha_digit_underscore / lexy::dsl::ascii::space;

        static constexpr auto rule = lexy::dsl::quoted(rule_chars);

        static constexpr auto value = lexy::as_string<std::string>;
    };

    struct object {
        using object_type = ast::attribute_definition::object_type;

        struct node {
            static constexpr auto rule = LEXY_LIT("BU_");

            static constexpr auto value = lexy::callback<object_type>([]() { return object_type::NODE; });
        };

        struct message {
            static constexpr auto rule = LEXY_LIT("BO_");

            static constexpr auto value = lexy::callback<object_type>([]() { return object_type::MESSAGE; });
        };

        struct signal {
            static constexpr auto rule = LEXY_LIT("SG_");

            static constexpr auto value = lexy::callback<object_type>([]() { return object_type::SIGNAL; });
        };

        static constexpr auto rule_choices = lexy::dsl::p<node> | lexy::dsl::p<message> | lexy::dsl::p<signal>;

        static constexpr auto rule = lexy::dsl::opt(rule_choices);

        static constexpr auto value =
            lexy::callback<object_type>([](object_type object_type) { return object_type; },
                                        [](lexy::nullopt /* n */) { return object_type::DATABASE; });
    };

    static constexpr auto rule_object = lexy::dsl::p<object>;

    static constexpr auto rule_name = lexy::dsl::p<quoted>;

    static constexpr auto rule_minus_or_digit = lexy::dsl::lit_c<'-'> / lexy::dsl::digit<>;

    struct data {
        struct integer {
            using integer_type = ast::attribute_definition::integer::type;

            /* FIXME: I have no idea what "HEX" does */
            static constexpr auto rule_prefix = LEXY_LIT("INT") | LEXY_LIT("HEX");

            static constexpr auto rule_int = lexy::dsl::integer<integer_type>(lexy::dsl::digits<>);

            static constexpr auto rule_min = rule_int;

            static constexpr auto rule_max = rule_int;

            static constexpr auto rule_config = rule_min + rule_whitespace + rule_max;

            static constexpr auto rule_config_opt = lexy::dsl::opt(lexy::dsl::peek(rule_minus_or_digit) >> rule_config);

            static constexpr auto rule_content = rule_prefix + rule_whitespace + rule_config_opt;

            static constexpr auto rule = lexy::dsl::peek(rule_prefix) >> rule_content;

            static constexpr auto value = lexy::callback<ast::attribute_definition::integer>(
                [](auto min, auto max) { return ast::attribute_definition::integer(min, max); },
                [](lexy::nullopt /* n */) { return ast::attribute_definition::integer(0, 0); });
        };

        struct floating_point {
            static constexpr auto rule_prefix = LEXY_LIT("FLOAT");

            static constexpr auto rule_float = lexy::dsl::p<utils::grammar::floating_point>;

            static constexpr auto rule_min = rule_float;

            static constexpr auto rule_max = rule_float;

            static constexpr auto rule_config = lexy::dsl::peek(rule_minus_or_digit) >>
                                                (rule_min + rule_whitespace + rule_max);

            static constexpr auto rule_config_opt = lexy::dsl::opt(rule_config);

            static constexpr auto rule_content = rule_prefix + rule_whitespace + rule_config_opt;

            static constexpr auto rule = lexy::dsl::peek(rule_prefix) >> rule_content;

            static constexpr auto value = lexy::callback<ast::attribute_definition::floating_point>(
                [](auto min, auto max) { return ast::attribute_definition::floating_point(min, max); },
                [](lexy::nullopt /* n */) { return ast::attribute_definition::floating_point(0.0F, 0.0F); });
        };

        struct string {
            static constexpr auto rule_prefix = LEXY_LIT("STRING");

            static constexpr auto rule_config = rule_quote >> lexy::dsl::p<quoted>;

            static constexpr auto rule_config_opt = lexy::dsl::opt(rule_config);

            static constexpr auto rule_content = rule_prefix + rule_whitespace + rule_config_opt;

            static constexpr auto rule = lexy::dsl::peek(rule_prefix) >> rule_content;

            static constexpr auto value = lexy::callback<ast::attribute_definition::string>(
                [](lexy::nullopt /* n */) { return ast::attribute_definition::string(""); },
                [](std::optional<std::string> s) { return ast::attribute_definition::string(s.value_or("")); });
        };

        struct enumeration {
            static constexpr auto rule_prefix = LEXY_LIT("ENUM");

            struct list {
                static constexpr auto rule_comma_sep = lexy::dsl::sep(lexy::dsl::comma);

                static constexpr auto rule = lexy::dsl::list(lexy::dsl::p<quoted>, rule_comma_sep);

                static constexpr auto value = lexy::as_list<std::vector<std::string>>;
            };

            static constexpr auto rule_config = rule_quote >> lexy::dsl::p<list>;

            static constexpr auto rule_config_opt = lexy::dsl::opt(rule_config);

            static constexpr auto rule_content = rule_prefix + rule_whitespace + rule_config_opt;

            static constexpr auto rule = lexy::dsl::peek(rule_prefix) >> rule_content;

            static constexpr auto value = lexy::callback<ast::attribute_definition::enumeration>(
                [](lexy::nullopt /* n */) {
                    return ast::attribute_definition::enumeration(std::vector<std::string>());
                },
                [](std::optional<std::vector<std::string>> v) {
                    return ast::attribute_definition::enumeration(v.value_or(std::vector<std::string>()));
                });
        };

        static constexpr auto rule =
            lexy::dsl::p<integer> | lexy::dsl::p<floating_point> | lexy::dsl::p<string> | lexy::dsl::p<enumeration>;

        static constexpr auto value = lexy::construct<ast::attribute_definition::data_types>;
    };

    static constexpr auto rule_data = lexy::dsl::p<data>;

    static constexpr auto rule_semicolon = lexy::dsl::semicolon;

    static constexpr auto rule_eol = lexy::dsl::eol;

    static constexpr auto rule_content = rule_prefix + rule_whitespace + rule_object + rule_whitespace + rule_name +
                                         rule_whitespace + rule_data + rule_whitespace + rule_semicolon +
                                         rule_whitespace + rule_eol;

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >> rule_content;

    static constexpr auto value = lexy::construct<ast::attribute_definition>;
};

} /* namespace can::format::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_GRAMMAR_ATTRIBUTE_DEFINITION_HPP */
