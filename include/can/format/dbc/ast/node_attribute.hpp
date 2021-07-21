#ifndef INCLUDE_CAN_DATABASE_DBC_AST_NODE_ATTRIBUTE_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_NODE_ATTRIBUTE_HPP

#include "can/format/dbc/ast/attribute.hpp"

namespace can::format::dbc::ast {

class node_attribute : public attribute {
   public:
    node_attribute(std::string name, std::string node, std::string text);

    const std::string& get_node() const;

   private:
    std::string node_;
};

inline node_attribute::node_attribute(std::string name, std::string node, std::string text)
    : attribute(name, text), node_(std::move(node)) {}

inline const std::string& node_attribute::get_node() const {
    return node_;
}

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_NODE_ATTRIBUTE_HPP */