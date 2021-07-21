#ifndef INCLUDE_CAN_DATABASE_DBC_AST_DATABASE_ATTRIBUTE_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_DATABASE_ATTRIBUTE_HPP

#include "can/format/dbc/ast/attribute.hpp"

namespace can::format::dbc::ast {

class database_attribute : public attribute {
   public:
    database_attribute(std::string name, std::string text);
};

inline database_attribute::database_attribute(std::string name, std::string text) : attribute(name, text) {}

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_DATABASE_ATTRIBUTE_HPP */