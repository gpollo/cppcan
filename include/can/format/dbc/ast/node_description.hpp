#ifndef INCLUDE_CAN_DATABASE_DBC_AST_NODE_DESCRIPTION_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_NODE_DESCRIPTION_HPP

#include "can/format/dbc/ast/description.hpp"

namespace can::format::dbc::ast {

class node_description : public description {
   public:
    node_description(std::string name, std::string description);

    [[nodiscard]] const std::string& get_name() const;

   private:
    std::string name_;
};

inline node_description::node_description(std::string name, std::string description)
    : ast::description(std::move(description)), name_(std::move(name)) {}

inline const std::string& node_description::get_name() const {
    return name_;
}

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_NODE_DESCRIPTION_HPP */