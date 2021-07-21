#ifndef INCLUDE_CAN_DATABASE_DBC_AST_NODES_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_NODES_HPP

#include <string>
#include <vector>

namespace can::format::dbc::ast {

class nodes {
   public:
    nodes(std::vector<std::string> values);

    [[nodiscard]] const std::vector<std::string>& get_values() const;

   private:
    std::vector<std::string> values_;
};

inline nodes::nodes(std::vector<std::string> values) : values_(std::move(values)) {}

inline const std::vector<std::string>& nodes::get_values() const {
    return values_;
}

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_NODES_HPP */