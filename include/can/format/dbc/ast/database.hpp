#ifndef INCLUDE_CAN_DATABASE_DBC_AST_DATABASE_HPP
#define INCLUDE_CAN_DATABASE_DBC_AST_DATABASE_HPP

#include <exception>
#include <map>
#include <variant>

#include "can/format/dbc/ast/attribute.hpp"
#include "can/format/dbc/ast/attribute_definition.hpp"
#include "can/format/dbc/ast/attribute_definition_default.hpp"
#include "can/format/dbc/ast/database_attribute.hpp"
#include "can/format/dbc/ast/database_description.hpp"
#include "can/format/dbc/ast/message.hpp"
#include "can/format/dbc/ast/message_attribute.hpp"
#include "can/format/dbc/ast/message_description.hpp"
#include "can/format/dbc/ast/node_attribute.hpp"
#include "can/format/dbc/ast/node_description.hpp"
#include "can/format/dbc/ast/nodes.hpp"
#include "can/format/dbc/ast/requirements.hpp"
#include "can/format/dbc/ast/signal.hpp"
#include "can/format/dbc/ast/signal_attribute.hpp"
#include "can/format/dbc/ast/signal_description.hpp"
#include "can/format/dbc/ast/speed.hpp"
#include "can/format/dbc/ast/value_definitions.hpp"
#include "can/format/dbc/ast/value_table_definition.hpp"
#include "can/format/dbc/ast/version.hpp"

namespace can::format::dbc::ast {

class database {
   public:
    using description_variant =
        std::variant<node_description, message_description, signal_description, database_description>;
    using attribute_variant = std::variant<node_attribute, message_attribute, signal_attribute, database_attribute>;

    /* TODO: add [[nodiscard]] to each add() methods */

    bool add(const version& version);
    bool add(const requirements& requirements);
    bool add(const speed& speed);
    bool add(const nodes& nodes);
    bool add(const message& message);
    bool add(const description_variant& description);
    bool add(const database_description& database_description);
    bool add(const node_description& node_description);
    bool add(const message_description& message_description);
    bool add(const signal_description& signal_description);
    bool add(const attribute_definition& attribute_definition);
    bool add(const attribute_definition_default& attribute_definition_default);
    bool add(const attribute_variant& attribute_variant);
    bool add(const database_attribute& database_attribute);
    bool add(const node_attribute& node_attribute);
    bool add(const message_attribute& message_attribute);
    bool add(const signal_attribute& signal_attribute);
    bool add(const value_definitions& value_definitions);
    bool add(const value_table_definition& value_table_definition);

    [[nodiscard]] const std::optional<nodes>& get_nodes() const;
    [[nodiscard]] const std::map<unsigned int, message>& get_messages() const;
    [[nodiscard]] const attribute_definition* get_attribute_definition(const std::string& name) const;
    [[nodiscard]] const attribute_definition_default* get_attribute_definition_default(const std::string& name) const;

    [[nodiscard]] std::map<std::string, int64_t> get_integer_attributes(/* database */) const;
    [[nodiscard]] std::map<std::string, int64_t> get_integer_attributes(const std::string& node) const;
    [[nodiscard]] std::map<std::string, int64_t> get_integer_attributes(const message& message) const;
    [[nodiscard]] std::map<std::string, int64_t> get_integer_attributes(const message& message,
                                                                        const signal& signal) const;

    [[nodiscard]] std::map<std::string, float> get_float_attributes(/* database */) const;
    [[nodiscard]] std::map<std::string, float> get_float_attributes(const std::string& node) const;
    [[nodiscard]] std::map<std::string, float> get_float_attributes(const message& message) const;
    [[nodiscard]] std::map<std::string, float> get_float_attributes(const message& message, const signal& signal) const;

    [[nodiscard]] std::map<std::string, std::string> get_string_attributes(/* database */) const;
    [[nodiscard]] std::map<std::string, std::string> get_string_attributes(const std::string& node) const;
    [[nodiscard]] std::map<std::string, std::string> get_string_attributes(const message& message) const;
    [[nodiscard]] std::map<std::string, std::string> get_string_attributes(const message& message,
                                                                           const signal& signal) const;

    [[nodiscard]] std::map<uint64_t, std::string> get_signal_values(const message& message, const signal& signal) const;

   private:
    std::optional<version> version_{};
    std::optional<requirements> requirements_{};
    std::optional<speed> speed_{};
    std::optional<nodes> nodes_{};

    std::map<unsigned int, message> messages_{};

    std::optional<database_description> database_description_{};
    std::map<std::string, node_description> node_descriptions_{};
    std::map<unsigned int, message_description> message_descriptions_{};
    std::map<unsigned int, std::map<std::string, signal_description>> signal_descriptions_{};

    std::map<std::string, attribute_definition> attribute_definitions_{};
    std::map<std::string, attribute_definition_default> attribute_definition_defaults_{};

    std::map<std::string, database_attribute> database_attributes_{};
    std::map<std::string, std::map<std::string, node_attribute>> node_attributes_{};
    std::map<unsigned int, std::map<std::string, message_attribute>> message_attributes_{};
    std::map<unsigned int, std::map<std::string, std::map<std::string, signal_attribute>>> signal_attributes_{};

    std::map<unsigned int, std::map<std::string, value_definitions>> value_definitions_{};
    std::map<std::string, value_table_definition> value_table_definitions_{};

    void parse_attribute(std::map<std::string, int64_t>& attributes, const attribute_definition& attr_def,
                         const std::string& attr_text) const;
    void parse_attribute(std::map<std::string, float>& attributes, const attribute_definition& attr_def,
                         const std::string& attr_text) const;
    void parse_attribute(std::map<std::string, std::string>& attributes, const attribute_definition& attr_def,
                         const std::string& attr_text) const;

    template <typename T, typename A>
    [[nodiscard]] std::map<std::string, T> get_attributes(const std::map<std::string, A>& attributes) const;

    template <typename T>
    [[nodiscard]] std::map<std::string, T> get_database_attributes() const;

    template <typename T>
    [[nodiscard]] std::map<std::string, T> get_node_attributes(const std::string& node) const;

    template <typename T>
    [[nodiscard]] std::map<std::string, T> get_message_attributes(unsigned int message) const;

    template <typename T>
    [[nodiscard]] std::map<std::string, T> get_signal_attributes(unsigned int message, const std::string& signal) const;
};

} /* namespace can::format::dbc::ast */

#endif /* INCLUDE_CAN_DATABASE_DBC_AST_DATABASE_HPP */