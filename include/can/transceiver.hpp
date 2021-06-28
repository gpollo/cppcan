#ifndef INCLUDE_CAN_TRANSCEIVER_HPP
#define INCLUDE_CAN_TRANSCEIVER_HPP

#include "can/frame.hpp"

namespace can {

class transceiver {
   public:
    virtual ~transceiver()                           = default;
    virtual bool transmit(frame::ptr msg)            = 0;
    virtual frame::ptr receive(long timeout_ms = -1) = 0;
};

} /* namespace can */

#endif /* INCLUDE_CAN_TRANSCEIVER_HPP */