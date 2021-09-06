#ifndef INCLUDE_CAN_UTILS_BLOCKING_QUEUE_HPP
#define INCLUDE_CAN_UTILS_BLOCKING_QUEUE_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace can::utils {

/**
 * Simple blocking queue based on mutex for concurrent access. It shouldn't
 * be used with too much producers and consumers.
 *
 * TODO: Use a lockless multiple-producer, single consumer queue for can::listener.
 */
template <typename T, typename Container = std::deque<T>>
class blocking_queue {
   public:
    void push(const T& value) {
        std::lock_guard<std::mutex> guard(mutex_);
        queue_.push(value);
        condition_.notify_one();
    }

    void push(T&& value) {
        std::lock_guard<std::mutex> guard(mutex_);
        queue_.push(std::move(value));
        condition_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [&]() { return !queue_.empty(); });
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    std::optional<T> pop(unsigned int timeout_ms) {
        auto duration = std::chrono::milliseconds(timeout_ms);
        std::unique_lock<std::mutex> lock(mutex_);
        if (condition_.wait_for(lock, duration, [&]() { return !queue_.empty(); })) {
            T value = std::move(queue_.front());
            queue_.pop();
            return value;
        }

        return {};
    }

   private:
    std::queue<T, Container> queue_;
    std::condition_variable condition_;
    std::mutex mutex_;
};

} /* namespace can::utils */

#endif /* INCLUDE_CAN_UTILS_BLOCKING_QUEUE_HPP */