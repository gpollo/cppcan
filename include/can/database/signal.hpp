#ifndef INCLUDE_CAN_DATABASE_SIGNAL_HPP
#define INCLUDE_CAN_DATABASE_SIGNAL_HPP

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "can/utils/quark.hpp"

namespace can::database {

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
     * This method resolves, if possible, a raw value into the readable value.
     */
    [[nodiscard]] virtual const std::string& resolve_value(uint64_t raw_value) const = 0;

    /**
     * This method decodes the signal from a frame.
     */
    [[nodiscard]] float decode(uint8_t* bytes, size_t length) const;

    /**
     * This method decodes the signal and tries to resolve its value from a frame.
     */
    [[nodiscard]] std::variant<float, std::string> decode_and_resolve(uint8_t* bytes, size_t length) const;

    /**
     * This method returns whether this signal multiplexes other signals in the message.
     */
    [[nodiscard]] bool is_multiplexer() const;

    /**
     * This method returns the multiplexed value of this message if it is multiplexed.
     */
    [[nodiscard]] std::optional<unsigned short> get_multiplexed_value() const;

    /**
     * This method extracts the raw value of this message in the frame.
     */
    [[nodiscard]] uint64_t extract_raw_value(uint8_t* bytes, size_t length) const;

   private:
    const quark quark_;

    [[nodiscard]] float convert_raw_value(unsigned long raw_value) const;
};

} /* namespace can::database */

#endif /* INCLUDE_CAN_DATABASE_SIGNAL_HPP */