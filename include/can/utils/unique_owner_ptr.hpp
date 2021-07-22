#ifndef INCLUDE_CAN_UTILS_UNIQUE_OWNER_PTR_HPP
#define INCLUDE_CAN_UTILS_UNIQUE_OWNER_PTR_HPP

#include <cstddef>
#include <memory>

namespace can::utils {

template <typename T>
class unique_owner_ptr;

template <typename To, typename From>
unique_owner_ptr<To> unique_owner_pointer_cast(unique_owner_ptr<From>& ptr);

template <typename T>
class unique_owner_ptr {
   public:
    unique_owner_ptr(T* ptr) : ptr_(ptr) {}

    unique_owner_ptr(std::shared_ptr<T> ptr) : ptr_(ptr) {}

    /* "rule of five" implementation */

    ~unique_owner_ptr() {
        ptr_ = nullptr;
    }

    unique_owner_ptr(const unique_owner_ptr<T>& other) = delete;

    unique_owner_ptr<T>& operator=(const unique_owner_ptr<T>& other) = delete;

    unique_owner_ptr(unique_owner_ptr<T>&& other) {
        ptr_       = other.ptr_;
        other.ptr_ = nullptr;
    }

    unique_owner_ptr<T>& operator=(unique_owner_ptr<T>&& other) {
        if (this != &other) {
            ptr_       = other.ptr_;
            other.ptr_ = nullptr;
        }

        return *this;
    }

    /* dereferencing operations */

    T* get() const {
        return ptr_.get();
    }

    T& operator*() const {
        return ptr_.operator*();
    }

    T* operator->() const {
        return ptr_.operator->();
    }

    /* comparison operations */

    [[nodiscard]] bool operator==(std::nullptr_t other) const {
        return ptr_ == other;
    }

    [[nodiscard]] bool operator!=(std::nullptr_t other) const {
        return ptr_ != other;
    }

    [[nodiscard]] bool operator==(std::shared_ptr<T> other) const {
        return ptr_ == other;
    }

    [[nodiscard]] bool operator!=(std::shared_ptr<T> other) const {
        return ptr_ != other;
    }

    [[nodiscard]] bool operator==(const unique_owner_ptr<T>& other) const {
        return ptr_ == other.ptr_;
    }

    [[nodiscard]] bool operator!=(const unique_owner_ptr<T>& other) const {
        return ptr_ != other.ptr_;
    }

   private:
    std::shared_ptr<T> ptr_;

    template <typename To, typename From>
    friend unique_owner_ptr<To> unique_owner_pointer_cast(unique_owner_ptr<From>& ptr);
};

template <typename To, typename From>
inline unique_owner_ptr<To> unique_owner_pointer_cast(unique_owner_ptr<From>& ptr) {
    auto new_ptr = unique_owner_ptr<To>(std::dynamic_pointer_cast<To>(ptr.ptr_));
    ptr.ptr_     = nullptr;
    return new_ptr;
}

template <typename T, typename... Args>
inline unique_owner_ptr<T> make_unique_owner(Args&&... args) {
    return unique_owner_ptr<T>(std::make_shared<T>(args...));
}

} /* namespace can::utils */

#endif /* INCLUDE_CAN_UTILS_UNIQUE_OWNER_PTR_HPP */