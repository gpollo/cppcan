#ifndef INCLUDE_CAN_DATABASE_DBC_AST_MESSAGE_ATTRIBUTE_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_MESSAGE_ATTRIBUTE_HPP

#include "can/database/dbc/ast/attribute.hpp"

namespace can::database::dbc::ast {

class message_attribute : public attribute {
   public:
    message_attribute(std::string name, unsigned int message, std::string text);

    unsigned int get_message() const;

   private:
    unsigned int message_;
};

inline message_attribute::message_attribute(std::string name, unsigned int message, std::string text)
    : attribute(name, text), message_(message) {}

inline unsigned int message_attribute::get_message() const {
    return message_;
}

} /* namespace can::database::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_MESSAGE_ATTRIBUTE_HPP */