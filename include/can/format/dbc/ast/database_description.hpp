#ifndef INCLUDE_CAN_DATABASE_DBC_AST_DATABASE_DESCRIPTION_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_DATABASE_DESCRIPTION_HPP

#include "can/format/dbc/ast/description.hpp"

namespace can::format::dbc::ast {

class database_description : public description {
   public:
    database_description(std::string description);
};

inline database_description::database_description(std::string description) : ast::description(std::move(description)) {}

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_DATABASE_DESCRIPTION_HPP */