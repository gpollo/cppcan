#ifndef INCLUDE_CAN_DATABASE_DBC_AST_DESCRIPTION_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_DESCRIPTION_HPP

#include <string>

namespace can::database::dbc::ast {

class description {
   public:
    description(std::string description);

    const std::string& get_description() const;

   private:
    std::string description_;
};

inline description::description(std::string description) : description_(std::move(description)) {}

inline const std::string& description::get_description() const {
    return description_;
}

} /* namespace can::database::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_DESCRIPTION_HPP */