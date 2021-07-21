#ifndef INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_HPP

#include <string>

namespace can::format::dbc::ast {

class attribute {
   public:
    attribute(std::string name, std::string text);

    const std::string& get_name() const;
    const std::string& get_text() const;

   private:
    std::string name_;
    std::string text_;
};

inline attribute::attribute(std::string name, std::string text) : name_(std::move(name)), text_(std::move(text)) {}

inline const std::string& attribute::get_name() const {
    return name_;
}

inline const std::string& attribute::get_text() const {
    return text_;
}

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_HPP */