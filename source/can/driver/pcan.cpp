#include <array>
#include <cstring>
#include <map>

#ifdef BUILD_LINUX
#include <poll.h>
#endif /* BUILD_LINUX */

#ifdef BUILD_WINDOWS
#include <windows.h>
#undef interface
#endif /* BUILD_WINDOWS */

#include "PCANBasic.h"

#include "can/driver/pcan.hpp"
#include "can/log.hpp"
#include "can/utils/crop_cast.hpp"

namespace can::driver {

const static std::map<std::string, unsigned int> INTERFACE_TO_DEVICE = {
    {"PCAN_ISABUS1", PCAN_ISABUS1},   {"PCAN_ISABUS2", PCAN_ISABUS2},   {"PCAN_ISABUS3", PCAN_ISABUS3},
    {"PCAN_ISABUS4", PCAN_ISABUS4},   {"PCAN_ISABUS5", PCAN_ISABUS5},   {"PCAN_ISABUS6", PCAN_ISABUS6},
    {"PCAN_ISABUS7", PCAN_ISABUS7},   {"PCAN_ISABUS8", PCAN_ISABUS8},   {"PCAN_DNGBUS1", PCAN_DNGBUS1},
    {"PCAN_PCIBUS1", PCAN_PCIBUS1},   {"PCAN_PCIBUS2", PCAN_PCIBUS2},   {"PCAN_PCIBUS3", PCAN_PCIBUS3},
    {"PCAN_PCIBUS4", PCAN_PCIBUS4},   {"PCAN_PCIBUS5", PCAN_PCIBUS5},   {"PCAN_PCIBUS6", PCAN_PCIBUS6},
    {"PCAN_PCIBUS7", PCAN_PCIBUS7},   {"PCAN_PCIBUS8", PCAN_PCIBUS8},   {"PCAN_PCIBUS9", PCAN_PCIBUS9},
    {"PCAN_PCIBUS10", PCAN_PCIBUS10}, {"PCAN_PCIBUS11", PCAN_PCIBUS11}, {"PCAN_PCIBUS12", PCAN_PCIBUS12},
    {"PCAN_PCIBUS13", PCAN_PCIBUS13}, {"PCAN_PCIBUS14", PCAN_PCIBUS14}, {"PCAN_PCIBUS15", PCAN_PCIBUS15},
    {"PCAN_PCIBUS16", PCAN_PCIBUS16}, {"PCAN_USBBUS1", PCAN_USBBUS1},   {"PCAN_USBBUS2", PCAN_USBBUS2},
    {"PCAN_USBBUS3", PCAN_USBBUS3},   {"PCAN_USBBUS4", PCAN_USBBUS4},   {"PCAN_USBBUS5", PCAN_USBBUS5},
    {"PCAN_USBBUS6", PCAN_USBBUS6},   {"PCAN_USBBUS7", PCAN_USBBUS7},   {"PCAN_USBBUS8", PCAN_USBBUS8},
    {"PCAN_USBBUS9", PCAN_USBBUS9},   {"PCAN_USBBUS10", PCAN_USBBUS10}, {"PCAN_USBBUS11", PCAN_USBBUS11},
    {"PCAN_USBBUS12", PCAN_USBBUS12}, {"PCAN_USBBUS13", PCAN_USBBUS13}, {"PCAN_USBBUS14", PCAN_USBBUS14},
    {"PCAN_USBBUS15", PCAN_USBBUS15}, {"PCAN_USBBUS16", PCAN_USBBUS16}, {"PCAN_PCCBUS1", PCAN_PCCBUS1},
    {"PCAN_PCCBUS2", PCAN_PCCBUS2},   {"PCAN_LANBUS1", PCAN_LANBUS1},   {"PCAN_LANBUS2", PCAN_LANBUS2},
    {"PCAN_LANBUS3", PCAN_LANBUS3},   {"PCAN_LANBUS4", PCAN_LANBUS4},   {"PCAN_LANBUS5", PCAN_LANBUS5},
    {"PCAN_LANBUS6", PCAN_LANBUS6},   {"PCAN_LANBUS7", PCAN_LANBUS7},   {"PCAN_LANBUS8", PCAN_LANBUS8},
    {"PCAN_LANBUS9", PCAN_LANBUS9},   {"PCAN_LANBUS10", PCAN_LANBUS10}, {"PCAN_LANBUS11", PCAN_LANBUS11},
    {"PCAN_LANBUS12", PCAN_LANBUS12}, {"PCAN_LANBUS13", PCAN_LANBUS13}, {"PCAN_LANBUS14", PCAN_LANBUS14},
    {"PCAN_LANBUS15", PCAN_LANBUS15}, {"PCAN_LANBUS16", PCAN_LANBUS16}};

static constexpr uint64_t MSEC_TO_USEC = 1000;
static constexpr uint32_t SHIFT32      = 32;
static constexpr unsigned int MAX_DLC  = 8;

static std::string get_error(TPCANStatus status) {
    std::array<char, 256> pcan_error_buffer{};
    CAN_GetErrorText(status, 0, pcan_error_buffer.data());
    return std::string(pcan_error_buffer.data());
}

pcan::ptr pcan::create(const std::string& interface) {
#ifdef BUILD_LINUX
    pcan::event_type event = 0;
#endif /* BUILD_LINUX */

#ifdef BUILD_WINDOWS
    pcan::event_type event = nullptr;
#endif /* BUILD_WINDOWS */

    unsigned int device = 0;
    TPCANStatus status  = 0;

    if (!INTERFACE_TO_DEVICE.contains(interface)) {
        logger->error("invalid specified interface '{}'", interface);
        goto invalid_interface;
    }
    device = INTERFACE_TO_DEVICE.at(interface);

    status = CAN_Initialize(device, PCAN_BAUD_500K, 0, 0, 0);
    if (status != PCAN_ERROR_OK) {
        logger->error("could not initialize interface '{}': {}", interface, get_error(status));
        goto initialize_failed;
    }

    status = CAN_GetValue(device, PCAN_RECEIVE_EVENT, &event, sizeof(event));
    if (status != PCAN_ERROR_OK) {
        logger->error("could not get event of interface '{}': {}", interface, get_error(status));
        goto get_value_failed;
    }

    return ptr(std::shared_ptr<pcan>(new pcan(device, event)));

get_value_failed:
    status = CAN_Uninitialize(device);
    if (status != PCAN_ERROR_OK) {
        logger->error("could not uninitialize interface '{}': {}", interface, get_error(status));
    }
initialize_failed:
invalid_interface:
    return nullptr;
}

pcan::pcan(unsigned int device, event_type event) : device_(device), event_(event) {}

pcan::~pcan() {
    TPCANStatus status = CAN_Uninitialize(device_);
    if (status != PCAN_ERROR_OK) {
        logger->error("could not uninitialize interface: {}", get_error(status));
    }
}

bool pcan::set_bitrate(unsigned long bitrate) {
    constexpr auto MAX_BITRATE = static_cast<unsigned long>((std::numeric_limits<uint32_t>::max)());

    if (bitrate > MAX_BITRATE) {
        logger->error("invalid bitrate specified");
        return false;
    }

    uint32_t buffer    = bitrate;
    TPCANStatus status = CAN_SetValue(device_, PCAN_BITRATE_INFO, &buffer, sizeof(buffer));
    if (status != PCAN_ERROR_OK) {
        logger->error("could not set bitrate: {}", get_error(status));
        return false;
    }

    return true;
}

bool pcan::transmit(frame::ptr msg) {
    if (msg->length_ > MAX_DLC) {
        logger->error("invalid message length");
        return false;
    }

    TPCANMsg frame;
    frame.ID      = msg->identifier_;
    frame.LEN     = msg->length_;
    frame.MSGTYPE = PCAN_MESSAGE_STANDARD;
    /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): library function */
    std::copy(msg->bytes_, msg->bytes_ + msg->length_, frame.DATA);

    TPCANStatus status = CAN_Write(device_, &frame);
    if (status != PCAN_ERROR_OK) {
        logger->error("could not write message: {}", get_error(status));
        return false;
    }

    return true;
}

frame::ptr pcan::receive(long timeout_ms) {
#ifdef BUILD_LINUX
    int count = -1;
    while (count <= 0) {
        pollfd pfd = {.fd = event_, .events = POLLIN};

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
#endif /* BUILD_LINUX */

#ifdef BUILD_WINDOWS
    if (WaitForSingleObject(event_, utils::crop_cast<long, DWORD>(timeout_ms)) == WAIT_FAILED) {
        logger->error("failed to wait for event");
        return nullptr;
    }
#endif /* BUILD_WINDOWS */

    TPCANMsg frame;
    TPCANTimestamp ts;
    TPCANStatus status = CAN_Read(device_, &frame, &ts);
    if (status != PCAN_ERROR_OK) {
        if (status != PCAN_ERROR_QRCVEMPTY) {
            logger->error("could not read message: {}", get_error(status));
        }

        return nullptr;
    }

    uint64_t timestamp = (static_cast<uint64_t>(ts.millis_overflow) << SHIFT32) + ts.millis;
    timestamp          = timestamp * MSEC_TO_USEC + ts.micros;

    return frame::create(frame.ID, frame.LEN, frame.DATA, timestamp);
}

} /* namespace can::driver */