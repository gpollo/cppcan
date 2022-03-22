#ifndef INCLUDE_CAN_LISTENER_HPP
#define INCLUDE_CAN_LISTENER_HPP

#include <functional>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

#include "can/database.hpp"
#include "can/frame.hpp"
#include "can/transceiver.hpp"
#include "can/utils/blocking_queue.hpp"

namespace can {

class listener : public std::enable_shared_from_this<listener> {
   public:
    using ptr      = std::shared_ptr<listener>;
    using callback = std::function<void(const frame::ptr&)>;

    listener();
    ~listener();

    /**
     * This method attaches a new transceiver to the listener.
     */
    quark start(utils::unique_owner_ptr<transceiver> transceiver);

    /**
     * This method shutdowns a single transceiver.
     */
    void shutdown(quark transceiver);

    /**
     * This method shutdowns the listener and all of its transceivers. It is called in the destructor.
     */
    void shutdown();

    /**
     * This class is a wrapper around a subscriber. If will automatically unsubscribe the subscriber if it is freed.
     */
    class subscriber_guard {
       public:
        using ptr = std::unique_ptr<subscriber_guard>;

        subscriber_guard(listener::ptr listener, quark quark);
        ~subscriber_guard();

        /**
         * This method removes the subscriber from the listener. It may only be called once. It is called in the
         * destructor.
         */
        void unsubscribe();

       private:
        listener::ptr listener_;
        const quark quark_;
        bool subscribed_;
    };

    /**
     * This method creates a new subscriber for either all frame or a specific frame ID.
     */
    subscriber_guard::ptr subscribe(callback callback, std::optional<unsigned int> identifier = {});

   private:
    /**
     * This class represents a subscriber to raw frames.
     */
    struct subscriber {
        const callback callback_;
        const std::optional<unsigned int> identifier_;

        subscriber(callback callback, std::optional<unsigned int> identifier);
    };

    /**
     * Mutex used to protect subscriber list.
     */
    std::shared_mutex subscriber_mutex_;

    /**
     * The current list of subscribers.
     */
    std::unordered_map<quark, subscriber> subscribers_;

    /**
     * This method removes a subscriber from the listener.
     */
    void unsubscribe(quark quark);

    /**
     * This class represents a thread in the listener (procuder or consumer).
     */
    struct listener_thread {
        std::atomic_bool running_;
        std::thread thread_;

        template <typename Method, typename Class>
        listener_thread(Method method, Class obj, utils::unique_owner_ptr<transceiver> transceiver)
            : running_(true), thread_(method, obj, this, std::move(transceiver)) {}

        template <typename Method, typename Class>
        listener_thread(Method method, Class obj) : running_(true), thread_(method, obj, this) {}
    };

    /**
     * Mutex used to protect transceiver/threads list.
     */
    std::mutex transceiver_mutex_;

    /**
     * The current list of producer (transceiver) threads.
     */
    std::unordered_map<quark, listener_thread> producer_threads_;

    /**
     * The single consumer thread.
     */
    listener_thread consumer_thread_;

    /**
     * The queue of frames that the consumer needs to consume.
     */
    utils::blocking_queue<frame::ptr> frames_;

    /**
     * The thread function of a producer thread.
     */
    void producer_thread_function(listener_thread* thread, utils::unique_owner_ptr<transceiver> transceiver);

    /**
     * The thread function of the consumer thread.
     */
    void consumer_thread_function(listener_thread* thread);

    /**
     * This method dispatches a single frame to all relevant subscribers.
     */
    void dispatch_frame(const frame::ptr& frame);
};

} /* namespace can */

#endif /* INCLUDE_CAN_LISTENER_HPP */