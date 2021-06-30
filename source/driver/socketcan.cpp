#include <linux/can.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <regex>
#include <utility>

#include "libsocketcan.h"

#include "can/driver/socketcan.hpp"
#include "can/log.hpp"
#include "can/utils/crop_cast.hpp"

namespace can::driver {

static constexpr uint64_t SEC_TO_USEC = 1e6;

interface_list_ptr socketcan::list_interfaces() {
    const std::regex interface_regex(".*\\/(v?can\\d+)");
    const std::string sysfs_dir("/sys/class/net");

    auto interfaces = std::make_unique<std::list<std::string>>();
    for (const auto& it : std::filesystem::directory_iterator(sysfs_dir)) {
        std::string path = it.path().string();

        std::smatch matches;
        if (std::regex_search(path, matches, interface_regex)) {
            interfaces->push_back(matches[1].str());
        }
    }

    return interfaces;
}

socketcan_ptr socketcan::create(const std::string& interface) {
    ifreq ifr{};
    sockaddr_can addr{};

    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        logger->error("could not create socket: {}", strerror(errno));
        goto socket_failed;
    }

    /* retrieve the interface index */
    strncpy(ifr.ifr_name, interface.c_str(), interface.size() + 1);
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        logger->error("could not retrieve the interface '{}': {}", interface, strerror(errno));
        goto ioctl_failed;
    }

    /* bind socket to interface */
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    /* NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast): library type */
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        logger->error("could not bind socket from the interface '{}': {}", interface, strerror(errno));
        goto bind_failed;
    }

    return socketcan_ptr(new socketcan(sock, interface));

bind_failed:
ioctl_failed:
    if (close(sock) < 0) {
        logger->error("could not close socket of interface '{}': {}", interface, strerror(errno));
    }
socket_failed:
    return nullptr;
}

socketcan::socketcan(int socket, std::string interface) : socket_(socket), interface_(std::move(interface)) {}

socketcan::~socketcan() {
    if (close(socket_) < 0) {
        logger->error("could not close socket: {}", strerror(errno));
    }
}

bool socketcan::set_bitrate(unsigned long bitrate) {
    constexpr auto MAX_BITRATE = static_cast<unsigned long>(std::numeric_limits<uint32_t>::max());

    if (bitrate > MAX_BITRATE) {
        logger->error("invalid bitrate specified");
        return false;
    }

    if (can_set_bitrate(interface_.c_str(), static_cast<unsigned long>(bitrate)) < 0) {
        logger->error("could not set bitrate");
        return false;
    }

    return true;
}

bool socketcan::transmit(frame::ptr msg) {
    if (CAN_MAX_DLEN < msg->length_) {
        logger->error("invalid message length");
        return false;
    }

    can_frame frame{};
    frame.can_id = msg->identifier_;
    frame.len    = msg->length_;
    /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): library function */
    std::copy(msg->bytes_, msg->bytes_ + msg->length_, frame.data);

    ssize_t length = write(socket_, &frame, sizeof(frame));
    if (length < 0) {
        logger->error("could not write to socket: {}", strerror(errno));
        return false;
    }

    if (static_cast<size_t>(length) < sizeof(frame)) {
        logger->error("invalid length written");
        return false;
    }

    return true;
}

frame::ptr socketcan::receive(long timeout_ms) {
    /*
     * We need to lock between poll() and ioctl() since the
     * returned timestamp is for the last received message.
     */
    const std::lock_guard<std::mutex> lock(receive_mutex_);

    int count = -1;
    while (count <= 0) {
        pollfd pfd = {.fd = socket_, .events = POLLIN};

        count = poll(&pfd, 1, utils::crop_cast<long, int>(timeout_ms));
        if (count < 0) {
            if (errno == EINTR) {
                continue;
            }

            logger->error("could not poll socket: {}", strerror(errno));
            return nullptr;
        }

        if (count == 0) {
            return nullptr;
        }
    }

    can_frame frame{};
    ssize_t length = read(socket_, &frame, sizeof(frame));
    if (length < 0) {
        logger->error("could not read socket: {}", strerror(errno));
        return nullptr;
    }

    if (static_cast<size_t>(length) < sizeof(frame)) {
        logger->error("invalid length received");
        return nullptr;
    }

    timeval tv{};
    if (ioctl(socket_, SIOCGSTAMP, &tv) < 0) {
        logger->error("could not read message timestamp: {}", strerror(errno));
        return nullptr;
    }
    uint64_t timestamp = tv.tv_sec * SEC_TO_USEC + tv.tv_usec;

    return frame::create(frame.can_id & CAN_SFF_MASK, frame.len, frame.data, timestamp);
}

} /* namespace can::driver */