#ifndef INCLUDE_CAN_DATABASE_DBC_AST_MESSAGE_DESCRIPTION_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_MESSAGE_DESCRIPTION_HPP

#include "can/format/dbc/ast/description.hpp"

namespace can::format::dbc::ast {

class message_description : public description {
   public:
    message_description(unsigned int identifier, std::string description);

    [[nodiscard]] unsigned int get_identifier() const;

   private:
    unsigned int identifier_;
};

inline message_description::message_description(unsigned int identifier, std::string description)
    : ast::description(std::move(description)), identifier_(identifier) {}

inline unsigned int message_description::get_identifier() const {
    return identifier_;
}

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_MESSAGE_DESCRIPTION_HPP */