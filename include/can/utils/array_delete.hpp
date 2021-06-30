#ifndef INCLUDE_CAN_UTILS_ARRAY_DELETE_HPP
#define INCLUDE_CAN_UTILS_ARRAY_DELETE_HPP

namespace can::utils {

/*
 * Flexible Array Member (FAM) are allocated using `new[]`, but casted as
 * type `T`. By default, the object will be deleted using `delete` rather
 * than `delete[]`.
 *
 * The initial use case for this type is to be used in pointer containers,
 * such as `std::unique_ptr<T, Deleter>`, since it would normally call the
 * wrong pointer for type `T` containing a FAM.
 */

template <typename T>
struct array_delete {
    template <typename P>
    void operator()(P* ptr) const {
        /* NOLINTNEXTLINE(cppcoreguidelines-owning-memory): custom deleter */
        delete[](T*)(ptr);
    }
};

} /* namespace can::utils */

#endif /* INCLUDE_CAN_UTILS_ARRAY_DELETE_HPP */