#include "can/transceiver.hpp"

#ifdef ENABLE_DRIVER_CANDLELIGHT
#include "can/driver/candlelight.hpp"
#endif /* ENABLE_DRIVER_CANDLELIGHT */

#ifdef ENABLE_DRIVER_PCAN
#include "can/driver/socketcan.hpp"
#endif /* ENABLE_DRIVER_PCAN */

#ifdef ENABLE_DRIVER_SOCKETCAN
#include "can/driver/pcan.hpp"
#endif /* ENABLE_DRIVER_SOCKETCAN */

namespace can {

transceiver::ptr transceiver::create(const std::string& driver, const std::string& interface) {
#ifdef ENABLE_DRIVER_CANDLELIGHT
    if (driver == "candlelight") {
        return driver::candlelight::create(std::stoi(interface));
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

    /* TODO: log invalid driver */

    return nullptr;
}

} /* namespace can */