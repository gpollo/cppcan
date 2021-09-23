#ifndef INCLUDE_CAN_TRANSCEIVER_HPP
#define INCLUDE_CAN_TRANSCEIVER_HPP

#include <memory>
#include <string>

#include "can/frame.hpp"
#include "can/utils/unique_owner_ptr.hpp"

namespace can {

class transmitter {
   public:
    virtual ~transmitter()                = default;
    virtual bool transmit(frame::ptr msg) = 0;
};

class receiver {
   public:
    virtual ~receiver()                              = default;
    virtual frame::ptr receive(long timeout_ms = -1) = 0;
};

class transceiver : public transmitter, public receiver {
   public:
    class ptr {
       public:
        ptr(std::nullptr_t);
        ptr(std::shared_ptr<transceiver>);
        can::utils::unique_owner_ptr<transceiver> get_unique_transceiver();
        std::shared_ptr<transmitter> get_transmitter();
        bool operator==(std::nullptr_t other) const;
        bool operator!=(std::nullptr_t other) const;

       private:
        can::utils::unique_owner_ptr<transceiver> transceiver_;
        std::shared_ptr<transceiver> transmitter_;
    };

    static ptr create(const std::string& driver, const std::string& interface);

    virtual ~transceiver()                          = default;
    virtual bool set_bitrate(unsigned long bitrate) = 0;
};

} /* namespace can */

#endif /* INCLUDE_CAN_TRANSCEIVER_HPP */
