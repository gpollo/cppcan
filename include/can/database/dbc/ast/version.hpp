#ifndef INCLUDE_CAN_DATABASE_DBC_AST_VERSION_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_VERSION_HPP

#include <string>

namespace can::database::dbc::ast {

class version {
   public:
    version(auto value);

    const std::string& get_value() const;

   private:
    std::string value_;
};

inline version::version(auto value) : value_(value.begin(), value.end()) {}

inline const std::string& version::get_value() const {
    return value_;
}

} /* namespace can::database::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_VERSION_HPP */