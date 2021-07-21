#ifndef INCLUDE_CAN_DATABASE_DBC_MESSAGE_HPP
#define INCLUDE_CAN_DATABASE_DBC_MESSAGE_HPP

#include "can/format/dbc/signal.hpp"

namespace can::format::dbc {

class message : public can::database::message, public object {
   public:
    using ptr       = std::shared_ptr<message>;
    using const_ptr = std::shared_ptr<const message>;

    static ptr from_ast(const ast::database& database, const ast::message& message);

    message(const ast::message& message, std::map<std::string, signal::ptr> signals,
            std::map<std::string, int64_t> integer_attributes, std::map<std::string, float> float_attributes,
            std::map<std::string, std::string> string_attributes);

    /* inherited methods from can::database::message */

    [[nodiscard]] const std::string& get_name() const override;
    [[nodiscard]] unsigned int get_identifier() const override;
    [[nodiscard]] unsigned short get_byte_count() const override;
    [[nodiscard]] const std::string& get_node() const override;
    [[nodiscard]] std::vector<can::database::signal::const_ptr> get_signals() const override;
    [[nodiscard]] can::database::signal::const_ptr get_signal(const std::string& name) const override;

    /* inherited methods from can::format::dbc::object */

    using object::get_float_attributes;
    using object::get_integer_attributes;
    using object::get_string_attributes;

    /* dbc specific methods */

    [[nodiscard]] std::vector<dbc::signal::const_ptr> get_signals_dbc() const;
    [[nodiscard]] dbc::signal::const_ptr get_signal_dbc(const std::string& name) const;
    /* TODO: add description fields */

   private:
    const std::string name_;
    unsigned int identifier_;
    unsigned short byte_count_;
    std::string node_;
    std::map<std::string, signal::ptr> signals_{};
};

inline const std::string& message::get_name() const {
    return name_;
}

inline unsigned int message::get_identifier() const {
    return identifier_;
}

inline unsigned short message::get_byte_count() const {
    return byte_count_;
}

inline const std::string& message::get_node() const {
    return node_;
}

} /* namespace can::format::dbc */

#endif /* INCLUDE_CAN_DATABASE_DBC_MESSAGE_HPP */