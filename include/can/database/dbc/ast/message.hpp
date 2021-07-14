#ifndef INCLUDE_CAN_DATABASE_DBC_AST_MESSAGE_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_MESSAGE_HPP

#include "can/database/dbc/ast/signal.hpp"

namespace can::database::dbc::ast {

class message {
   public:
    message(unsigned int identifier, std::string name, unsigned short byte_count, std::string node,
            std::optional<std::vector<signal>> signals);

    const std::string& get_name() const;
    unsigned int get_identifier() const;
    unsigned short get_byte_count() const;
    const std::vector<signal>& get_signals() const;
    const std::string& get_node() const;

   private:
    std::string name_;
    unsigned int identifier_;
    unsigned short byte_count_;
    std::vector<signal> signals_;
    std::string node_;
};

inline const std::string& message::get_name() const {
    return name_;
}

inline unsigned int message::get_identifier() const {
    return identifier_;
}

inline unsigned short message::get_byte_count() const {
    return byte_count_;
}

inline const std::vector<signal>& message::get_signals() const {
    return signals_;
}

inline const std::string& message::get_node() const {
    return node_;
}

} /* namespace can::database::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_MESSAGE_HPP */