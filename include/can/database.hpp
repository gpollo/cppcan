#ifndef INCLUDE_CAN_DATABASE_DATABASE_HPP
#define INCLUDE_CAN_DATABASE_DATABASE_HPP

#include <filesystem>
#include <optional>
#include <variant>
#include <vector>

#include "can/types.hpp"
#include "can/utils/quark.hpp"

namespace can {

class database {
   public:
    using ptr       = std::shared_ptr<database>;
    using const_ptr = std::shared_ptr<const database>;

    static ptr create(const std::filesystem::path& path);

    class signal {
       public:
        using ptr       = std::shared_ptr<signal>;
        using const_ptr = std::shared_ptr<const signal>;

        /**
         * If the signal is encoded using intel encoding, its endianness will
         * be of type little-endian.
         *
         * If the signal is encoded using motorola encoding, its endianness will
         * be of type big-endian.
         */
        enum class endian { LITTLE, BIG };

        /**
         * If the signal does not have any kind of multiplexing, the value of
         * this variant will be `false`.
         *
         * If the signal multiplexes other signals, the value of this variant
         * will be `true`.
         *
         * If the signal is multiplexed by another signal, the value of this
         * variant will be a positive number.
         */
        using multiplexing = std::variant<bool, unsigned short>;

        signal();
        virtual ~signal() = default;

        /**
         * This method returns the quark (name alternative) to the message.
         */
        [[nodiscard]] unsigned long get_quark() const;

        /**
         * This method returns the name of the signal.
         */
        [[nodiscard]] virtual const std::string& get_name() const = 0;

        /**
         * This method returns the start bit index of the signal.
         *
         * For example, if the message is encoded in the second and
         * third bytes of the message, the start bit index will be 8.
         */
        [[nodiscard]] virtual unsigned char get_start_bit() const = 0;

        /**
         * This method returns the number of bit used by the signal.
         *
         * For example, if the message is encoded in the entire second
         * and third bytes of the message, the bit count will be 16.
         */
        [[nodiscard]] virtual unsigned char get_bit_count() const = 0;

        /**
         * This method returns the byte order of the signal.
         */
        [[nodiscard]] virtual endian get_byte_order() const = 0;

        /**
         * This method returns whether the signal is integral or not.
         */
        [[nodiscard]] virtual bool is_integral() const = 0;

        /**
         * This method returns whether the signal is signed or not.
         */
        [[nodiscard]] virtual bool is_signed() const = 0;

        /**
         * This method returns the scale of the signal.
         *
         * After extracting raw bits from the message, those bits are
         * multiplied by the scale and an offset is added.
         */
        [[nodiscard]] virtual float get_scale() const = 0;

        /**
         * This method returns the offset of the signal.
         *
         * After extracting raw bits from the message, those bits are
         * multiplied by the scale and an offset is added.
         */
        [[nodiscard]] virtual float get_offset() const = 0;

        /**
         * This method returns the minimum value, if any, of the signal.
         */
        [[nodiscard]] virtual std::optional<float> get_min() const = 0;

        /**
         * This method returns the maximum value, if any, of the signal.
         */
        [[nodiscard]] virtual std::optional<float> get_max() const = 0;

        /**
         * This method returns the unit, if any, of the signal.
         */
        [[nodiscard]] virtual const std::string& get_unit() const = 0;

        /**
         * This method returns the node listening to this signal.
         *
         * TODO: return a object containing more informations to the node
         */
        [[nodiscard]] virtual const std::vector<std::string>& get_nodes() const = 0;

        /**
         * This method returns the multiplexing informations of this signal.
         */
        [[nodiscard]] virtual multiplexing get_multiplexing() const = 0;

        /**
         * This method extracts the raw value of this message in the frame.
         */
        [[nodiscard]] uint64_t extract(uint8_t* bytes, size_t length) const;

        /**
         * This method decodes the signal value from the raw value.
         */
        [[nodiscard]] float decode(uint64_t raw_value) const;

        /**
         * This method resolves, if possible, the readable value from the raw value.
         */
        [[nodiscard]] virtual const std::string& resolve(uint64_t raw_value) const = 0;

        /**
         * This method returns whether this signal multiplexes other signals in the message.
         */
        [[nodiscard]] bool is_multiplexer() const;

        /**
         * This method returns the multiplexed value of this message if it is multiplexed.
         */
        [[nodiscard]] std::optional<unsigned short> get_multiplexed_value() const;

       private:
        const quark quark_;

        [[nodiscard]] float convert_raw_value(unsigned long raw_value) const;
    };

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
         * This method returns the signal in the message, if present, with the specified quark.
         */
        [[nodiscard]] virtual signal::const_ptr get_signal(quark quark) const = 0;

        /**
         * This method extracts signals from the given frame using the messages definition.
         */
        [[nodiscard]] std::vector<std::pair<signal::const_ptr, uint64_t>> extract(uint8_t* bytes, size_t length) const;

        /**
         * This method decodes signals from the given frame using the messages definition.
         */
        [[nodiscard]] std::vector<std::pair<signal::const_ptr, float>> decode(uint8_t* bytes, size_t length) const;

        /**
         * This method resolves signals from the given frame using the messages definition.
         */
        [[nodiscard]] std::vector<std::pair<signal::const_ptr, std::variant<float, std::string>>> resolve(
            uint8_t* bytes, size_t length) const;

       private:
        const quark quark_;
    };

    database()          = default;
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
     * This method extracts signals from the given frame if the message definition exists.
     */
    [[nodiscard]] std::vector<std::pair<signal::const_ptr, uint64_t>> extract(unsigned int identifier, uint8_t* bytes,
                                                                              size_t length) const;

    /**
     * This method decodes signals from the given frame if the message definition exists.
     */
    [[nodiscard]] std::vector<std::pair<signal::const_ptr, float>> decode(unsigned int identifier, uint8_t* bytes,
                                                                          size_t length) const;
};

inline std::vector<std::pair<database::signal::const_ptr, uint64_t>> database::extract(unsigned int identifier,
                                                                                       uint8_t* bytes,
                                                                                       size_t length) const {
    auto message = get_message(identifier);
    if (message == nullptr) {
        return {};
    }

    return message->extract(bytes, length);
}

inline std::vector<std::pair<database::signal::const_ptr, float>> database::decode(unsigned int identifier,
                                                                                   uint8_t* bytes,
                                                                                   size_t length) const {
    auto message = get_message(identifier);
    if (message == nullptr) {
        return {};
    }

    return message->decode(bytes, length);
}

} /* namespace can */

#endif /* INCLUDE_CAN_DATABASE_DATABASE_HPP */