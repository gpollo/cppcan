#include <algorithm>
#include <map>

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

/* NOLINTNEXTLINE(cppcoreguidelines-macro-usage): required for __FILE__ and __LINE__ */
#define CANDLE_LOG_ERROR(func, handle)                                                   \
    {                                                                                    \
        if (handle == nullptr) {                                                         \
            fprintf(stderr, "%s:%d:" func ": Unknown error\n", __FILE__, __LINE__);      \
        } else {                                                                         \
            const std::string& text = ERROR_TO_STRING.at(candle_dev_last_error(handle)); \
            fprintf(stderr, "%s:%d:" func ": %s\n", __FILE__, __LINE__, text.data());    \
        }                                                                                \
    }

candlelight_ptr candlelight::create(uint8_t device) {
    candle_list_handle handle_list = nullptr;
    candle_handle handle           = nullptr;
    candle_devstate_t state        = CANDLE_DEVSTATE_INUSE;

    if (!candle_list_scan(&handle_list)) {
        CAN_LOG_ERROR("candle_list_scan: ERROR_UNKNOWN");
        goto candle_list_scan_failed;
    }

    if (!candle_dev_get(handle_list, device, &handle)) {
        CANDLE_LOG_ERROR("candle_dev_get", handle);
        goto candle_dev_get_failed;
    }

    if (!candle_dev_get_state(handle, &state)) {
        CANDLE_LOG_ERROR("candle_dev_get_state", handle);
        goto candle_dev_get_state_failed;
    }

    if (state == CANDLE_DEVSTATE_INUSE) {
        CAN_LOG_ERROR("create: Device already in use");
        goto device_in_use;
    }

    if (!candle_dev_open(handle)) {
        CANDLE_LOG_ERROR("candle_dev_open", handle);
        goto candle_dev_open_failed;
    }

    candle_list_free(handle_list);

    return candlelight_ptr(new candlelight(handle));

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

bool candlelight::transmit(frame::ptr msg) {
    if (msg->length_ > MAX_DLC) {
        CAN_LOG_ERROR("transmit: Unsupported data length of %llu", msg->length_);
        return false;
    }

    candle_frame_t frame{};
    frame.can_id  = msg->identifier_;
    frame.can_dlc = msg->length_;
    frame.flags   = 0;
    /* NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): library function */
    std::copy(msg->bytes_, msg->bytes_ + msg->length_, frame.data);

    if (!candle_frame_send(handle_, 0, &frame)) {
        CANDLE_LOG_ERROR("candle_frame_send", handle_);
        return false;
    }

    return true;
}

frame::ptr candlelight::receive(long timeout_ms) {
    candle_frame_t frame;
    if (!candle_frame_read(handle_, &frame, utils::crop_cast<long, uint32_t>(timeout_ms))) {
        CANDLE_LOG_ERROR("candle_frame_read", handle_);
        return nullptr;
    }

    if (frame.timestamp_us == 0) {
        if (!candle_dev_get_timestamp_us(handle_, &frame.timestamp_us)) {
            CANDLE_LOG_ERROR("candle_dev_get_timestamp_us", handle_);
            return nullptr;
        }
    }

    return frame::create(frame.can_id, frame.can_dlc, frame.data, frame.timestamp_us);
}

} /* namespace can::driver */