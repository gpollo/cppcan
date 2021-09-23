#ifndef INCLUDE_CAN_DRIVER_CANDLELIGHT_HPP
#define INCLUDE_CAN_DRIVER_CANDLELIGHT_HPP

#include <list>
#include <memory>

#include "can/transceiver.hpp"

#if !defined(BUILD_WINDOWS)
#error "This driver only works under Windows"
#endif

namespace can::driver {

class candlelight : public transceiver {
   public:
    static std::list<std::string> list_interfaces();
    static ptr create(const std::string& device);
    ~candlelight() override;

    bool set_bitrate(unsigned long bitrate) override;
    bool transmit(frame::ptr msg) override;
    frame::ptr receive(long timeout_ms = -1) override;

   private:
    void* handle_;

    candlelight(void* handle);
};

} /* namespace can::driver */

#endif /* INCLUDE_CAN_DRIVER_CANDLELIGHT_HPP */