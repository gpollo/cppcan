#include <linux/can.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>

#include "can/driver/socketcan.hpp"
#include "can/log.hpp"

namespace can::driver {

static constexpr uint64_t SEC_TO_USEC = 1e6;

socketcan_ptr socketcan::create(const std::string& interface) {
    ifreq ifr{};
    sockaddr_can addr{};

    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        CAN_LOG_PERROR("socket");
        goto socket_failed;
    }

    /* retrieve the interface index */
    strncpy(ifr.ifr_name, interface.c_str(), interface.size() + 1);
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        CAN_LOG_PERROR("ioctl");
        goto ioctl_failed;
    }

    /* bind socket to interface */
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    /* NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast): library type */
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        CAN_LOG_PERROR("bind");
        goto bind_failed;
    }

    return socketcan_ptr(new socketcan(sock));

bind_failed:
ioctl_failed:
    if (close(sock) < 0) {
        CAN_LOG_PERROR("close");
    }
socket_failed:
    return nullptr;
}

socketcan::socketcan(int socket) : socket_(socket) {}

socketcan::~socketcan() {
    if (close(socket_) < 0) {
        CAN_LOG_PERROR("close");
    }
}

bool socketcan::transmit(frame::ptr msg) {
    if (CAN_MAX_DLEN < msg->length_) {
        CAN_LOG_ERROR("transmit: Invalid length");
        return false;
    }

    can_frame frame{};
    frame.can_id = msg->identifier_;
    frame.len    = msg->length_;
    /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): library function */
    std::copy(msg->bytes_, msg->bytes_ + msg->length_, frame.data);

    ssize_t length = write(socket_, &frame, sizeof(frame));
    if (length < 0) {
        CAN_LOG_PERROR("write");
        return false;
    }

    if (static_cast<size_t>(length) < sizeof(frame)) {
        CAN_LOG_ERROR("transmit: Invalid length");
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

        count = poll(&pfd, 1, (int)timeout_ms);
        if (count < 0) {
            if (errno == EINTR) {
                continue;
            }

            CAN_LOG_PERROR("poll");
            return nullptr;
        }

        if (count == 0) {
            return nullptr;
        }
    }

    can_frame frame{};
    ssize_t length = read(socket_, &frame, sizeof(frame));
    if (length < 0) {
        CAN_LOG_PERROR("read");
        return nullptr;
    }

    if (static_cast<size_t>(length) < sizeof(frame)) {
        CAN_LOG_ERROR("receive: Invalid length");
        return nullptr;
    }

    timeval tv{};
    if (ioctl(socket_, SIOCGSTAMP, &tv) < 0) {
        CAN_LOG_PERROR("ioctl");
        return nullptr;
    }
    uint64_t timestamp = tv.tv_sec * SEC_TO_USEC + tv.tv_usec;

    return frame::create(frame.can_id & CAN_SFF_MASK, frame.len, frame.data, timestamp);
}

} /* namespace can::driver */