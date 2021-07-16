#ifndef INCLUDE_CAN_TRANSCEIVER_HPP
#define INCLUDE_CAN_TRANSCEIVER_HPP

#include <list>
#include <memory>

#include "can/frame.hpp"

namespace can {

using interface_list_ptr = std::unique_ptr<std::list<std::string>>;

class transceiver {
   public:
    using ptr = std::shared_ptr<transceiver>;

    static ptr create(const std::string& driver, const std::string& interface);

    virtual ~transceiver()                           = default;
    virtual bool set_bitrate(unsigned long bitrate)  = 0;
    virtual bool transmit(frame::ptr msg)            = 0;
    virtual frame::ptr receive(long timeout_ms = -1) = 0;
};

} /* namespace can */

#endif /* INCLUDE_CAN_TRANSCEIVER_HPP */
