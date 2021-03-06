#ifndef INCLUDE_CAN_DRIVER_PCAN_HPP
#define INCLUDE_CAN_DRIVER_PCAN_HPP

#include <list>
#include <memory>

#include "can/transceiver.hpp"

#if !defined(BUILD_LINUX) && !defined(BUILD_WINDOWS)
#error "This driver only works under Linux or Windows"
#endif

namespace can::driver {

class pcan : public transceiver {
   public:
#ifdef BUILD_LINUX
    using event_type = int;
#endif /* BUILD_LINUX */

#ifdef BUILD_WINDOWS
    using event_type = void*;
#endif /* BUILD_WINDOWS */

    static std::list<std::string> list_interfaces();
    static ptr create(const std::string& interface);
    ~pcan() override;

    bool set_bitrate(unsigned long bitrate) override;
    bool transmit(frame::ptr msg) override;
    frame::ptr receive(long timeout_ms = -1) override;

   private:
    const unsigned int device_;
    const event_type event_;

    pcan(unsigned int device, event_type event);

    frame::ptr try_receive();
};

} /* namespace can::driver */

#endif /* INCLUDE_CAN_DRIVER_PCAN_HPP */