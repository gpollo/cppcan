#include <cassert>
#include <cmath>
#include <string>

#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"

#include "can/format/dbc/grammar/message.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::format::dbc::grammar::message>(input, ignore_error);
}

static bool almost_equal(float value, float expected) {
    constexpr float EPSILON = 0.0000000000000000001F;
    return (std::fabs(value - expected) < EPSILON);
}

static void test_empty_message() {
    char input[] = "BO_ 1 AIRS_OPENING: 0 MASTER\n";
    auto sig_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(sig_opt.has_value());
    auto msg = sig_opt.value();

    assert(msg.get_identifier() == 1U);
    assert(msg.get_name() == "AIRS_OPENING");
    assert(msg.get_byte_count() == 0U);
    assert(msg.get_node() == "MASTER");
    assert(msg.get_signals().empty());
}

static void test_simple_message() {
    char input[] =
        "BO_ 819 BRAKE_PEDAL_RAW: 5 TCS\n"
        " SG_ BRAKE_PEDAL_RAW_FRONT : 0|12@1+ (0.001343101343101343,0) [0|5.5] \"V\"  DEBUG\n"
        " SG_ BRAKE_PEDAL_RAW_BACK   : 12|12@1+ (0.001343101343101343,0)  [0|5.5] \"V\" DEBUG\n"
        "SG_  BRAKE_PEDAL_RAW_FRONT_VALID :  24|1@1+ (1,0) [0|1] \"\" DEBUG\n"
        " SG_ BRAKE_PEDAL_RAW_BACK_VALID   : 25|1@1+  (1,0) [0|1] \"\" DEBUG\n";
    auto sig_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(sig_opt.has_value());
    auto msg = sig_opt.value();

    assert(msg.get_identifier() == 819U);
    assert(msg.get_name() == "BRAKE_PEDAL_RAW");
    assert(msg.get_byte_count() == 5U);
    assert(msg.get_node() == "TCS");
    assert(msg.get_signals().size() == 4U);

    auto sig = msg.get_signals()[0];
    assert(sig.get_name() == "BRAKE_PEDAL_RAW_FRONT");
    assert(sig.get_start_bit() == 0U);
    assert(sig.get_bit_count() == 12U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_scale(), 0.001343101343101343F));
    assert(almost_equal(sig.get_offset(), 0.0F));
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 5.5F));
    assert(sig.get_unit() == "V");
    assert(sig.get_nodes().size() == 1);
    assert(sig.get_nodes()[0] == "DEBUG");
    assert(!std::get<bool>(sig.get_multiplexing()));

    sig = msg.get_signals()[1];
    assert(sig.get_name() == "BRAKE_PEDAL_RAW_BACK");
    assert(sig.get_start_bit() == 12U);
    assert(sig.get_bit_count() == 12U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_scale(), 0.001343101343101343F));
    assert(almost_equal(sig.get_offset(), 0.0F));
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 5.5F));
    assert(sig.get_unit() == "V");
    assert(sig.get_nodes().size() == 1);
    assert(sig.get_nodes()[0] == "DEBUG");
    assert(!std::get<bool>(sig.get_multiplexing()));

    sig = msg.get_signals()[2];
    assert(sig.get_name() == "BRAKE_PEDAL_RAW_FRONT_VALID");
    assert(sig.get_start_bit() == 24U);
    assert(sig.get_bit_count() == 1U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_scale(), 1.0F));
    assert(almost_equal(sig.get_offset(), 0.0F));
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 1.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 1);
    assert(sig.get_nodes()[0] == "DEBUG");
    assert(!std::get<bool>(sig.get_multiplexing()));

    sig = msg.get_signals()[3];
    assert(sig.get_name() == "BRAKE_PEDAL_RAW_BACK_VALID");
    assert(sig.get_start_bit() == 25U);
    assert(sig.get_bit_count() == 1U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_scale(), 1.0F));
    assert(almost_equal(sig.get_offset(), 0.0F));
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 1.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 1);
    assert(sig.get_nodes()[0] == "DEBUG");
    assert(!std::get<bool>(sig.get_multiplexing()));
}

static void test_multiplexed_message() {
    char input[] =
        "BO_ 200 SENSOR_SONARS: 8 SENSOR\n"
        " SG_ SENSOR_SONARS_mux M : 0|4@1+ (1,0) [0|0] \"\" DRIVER,IO\n"
        " SG_ SENSOR_SONARS_err_count : 4|12@1+ (1,0) [0|0] \"\" DRIVER,IO\n"
        " SG_ SENSOR_SONARS_left m0 : 16|12@1+ (0.1,0) [0|0] \"\" DRIVER,IO\n"
        " SG_ SENSOR_SONARS_middle m0 : 28|12@1+ (0.1,0) [0|0] \"\" DRIVER,IO\n"
        " SG_ SENSOR_SONARS_right m0 : 40|12@1+ (0.1,0) [0|0] \"\" DRIVER,IO\n"
        " SG_ SENSOR_SONARS_rear m0 : 52|12@1+ (0.1,0) [0|0] \"\" DRIVER,IO\n"
        " SG_ SENSOR_SONARS_no_filt_left m1 : 16|12@1+ (0.1,0) [0|0] \"\" DBG\n"
        " SG_ SENSOR_SONARS_no_filt_middle m1 : 28|12@1+ (0.1,0) [0|0] \"\" DBG\n"
        " SG_ SENSOR_SONARS_no_filt_right m1 : 40|12@1+ (0.1,0) [0|0] \"\" DBG\n"
        " SG_ SENSOR_SONARS_no_filt_rear m1 : 52|12@1+ (0.1,0) [0|0] \"\" DBG\n";
    auto sig_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(sig_opt.has_value());
    auto msg = sig_opt.value();

    assert(msg.get_identifier() == 200U);
    assert(msg.get_name() == "SENSOR_SONARS");
    assert(msg.get_byte_count() == 8U);
    assert(msg.get_node() == "SENSOR");
    assert(msg.get_signals().size() == 10U);

    auto sig = msg.get_signals()[0];
    assert(sig.get_name() == "SENSOR_SONARS_mux");
    assert(sig.get_start_bit() == 0U);
    assert(sig.get_bit_count() == 4U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 0.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 2);
    assert(sig.get_nodes()[0] == "DRIVER");
    assert(sig.get_nodes()[1] == "IO");
    assert(std::get<bool>(sig.get_multiplexing()));

    sig = msg.get_signals()[1];
    assert(sig.get_name() == "SENSOR_SONARS_err_count");
    assert(sig.get_start_bit() == 4U);
    assert(sig.get_bit_count() == 12U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 0.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 2);
    assert(sig.get_nodes()[0] == "DRIVER");
    assert(sig.get_nodes()[1] == "IO");
    assert(!std::get<bool>(sig.get_multiplexing()));

    sig = msg.get_signals()[2];
    assert(sig.get_name() == "SENSOR_SONARS_left");
    assert(sig.get_start_bit() == 16U);
    assert(sig.get_bit_count() == 12U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 0.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 2);
    assert(sig.get_nodes()[0] == "DRIVER");
    assert(sig.get_nodes()[1] == "IO");
    assert(std::get<unsigned short>(sig.get_multiplexing()) == 0);

    sig = msg.get_signals()[3];
    assert(sig.get_name() == "SENSOR_SONARS_middle");
    assert(sig.get_start_bit() == 28U);
    assert(sig.get_bit_count() == 12U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 0.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 2);
    assert(sig.get_nodes()[0] == "DRIVER");
    assert(sig.get_nodes()[1] == "IO");
    assert(std::get<unsigned short>(sig.get_multiplexing()) == 0);

    sig = msg.get_signals()[4];
    assert(sig.get_name() == "SENSOR_SONARS_right");
    assert(sig.get_start_bit() == 40U);
    assert(sig.get_bit_count() == 12U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 0.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 2);
    assert(sig.get_nodes()[0] == "DRIVER");
    assert(sig.get_nodes()[1] == "IO");
    assert(std::get<unsigned short>(sig.get_multiplexing()) == 0);

    sig = msg.get_signals()[5];
    assert(sig.get_name() == "SENSOR_SONARS_rear");
    assert(sig.get_start_bit() == 52U);
    assert(sig.get_bit_count() == 12U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 0.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 2);
    assert(sig.get_nodes()[0] == "DRIVER");
    assert(sig.get_nodes()[1] == "IO");
    assert(std::get<unsigned short>(sig.get_multiplexing()) == 0);

    sig = msg.get_signals()[6];
    assert(sig.get_name() == "SENSOR_SONARS_no_filt_left");
    assert(sig.get_start_bit() == 16U);
    assert(sig.get_bit_count() == 12U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 0.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 1);
    assert(sig.get_nodes()[0] == "DBG");
    assert(std::get<unsigned short>(sig.get_multiplexing()) == 1);

    sig = msg.get_signals()[7];
    assert(sig.get_name() == "SENSOR_SONARS_no_filt_middle");
    assert(sig.get_start_bit() == 28U);
    assert(sig.get_bit_count() == 12U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 0.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 1);
    assert(sig.get_nodes()[0] == "DBG");
    assert(std::get<unsigned short>(sig.get_multiplexing()) == 1);

    sig = msg.get_signals()[8];
    assert(sig.get_name() == "SENSOR_SONARS_no_filt_right");
    assert(sig.get_start_bit() == 40U);
    assert(sig.get_bit_count() == 12U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 0.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 1);
    assert(sig.get_nodes()[0] == "DBG");
    assert(std::get<unsigned short>(sig.get_multiplexing()) == 1);

    sig = msg.get_signals()[9];
    assert(sig.get_name() == "SENSOR_SONARS_no_filt_rear");
    assert(sig.get_start_bit() == 52U);
    assert(sig.get_bit_count() == 12U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 0.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 1);
    assert(sig.get_nodes()[0] == "DBG");
    assert(std::get<unsigned short>(sig.get_multiplexing()) == 1);
}

int main() {
    test_empty_message();
    test_simple_message();
    test_multiplexed_message();

    return 0;
}