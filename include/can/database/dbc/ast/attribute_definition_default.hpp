#ifndef INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_DEFINITION_DEFAULT_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_DEFINITION_DEFAULT_HPP

#include "can/database/dbc/ast/attribute.hpp"

namespace can::database::dbc::ast {

class attribute_definition_default : public attribute {
   public:
    using attribute::attribute;
};

} /* namespace can::database::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_DEFINITION_DEFAULT_HPP */