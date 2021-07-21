#ifndef INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_DEFINITION_DEFAULT_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_DEFINITION_DEFAULT_HPP

#include "can/format/dbc/ast/attribute.hpp"

namespace can::format::dbc::ast {

class attribute_definition_default : public attribute {
   public:
    using attribute::attribute;
};

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_DEFINITION_DEFAULT_HPP */