#ifndef INCLUDE_CAN_DATABASE_DBC_AST_SIGNAL_ATTRIBUTE_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_SIGNAL_ATTRIBUTE_HPP

#include "can/database/dbc/ast/attribute.hpp"

namespace can::database::dbc::ast {

class signal_attribute : public attribute {
   public:
    signal_attribute(std::string name, unsigned int message, std::string signal, std::string text);

    unsigned int get_message() const;
    const std::string& get_signal() const;

   private:
    unsigned int message_;
    std::string signal_;
};

inline signal_attribute::signal_attribute(std::string name, unsigned int message, std::string signal, std::string text)
    : attribute(name, text), message_(message), signal_(std::move(signal)) {}

inline unsigned int signal_attribute::get_message() const {
    return message_;
}

inline const std::string& signal_attribute::get_signal() const {
    return signal_;
}

} /* namespace can::database::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_SIGNAL_ATTRIBUTE_HPP */