#ifndef INCLUDE_CAN_DATABASE_DBC_GRAMMAR_SIGNAL_HPP
#define INCLUDE_CAN_DATABASE_DBC_GRAMMAR_SIGNAL_HPP

#include <array>
#include <string>
#include <vector>

#include "lexy/callback/base.hpp"
#include "lexy/callback/integer.hpp"
#include "lexy/callback/object.hpp"
#include "lexy/dsl.hpp"
#include "lexy/dsl/ascii.hpp"
#include "lexy/dsl/branch.hpp"
#include "lexy/dsl/capture.hpp"
#include "lexy/dsl/code_point.hpp"
#include "lexy/dsl/delimited.hpp"
#include "lexy/dsl/digit.hpp"
#include "lexy/dsl/eof.hpp"
#include "lexy/dsl/if.hpp"
#include "lexy/dsl/integer.hpp"
#include "lexy/dsl/list.hpp"
#include "lexy/dsl/literal.hpp"
#include "lexy/dsl/loop.hpp"
#include "lexy/dsl/newline.hpp"
#include "lexy/dsl/option.hpp"
#include "lexy/dsl/production.hpp"
#include "lexy/dsl/punctuator.hpp"
#include "lexy/dsl/separator.hpp"
#include "lexy/dsl/sign.hpp"

#include "can/format/dbc/ast/signal.hpp"
#include "can/utils/grammar/c_identifier.hpp"
#include "can/utils/grammar/floating_point.hpp"

namespace can::format::dbc::grammar {

struct signal {
    static constexpr auto rule_whitespace = lexy::dsl::whitespace(lexy::dsl::ascii::blank);

    static constexpr auto rule_colon = lexy::dsl::colon;

    static constexpr auto rule_prefix = rule_whitespace + LEXY_LIT("SG_");

    static constexpr auto rule_name = lexy::dsl::p<can::utils::grammar::c_identifier>;

    struct multiplexing {
        static constexpr auto rule =
            (lexy::dsl::lit_c<'m'> >> lexy::dsl::integer<unsigned short>(lexy::dsl::digits<>)) |
            lexy::dsl::capture(lexy::dsl::lit_c<'M'>) | (lexy::dsl::else_ >> rule_whitespace);

        static constexpr auto value =
            lexy::callback<std::variant<bool, unsigned short>>([](unsigned short value) { return value; },

                                                               [](auto n) { return (n[0] == 'M'); },

                                                               []() { return false; });
    };

    static constexpr auto rule_multiplexing = lexy::dsl::p<multiplexing>;

    struct endianness {
        static constexpr auto rule = lexy::dsl::capture(lexy::dsl::lit_c<'0'> / lexy::dsl::lit_c<'1'>);

        static constexpr auto value = lexy::callback<can::database::signal::endian>([](auto c) {
            return (c[0] == '0') ? can::database::signal::endian::BIG : can::database::signal::endian::LITTLE;
        });
    };

    struct signess {
        static constexpr auto rule = lexy::dsl::capture(lexy::dsl::lit_c<'-'> / lexy::dsl::lit_c<'+'>);

        static constexpr auto value = lexy::callback<bool>([](auto c) { return (c[0] == '-'); });
    };

    static constexpr auto rule_bits = [] {
        auto start_bit  = lexy::dsl::integer<unsigned short>(lexy::dsl::digits<>);
        auto pipe       = lexy::dsl::lit_c<'|'>;
        auto bit_count  = lexy::dsl::integer<unsigned short>(lexy::dsl::digits<>);
        auto at         = lexy::dsl::at_sign;
        auto byte_order = lexy::dsl::p<endianness>;
        auto is_signed  = lexy::dsl::p<signess>;

        return start_bit + pipe + bit_count + at + byte_order + is_signed;
    }();

    static constexpr auto rule_offset_scale = [] {
        auto paren_open  = lexy::dsl::lit_c<'('>;
        auto scale       = lexy::dsl::p<utils::grammar::floating_point>;
        auto comma       = lexy::dsl::comma;
        auto offset      = lexy::dsl::p<utils::grammar::floating_point>;
        auto paren_close = lexy::dsl::lit_c<')'>;

        return paren_open + scale + comma + offset + paren_close;
    }();

    /* TODO: no min/max if both are '0' */
    static constexpr auto rule_min_max = [] {
        auto bracket_open  = lexy::dsl::lit_c<'['>;
        auto min           = lexy::dsl::p<utils::grammar::floating_point>;
        auto pipe          = lexy::dsl::lit_c<'|'>;
        auto max           = lexy::dsl::p<utils::grammar::floating_point>;
        auto bracket_close = lexy::dsl::lit_c<']'>;

        return bracket_open + min + pipe + max + bracket_close;
    }();

    struct unit {
        static constexpr auto rule = lexy::dsl::quoted(lexy::dsl::code_point - lexy::dsl::ascii::control);

        static constexpr auto value = lexy::as_string<std::string>;
    };

    static constexpr auto rule_unit = lexy::dsl::p<unit>;

    struct nodes {
        static constexpr auto rule =
            lexy::dsl::list(lexy::dsl::p<utils::grammar::c_identifier>, lexy::dsl::sep(lexy::dsl::comma));

        static constexpr auto value = lexy::as_list<std::vector<std::string>>;
    };

    static constexpr auto rule_nodes = lexy::dsl::p<nodes>;

    static constexpr auto rule_eol = lexy::dsl::eol;

    static constexpr auto rule = lexy::dsl::peek(rule_prefix) >>
                                 (rule_prefix + rule_whitespace + rule_name + rule_whitespace + rule_multiplexing +
                                  rule_whitespace + rule_colon + rule_whitespace + rule_bits + rule_whitespace +
                                  rule_offset_scale + rule_whitespace + rule_min_max + rule_whitespace + rule_unit +
                                  rule_whitespace + rule_nodes + rule_eol);

    static constexpr auto value = lexy::callback<ast::signal>(
        [](std::string name, std::variant<bool, unsigned short> multiplexing, unsigned short start_bit,
           unsigned short bit_count, auto byte_order, auto is_signed, float scale, float offset, float min, float max,
           std::string unit, std::vector<std::string> nodes) {
            return ast::signal(name, start_bit, bit_count, byte_order, is_signed, scale, offset, min, max, unit, nodes,
                               multiplexing);
        });
};

} /* namespace can::format::dbc::grammar */

#endif /* INCLUDE_CAN_DATABASE_DBC_GRAMMAR_SIGNAL_HPP */