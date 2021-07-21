#ifndef INCLUDE_CAN_DATABASE_DBC_SIGNAL_HPP
#define INCLUDE_CAN_DATABASE_DBC_SIGNAL_HPP

#include <map>
#include <memory>

#include "can/format/dbc/ast/database.hpp"
#include "can/format/dbc/object.hpp"

namespace can::format::dbc {

class signal : public can::database::signal, public object {
   public:
    using ptr       = std::shared_ptr<signal>;
    using const_ptr = std::shared_ptr<const signal>;

    static ptr from_ast(const ast::database& database, const ast::message& message, const ast::signal& signal);

    signal(const ast::signal& sig, std::map<uint64_t, std::string> values,
           std::map<std::string, int64_t> integer_attributes, std::map<std::string, float> float_attributes,
           std::map<std::string, std::string> string_attributes);

    /* inherited methods from can::database::signal */

    [[nodiscard]] const std::string& get_name() const override;
    [[nodiscard]] unsigned char get_start_bit() const override;
    [[nodiscard]] unsigned char get_bit_count() const override;
    [[nodiscard]] endian get_byte_order() const override;
    [[nodiscard]] bool is_integral() const override;
    [[nodiscard]] bool is_signed() const override;
    [[nodiscard]] float get_scale() const override;
    [[nodiscard]] float get_offset() const override;
    [[nodiscard]] std::optional<float> get_min() const override;
    [[nodiscard]] std::optional<float> get_max() const override;
    [[nodiscard]] const std::string& get_unit() const override;
    [[nodiscard]] const std::vector<std::string>& get_nodes() const override;
    [[nodiscard]] multiplexing get_multiplexing() const override;
    [[nodiscard]] const std::string& resolve_value(uint64_t raw_value) const override;

    /* inherited methods from can::format::dbc::object */

    using object::get_float_attributes;
    using object::get_integer_attributes;
    using object::get_string_attributes;

    /* dbc specific methods */

    /* TODO: add description fields */

   private:
    std::string name_;
    unsigned char start_bit_;
    unsigned char bit_count_;
    endian byte_order_;
    bool is_integral_;
    bool is_signed_;
    float scale_;
    float offset_;
    float min_;
    float max_;
    std::string unit_;
    std::vector<std::string> nodes_;
    multiplexing multiplexing_;
    std::map<uint64_t, std::string> values_;
};

inline const std::string& signal::get_name() const {
    return name_;
}

inline unsigned char signal::get_start_bit() const {
    return start_bit_;
}

inline unsigned char signal::get_bit_count() const {
    return bit_count_;
}

inline signal::endian signal::get_byte_order() const {
    return byte_order_;
}

inline bool signal::is_integral() const {
    return is_integral_;
}

inline bool signal::is_signed() const {
    return is_signed_;
}

inline float signal::get_scale() const {
    return scale_;
}

inline float signal::get_offset() const {
    return offset_;
}

inline std::optional<float> signal::get_min() const {
    return min_;
}

inline std::optional<float> signal::get_max() const {
    return max_;
}

inline const std::string& signal::get_unit() const {
    return unit_;
}

inline const std::vector<std::string>& signal::get_nodes() const {
    return nodes_;
}

inline signal::multiplexing signal::get_multiplexing() const {
    return multiplexing_;
}

inline const std::string& signal::resolve_value(uint64_t raw_value) const {
    static const std::string EMPTY_STRING;

    if (values_.contains(raw_value)) {
        return values_.at(raw_value);
    }

    return EMPTY_STRING;
}

} /* namespace can::format::dbc */

#endif /* INCLUDE_CAN_DATABASE_DBC_SIGNAL_HPP */