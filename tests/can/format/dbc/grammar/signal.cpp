#include <cassert>
#include <cmath>
#include <string>

#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"

#include "can/format/dbc/grammar/signal.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::format::dbc::grammar::signal>(input, ignore_error);
}

static bool almost_equal(float value, float expected) {
    constexpr float EPSILON = 0.00000000001F;
    return (std::fabs(value - expected) < EPSILON);
}

static void test_signal_simple_1() {
    char input[] = "SG_ IMU_GYRO_Z : 32|16@1- (0.015259,0) [-500|500] \"dps\" DEBUG\n";
    auto sig_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(sig_opt.has_value());
    auto sig = sig_opt.value();

    assert(sig.get_name() == "IMU_GYRO_Z");
    assert(sig.get_start_bit() == 32U);
    assert(sig.get_bit_count() == 16U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(sig.is_signed());
    assert(almost_equal(sig.get_scale(), 0.015259F));
    assert(almost_equal(sig.get_offset(), 0.0F));
    assert(almost_equal(sig.get_min(), -500.0F));
    assert(almost_equal(sig.get_max(), 500.0F));
    assert(sig.get_unit() == "dps");
    assert(sig.get_nodes().size() == 1U);
    assert(sig.get_nodes()[0] == "DEBUG");
    assert(!std::get<bool>(sig.get_multiplexing()));
}

static void test_signal_simple_2() {
    char input[] = "SG_ VOLTS_MIN : 0|16@0+ (0.0427249,1600) [1600|4400] \"mV\" DEBUG,TCS\n";
    auto sig_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(sig_opt.has_value());
    auto sig = sig_opt.value();

    assert(sig.get_name() == "VOLTS_MIN");
    assert(sig.get_start_bit() == 0U);
    assert(sig.get_bit_count() == 16U);
    assert(sig.get_byte_order() == can::database::signal::endian::BIG);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_scale(), 0.0427249F));
    assert(almost_equal(sig.get_offset(), 1600.0F));
    assert(almost_equal(sig.get_min(), 1600.0F));
    assert(almost_equal(sig.get_max(), 4400.0F));
    assert(sig.get_unit() == "mV");
    assert(sig.get_nodes().size() == 2U);
    assert(sig.get_nodes()[0] == "DEBUG");
    assert(sig.get_nodes()[1] == "TCS");
    assert(!std::get<bool>(sig.get_multiplexing()));
}

static void test_signal_simple_3() {
    char input[] = "SG_  AIR_MINUS_CLOSED :0|1@1+ (1,0)  [0|1] \"\"  DEBUG,TCS\n";
    auto sig_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(sig_opt.has_value());
    auto sig = sig_opt.value();

    assert(sig.get_name() == "AIR_MINUS_CLOSED");
    assert(sig.get_start_bit() == 0U);
    assert(sig.get_bit_count() == 1U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_scale(), 1.0F));
    assert(almost_equal(sig.get_offset(), 0.0F));
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 1.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 2U);
    assert(sig.get_nodes()[0] == "DEBUG");
    assert(sig.get_nodes()[1] == "TCS");
    assert(!std::get<bool>(sig.get_multiplexing()));
}

static void test_signal_multiplexer() {
    char input[] = "SG_ MULTIPLEXER M: 0|2@1+ (1,0) [0|1] \"\" DEBUG\n";
    auto sig_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(sig_opt.has_value());
    auto sig = sig_opt.value();

    assert(sig.get_name() == "MULTIPLEXER");
    assert(sig.get_start_bit() == 0U);
    assert(sig.get_bit_count() == 2U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_scale(), 1.0F));
    assert(almost_equal(sig.get_offset(), 0.0F));
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 1.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 1U);
    assert(sig.get_nodes()[0] == "DEBUG");
    assert(std::get<bool>(sig.get_multiplexing()));
}

static void test_signal_multiplexed() {
    char input[] = "SG_ MULTIPLEXED m12: 0|2@1+ (1,0) [0|1] \"\" DEBUG\n";
    auto sig_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(sig_opt.has_value());
    auto sig = sig_opt.value();

    assert(sig.get_name() == "MULTIPLEXED");
    assert(sig.get_start_bit() == 0U);
    assert(sig.get_bit_count() == 2U);
    assert(sig.get_byte_order() == can::database::signal::endian::LITTLE);
    assert(!sig.is_signed());
    assert(almost_equal(sig.get_scale(), 1.0F));
    assert(almost_equal(sig.get_offset(), 0.0F));
    assert(almost_equal(sig.get_min(), 0.0F));
    assert(almost_equal(sig.get_max(), 1.0F));
    assert(sig.get_unit().empty());
    assert(sig.get_nodes().size() == 1U);
    assert(sig.get_nodes()[0] == "DEBUG");
    assert(std::get<unsigned short>(sig.get_multiplexing()) == 12U);
}

static void test_signal_invalids() {
    // FIXME: assert(!parse(lexy::zstring_input("SG_ INVALID m : 0|1@1+ (1,0) [0|1] \"\" DEBUG\n")).has_value());
    assert(!parse(lexy::zstring_input<lexy::utf8_encoding>("SG_ INVALID a : 0|1@1+ (1,0) [0|1] \"\" DEBUG\n"))
                .has_value());
    assert(!parse(lexy::zstring_input<lexy::utf8_encoding>("SG_ INVALID M1 : 0|1@1+ (1,0) [0|1] \"\" DEBUG\n"))
                .has_value());
}

int main() {
    test_signal_simple_1();
    test_signal_simple_2();
    test_signal_simple_3();
    test_signal_multiplexer();
    test_signal_multiplexed();
    test_signal_invalids();

    return 0;
}