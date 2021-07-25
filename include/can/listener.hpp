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
#include "can/utils/unique_owner_ptr.hpp"

namespace can {

class listener : public std::enable_shared_from_this<listener> {
   public:
    using ptr              = std::shared_ptr<listener>;
    using raw_callback     = std::function<void(const frame::ptr&)>;
    using message_callback = std::function<void(database::message::const_ptr,
                                                const std::vector<std::pair<database::signal::const_ptr, float>>&)>;
    using signal_callback  = std::function<void(database::signal::const_ptr, float)>;

    listener(database::const_ptr database);
    ~listener();

    class subscriber_guard {
       public:
        using ptr = utils::unique_owner_ptr<subscriber_guard>;

        subscriber_guard(listener::ptr listener);
        virtual ~subscriber_guard() = default;

        virtual void unsubscribe() = 0;

       protected:
        bool subscribed_ = true;
        listener::ptr listener_;
    };

    subscriber_guard::ptr subscribe(raw_callback callback);
    subscriber_guard::ptr subscribe(unsigned int identifier, message_callback callback);
    subscriber_guard::ptr subscribe(unsigned int identifier, const std::string& signal_name, signal_callback callback);
    void shutdown();
    void shutdown(quark transceiver);
    quark add_transceiver(utils::unique_owner_ptr<transceiver> transceiver);

   private:
    struct raw_subscriber {
        const raw_callback callback_;

        raw_subscriber(raw_callback callback);
    };

    struct message_subscriber {
        const unsigned int identifier_;
        const message_callback callback_;

        message_subscriber(unsigned int identifier, message_callback callback);
    };

    struct signal_subscriber {
        const unsigned int identifier_;
        const std::string signal_name_;
        const signal_callback callback_;
        std::optional<quark> signal_quark_;

        signal_subscriber(unsigned int identifier, std::string signal_name, signal_callback callback);
    };

    class raw_subscriber_guard : public subscriber_guard {
       public:
        raw_subscriber_guard(listener::ptr listener, quark quark);
        ~raw_subscriber_guard() override;
        void unsubscribe() override;

       private:
        const quark quark_;
    };

    class message_subscriber_guard : public subscriber_guard {
       public:
        message_subscriber_guard(listener::ptr listener, unsigned int identifier, quark quark);
        ~message_subscriber_guard() override;
        void unsubscribe() override;

       private:
        const unsigned int identifier_;
        const quark quark_;
    };

    class signal_subscriber_guard : public subscriber_guard {
       public:
        signal_subscriber_guard(listener::ptr listener, unsigned int identifier, const std::string& signal_name,
                                quark quark);
        ~signal_subscriber_guard() override;
        void unsubscribe() override;

       private:
        const unsigned int identifier_;
        const std::string signal_name_;
        const quark quark_;
    };

    struct listener_thread {
        std::atomic_bool running_;
        std::thread thread_;

        template <typename Method, typename Class>
        listener_thread(Method method, Class obj, utils::unique_owner_ptr<transceiver> transceiver)
            : running_(true), thread_(method, obj, this, std::move(transceiver)) {}

        template <typename Method, typename Class>
        listener_thread(Method method, Class obj) : running_(true), thread_(method, obj, this) {}
    };

    std::mutex transceiver_mutex_;
    std::shared_mutex subscriber_mutex_;

    std::unordered_map<quark, listener_thread> producer_threads_;
    listener_thread consumer_thread_;

    std::unordered_map<quark, raw_subscriber> raw_subscribers_;
    std::unordered_map<unsigned int, std::unordered_map<quark, message_subscriber>> message_subscribers_;
    std::unordered_map<unsigned int, std::unordered_map<quark, signal_subscriber>> signal_subscribers_;

    database::const_ptr database_;
    utils::blocking_queue<frame::ptr> frames_;

    void producer_thread_function(listener_thread* thread, utils::unique_owner_ptr<transceiver> transceiver);
    void consumer_thread_function(listener_thread* thread);
    void handle_raw_subscribers(const frame::ptr& frame);
    void handle_message_subscribers(const frame::ptr& frame);
    void handle_signal_subscribers(const frame::ptr& frame);

    void unsubscribe(quark quark);
    void unsubscribe(unsigned int identifier, quark quark);
    void unsubscribe(unsigned int identifier, const std::string& signal_name, quark quark);
};

} /* namespace can */

#endif /* INCLUDE_CAN_LISTENER_HPP */