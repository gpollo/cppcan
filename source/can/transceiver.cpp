#include "can/transceiver.hpp"
#include "can/log.hpp"

#ifdef ENABLE_DRIVER_CANDLELIGHT
#include "can/driver/candlelight.hpp"
#endif /* ENABLE_DRIVER_CANDLELIGHT */

#ifdef ENABLE_DRIVER_PCAN
#include "can/driver/pcan.hpp"
#endif /* ENABLE_DRIVER_PCAN */

#ifdef ENABLE_DRIVER_SOCKETCAN
#include "can/driver/socketcan.hpp"
#endif /* ENABLE_DRIVER_SOCKETCAN */

namespace can {

/* transceiver::ptr class */

transceiver::ptr::ptr(std::nullptr_t /* ptr */) : transceiver_(nullptr), transmitter_(nullptr) {}

transceiver::ptr::ptr(std::shared_ptr<transceiver> ptr) : transceiver_(ptr), transmitter_(ptr) {}

can::utils::unique_owner_ptr<transceiver> transceiver::ptr::get_unique_transceiver() {
    return std::move(transceiver_);
}

std::shared_ptr<transmitter> transceiver::ptr::get_transmitter() {
    return transmitter_;
}

bool transceiver::ptr::operator==(std::nullptr_t other) const {
    return (transmitter_ == other);
}

bool transceiver::ptr::operator!=(std::nullptr_t other) const {
    return (transmitter_ != other);
}

/* transceiver class */

std::map<std::string, std::list<std::string>> transceiver::list_interfaces() {
    std::map<std::string, std::list<std::string>> interfaces;

#ifdef ENABLE_DRIVER_CANDLELIGHT
    interfaces["candlelight"] = driver::candlelight::list_interfaces();
#endif /* ENABLE_DRIVER_CANDLELIGHT */

#ifdef ENABLE_DRIVER_PCAN
    interfaces["pcan"] = driver::pcan::list_interfaces();
#endif /* ENABLE_DRIVER_PCAN */

#ifdef ENABLE_DRIVER_SOCKETCAN
    interfaces["socketcan"] = driver::socketcan::list_interfaces();
#endif /* ENABLE_DRIVER_SOCKETCAN */

    return interfaces;
}

transceiver::ptr transceiver::create(const std::string& driver, const std::string& interface) {
#ifdef ENABLE_DRIVER_CANDLELIGHT
    if (driver == "candlelight") {
        return driver::candlelight::create(interface);
    }
#endif /* ENABLE_DRIVER_CANDLELIGHT */

#ifdef ENABLE_DRIVER_PCAN
    if (driver == "pcan") {
        return driver::pcan::create(interface);
    }
#endif /* ENABLE_DRIVER_PCAN */

#ifdef ENABLE_DRIVER_SOCKETCAN
    if (driver == "socketcan") {
        return driver::socketcan::create(interface);
    }
#endif /* ENABLE_DRIVER_SOCKETCAN */

    logger->error("invalid driver specified '{}'", driver);
    return nullptr;
}

} /* namespace can */