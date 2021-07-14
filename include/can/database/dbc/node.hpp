#ifndef INCLUDE_CAN_DATABASE_DBC_NODE_HPP
#define INCLUDE_CAN_DATABASE_DBC_NODE_HPP

#include <memory>

#include "can/database/dbc/ast/database.hpp"
#include "can/database/dbc/object.hpp"

namespace can::database::dbc {

class node : public object {
   public:
    using ptr = std::shared_ptr<node>;

    static ptr from_ast(const ast::database& database, std::string name);

    node(std::string name, std::map<std::string, int64_t> integer_attributes,
         std::map<std::string, float> float_attributes, std::map<std::string, std::string> string_attributes);

    /* inherited methods from can::database::dbc::object */

    using object::get_float_attributes;
    using object::get_integer_attributes;
    using object::get_string_attributes;

    /* dbc specific methods */

    [[nodiscard]] const std::string& get_name() const;
    /* TODO: add description fields */

   private:
    std::string name_;
};

inline const std::string& node::get_name() const {
    return name_;
}

} /* namespace can::database::dbc */

#endif /* INCLUDE_CAN_DATABASE_DBC_NODE_HPP */