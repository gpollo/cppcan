#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <variant>

#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"
#include "lexy_ext/report_error.hpp"

#include "can/format/dbc/database.hpp"
#include "can/format/dbc/grammar/database.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static bool almost_equal(float value, float expected) {
    constexpr float EPSILON = 0.0000000000000000001F;
    return (std::fabs(value - expected) < EPSILON);
}

static auto parse(auto input) {
    auto ast = lexy::parse<can::format::dbc::grammar::database>(input, lexy_ext::report_error);
    assert(ast.has_value());

    can::format::dbc::database::ptr db;
    try {
        db = can::format::dbc::database::from_ast(*ast.value());
    } catch (const std::string& error) {
        std::cout << error << std::endl;
        assert(false);
    }

    return db;
}

static void test_simple_database() {
    char input[] =
        "VERSION \"\"\n"
        "\n"
        "\n"
        "NS_ :\n"
        "\tBA_\n"
        "\tBA_DEF_\n"
        "\tBA_DEF_DEF_\n"
        "\tBA_SGTYPE_\n"
        "\tBO_TX_BU_\n"
        "\tCAT_\n"
        "\tCAT_DEF_\n"
        "\tCM_\n"
        "\tVAL_\n"
        "\tVAL_TABLE_\n"
        "\n"
        "BS_:\n"
        "\n"
        "BU_ : MASTER DEBUG\n"
        "\n"
        "BO_ 1 AIRS_OPENING: 0 MASTER\n"
        "\n"
        "BO_ 64 EXTRM_TEMPS: 4 MASTER\n"
        " SG_ EXTRM_TEMPS_MINIMUM : 0|14@1+ (0.0054931640625,0) [0|90] \"C\" DEBUG\n"
        " SG_ EXTRM_TEMPS_MAXIMUM : 14|14@1+ (0.0054931640625,0) [0|90] \"C\" DEBUG\n"
        "\n"
        "BO_ 96 EXTRM_VOLTS: 8 MASTER\n"
        " SG_ EXTRM_VOLTS_MINIMUM : 0|16@1+ (0.042724609375,1600) [1600|4400] \"mV\" DEBUG\n"
        " SG_ EXTRM_VOLTS_MAXIMUM : 16|16@1+ (0.042724609375,1600) [1600|4400] \"mV\" DEBUG\n"
        "\n"
        "BO_ 768 TEMPS: 3 MASTER\n"
        " SG_ TEMPS_MODULE : 0|4@1+ (1,0) [0|11] \"\" DEBUG\n"
        " SG_ TEMPS_CHANNEL : 4|4@1+ (1,0) [0|15] \"\" DEBUG\n"
        " SG_ TEMPS_VALUE : 8|14@1+ (0.0054931640625,0) [0|90] \"C\" DEBUG\n"
        "\n"
        "VAL_ 768 TEMPS_MODULE MODULE_NAMES ; \n"
        "\n"
        "VAL_ 768 TEMPS_CHANNEL "
        " 0 \"CHANNEL_1\" 1 \"CHANNEL_2\""
        " 2 \"CHANNEL_3\" 3 \"CHANNEL_4\""
        " 4 \"CHANNEL_5\" 5 \"CHANNEL_6\""
        " 6 \"CHANNEL_7\" 7 \"CHANNEL_8\""
        " 8 \"CHANNEL_9\" 9 \"CHANNEL_10\""
        " 10 \"CHANNEL_11\" 11 \"CHANNEL_12\""
        " 12 \"CHANNEL_13\" 13 \"CHANNEL_14\""
        " 14 \"CHANNEL_15\" 15 \"CHANNEL_16\"; \n"
        "\n"
        "VAL_TABLE_ MODULE_NAMES"
        " 0 \"MODULE_1\" 1 \"MODULE_2\""
        " 2 \"MODULE_3\" 3 \"MODULE_4\""
        " 4 \"MODULE_5\" 5 \"MODULE_6\""
        " 6 \"MODULE_7\" 7 \"MODULE_8\";\n"
        "\n"
        "CM_ \"test database\";\n"
        "\n"
        "CM_ BU_ MASTER \"master node description\";\n"
        "CM_ BU_ DEBUG \"debug node description\";\n"
        "\n"
        "CM_ BO_ 1 \"sent the moment an error is detected\"; \n"
        "\n"
        "CM_ SG_ 64 EXTRM_TEMPS_MINIMUM \"current minimum temperature\"; \n"
        "CM_ SG_ 64 EXTRM_TEMPS_MAXIMUM \"current maximum temperature\" ;\n"
        "\n"
        "BA_DEF_ \"BusType\" STRING ;\n"
        "BA_DEF_ BU_ \"CriticalNode\" ENUM \"Yes\",\"No\";\n"
        "BA_DEF_ BO_ \"GenMsgCycleTime\" INT 0 0;\n"
        "BA_DEF_ SG_ \"SignalType\" STRING ;\n"
        "\n"
        "BA_DEF_DEF_ \"BusType\" \"CAN\";\n"
        "BA_DEF_DEF_ \"CriticalNode\" \"Yes\";\n"
        "BA_DEF_DEF_ \"GenMsgCycleTime\" 0;\n"
        "BA_DEF_DEF_ \"SignalType\" \"uint32_t\";\n"
        "\n"
        "BA_ \"CriticalNode\" BU_ MASTER \"No\";\n"
        "BA_ \"GenMsgCycleTime\" BO_ 64 500;\n"
        "BA_ \"GenMsgCycleTime\" BO_ 96 500;\n"
        "BA_ \"GenMsgCycleTime\" BO_ 768 50;\n"
        "BA_ \"SignalType\" SG_ 64 EXTRM_TEMPS_MINIMUM \"float\";\n"
        "BA_ \"SignalType\" SG_ 64 EXTRM_TEMPS_MAXIMUM \"float\";\n";

    auto database = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));

    const auto& messages = database->get_messages_dbc();
    assert(messages.size() == 4);

    auto msg1 = messages[0];
    assert(msg1->get_name() == "AIRS_OPENING");
    assert(msg1->get_identifier() == 1);
    assert(msg1->get_byte_count() == 0);
    assert(msg1->get_node() == "MASTER");
    assert(msg1->get_integer_attributes().empty());
    assert(msg1->get_float_attributes().empty());
    assert(msg1->get_string_attributes().empty());
    assert(msg1->get_signals().empty());

    auto msg64 = messages[1];
    assert(msg64->get_name() == "EXTRM_TEMPS");
    assert(msg64->get_identifier() == 64);
    assert(msg64->get_byte_count() == 4);
    assert(msg64->get_node() == "MASTER");
    assert(msg64->get_integer_attributes().size() == 1);
    assert(msg64->get_integer_attributes().at("GenMsgCycleTime") == 500);
    assert(msg64->get_float_attributes().empty());
    assert(msg64->get_string_attributes().empty());
    assert(msg64->get_signals().size() == 2);

    auto msg64_sig0 = msg64->get_signal_dbc("EXTRM_TEMPS_MINIMUM");
    assert(msg64_sig0->get_name() == "EXTRM_TEMPS_MINIMUM");
    assert(msg64_sig0->get_start_bit() == 0);
    assert(msg64_sig0->get_bit_count() == 14);
    assert(msg64_sig0->get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!msg64_sig0->is_signed());
    assert(almost_equal(msg64_sig0->get_scale(), 0.0054931640625F));
    assert(almost_equal(msg64_sig0->get_offset(), 0.0F));
    assert(almost_equal(msg64_sig0->get_min().value(), 0.0F));
    assert(almost_equal(msg64_sig0->get_max().value(), 90.0F));
    assert(msg64_sig0->get_unit() == "C");
    assert(msg64_sig0->get_nodes().size() == 1);
    assert(msg64_sig0->get_nodes().at(0) == "DEBUG");
    assert(std::get<bool>(msg64_sig0->get_multiplexing()) == false);
    assert(msg64_sig0->get_integer_attributes().empty());
    assert(msg64_sig0->get_float_attributes().empty());
    assert(msg64_sig0->get_string_attributes().size() == 1);
    assert(msg64_sig0->get_string_attributes().at("SignalType") == "float");

    auto msg64_sig1 = msg64->get_signal_dbc("EXTRM_TEMPS_MAXIMUM");
    assert(msg64_sig1->get_name() == "EXTRM_TEMPS_MAXIMUM");
    assert(msg64_sig1->get_start_bit() == 14);
    assert(msg64_sig1->get_bit_count() == 14);
    assert(msg64_sig1->get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!msg64_sig1->is_signed());
    assert(almost_equal(msg64_sig1->get_scale(), 0.0054931640625F));
    assert(almost_equal(msg64_sig1->get_offset(), 0.0F));
    assert(almost_equal(msg64_sig1->get_min().value(), 0.0F));
    assert(almost_equal(msg64_sig1->get_max().value(), 90.0F));
    assert(msg64_sig1->get_unit() == "C");
    assert(msg64_sig1->get_nodes().size() == 1);
    assert(msg64_sig1->get_nodes().at(0) == "DEBUG");
    assert(std::get<bool>(msg64_sig1->get_multiplexing()) == false);
    assert(msg64_sig1->get_integer_attributes().empty());
    assert(msg64_sig1->get_float_attributes().empty());
    assert(msg64_sig1->get_string_attributes().size() == 1);
    assert(msg64_sig1->get_string_attributes().at("SignalType") == "float");

    auto msg96 = messages[2];
    assert(msg96->get_name() == "EXTRM_VOLTS");
    assert(msg96->get_identifier() == 96);
    assert(msg96->get_byte_count() == 8);
    assert(msg96->get_node() == "MASTER");
    assert(msg96->get_integer_attributes().size() == 1);
    assert(msg96->get_integer_attributes().at("GenMsgCycleTime") == 500);
    assert(msg96->get_float_attributes().empty());
    assert(msg96->get_string_attributes().empty());
    assert(msg96->get_signals().size() == 2);

    auto msg96_sig0 = msg96->get_signal_dbc("EXTRM_VOLTS_MINIMUM");
    assert(msg96_sig0->get_name() == "EXTRM_VOLTS_MINIMUM");
    assert(msg96_sig0->get_start_bit() == 0);
    assert(msg96_sig0->get_bit_count() == 16);
    assert(msg96_sig0->get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!msg96_sig0->is_signed());
    assert(almost_equal(msg96_sig0->get_scale(), 0.042724609375F));
    assert(almost_equal(msg96_sig0->get_offset(), 1600.0F));
    assert(almost_equal(msg96_sig0->get_min().value(), 1600.0F));
    assert(almost_equal(msg96_sig0->get_max().value(), 4400.0F));
    assert(msg96_sig0->get_unit() == "mV");
    assert(msg96_sig0->get_nodes().size() == 1);
    assert(msg96_sig0->get_nodes().at(0) == "DEBUG");
    assert(std::get<bool>(msg96_sig0->get_multiplexing()) == false);
    assert(msg96_sig0->get_integer_attributes().empty());
    assert(msg96_sig0->get_float_attributes().empty());
    assert(msg96_sig0->get_string_attributes().empty());

    auto msg96_sig1 = msg96->get_signal_dbc("EXTRM_VOLTS_MAXIMUM");
    assert(msg96_sig1->get_name() == "EXTRM_VOLTS_MAXIMUM");
    assert(msg96_sig1->get_start_bit() == 16);
    assert(msg96_sig1->get_bit_count() == 16);
    assert(msg96_sig1->get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!msg96_sig1->is_signed());
    assert(almost_equal(msg96_sig1->get_scale(), 0.042724609375F));
    assert(almost_equal(msg96_sig1->get_offset(), 1600.0F));
    assert(almost_equal(msg96_sig1->get_min().value(), 1600.0F));
    assert(almost_equal(msg96_sig1->get_max().value(), 4400.0F));
    assert(msg96_sig1->get_unit() == "mV");
    assert(msg96_sig1->get_nodes().size() == 1);
    assert(msg96_sig1->get_nodes().at(0) == "DEBUG");
    assert(std::get<bool>(msg96_sig1->get_multiplexing()) == false);
    assert(msg96_sig1->get_integer_attributes().empty());
    assert(msg96_sig1->get_float_attributes().empty());
    assert(msg96_sig1->get_string_attributes().empty());

    auto msg768 = messages[3];
    assert(msg768->get_name() == "TEMPS");
    assert(msg768->get_identifier() == 768);
    assert(msg768->get_byte_count() == 3);
    assert(msg768->get_node() == "MASTER");
    assert(msg768->get_integer_attributes().size() == 1);
    assert(msg768->get_integer_attributes().at("GenMsgCycleTime") == 50);
    assert(msg768->get_float_attributes().empty());
    assert(msg768->get_string_attributes().empty());
    assert(msg768->get_signals().size() == 3);

    auto msg768_sig0 = msg768->get_signal_dbc("TEMPS_MODULE");
    assert(msg768_sig0->get_name() == "TEMPS_MODULE");
    assert(msg768_sig0->get_start_bit() == 0);
    assert(msg768_sig0->get_bit_count() == 4);
    assert(msg768_sig0->get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!msg768_sig0->is_signed());
    assert(almost_equal(msg768_sig0->get_scale(), 1.0));
    assert(almost_equal(msg768_sig0->get_offset(), 0.0F));
    assert(almost_equal(msg768_sig0->get_min().value(), 0.0F));
    assert(almost_equal(msg768_sig0->get_max().value(), 11.0F));
    assert(msg768_sig0->get_unit().empty());
    assert(msg768_sig0->get_nodes().size() == 1);
    assert(msg768_sig0->get_nodes().at(0) == "DEBUG");
    assert(std::get<bool>(msg768_sig0->get_multiplexing()) == false);
    assert(msg768_sig0->get_integer_attributes().empty());
    assert(msg768_sig0->get_float_attributes().empty());
    assert(msg768_sig0->get_string_attributes().empty());
    assert(msg768_sig0->resolve(0) == "MODULE_1");
    assert(msg768_sig0->resolve(1) == "MODULE_2");
    assert(msg768_sig0->resolve(2) == "MODULE_3");
    assert(msg768_sig0->resolve(3) == "MODULE_4");
    assert(msg768_sig0->resolve(4) == "MODULE_5");
    assert(msg768_sig0->resolve(5) == "MODULE_6");
    assert(msg768_sig0->resolve(6) == "MODULE_7");
    assert(msg768_sig0->resolve(7) == "MODULE_8");

    auto msg768_sig1 = msg768->get_signal_dbc("TEMPS_CHANNEL");
    assert(msg768_sig1->get_name() == "TEMPS_CHANNEL");
    assert(msg768_sig1->get_start_bit() == 4);
    assert(msg768_sig1->get_bit_count() == 4);
    assert(msg768_sig1->get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!msg768_sig1->is_signed());
    assert(almost_equal(msg768_sig1->get_scale(), 1.0));
    assert(almost_equal(msg768_sig1->get_offset(), 0.0F));
    assert(almost_equal(msg768_sig1->get_min().value(), 0.0F));
    assert(almost_equal(msg768_sig1->get_max().value(), 15.0F));
    assert(msg768_sig1->get_unit().empty());
    assert(msg768_sig1->get_nodes().size() == 1);
    assert(msg768_sig1->get_nodes().at(0) == "DEBUG");
    assert(std::get<bool>(msg768_sig1->get_multiplexing()) == false);
    assert(msg768_sig1->get_integer_attributes().empty());
    assert(msg768_sig1->get_float_attributes().empty());
    assert(msg768_sig1->get_string_attributes().empty());
    assert(msg768_sig1->resolve(0) == "CHANNEL_1");
    assert(msg768_sig1->resolve(1) == "CHANNEL_2");
    assert(msg768_sig1->resolve(2) == "CHANNEL_3");
    assert(msg768_sig1->resolve(3) == "CHANNEL_4");
    assert(msg768_sig1->resolve(4) == "CHANNEL_5");
    assert(msg768_sig1->resolve(5) == "CHANNEL_6");
    assert(msg768_sig1->resolve(6) == "CHANNEL_7");
    assert(msg768_sig1->resolve(7) == "CHANNEL_8");
    assert(msg768_sig1->resolve(8) == "CHANNEL_9");
    assert(msg768_sig1->resolve(9) == "CHANNEL_10");
    assert(msg768_sig1->resolve(10) == "CHANNEL_11");
    assert(msg768_sig1->resolve(11) == "CHANNEL_12");
    assert(msg768_sig1->resolve(12) == "CHANNEL_13");
    assert(msg768_sig1->resolve(13) == "CHANNEL_14");
    assert(msg768_sig1->resolve(14) == "CHANNEL_15");
    assert(msg768_sig1->resolve(15) == "CHANNEL_16");

    auto msg768_sig2 = msg768->get_signal_dbc("TEMPS_VALUE");
    assert(msg768_sig2->get_name() == "TEMPS_VALUE");
    assert(msg768_sig2->get_start_bit() == 8);
    assert(msg768_sig2->get_bit_count() == 14);
    assert(msg768_sig2->get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!msg768_sig2->is_signed());
    assert(almost_equal(msg768_sig2->get_scale(), 0.0054931640625F));
    assert(almost_equal(msg768_sig2->get_offset(), 0.0F));
    assert(almost_equal(msg768_sig2->get_min().value(), 0.0F));
    assert(almost_equal(msg768_sig2->get_max().value(), 90.0F));
    assert(msg768_sig2->get_unit() == "C");
    assert(msg768_sig2->get_nodes().size() == 1);
    assert(msg768_sig2->get_nodes().at(0) == "DEBUG");
    assert(std::get<bool>(msg768_sig2->get_multiplexing()) == false);
    assert(msg768_sig2->get_integer_attributes().empty());
    assert(msg768_sig2->get_float_attributes().empty());
    assert(msg768_sig2->get_string_attributes().empty());

    const auto& nodes = database->get_nodes_dbc();
    assert(nodes.size() == 2);

    const auto node_master = nodes.at("MASTER");
    assert(node_master->get_integer_attributes().empty());
    assert(node_master->get_float_attributes().empty());
    assert(node_master->get_string_attributes().size() == 1);
    assert(node_master->get_string_attributes().at("CriticalNode") == "No");

    const auto node_debug = nodes.at("DEBUG");
    assert(node_debug->get_integer_attributes().empty());
    assert(node_debug->get_float_attributes().empty());
    assert(node_debug->get_string_attributes().empty());
}

int main() {
    test_simple_database();

    return 0;
}