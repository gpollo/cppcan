#ifndef INCLUDE_CAN_DATABASE_DBC_GRAMMAR_DATABASE_HPP
#define INCLUDE_CAN_DATABASE_DBC_GRAMMAR_DATABASE_HPP

#include "lexy/callback/bind.hpp"
#include "lexy/callback/integer.hpp"
#include "lexy/dsl/ascii.hpp"
#include "lexy/dsl/branch.hpp"
#include "lexy/dsl/eof.hpp"
#include "lexy/dsl/integer.hpp"
#include "lexy/dsl/list.hpp"
#include "lexy/dsl/newline.hpp"
#include "lexy/dsl/whitespace.hpp"

#include "can/format/dbc/ast/database.hpp"
#include "can/format/dbc/grammar/attribute.hpp"
#include "can/format/dbc/grammar/attribute_definition.hpp"
#include "can/format/dbc/grammar/attribute_definition_default.hpp"
#include "can/format/dbc/grammar/description.hpp"
#include "can/format/dbc/grammar/message.hpp"
#include "can/format/dbc/grammar/nodes.hpp"
#include "can/format/dbc/grammar/requirements.hpp"
#include "can/format/dbc/grammar/speed.hpp"
#include "can/format/dbc/grammar/value_definitions.hpp"
#include "can/format/dbc/grammar/value_table_definition.hpp"
#include "can/format/dbc/grammar/version.hpp"

namespace can::format::dbc::grammar {

struct database {
    static constexpr auto rule = lexy::dsl::list(
        lexy::dsl::p<attribute_definition_default> | lexy::dsl::p<attribute_definition> | lexy::dsl::p<attribute> |
        lexy::dsl::p<description> | lexy::dsl::p<message> | lexy::dsl::p<nodes> | lexy::dsl::p<requirements> |
        lexy::dsl::p<speed> | lexy::dsl::p<value_table_definition> | lexy::dsl::p<value_definitions> |
        lexy::dsl::p<version> | lexy::dsl::newline | lexy::dsl::ascii::blank) /* FIXME: + lexy::dsl::eof */;

    static constexpr auto value = lexy::fold<ast::database*>(nullptr, [](ast::database* database, auto value) {
        if (database == nullptr) {
            database = new ast::database();
        }

        database->add(value);

        return database;
    });
};

} /* namespace can::format::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_GRAMMAR_DATABASE_HPP */