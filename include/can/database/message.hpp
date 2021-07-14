#ifndef INCLUDE_CAN_DATABASE_MESSAGE_HPP
#define INCLUDE_CAN_DATABASE_MESSAGE_HPP

#include <unordered_map>
#include <utility>
#include <vector>

#include "can/database/signal.hpp"

namespace can::database {

class message {
   public:
    using ptr       = std::shared_ptr<message>;
    using const_ptr = std::shared_ptr<const message>;

    message();
    virtual ~message() = default;

    /**
     * This method returns the quark (name alternative) to the message.
     */
    [[nodiscard]] unsigned long get_quark() const;

    /**
     * This method returns the name of the message.
     */
    [[nodiscard]] virtual const std::string& get_name() const = 0;

    /**
     * This method returns the numerical identifier of the message.
     */
    [[nodiscard]] virtual unsigned int get_identifier() const = 0;

    /**
     * This method returns the length, in bytes, of the message.
     */
    [[nodiscard]] virtual unsigned short get_byte_count() const = 0;

    /**
     * This method returns the node that sends this message.
     */
    [[nodiscard]] virtual const std::string& get_node() const = 0;

    /**
     * This method returns the list of signals indexed by name in the message.
     */
    [[nodiscard]] virtual std::vector<signal::const_ptr> get_signals() const = 0;

    /**
     * This method returns the signal in the message, if present, with the specified name.
     */
    [[nodiscard]] virtual signal::const_ptr get_signal(const std::string& name) const = 0;

    /**
     * This method decodes signals from the given frame using the messages definition.
     */
    [[nodiscard]] std::vector<std::pair<signal::const_ptr, float>> decode(uint8_t* bytes, size_t length) const;

   private:
    const quark quark_;
};

} /* namespace can::database */

#endif /* INCLUDE_CAN_DATABASE_MESSAGE_HPP */