#include <algorithm>
#include <map>
#include <memory>

#include "candle.h"

#include "can/driver/candlelight.hpp"
#include "can/log.hpp"
#include "can/utils/crop_cast.hpp"

namespace can::driver {

static const std::map<candle_err_t, std::string> ERROR_TO_STRING = {
    {CANDLE_ERR_OK, "CANDLE_ERR_OK"},
    {CANDLE_ERR_CREATE_FILE, "CANDLE_ERR_CREATE_FILE"},
    {CANDLE_ERR_WINUSB_INITIALIZE, "CANDLE_ERR_WINUSB_INITIALIZE"},
    {CANDLE_ERR_QUERY_INTERFACE, "CANDLE_ERR_QUERY_INTERFACE"},
    {CANDLE_ERR_QUERY_PIPE, "CANDLE_ERR_QUERY_PIPE"},
    {CANDLE_ERR_PARSE_IF_DESCR, "CANDLE_ERR_PARSE_IF_DESCR"},
    {CANDLE_ERR_SET_HOST_FORMAT, "CANDLE_ERR_SET_HOST_FORMAT"},
    {CANDLE_ERR_GET_DEVICE_INFO, "CANDLE_ERR_GET_DEVICE_INFO"},
    {CANDLE_ERR_GET_BITTIMING_CONST, "CANDLE_ERR_GET_BITTIMING_CONST"},
    {CANDLE_ERR_PREPARE_READ, "CANDLE_ERR_PREPARE_READ"},
    {CANDLE_ERR_SET_DEVICE_MODE, "CANDLE_ERR_SET_DEVICE_MODE"},
    {CANDLE_ERR_SET_BITTIMING, "CANDLE_ERR_SET_BITTIMING"},
    {CANDLE_ERR_BITRATE_FCLK, "CANDLE_ERR_BITRATE_FCLK"},
    {CANDLE_ERR_BITRATE_UNSUPPORTED, "CANDLE_ERR_BITRATE_UNSUPPORTED"},
    {CANDLE_ERR_SEND_FRAME, "CANDLE_ERR_SEND_FRAME"},
    {CANDLE_ERR_READ_TIMEOUT, "CANDLE_ERR_READ_TIMEOUT"},
    {CANDLE_ERR_READ_WAIT, "CANDLE_ERR_READ_WAIT"},
    {CANDLE_ERR_READ_RESULT, "CANDLE_ERR_READ_RESULT"},
    {CANDLE_ERR_READ_SIZE, "CANDLE_ERR_READ_SIZE"},
    {CANDLE_ERR_SETUPDI_IF_DETAILS, "CANDLE_ERR_SETUPDI_IF_DETAILS"},
    {CANDLE_ERR_SETUPDI_IF_DETAILS2, "CANDLE_ERR_SETUPDI_IF_DETAILS2"},
    {CANDLE_ERR_MALLOC, "CANDLE_ERR_MALLOC"},
    {CANDLE_ERR_PATH_LEN, "CANDLE_ERR_PATH_LEN"},
    {CANDLE_ERR_CLSID, "CANDLE_ERR_CLSID"},
    {CANDLE_ERR_GET_DEVICES, "CANDLE_ERR_GET_DEVICES"},
    {CANDLE_ERR_SETUPDI_IF_ENUM, "CANDLE_ERR_SETUPDI_IF_ENUM"},
    {CANDLE_ERR_SET_TIMESTAMP_MODE, "CANDLE_ERR_SET_TIMESTAMP_MODE"},
    {CANDLE_ERR_DEV_OUT_OF_RANGE, "CANDLE_ERR_DEV_OUT_OF_RANGE"},
    {CANDLE_ERR_GET_TIMESTAMP, "CANDLE_ERR_GET_TIMESTAMP"},
    {CANDLE_ERR_SET_PIPE_RAW_IO, "CANDLE_ERR_SET_PIPE_RAW_IO"},
};

static constexpr unsigned int MAX_DLC = 8;

static std::string get_error(candle_handle handle) {
    return ERROR_TO_STRING.at(candle_dev_last_error(handle));
}

interface_list_ptr candlelight::list_interfaces() {
    auto interfaces = std::make_unique<std::list<std::string>>();

    candle_list_handle list = nullptr;
    if (!candle_list_scan(&list)) {
        logger->error("could not list candle devices: UNKNOWN_ERROR");
        goto candle_list_scan_failed;
    }

    uint8_t device_count;
    if (!candle_list_length(list, &device_count)) {
        logger->error("could get list length: UNKNOWN_ERROR");
        goto candle_list_length_failed;
    }

    for (uint8_t i = 0; i < device_count; i++) {
        interfaces->push_back("DEV" + std::to_string(i));
    }

    candle_list_free(list);

    return interfaces;

candle_list_length_failed:
    candle_list_free(list);
candle_list_scan_failed:
    return nullptr;
}

candlelight::ptr candlelight::create(uint8_t device) {
    candle_list_handle handle_list = nullptr;
    candle_handle handle           = nullptr;
    candle_devstate_t state        = CANDLE_DEVSTATE_INUSE;

    if (!candle_list_scan(&handle_list)) {
        logger->error("could not list candle devices: UNKNOWN_ERROR");
        goto candle_list_scan_failed;
    }

    if (!candle_dev_get(handle_list, device, &handle)) {
        logger->error("could not get candle device '{}': {}", device, get_error(handle));
        goto candle_dev_get_failed;
    }

    if (!candle_dev_get_state(handle, &state)) {
        logger->error("could not get candle device '{}': {}", device, get_error(handle));
        goto candle_dev_get_state_failed;
    }

    if (state == CANDLE_DEVSTATE_INUSE) {
        logger->error("could not get candle device '{}': DEVSTATE_INUSE", device);
        goto device_in_use;
    }

    if (!candle_dev_open(handle)) {
        logger->error("could not open candle device '{}': {}", get_error(handle));
        goto candle_dev_open_failed;
    }

    candle_list_free(handle_list);

    return ptr(std::shared_ptr<candlelight>(new candlelight(handle)));

candle_dev_open_failed:
device_in_use:
candle_dev_get_state_failed:
    candle_dev_free(handle);
candle_dev_get_failed:
    candle_list_free(handle_list);
candle_list_scan_failed:
    return nullptr;
}

candlelight::candlelight(void* handle) : handle_(handle) {}

candlelight::~candlelight() {
    candle_dev_close(handle_);
}

bool candlelight::set_bitrate(unsigned long bitrate) {
    constexpr auto MAX_BITRATE = static_cast<unsigned long>(std::numeric_limits<uint32_t>::max());

    if (bitrate > MAX_BITRATE) {
        logger->error("invalid bitrate specified");
        return false;
    }

    if (!candle_channel_set_bitrate(handle_, 0, static_cast<uint32_t>(bitrate))) {
        logger->error("could not set bitrate: {}", get_error(handle_));
        return false;
    }

    return true;
}

bool candlelight::transmit(frame::ptr msg) {
    if (msg->length_ > MAX_DLC) {
        logger->error("unsupported message length of '{}'", msg->length_);
        return false;
    }

    candle_frame_t frame{};
    frame.can_id  = msg->identifier_;
    frame.can_dlc = msg->length_;
    frame.flags   = 0;
    /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): library function */
    std::copy(msg->bytes_, msg->bytes_ + msg->length_, frame.data);

    if (!candle_frame_send(handle_, 0, &frame)) {
        logger->error("could not send frame: {}", get_error(handle_));
        return false;
    }

    return true;
}

frame::ptr candlelight::receive(long timeout_ms) {
    candle_frame_t frame;
    if (!candle_frame_read(handle_, &frame, utils::crop_cast<long, uint32_t>(timeout_ms))) {
        logger->error("could not read frame: {}", get_error(handle_));
        return nullptr;
    }

    if (frame.timestamp_us == 0) {
        if (!candle_dev_get_timestamp_us(handle_, &frame.timestamp_us)) {
            logger->error("could get timestamp: {}", get_error(handle_));
            return nullptr;
        }
    }

    return frame::create(frame.can_id, frame.can_dlc, frame.data, frame.timestamp_us);
}

} /* namespace can::driver */