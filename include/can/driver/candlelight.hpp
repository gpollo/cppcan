#ifndef INCLUDE_CAN_DRIVER_CANDLELIGHT_HPP
#define INCLUDE_CAN_DRIVER_CANDLELIGHT_HPP

#include <memory>

#include "can/transceiver.hpp"

#if !defined(BUILD_WINDOWS)
#error "This driver only works under Windows"
#endif

namespace can::driver {

class candlelight;
using candlelight_ptr = std::shared_ptr<candlelight>;

class candlelight : public transceiver {
   public:
    static candlelight_ptr create(uint8_t device);
    ~candlelight() override;

    bool set_bitrate(unsigned long bitrate);
    bool transmit(frame::ptr msg) override;
    frame::ptr receive(long timeout_ms = -1) override;

   private:
    void* handle_;

    candlelight(void* handle);
};

} /* namespace can::driver */

#endif /* INCLUDE_CAN_DRIVER_CANDLELIGHT_HPP */