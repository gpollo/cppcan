#include <cassert>
#include <string>

#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"

#include "can/format/dbc/grammar/requirements.hpp"

static constexpr auto ignore_error = lexy::callback([](const auto& context, const auto& error) {});

static auto parse(auto input) {
    return lexy::parse<can::format::dbc::grammar::requirements>(input, ignore_error);
}

static void test_simple_requirements() {
    char input[] =
        "NS_ : \n"
        " BA_   \n"
        "   BA_DEF_\n"
        "   BA_DEF_DEF_  \n"
        "    BA_DEF_DEF_REL_\n"
        "   BA_DEF_REL_\n"
        "   BA_DEF_SGTYPE_\n"
        "   BA_REL_\n"
        "   BA_SGTYPE_\n"
        "   BO_TX_BU_\n"
        "   BU_BO_REL_ \n"
        "   BU_EV_REL_\n"
        "   VAL_\n"
        "   VAL_TABLE_\n";
    auto requirements_opt = parse(lexy::string_input<lexy::utf8_encoding>(input, sizeof(input)));
    assert(requirements_opt.has_value());
    auto requirements = requirements_opt.value();

    assert(requirements.size() == 13);
    assert(requirements[0] == "BA_");
    assert(requirements[1] == "BA_DEF_");
    assert(requirements[2] == "BA_DEF_DEF_");
    assert(requirements[3] == "BA_DEF_DEF_REL_");
    assert(requirements[4] == "BA_DEF_REL_");
    assert(requirements[5] == "BA_DEF_SGTYPE_");
    assert(requirements[6] == "BA_REL_");
    assert(requirements[7] == "BA_SGTYPE_");
    assert(requirements[8] == "BO_TX_BU_");
    assert(requirements[9] == "BU_BO_REL_");
    assert(requirements[10] == "BU_EV_REL_");
    assert(requirements[11] == "VAL_");
    assert(requirements[12] == "VAL_TABLE_");
}

int main() {
    test_simple_requirements();

    return 0;
}