#ifndef INCLUDE_CAN_DATABASE_DBC_DATABASE_HPP
#define INCLUDE_CAN_DATABASE_DBC_DATABASE_HPP

#include <filesystem>

#include "can/format/dbc/message.hpp"
#include "can/format/dbc/node.hpp"

namespace can::format::dbc {

class database : public can::database::database, public object {
   public:
    using ptr = std::shared_ptr<database>;

    static ptr from_path(const std::filesystem::path& path);
    static ptr from_ast(const ast::database& database);

    database(const ast::database& database, std::map<unsigned int, dbc::message::ptr> messages_by_identifier,
             std::map<std::string, dbc::message::ptr> messages_by_name, std::map<std::string, node::ptr> nodes_dbc,
             std::map<std::string, int64_t> integer_attributes, std::map<std::string, float> float_attributes,
             std::map<std::string, std::string> string_attributes);

    /* inherited methods from can::database::message */

    [[nodiscard]] std::vector<can::database::message::const_ptr> get_messages() const override;
    [[nodiscard]] can::database::message::const_ptr get_message(unsigned int identifier) const override;
    [[nodiscard]] can::database::message::const_ptr get_message(const std::string& name) const override;

    /* inherited methods from can::format::dbc::object */

    using object::get_float_attributes;
    using object::get_integer_attributes;
    using object::get_string_attributes;

    /* dbc specific methods */

    [[nodiscard]] std::vector<dbc::message::const_ptr> get_messages_dbc() const;
    [[nodiscard]] dbc::message::const_ptr get_message_dbc(unsigned int identifier) const;
    [[nodiscard]] dbc::message::const_ptr get_message_dbc(const std::string& name) const;
    [[nodiscard]] const std::map<std::string, node::ptr>& get_nodes_dbc() const;
    /* TODO: add description fields */

   private:
    std::map<unsigned int, dbc::message::ptr> messages_by_identifier_;
    std::map<std::string, dbc::message::ptr> messages_by_name_;
    std::map<std::string, node::ptr> nodes_dbc_;
};

inline const std::map<std::string, node::ptr>& database::get_nodes_dbc() const {
    return nodes_dbc_;
}

} /* namespace can::format::dbc */

#endif /* INCLUDE_CAN_DATABASE_DBC_DATABASE_HPP */