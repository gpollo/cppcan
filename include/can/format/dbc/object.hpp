#ifndef INCLUDE_CAN_DATABASE_DBC_OBJECT_HPP
#define INCLUDE_CAN_DATABASE_DBC_OBJECT_HPP

#include <cstdint>
#include <map>
#include <string>

namespace can::format::dbc {

class object {
   public:
    object(std::map<std::string, int64_t> integer_attributes, std::map<std::string, float> float_attributes,
           std::map<std::string, std::string> string_attributes);

    [[nodiscard]] const std::map<std::string, int64_t>& get_integer_attributes() const;
    [[nodiscard]] const std::map<std::string, float>& get_float_attributes() const;
    [[nodiscard]] const std::map<std::string, std::string>& get_string_attributes() const;

   private:
    std::map<std::string, int64_t> integer_attributes_;
    std::map<std::string, float> float_attributes_;
    std::map<std::string, std::string> string_attributes_;
};

inline const std::map<std::string, int64_t>& object::get_integer_attributes() const {
    return integer_attributes_;
}

inline const std::map<std::string, float>& object::get_float_attributes() const {
    return float_attributes_;
}

inline const std::map<std::string, std::string>& object::get_string_attributes() const {
    return string_attributes_;
}

} /* namespace can::format::dbc */

#endif /* INCLUDE_CAN_DATABASE_DBC_OBJECT_HPP */