#ifndef INCLUDE_CAN_DATABASE_DBC_AST_VALUE_DEFINITIONS_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_VALUE_DEFINITIONS_HPP

#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "can/database/dbc/ast/value_table_definition.hpp"

namespace can::database::dbc::ast {

class value_definitions {
   public:
    using entries = value_table_definition::entries;

    value_definitions(unsigned int message, std::string signal, std::variant<entries, std::string> table);

    unsigned int get_message() const;
    const std::string& get_signal() const;
    const std::variant<entries, std::string>& get_table() const;

   private:
    unsigned int message_;
    std::string signal_;
    std::variant<entries, std::string> table_;
};

inline value_definitions::value_definitions(unsigned int message, std::string signal,
                                            std::variant<entries, std::string> table)
    : message_(message), signal_(std::move(signal)), table_(std::move(table)) {}

inline unsigned int value_definitions::get_message() const {
    return message_;
}

inline const std::string& value_definitions::get_signal() const {
    return signal_;
}

inline const std::variant<value_definitions::entries, std::string>& value_definitions::get_table() const {
    return table_;
}

} /* namespace can::database::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_VALUE_DEFINITIONS_HPP */