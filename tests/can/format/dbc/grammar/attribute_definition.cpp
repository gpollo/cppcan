#include <cassert>
#include <cmath>
#include <cstring>
#include <string>
#include <variant>

#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"

#include "can/format/dbc/grammar/attribute_definition.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::format::dbc::grammar::attribute_definition>(input, ignore_error);
}

using attribute_definition = can::format::dbc::ast::attribute_definition;
using object_type          = attribute_definition::object_type;
using int_type             = attribute_definition::integer::type;

static void test_attribute_string(auto input, object_type type, const std::string& name, const std::string& value) {
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, strlen(input) + 1));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(attribute.get_name() == name);
    assert(attribute.get_object() == type);
    auto data_var = attribute.get_data();
    assert(std::holds_alternative<attribute_definition::string>(data_var));
    auto data = std::get<attribute_definition::string>(data_var);
    assert(data.get_value() == value);
}

static void test_attribute_integer(auto input, object_type type, const std::string& name, int_type min, int_type max) {
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, strlen(input) + 1));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(attribute.get_name() == name);
    assert(attribute.get_object() == type);
    auto data_var = attribute.get_data();
    assert(std::holds_alternative<attribute_definition::integer>(data_var));
    auto data = std::get<attribute_definition::integer>(data_var);
    assert(data.get_min() == min);
    assert(data.get_max() == max);
}

static void test_attribute_float(auto input, object_type type, const std::string& name, float min, float max) {
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, strlen(input) + 1));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(attribute.get_name() == name);
    assert(attribute.get_object() == type);
    auto data_var = attribute.get_data();
    assert(std::holds_alternative<attribute_definition::floating_point>(data_var));
    auto data               = std::get<attribute_definition::floating_point>(data_var);
    constexpr float EPSILON = 0.00001F;
    assert(std::fabs(data.get_min() - min) < EPSILON);
    assert(std::fabs(data.get_max() - max) < EPSILON);
}

static void test_attribute_enum(auto input, object_type type, const std::string& name,
                                std::vector<std::string> values) {
    auto attribute_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, strlen(input) + 1));
    assert(attribute_opt.has_value());
    auto attribute = attribute_opt.value();

    assert(attribute.get_name() == name);
    assert(attribute.get_object() == type);
    auto data_var = attribute.get_data();
    assert(std::holds_alternative<attribute_definition::enumeration>(data_var));
    auto data = std::get<attribute_definition::enumeration>(data_var);
    assert(data.get_values() == values);
}

static void test_database_attributes() {
    char str_empty[] = "BA_DEF_  \"str attr\" STRING;\n";
    test_attribute_string(str_empty, object_type::DATABASE, "str attr", "");

    char str[] = "BA_DEF_  \"str attr\" STRING \"test value\";\n";
    test_attribute_string(str, object_type::DATABASE, "str attr", "test value");

    char integer_empty[] = "BA_DEF_  \"int attr\" INT ;\n";
    test_attribute_integer(integer_empty, object_type::DATABASE, "int attr", 0, 0);

    char integer[] = "BA_DEF_  \"int attr\" INT 0 100;\n";
    test_attribute_integer(integer, object_type::DATABASE, "int attr", 0, 100);

    char float_empty[] = "BA_DEF_  \"float attr\" FLOAT ;\n";
    test_attribute_float(float_empty, object_type::DATABASE, "float attr", 0.0F, 0.0F);

    char float_[] = "BA_DEF_  \"float attr\" FLOAT -1.2 132.2;\n";
    test_attribute_float(float_, object_type::DATABASE, "float attr", -1.2F, 132.2F);

    char enum_empty[] = "BA_DEF_  \"enum attr\" ENUM ;\n";
    test_attribute_enum(enum_empty, object_type::DATABASE, "enum attr", std::vector<std::string>());

    char enum_[] = "BA_DEF_  \"enum attr\" ENUM \"value 1\",\"value 2\",\"value 3\";\n";
    test_attribute_enum(enum_, object_type::DATABASE, "enum attr", {"value 1", "value 2", "value 3"});
}

static void test_node_attributes() {
    char str_empty[] = "BA_DEF_ BU_  \"str attr\" STRING;\n";
    test_attribute_string(str_empty, object_type::NODE, "str attr", "");

    char str[] = "BA_DEF_  BU_ \"str attr\" STRING \"test value\";\n";
    test_attribute_string(str, object_type::NODE, "str attr", "test value");

    char integer_empty[] = "BA_DEF_ BU_ \"int attr\" INT ;\n";
    test_attribute_integer(integer_empty, object_type::NODE, "int attr", 0, 0);

    char integer[] = "BA_DEF_ BU_ \"int attr\" INT 0 100;\n";
    test_attribute_integer(integer, object_type::NODE, "int attr", 0, 100);

    char float_empty[] = "BA_DEF_    BU_ \"float attr\" FLOAT ;\n";
    test_attribute_float(float_empty, object_type::NODE, "float attr", 0.0F, 0.0F);

    char float_[] = "BA_DEF_ BU_   \"float attr\" FLOAT -1.2 132.2;\n";
    test_attribute_float(float_, object_type::NODE, "float attr", -1.2F, 132.2F);

    char enum_empty[] = "BA_DEF_ BU_ \"enum attr\" ENUM ;\n";
    test_attribute_enum(enum_empty, object_type::NODE, "enum attr", std::vector<std::string>());

    char enum_[] = "BA_DEF_ BU_ \"enum attr\" ENUM \"value 1\",\"value 2\",\"value 3\";\n";
    test_attribute_enum(enum_, object_type::NODE, "enum attr", {"value 1", "value 2", "value 3"});
}

static void test_message_attributes() {
    char str_empty[] = "BA_DEF_ BO_  \"str attr\" STRING;\n";
    test_attribute_string(str_empty, object_type::MESSAGE, "str attr", "");

    char str[] = "BA_DEF_  BO_ \"str attr\" STRING \"test value\";\n";
    test_attribute_string(str, object_type::MESSAGE, "str attr", "test value");

    char integer_empty[] = "BA_DEF_ BO_ \"int attr\" INT ;\n";
    test_attribute_integer(integer_empty, object_type::MESSAGE, "int attr", 0, 0);

    char integer[] = "BA_DEF_ BO_ \"int attr\" INT 0 100;\n";
    test_attribute_integer(integer, object_type::MESSAGE, "int attr", 0, 100);

    char float_empty[] = "BA_DEF_    BO_ \"float attr\" FLOAT ;\n";
    test_attribute_float(float_empty, object_type::MESSAGE, "float attr", 0.0F, 0.0F);

    char float_[] = "BA_DEF_ BO_   \"float attr\" FLOAT -1.2 132.2;\n";
    test_attribute_float(float_, object_type::MESSAGE, "float attr", -1.2F, 132.2F);

    char enum_empty[] = "BA_DEF_ BO_ \"enum attr\" ENUM ;\n";
    test_attribute_enum(enum_empty, object_type::MESSAGE, "enum attr", std::vector<std::string>());

    char enum_[] = "BA_DEF_ BO_ \"enum attr\" ENUM \"value 1\",\"value 2\",\"value 3\";\n";
    test_attribute_enum(enum_, object_type::MESSAGE, "enum attr", {"value 1", "value 2", "value 3"});
}

static void test_signal_attributes() {
    char str_empty[] = "BA_DEF_ SG_  \"str attr\" STRING;\n";
    test_attribute_string(str_empty, object_type::SIGNAL, "str attr", "");

    char str[] = "BA_DEF_  SG_ \"str attr\" STRING \"test value\";\n";
    test_attribute_string(str, object_type::SIGNAL, "str attr", "test value");

    char integer_empty[] = "BA_DEF_ SG_ \"int attr\" INT ;\n";
    test_attribute_integer(integer_empty, object_type::SIGNAL, "int attr", 0, 0);

    char integer[] = "BA_DEF_ SG_ \"int attr\" INT 0 100;\n";
    test_attribute_integer(integer, object_type::SIGNAL, "int attr", 0, 100);

    char float_empty[] = "BA_DEF_    SG_ \"float attr\" FLOAT ;\n";
    test_attribute_float(float_empty, object_type::SIGNAL, "float attr", 0.0F, 0.0F);

    char float_[] = "BA_DEF_ SG_   \"float attr\" FLOAT -1.2 132.2;\n";
    test_attribute_float(float_, object_type::SIGNAL, "float attr", -1.2F, 132.2F);

    char enum_empty[] = "BA_DEF_ SG_ \"enum attr\" ENUM ;\n";
    test_attribute_enum(enum_empty, object_type::SIGNAL, "enum attr", std::vector<std::string>());

    char enum_[] = "BA_DEF_ SG_ \"enum attr\" ENUM \"value 1\",\"value 2\",\"value 3\";\n";
    test_attribute_enum(enum_, object_type::SIGNAL, "enum attr", {"value 1", "value 2", "value 3"});
}

int main() {
    test_database_attributes();
    test_node_attributes();
    test_message_attributes();
    test_signal_attributes();

    return 0;
}