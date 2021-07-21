#ifndef INCLUDE_CAN_DATABASE_DBC_AST_VALUE_TABLE_DEFINITION_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_VALUE_TABLE_DEFINITION_HPP

#include <string>
#include <utility>
#include <vector>

namespace can::format::dbc::ast {

class value_table_definition {
   public:
    using entry   = std::pair<uint64_t, std::string>;
    using entries = std::vector<entry>;

    value_table_definition(std::string name, entries entries);

    const std::string& get_name() const;
    const entries& get_entries() const;

   private:
    std::string name_;
    entries entries_;
};

inline value_table_definition::value_table_definition(std::string name, entries entries)
    : name_(std::move(name)), entries_(std::move(entries)) {}

inline const std::string& value_table_definition::get_name() const {
    return name_;
}

inline const value_table_definition::entries& value_table_definition::get_entries() const {
    return entries_;
}

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_VALUE_TABLE_DEFINITION_HPP */