#ifndef INCLUDE_CAN_UTILS_CROP_CAST_HPP
#define INCLUDE_CAN_UTILS_CROP_CAST_HPP

#include <cstdint>
#include <functional>
#include <limits>
#include <type_traits>
#include <utility>

namespace can::utils {

namespace detail::integral_integral::unsigned_unsigned {

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::false_type /* narrow */) {
    return static_cast<To>(value);
}

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::true_type /* narrow */) {
    constexpr To TO_MAX = (std::numeric_limits<To>::max)();

    if (value > static_cast<From>(TO_MAX)) {
        return TO_MAX;
    }

    return static_cast<To>(value);
}

} /* namespace detail::integral_integral::unsigned_unsigned */

namespace detail::integral_integral::signed_unsigned {

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::false_type /* narrow */) {
    if (value < 0) {
        return 0;
    }

    return static_cast<To>(value);
}

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::true_type /* narrow */) {
    constexpr To TO_MIN = (std::numeric_limits<To>::min)();
    constexpr To TO_MAX = (std::numeric_limits<To>::max)();

    if (value < static_cast<From>(TO_MIN)) {
        return TO_MIN;
    }

    if (value > static_cast<From>(TO_MAX)) {
        return TO_MAX;
    }

    return static_cast<To>(value);
}

} /* namespace detail::integral_integral::signed_unsigned */

namespace detail::integral_integral::signed_signed {

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::false_type /* narrow */) {
    return static_cast<To>(value);
}

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::true_type /* narrow */) {
    constexpr To TO_MIN = (std::numeric_limits<To>::min)();
    constexpr To TO_MAX = (std::numeric_limits<To>::max)();

    if (value < static_cast<From>(TO_MIN)) {
        return TO_MIN;
    }

    if (value > static_cast<From>(TO_MAX)) {
        return TO_MAX;
    }

    return static_cast<To>(value);
}

} /* namespace detail::integral_integral::signed_signed */

namespace detail::integral_integral::unsigned_signed {

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::false_type /* narrow */) {
    return static_cast<To>(value);
}

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::true_type /* narrow */) {
    constexpr To TO_MAX = (std::numeric_limits<To>::max)();

    if (value > static_cast<From>(TO_MAX)) {
        return TO_MAX;
    }

    return static_cast<To>(value);
}

} /* namespace detail::integral_integral::unsigned_signed */

namespace detail::integral_integral {

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::false_type /* from_signed */, std::false_type /* to_signed */) {
    constexpr auto FROM_DIGITS = std::numeric_limits<From>::digits;
    constexpr auto TO_DIGITS   = std::numeric_limits<To>::digits;
    constexpr auto NARROWING   = std::integral_constant<bool, std::cmp_greater(FROM_DIGITS, TO_DIGITS)>();
    return unsigned_unsigned::crop_cast<From, To>(value, NARROWING);
}

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::true_type /* from_signed */, std::false_type /* to_signed */) {
    constexpr auto FROM_DIGITS = std::numeric_limits<From>::digits;
    constexpr auto TO_DIGITS   = std::numeric_limits<To>::digits;
    constexpr auto NARROWING   = std::integral_constant<bool, std::cmp_greater(FROM_DIGITS, TO_DIGITS)>();
    return signed_unsigned::crop_cast<From, To>(value, NARROWING);
}

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::true_type /* from_signed */, std::true_type /* to_signed */) {
    constexpr auto FROM_DIGITS = std::numeric_limits<From>::digits;
    constexpr auto TO_DIGITS   = std::numeric_limits<To>::digits;
    constexpr auto NARROWING   = std::integral_constant<bool, std::cmp_greater(FROM_DIGITS, TO_DIGITS)>();
    return signed_signed::crop_cast<From, To>(value, NARROWING);
}

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::false_type /* from_signed */, std::true_type /* to_signed */) {
    constexpr auto FROM_DIGITS = std::numeric_limits<From>::digits;
    constexpr auto TO_DIGITS   = std::numeric_limits<To>::digits;
    constexpr auto NARROWING   = std::integral_constant<bool, std::cmp_greater(FROM_DIGITS, TO_DIGITS)>();
    return unsigned_signed::crop_cast<From, To>(value, NARROWING);
}

} /* namespace detail::integral_integral */

namespace detail::float_integral {

template <typename From, typename To>
static inline constexpr To crop_cast(From value) {
    constexpr To TO_MIN = (std::numeric_limits<To>::min)();
    constexpr To TO_MAX = (std::numeric_limits<To>::max)();

    if (value <= static_cast<From>(TO_MIN)) {
        return TO_MIN;
    }

    if (value >= static_cast<From>(TO_MAX)) {
        return TO_MAX;
    }

    return static_cast<To>(value);
}

} /* namespace detail::float_integral */

namespace detail {

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::true_type /* integral */, std::true_type /* integral */) {
    constexpr auto FROM_SIGNED = std::is_signed<From>();
    constexpr auto TO_SIGNED   = std::is_signed<To>();
    return integral_integral::crop_cast<From, To>(value, FROM_SIGNED, TO_SIGNED);
}

template <typename From, typename To>
static inline constexpr To crop_cast(From value, std::false_type /* integral */, std::true_type /* integral */) {
    return float_integral::crop_cast<From, To>(value);
}

} /* namespace detail */

template <typename From, typename To>
static inline constexpr To crop_cast(From value) {
    constexpr auto FROM_INTEGRAL = std::is_integral<From>();
    constexpr auto TO_INTEGRAL   = std::is_integral<To>();
    return detail::crop_cast<From, To>(value, FROM_INTEGRAL, TO_INTEGRAL);
}

} /* namespace can::utils */

#endif /* INCLUDE_CAN_UTILS_CROP_CAST_HPP */