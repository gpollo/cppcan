#ifndef INCLUDE_CAN_DATABASE_DBC_AST_SIGNAL_DESCRIPTION_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_SIGNAL_DESCRIPTION_HPP

#include "can/format/dbc/ast/description.hpp"

namespace can::format::dbc::ast {

class signal_description : public description {
   public:
    signal_description(unsigned int message, std::string signal, std::string description);

    [[nodiscard]] unsigned int get_message() const;
    [[nodiscard]] const std::string& get_signal() const;

   private:
    unsigned int message_;
    std::string signal_;
};

inline signal_description::signal_description(unsigned int message, std::string signal, std::string description)
    : ast::description(std::move(description)), message_(message), signal_(std::move(signal)) {}

inline unsigned int signal_description::get_message() const {
    return message_;
}

inline const std::string& signal_description::get_signal() const {
    return signal_;
}

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_SIGNAL_DESCRIPTION_HPP */