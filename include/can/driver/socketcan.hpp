#ifndef INCLUDE_CAN_DRIVER_SOCKETCAN_HPP
#define INCLUDE_CAN_DRIVER_SOCKETCAN_HPP

#include <memory>
#include <mutex>

#include "can/transceiver.hpp"

#if !defined(BUILD_LINUX)
#error "This driver only works under Linux"
#endif

namespace can::driver {

class socketcan : public transceiver {
   public:
    static interface_list_ptr list_interfaces();
    static ptr create(const std::string& interface);
    ~socketcan() override;

    bool set_bitrate(unsigned long bitrate) override;
    bool transmit(frame::ptr msg) override;
    frame::ptr receive(long timeout_ms = -1) override;

   private:
    const int socket_;
    const std::string interface_;
    std::mutex receive_mutex_;

    socketcan(int socket, std::string interface);
};

} /* namespace can::driver */

#endif /* INCLUDE_CAN_DRIVER_SOCKETCAN_HPP */