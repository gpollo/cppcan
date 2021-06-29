#include <cassert>

#include "can/utils/crop_cast.hpp"

/*
 * helpers to make the tests look better
 */

template <typename T>
struct min {
    static constexpr T value = std::numeric_limits<T>::min();
};

template <typename T>
struct max {
    static constexpr T value = std::numeric_limits<T>::max();
};

/* clang-format off */

/*
 * unsigned integral to unsigned integral conversion
 */

/* no narrowing, no cropping */
static_assert(can::utils::crop_cast<uint16_t, uint32_t>(min<uint16_t>::value) == min<uint16_t>::value);
static_assert(can::utils::crop_cast<uint16_t, uint32_t>(max<uint16_t>::value) == max<uint16_t>::value);

/* narrowing, no cropping */
static_assert(can::utils::crop_cast<uint16_t, uint8_t>(min<uint8_t>::value) == min<uint8_t>::value);
static_assert(can::utils::crop_cast<uint16_t, uint8_t>(max<uint8_t>::value) == max<uint8_t>::value);

/* narrowing, positive croping */
static_assert(can::utils::crop_cast<uint32_t, uint16_t>(max<uint16_t>::value + 1) == max<uint16_t>::value);

/*
 * signed integral to unsigned integral conversion
 */

/* no narrowing, no cropping */
static_assert(can::utils::crop_cast<int16_t, uint32_t>(min<uint32_t>::value) == min<uint32_t>::value);
static_assert(can::utils::crop_cast<int16_t, uint32_t>(max<int16_t>::value) == max<int16_t>::value);

/* no narrowing, negative cropping */
static_assert(can::utils::crop_cast<int16_t, uint32_t>(min<int16_t>::value) == min<uint32_t>::value);

/* narrowing, no cropping */
static_assert(can::utils::crop_cast<int32_t, uint16_t>(min<uint16_t>::value) == min<uint16_t>::value);
static_assert(can::utils::crop_cast<int32_t, uint16_t>(max<uint16_t>::value) == max<uint16_t>::value);

/* narrowing, negative cropping */
static_assert(can::utils::crop_cast<int32_t, uint16_t>(min<int32_t>::value) == min<uint16_t>::value);

/* narrowing, positive cropping */
static_assert(can::utils::crop_cast<int32_t, uint16_t>(max<int32_t>::value) == max<uint16_t>::value);

/*
 * signed integral to signed integral conversion
 */

/* no narrowing, no cropping */
static_assert(can::utils::crop_cast<int16_t, int32_t>(min<int16_t>::value) == min<int16_t>::value);
static_assert(can::utils::crop_cast<int16_t, int32_t>(max<int16_t>::value) == max<int16_t>::value);

/* narrowing, no cropping */
static_assert(can::utils::crop_cast<int32_t, int16_t>(min<int16_t>::value) == min<int16_t>::value);
static_assert(can::utils::crop_cast<int32_t, int16_t>(max<int16_t>::value) == max<int16_t>::value);

/* narrowing, negative croping */
static_assert(can::utils::crop_cast<int32_t, int16_t>(min<int32_t>::value) == min<int16_t>::value);

/* narrowing, positive croping */
static_assert(can::utils::crop_cast<int32_t, int16_t>(max<int32_t>::value) == max<int16_t>::value);

/*
 * unsigned integral to signed integral conversion
 */

/* no narrowing, no cropping */
static_assert(can::utils::crop_cast<uint16_t, int32_t>(min<uint16_t>::value) == min<uint16_t>::value);
static_assert(can::utils::crop_cast<uint16_t, int32_t>(max<uint16_t>::value) == max<uint16_t>::value);

/* narrowing, no cropping */
static_assert(can::utils::crop_cast<uint32_t, int16_t>(min<uint32_t>::value) == min<uint32_t>::value);
static_assert(can::utils::crop_cast<uint32_t, int16_t>(max<int16_t>::value) == max<int16_t>::value);

/* narrowing, positive croping */
static_assert(can::utils::crop_cast<uint32_t, int16_t>(max<uint32_t>::value) == max<int16_t>::value);

/*
 * float to signed integral conversion
 */

/* no cropping */
static_assert(can::utils::crop_cast<float, int32_t>(static_cast<float>(min<int32_t>::value)) == min<int32_t>::value);
static_assert(can::utils::crop_cast<float, int32_t>(static_cast<float>(max<int32_t>::value)) == max<int32_t>::value);
static_assert(can::utils::crop_cast<double, int32_t>(static_cast<double>(min<int32_t>::value)) == min<int32_t>::value);
static_assert(can::utils::crop_cast<double, int32_t>(static_cast<double>(max<int32_t>::value)) == max<int32_t>::value);

/* negative cropping */
static_assert(can::utils::crop_cast<float, int32_t>(static_cast<float>(min<int32_t>::value) - 1) == min<int32_t>::value);
static_assert(can::utils::crop_cast<double, int32_t>(static_cast<double>(min<int32_t>::value) - 1) == min<int32_t>::value);
static_assert(can::utils::crop_cast<float, int32_t>(2.0F * static_cast<float>(min<int32_t>::value)) == min<int32_t>::value);
static_assert(can::utils::crop_cast<double, int32_t>(2.0F * static_cast<double>(min<int32_t>::value)) == min<int32_t>::value);

/* positive cropping */
static_assert(can::utils::crop_cast<float, int32_t>(static_cast<float>(max<int32_t>::value) + 1) == max<int32_t>::value);
static_assert(can::utils::crop_cast<double, int32_t>(static_cast<double>(max<int32_t>::value) + 1) == max<int32_t>::value);
static_assert(can::utils::crop_cast<float, int32_t>(2.0F * static_cast<float>(max<int32_t>::value)) == max<int32_t>::value);
static_assert(can::utils::crop_cast<double, int32_t>(2.0F * static_cast<double>(max<int32_t>::value)) == max<int32_t>::value);

/*
 * float to unsigned integral conversion
 */

/* no cropping */
static_assert(can::utils::crop_cast<float, uint32_t>(static_cast<float>(min<uint32_t>::value)) == min<uint32_t>::value);
static_assert(can::utils::crop_cast<float, uint32_t>(static_cast<float>(max<uint32_t>::value)) == max<uint32_t>::value);
static_assert(can::utils::crop_cast<double, uint32_t>(static_cast<double>(min<uint32_t>::value)) == min<uint32_t>::value);
static_assert(can::utils::crop_cast<double, uint32_t>(static_cast<double>(max<uint32_t>::value)) == max<uint32_t>::value);

/* negative cropping */
static_assert(can::utils::crop_cast<float, uint32_t>(static_cast<float>(min<uint32_t>::value) - 1) == min<uint32_t>::value);
static_assert(can::utils::crop_cast<double, uint32_t>(static_cast<double>(min<uint32_t>::value) - 1) == min<uint32_t>::value);
static_assert(can::utils::crop_cast<float, uint32_t>(2.0F * static_cast<float>(min<uint32_t>::value)) == min<uint32_t>::value);
static_assert(can::utils::crop_cast<double, uint32_t>(2.0F * static_cast<double>(min<uint32_t>::value)) == min<uint32_t>::value);

/* positive cropping */
static_assert(can::utils::crop_cast<float, uint32_t>(static_cast<float>(max<uint32_t>::value) + 1) == max<uint32_t>::value);
static_assert(can::utils::crop_cast<double, uint32_t>(static_cast<double>(max<uint32_t>::value) + 1) == max<uint32_t>::value);
static_assert(can::utils::crop_cast<float, uint32_t>(2.0F * static_cast<float>(max<uint32_t>::value)) == max<uint32_t>::value);
static_assert(can::utils::crop_cast<double, uint32_t>(2.0F * static_cast<double>(max<uint32_t>::value)) == max<uint32_t>::value);

int main() {
    return 0;
}
