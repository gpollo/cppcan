#ifndef INCLUDE_CAN_DRIVER_SOCKETCAN_HPP
#define INCLUDE_CAN_DRIVER_SOCKETCAN_HPP

#include <memory>
#include <mutex>

#include "can/transceiver.hpp"

#if !defined(BUILD_LINUX)
#error "This driver only works under Linux"
#endif

namespace can::driver {

class socketcan;
using socketcan_ptr = std::shared_ptr<socketcan>;

class socketcan : public transceiver {
   public:
    static socketcan_ptr create(const std::string& interface);
    ~socketcan() override;

    bool transmit(frame::ptr msg) override;
    frame::ptr receive(long timeout_ms = -1) override;

   private:
    const int socket_;
    std::mutex receive_mutex_;

    socketcan(int socket);
};

} /* namespace can::driver */

#endif /* INCLUDE_CAN_DRIVER_SOCKETCAN_HPP */