#include <array>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <utility>

#include "can/driver/socketcan.hpp"

static void print_message(can::frame::ptr msg) {
    printf("received: ts=%lu id=%u data={ ", msg->timestamp_, msg->identifier_);
    for (size_t i = 0; i < msg->length_; i++) {
        printf("%02X ", msg->bytes_[i]);
    }
    printf("}\n");
}

static void test_interface(const std::string& device) {
    auto transceiver1 = can::driver::socketcan::create(device);
    assert(transceiver1 != nullptr);

    auto transceiver2 = can::driver::socketcan::create(device);
    assert(transceiver2 != nullptr);

    for (int i = 0; i < 100; i++) {
        std::array<uint8_t, 8> bytes{};
        bytes[0]      = (2 * i) % 255;
        bytes[1]      = (4 * i) % 255;
        bytes[2]      = (5 * i) % 255;
        bytes[3]      = (1 * i) % 255;
        bytes[4]      = (8 * i) % 255;
        bytes[5]      = (9 * i) % 255;
        bytes[6]      = (3 * i) % 255;
        bytes[7]      = (7 * i) % 255;
        size_t length = i % 8 + 1;

        auto send_msg = can::frame::create(i, length, bytes.data());
        assert(transceiver1->transmit(std::move(send_msg)));

        auto recv_msg = transceiver2->receive();
        assert(recv_msg != nullptr);

        assert(recv_msg->identifier_ == static_cast<uint32_t>(i));
        assert(recv_msg->length_ == length);
        for (size_t i = 0; i < length; i++) {
            assert(recv_msg->bytes_[i] == bytes[i]);
        }

        print_message(std::move(recv_msg));
    }
}

int main() {
    auto interfaces = can::driver::socketcan::list_interfaces();
    if (interfaces->empty()) {
        std::cout << "No interface found" << std::endl;
        return 0;
    }

    for (const auto& interface : *interfaces) {
        std::cout << "Testing interface '" << interface << "'" << std::endl;
        test_interface(interface);
        std::cout << std::endl;
    }

    return 0;
}