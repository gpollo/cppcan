#ifndef INCLUDE_CAN_DATABASE_DATABASE_HPP
#define INCLUDE_CAN_DATABASE_DATABASE_HPP

#include <filesystem>

#include "can/database/message.hpp"

namespace can::database {

class database {
   public:
    using ptr = std::shared_ptr<database>;

    static ptr create(const std::filesystem::path& path);

    virtual ~database() = default;

    /**
     * This method returns the list of messages in the database.
     */
    [[nodiscard]] virtual std::vector<message::const_ptr> get_messages() const = 0;

    /**
     * This method returns the message in the database, if present, with the specified identifier.
     */
    [[nodiscard]] virtual message::const_ptr get_message(unsigned int identifier) const = 0;

    /**
     * This method returns the message in the database, if present, with the specified name.
     */
    [[nodiscard]] virtual message::const_ptr get_message(const std::string& name) const = 0;

    /**
     * This method decodes signals from the given frame if the message definition exists.
     */
    [[nodiscard]] std::vector<std::pair<signal::const_ptr, float>> decode(unsigned int identifier, uint8_t* bytes,
                                                                          size_t length) const;
};

inline std::vector<std::pair<signal::const_ptr, float>> database::decode(unsigned int identifier, uint8_t* bytes,
                                                                         size_t length) const {
    auto message = get_message(identifier);
    if (message == nullptr) {
        return {};
    }

    return message->decode(bytes, length);
}

} /* namespace can::database */

#endif /* INCLUDE_CAN_DATABASE_DATABASE_HPP */