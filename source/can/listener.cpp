#include <tuple>

#include "can/listener.hpp"
#include "can/log.hpp"
#include "can/utils/quark.hpp"

namespace can {

/* listener::raw_subscriber class */

listener::raw_subscriber::raw_subscriber(raw_callback callback) : callback_(std::move(callback)) {}

/* listener::message_subscriber class */

listener::message_subscriber::message_subscriber(unsigned int identifier, message_callback callback)
    : identifier_(identifier), callback_(std::move(callback)) {}

/* listener::signal_subscriber class */

listener::signal_subscriber::signal_subscriber(unsigned int identifier, std::string signal_name,
                                               signal_callback callback)
    : identifier_(identifier), signal_name_(std::move(signal_name)), callback_(std::move(callback)) {}

/* listener::subscriber_guard class */

listener::subscriber_guard::subscriber_guard(listener::ptr listener) : listener_(listener) {}

/* listener::raw_subscriber_guard class */

listener::raw_subscriber_guard::raw_subscriber_guard(listener::ptr listener, quark quark)
    : subscriber_guard(std::move(listener)), quark_(quark) {
    logger->info("new raw subscriber [quark={}]", quark_);
}

listener::raw_subscriber_guard::~raw_subscriber_guard() {
    unsubscribe();
}

void listener::raw_subscriber_guard::unsubscribe() {
    if (subscribed_) {
        subscribed_ = false;
        listener_->unsubscribe(quark_);
        logger->info("subscriber has been deleted [quark={}]", quark_);
    }
}

/* listener::message_subscriber_guard class */

listener::message_subscriber_guard::message_subscriber_guard(listener::ptr listener, unsigned int identifier,
                                                             quark quark)
    : subscriber_guard(std::move(listener)), identifier_(identifier), quark_(quark) {}

listener::message_subscriber_guard::~message_subscriber_guard() {
    unsubscribe();
}

void listener::message_subscriber_guard::unsubscribe() {
    if (subscribed_) {
        subscribed_ = false;
        listener_->unsubscribe(identifier_, quark_);
    }
}

/* listener::signal_subscriber_guard class */

listener::signal_subscriber_guard::signal_subscriber_guard(listener::ptr listener, unsigned int identifier,
                                                           const std::string& signal_name, quark quark)
    : subscriber_guard(std::move(listener)),
      identifier_(identifier),
      signal_name_(std::move(signal_name)),
      quark_(quark) {}

listener::signal_subscriber_guard::~signal_subscriber_guard() {
    unsubscribe();
}

void listener::signal_subscriber_guard::unsubscribe() {
    if (subscribed_) {
        subscribed_ = false;
        listener_->unsubscribe(identifier_, signal_name_, quark_);
    }
}

/* listener class */

listener::listener(database::const_ptr database)
    : consumer_thread_(&listener::consumer_thread_function, this), database_(database) {}

listener::~listener() {
    shutdown();
}

listener::subscriber_guard::ptr listener::subscribe(raw_callback callback) {
    std::unique_lock guard(subscriber_mutex_);

    auto quark = utils::quark::get_next();
    raw_subscribers_.emplace(std::piecewise_construct, std::forward_as_tuple(quark), std::forward_as_tuple(callback));

    auto this_ptr  = shared_from_this();
    auto sub_guard = utils::make_unique_owner<raw_subscriber_guard>(this_ptr, quark);
    return std::move(utils::unique_owner_pointer_cast<subscriber_guard>(sub_guard));
}

listener::subscriber_guard::ptr listener::subscribe(unsigned int identifier, message_callback callback) {
    std::unique_lock guard(subscriber_mutex_);

    if (!message_subscribers_.contains(identifier)) {
        message_subscribers_.insert({identifier, {}});
    }

    auto quark        = utils::quark::get_next();
    auto& subscribers = message_subscribers_.at(identifier);
    subscribers.emplace(std::piecewise_construct, std::forward_as_tuple(quark),
                        std::forward_as_tuple(identifier, callback));

    auto this_ptr  = shared_from_this();
    auto sub_guard = utils::make_unique_owner<message_subscriber_guard>(this_ptr, identifier, quark);
    return std::move(utils::unique_owner_pointer_cast<subscriber_guard>(sub_guard));
}

listener::subscriber_guard::ptr listener::subscribe(unsigned int identifier, const std::string& signal_name,
                                                    signal_callback callback) {
    std::unique_lock guard(subscriber_mutex_);

    if (!signal_subscribers_.contains(identifier)) {
        signal_subscribers_.insert({identifier, {}});
    }

    auto quark        = utils::quark::get_next();
    auto& subscribers = signal_subscribers_.at(identifier);
    subscribers.emplace(std::piecewise_construct, std::forward_as_tuple(quark),
                        std::forward_as_tuple(identifier, signal_name, callback));

    auto this_ptr  = shared_from_this();
    auto sub_guard = utils::make_unique_owner<signal_subscriber_guard>(this_ptr, identifier, signal_name, quark);
    return std::move(utils::unique_owner_pointer_cast<subscriber_guard>(sub_guard));
}

void listener::shutdown() {
    std::lock_guard<std::mutex> guard(transceiver_mutex_);

    logger->info("shutting down listener");

    for (auto& [quark, producer_thread] : producer_threads_) {
        producer_thread.running_ = false;
    }
    consumer_thread_.running_ = false;

    for (auto& [quark, producer_thread] : producer_threads_) {
        if (producer_thread.thread_.joinable()) {
            producer_thread.thread_.join();
        }
    }
    if (consumer_thread_.thread_.joinable()) {
        consumer_thread_.thread_.join();
    }

    producer_threads_.clear();
}

void listener::shutdown(quark transceiver) {
    std::lock_guard<std::mutex> guard(transceiver_mutex_);

    if (!producer_threads_.contains(transceiver)) {
        return;
    }

    logger->info("shutting down transceiver [quark={}]", transceiver);

    auto& producer_thread    = producer_threads_.at(transceiver);
    producer_thread.running_ = false;
    producer_thread.thread_.join();
    producer_threads_.erase(transceiver);
}

quark listener::add_transceiver(utils::unique_owner_ptr<transceiver> transceiver) {
    std::lock_guard<std::mutex> guard(transceiver_mutex_);

    auto quark = utils::quark::get_next();
    producer_threads_.emplace(std::piecewise_construct, std::forward_as_tuple(quark),
                              std::forward_as_tuple(&listener::producer_thread_function, this, std::move(transceiver)));

    return quark;
}

void listener::producer_thread_function(listener_thread* thread, utils::unique_owner_ptr<transceiver> transceiver) {
    logger->info("producer thread started");

    while (thread->running_) {
        auto frame = transceiver->receive();
        frames_.push(std::move(frame));
    }

    logger->info("producer thread finished");
}

void listener::consumer_thread_function(listener_thread* thread) {
    logger->info("consumer thread started {}");

    while (thread->running_) {
        auto frame = frames_.pop();
        handle_raw_subscribers(frame);
        handle_message_subscribers(frame);
        handle_signal_subscribers(frame);
    }

    logger->info("consumer thread finished");
}

void listener::handle_raw_subscribers(const frame::ptr& frame) {
    std::shared_lock guard(subscriber_mutex_);

    for (const auto& [quark, subscriber] : raw_subscribers_) {
        subscriber.callback_(frame);
    }
}

void listener::handle_message_subscribers(const frame::ptr& frame) {
    std::shared_lock guard(subscriber_mutex_);

    if (!message_subscribers_.contains(frame->identifier_)) {
        return;
    }

    auto subscribers = message_subscribers_.at(frame->identifier_);
    if (subscribers.empty()) {
        return;
    }

    auto message = database_->get_message(frame->identifier_);
    if (message == nullptr) {
        logger->error("could not find message with ID {}", frame->identifier_);
        return;
    }

    auto values = message->decode(frame->bytes_, frame->length_);
    if (values.empty()) {
        return;
    }

    for (const auto& [quark, subscriber] : subscribers) {
        subscriber.callback_(message, values);
    }
}

void listener::handle_signal_subscribers(const frame::ptr& frame) {
    std::shared_lock guard(subscriber_mutex_);

    if (!signal_subscribers_.contains(frame->identifier_)) {
        return;
    }

    auto subscribers = signal_subscribers_.at(frame->identifier_);
    if (subscribers.empty()) {
        return;
    }

    auto message = database_->get_message(frame->identifier_);
    if (message == nullptr) {
        logger->error("could not find message with ID {}", frame->identifier_);
        return;
    }

    for (auto& [quark, subscriber] : subscribers) {
        database::signal::const_ptr signal = nullptr;

        if (!subscriber.signal_quark_.has_value()) {
            signal = message->get_signal(subscriber.signal_name_);
            if (signal == nullptr) {
                logger->error("could not find signal '{}' in message '{}'", subscriber.signal_name_,
                              message->get_name());
                continue;
            }

            subscriber.signal_quark_ = signal->get_quark();
        }

        if (signal == nullptr) {
            signal = message->get_signal(subscriber.signal_quark_.value());
        }

        if (signal == nullptr) {
            logger->error("could not find signal '{}' in message '{}'", subscriber.signal_name_, message->get_name());
            subscriber.signal_quark_ = {};
            continue;
        }

        auto raw_value     = signal->extract(frame->bytes_, frame->length_);
        auto decoded_value = signal->decode(raw_value);
        subscriber.callback_(signal, decoded_value);
    }
}

void listener::unsubscribe(quark quark) {
    std::unique_lock guard(subscriber_mutex_);

    if (!raw_subscribers_.contains(quark)) {
        return;
    }

    raw_subscribers_.erase(quark);
}

void listener::unsubscribe(unsigned int identifier, quark quark) {
    std::unique_lock guard(subscriber_mutex_);

    if (!message_subscribers_.contains(identifier)) {
        return;
    }

    auto& subscribers = message_subscribers_.at(identifier);
    if (!subscribers.contains(quark)) {
        return;
    }

    subscribers.erase(quark);
}

void listener::unsubscribe(unsigned int identifier, const std::string& signal_name, quark quark) {
    std::unique_lock guard(subscriber_mutex_);

    if (!signal_subscribers_.contains(identifier)) {
        return;
    }

    auto& subscribers = signal_subscribers_.at(identifier);
    if (!subscribers.contains(quark)) {
        return;
    }

    subscribers.erase(quark);
}

} /* namespace can */