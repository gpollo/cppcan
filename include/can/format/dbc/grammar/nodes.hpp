#ifndef INCLUDE_CAN_DATABASE_DBC_GRAMMAR_NODES_HPP
#define INCLUDE_CAN_DATABASE_DBC_GRAMMAR_NODES_HPP

#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"

#include "can/format/dbc/ast/nodes.hpp"
#include "can/utils/grammar/c_identifier.hpp"
#include "lexy/dsl/peek.hpp"

namespace can::format::dbc::grammar {

struct nodes {
    static constexpr auto rule_prefix = LEXY_LIT("BU_");

    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    static constexpr auto rule_colon = lexy::dsl::colon;

    struct values {
        static constexpr auto rule =
            lexy::dsl::list(lexy::dsl::p<can::utils::grammar::c_identifier>,
                            lexy::dsl::trailing_sep(lexy::dsl::while_one(lexy::dsl::ascii::blank)));

        static constexpr auto value = lexy::as_list<std::vector<std::string>>;
    };

    static constexpr auto rule_values = lexy::dsl::p<values>;

    static constexpr auto rule_eol = lexy::dsl::eol;

    static constexpr auto rule_content =
        rule_prefix + rule_whitespace + rule_colon + rule_whitespace + rule_values + rule_eol;

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >> rule_content;

    static constexpr auto value = lexy::construct<ast::nodes>;
};

} /* namespace can::format::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_GRAMMAR_NODES_HPP */