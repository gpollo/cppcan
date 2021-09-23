#include <array>
#include <cstring>
#include <map>

#ifdef BUILD_LINUX
#include <poll.h>
#endif /* BUILD_LINUX */

#ifdef BUILD_WINDOWS
#include <can/utils/windows.hpp>
#undef interface
#endif /* BUILD_WINDOWS */

#include "PCANBasic.h"

#include "can/driver/pcan.hpp"
#include "can/log.hpp"
#include "can/utils/crop_cast.hpp"

namespace can::driver {

const static std::map<std::string, unsigned int> INTERFACE_TO_DEVICE = {
    {"PCAN_USBBUS1", PCAN_USBBUS1},   {"PCAN_USBBUS2", PCAN_USBBUS2},   {"PCAN_USBBUS3", PCAN_USBBUS3},
    {"PCAN_USBBUS4", PCAN_USBBUS4},   {"PCAN_USBBUS5", PCAN_USBBUS5},   {"PCAN_USBBUS6", PCAN_USBBUS6},
    {"PCAN_USBBUS7", PCAN_USBBUS7},   {"PCAN_USBBUS8", PCAN_USBBUS8},   {"PCAN_USBBUS9", PCAN_USBBUS9},
    {"PCAN_USBBUS10", PCAN_USBBUS10}, {"PCAN_USBBUS11", PCAN_USBBUS11}, {"PCAN_USBBUS12", PCAN_USBBUS12},
    {"PCAN_USBBUS13", PCAN_USBBUS13}, {"PCAN_USBBUS14", PCAN_USBBUS14}, {"PCAN_USBBUS15", PCAN_USBBUS15},
    {"PCAN_USBBUS16", PCAN_USBBUS16}};

const static std::map<unsigned int, std::string> DEVICE_TO_INTERFACE = {
    {PCAN_USBBUS1, "PCAN_USBBUS1"},   {PCAN_USBBUS2, "PCAN_USBBUS2"},   {PCAN_USBBUS3, "PCAN_USBBUS3"},
    {PCAN_USBBUS4, "PCAN_USBBUS4"},   {PCAN_USBBUS5, "PCAN_USBBUS5"},   {PCAN_USBBUS6, "PCAN_USBBUS6"},
    {PCAN_USBBUS7, "PCAN_USBBUS7"},   {PCAN_USBBUS8, "PCAN_USBBUS8"},   {PCAN_USBBUS9, "PCAN_USBBUS9"},
    {PCAN_USBBUS10, "PCAN_USBBUS10"}, {PCAN_USBBUS11, "PCAN_USBBUS11"}, {PCAN_USBBUS12, "PCAN_USBBUS12"},
    {PCAN_USBBUS13, "PCAN_USBBUS13"}, {PCAN_USBBUS14, "PCAN_USBBUS14"}, {PCAN_USBBUS15, "PCAN_USBBUS15"},
    {PCAN_USBBUS16, "PCAN_USBBUS16"}};

static constexpr uint64_t MSEC_TO_USEC = 1000;
static constexpr uint32_t SHIFT32      = 32;
static constexpr unsigned int MAX_DLC  = 8;

static std::string get_error(TPCANStatus status) {
    std::array<char, 256> pcan_error_buffer{};
    CAN_GetErrorText(status, 0, pcan_error_buffer.data());
    return std::string(pcan_error_buffer.data());
}

std::list<std::string> pcan::list_interfaces() {
    TPCANStatus status            = 0;
    TPCANChannelInformation* info = nullptr;
    std::list<std::string> interfaces;

    DWORD channel_count;
    status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS_COUNT, (void*)&channel_count, sizeof(channel_count));
    if (status != PCAN_ERROR_OK) {
        logger->error("could not get channel count: {}", get_error(status));
        goto channel_count_failed;
    }

    if (channel_count == 0) {
        return {};
    }

    info = new TPCANChannelInformation[channel_count];

    status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS, (void*)info,
                          channel_count * sizeof(TPCANChannelInformation));
    if (status != PCAN_ERROR_OK) {
        logger->error("could not get attached channels: {}", get_error(status));
        goto attached_channels_failed;
    }

    for (int i = 0; i < (int)channel_count; i++) {
        if (!(info[i].channel_condition & PCAN_CHANNEL_AVAILABLE)) {
            continue;
        }

        if (!DEVICE_TO_INTERFACE.contains(info[i].channel_handle)) {
            logger->warn("unknown channel handle {}", info[i].channel_handle);
            continue;
        }

        interfaces.push_back(DEVICE_TO_INTERFACE.at(info[i].channel_handle));
    }

    delete[] info;
    return interfaces;

attached_channels_failed:
    delete[] info;
channel_count_failed:
    return {};
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

#ifdef BUILD_LINUX
    status = CAN_GetValue(device, PCAN_RECEIVE_EVENT, &event, sizeof(event));
    if (status != PCAN_ERROR_OK) {
        logger->error("could not get event of interface '{}': {}", interface, get_error(status));
        goto pcan_receive_event_failed;
    }
#endif /* BUILD_LINUX */

#ifdef BUILD_WINDOWS
    event = CreateEvent(nullptr, FALSE, FALSE, "");
    if (event == nullptr) {
        logger->error("could not create event handler: {}", utils::windows::get_last_error());
        goto create_event_failed;
    }

    status = CAN_SetValue(device, PCAN_RECEIVE_EVENT, &event, sizeof(event));
    if (status != PCAN_ERROR_OK) {
        logger->error("could not set event of interface '{}': {}", interface, get_error(status));
        goto pcan_receive_event_failed;
    }
#endif /* BUILD_WINDOWS */

    return ptr(std::shared_ptr<pcan>(new pcan(device, event)));

pcan_receive_event_failed:
#ifdef BUILD_WINDOWS
    CloseHandle(event);
#endif /* BUILD_WINDOWS */
create_event_failed:
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

    CloseHandle(event_);
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

frame::ptr pcan::try_receive() {
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

frame::ptr pcan::receive(long timeout_ms) {
    auto frame = try_receive();
    if (frame != nullptr) {
        return frame;
    }

    ResetEvent(event_);

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
    auto timeout = (timeout_ms < 0) ? INFINITE : utils::crop_cast<long, DWORD>(timeout_ms);
    if (WaitForSingleObject(event_, timeout) == WAIT_FAILED) {
        logger->error("failed to wait for event: {}", utils::windows::get_last_error());
        return nullptr;
    }
#endif /* BUILD_WINDOWS */

    return try_receive();
}

} /* namespace can::driver */