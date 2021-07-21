#ifndef INCLUDE_CAN_DATABASE_DBC_AST_SIGNAL_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_SIGNAL_HPP

#include "can/database.hpp"

namespace can::format::dbc::ast {

class signal {
   public:
    using endian = can::database::signal::endian;

    signal(std::string name, unsigned char start_bit, unsigned char bit_count, endian byte_order, bool is_signed,
           float scale, float offset, float min, float max, std::string unit, std::vector<std::string> nodes,
           std::variant<bool, unsigned short> multiplexing);

    const std::string& get_name() const;
    unsigned char get_start_bit() const;
    unsigned char get_bit_count() const;
    endian get_byte_order() const;
    bool is_signed() const;
    float get_scale() const;
    float get_offset() const;
    float get_min() const;
    float get_max() const;
    const std::string& get_unit() const;
    const std::vector<std::string>& get_nodes() const;
    std::variant<bool, unsigned short> get_multiplexing() const;

   private:
    std::string name_;

    unsigned char start_bit_;
    unsigned char bit_count_;
    endian byte_order_;
    bool is_signed_;

    float scale_;
    float offset_;

    float min_ = {};
    float max_ = {};

    std::string unit_;
    std::vector<std::string> nodes_;

    std::variant<bool, unsigned short> multiplexing_;
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

inline bool signal::is_signed() const {
    return is_signed_;
}

inline float signal::get_scale() const {
    return scale_;
}

inline float signal::get_offset() const {
    return offset_;
}

inline float signal::get_min() const {
    return min_;
}

inline float signal::get_max() const {
    return max_;
}

inline const std::string& signal::get_unit() const {
    return unit_;
}

inline const std::vector<std::string>& signal::get_nodes() const {
    return nodes_;
}

inline std::variant<bool, unsigned short> signal::get_multiplexing() const {
    return multiplexing_;
}

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_SIGNAL_HPP */