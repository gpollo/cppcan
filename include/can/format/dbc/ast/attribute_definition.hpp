#ifndef INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_DEFINITION_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_DEFINITION_HPP

#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace can::format::dbc::ast {

class attribute_definition {
   public:
    template <typename T>
    class number {
       public:
        using type = T;

        number(T min, T max);

        [[nodiscard]] T get_min() const;
        [[nodiscard]] T get_max() const;
        [[nodiscard]] T parse(const std::string& text) const;

       private:
        T min_;
        T max_;
    };

    using integer = number<int64_t>;

    using floating_point = number<float>;

    class string {
       public:
        /* TODO: remove value */
        string(std::string value);

        [[nodiscard]] const std::string& get_value() const;
        [[nodiscard]] std::string parse(const std::string& text) const;

       private:
        std::string value_;
    };

    class enumeration {
       public:
        enumeration(std::vector<std::string> values);

        [[nodiscard]] const std::vector<std::string>& get_values() const;
        [[nodiscard]] std::string parse(const std::string& text) const;

       private:
        std::vector<std::string> values_;
    };

    using data_types = std::variant<integer, floating_point, string, enumeration>;

    enum class object_type { DATABASE, NODE, MESSAGE, SIGNAL };

    attribute_definition(object_type object, std::string name, data_types data);

    [[nodiscard]] const std::string& get_name() const;
    [[nodiscard]] object_type get_object() const;
    [[nodiscard]] const data_types& get_data() const;

    [[nodiscard]] std::optional<integer> get_integer() const;
    [[nodiscard]] std::optional<floating_point> get_floating_point() const;
    [[nodiscard]] std::optional<string> get_string() const;
    [[nodiscard]] std::optional<enumeration> get_enumeration() const;

   private:
    std::string name_;
    object_type object_;
    data_types data_;
};

/* attribute_definition::number class */

template <typename T>
inline attribute_definition::number<T>::number(T min, T max) : min_(min), max_(max) {}

template <typename T>
inline T attribute_definition::number<T>::get_min() const {
    return min_;
}

template <typename T>
inline T attribute_definition::number<T>::get_max() const {
    return max_;
}

template <typename T>
inline T attribute_definition::number<T>::parse(const std::string& text) const {
    size_t next;
    T value;

    if (std::is_floating_point<T>::value) {
        value = std::stof(text, &next);
        if (next == text.size()) {
            throw std::string("failed to parse float attribute");
        }
    } else if (std::is_integral<T>::value) {
        value = std::stol(text, &next);
        if (next != text.size()) {
            throw std::string("failed to parse integer attribute");
        }
    } else {
        throw std::string("invalid number attribute type");
    }

    if (min_ != 0 || max_ != 0) {
        if (value < min_ || max_ < value) {
            throw std::string("attribute value is out of range");
        }
    }

    return value;
}

/* attribute_definition::string class */

inline const std::string& attribute_definition::string::get_value() const {
    return value_;
}

inline std::string attribute_definition::string::parse(const std::string& text) const {
    return text;
}

/* attribute_definition::enumeration class */

inline const std::vector<std::string>& attribute_definition::enumeration::get_values() const {
    return values_;
}

/* attribute_definition class */

inline const std::string& attribute_definition::get_name() const {
    return name_;
}

inline attribute_definition::object_type attribute_definition::get_object() const {
    return object_;
}

inline const attribute_definition::data_types& attribute_definition::get_data() const {
    return data_;
}

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_ATTRIBUTE_DEFINITION_HPP */
