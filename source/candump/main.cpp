#include <cstdlib>
#include <variant>

#include "fmt/core.h"

#include "can/database/database.hpp"
#include "can/transceiver.hpp"

namespace arg {

static std::string program;
static std::string database;
static std::optional<bool> use_color = {};

static void show_help(auto f) {
    fmt::print(f, "Usage: {} [OPTION]...\n", program);
    fmt::print(f, "\n");
    fmt::print(f, "  --database PATH    path to a CAN database\n");
    fmt::print(f, "  --colors WHEN      enable/disable color printing\n");
    fmt::print(f, "                         choice: always, never [auto]\n");
    fmt::print(f, "  --help             show this help\n");
}

static void fail_unknown_option(const char* opt) {
    fmt::print(stderr, "{}: unrecognized option '{}'\n", program, opt);
    fmt::print(stderr, "Try '{} --help' for more information.\n", program);
    exit(1);
}

static void fail_missing_arg(const char* opt) {
    fmt::print(stderr, "{}: option '{}' requires an argument\n", program, opt);
    fmt::print(stderr, "Try '{} --help' for more information.\n", program);
    exit(1);
}

static void fail_invalid_choice(const char* choice, const char* opt) {
    fmt::print(stderr, "{}: invalid choice '{}' for option '{}'\n", program, choice, opt);
    fmt::print(stderr, "Try '{} --help' for more information.\n", program);
    exit(1);
}

static void parse(int argc, const char* argv[]) {
    program = std::string(argv[0]);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--database") == 0) {
            if (i > argc - 1) {
                fail_missing_arg(argv[i]);
            }

            database = std::string(argv[++i]);
        } else if (strcmp(argv[i], "--colors") == 0) {
            if (i > argc - 1) {
                fail_missing_arg(argv[i]);
            }

            const char* choice = argv[++i];
            if (strcmp(choice, "always") == 0) {
                use_color = true;
            } else if (strcmp(choice, "never") == 0) {
                use_color = false;
            } else if (strcmp(choice, "auto") == 0) {
                use_color = {};
            } else {
                fail_invalid_choice(choice, argv[i - 1]);
            }
        } else if (strcmp("--help", argv[i]) == 0) {
            show_help(stdout);
            exit(0);
        } else {
            fail_unknown_option(argv[i]);
        }
    }
}

} /* namespace arg */

template <>
struct fmt::formatter<can::frame::ptr> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const can::frame::ptr& frame, FormatContext& ctx) {
        auto out = fmt::format(" ({:10.6f})  ", frame->timestamp_ / 1e6);
        out += fmt::format("{:08X}   ", frame->identifier_);
        out += fmt::format("[{:d}]  ", frame->length_);
        for (auto i = 0U; i < frame->length_; i++) {
            out += fmt::format("{:02X} ", frame->bytes_[i]);
        }
        return fmt::format_to(ctx.out(), out);
    }
};

static bool print_message(can::database::database::ptr database, can::frame::ptr& frame) {
    if (database == nullptr) {
        return false;
    }

    auto message = database->get_message(frame->identifier_);
    if (message == nullptr) {
        return false;
    }

    auto values = message->decode(frame->bytes_, frame->length_);
    if (values.empty()) {
        fmt::print("{}()\n", message->get_name());
    } else {
        fmt::print("{}(\n", message->get_name());
        for (auto [signal, value] : values) {
            fmt::print("    {}: {} {}\n", signal->get_name(), value, signal->get_unit());
        }
        fmt::print(")\n", message->get_name());
    }

    return true;
}

int main(int argc, const char* argv[]) {
    if (argc < 1) {
        exit(1);
    }

    arg::parse(argc, argv);

    can::database::database::ptr database;
    if (!arg::database.empty()) {
        database = can::database::database::create(arg::database);
    }

    auto transceiver = can::transceiver::create("socketcan", "vcan0");
    if (transceiver == nullptr) {
        exit(1);
    }

    while (true) {
        auto frame = transceiver->receive();
        if (frame == nullptr) {
            continue;
        }

        fmt::print("{}\n", frame);
        print_message(database, frame);
    }

    return 0;
}