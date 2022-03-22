#include <tuple>

#include "can/listener.hpp"
#include "can/log.hpp"
#include "can/utils/quark.hpp"

namespace can {

/* listener class */

listener::listener() : consumer_thread_(&listener::consumer_thread_function, this) {}

listener::~listener() {
    shutdown();
}

quark listener::start(utils::unique_owner_ptr<transceiver> transceiver) {
    std::lock_guard<std::mutex> guard(transceiver_mutex_);

    auto quark = utils::quark::get_next();
    producer_threads_.emplace(std::piecewise_construct, std::forward_as_tuple(quark),
                              std::forward_as_tuple(&listener::producer_thread_function, this, std::move(transceiver)));

    return quark;
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

listener::subscriber_guard::ptr listener::subscribe(callback callback, std::optional<unsigned int> identifier) {
    std::unique_lock guard(subscriber_mutex_);

    auto quark = utils::quark::get_next();
    subscribers_.emplace(std::piecewise_construct, std::forward_as_tuple(quark),
                         std::forward_as_tuple(callback, identifier));

    return std::make_unique<subscriber_guard>(shared_from_this(), quark);
}

void listener::unsubscribe(quark quark) {
    std::unique_lock guard(subscriber_mutex_);

    if (subscribers_.contains(quark)) {
        subscribers_.erase(quark);
    }
}

void listener::producer_thread_function(listener_thread* thread, utils::unique_owner_ptr<transceiver> transceiver) {
    logger->info("producer thread started");

    while (thread->running_) {
        auto frame = transceiver->receive(1000);
        if (frame != nullptr) {
            frames_.push(std::move(frame));
        }
    }

    logger->info("producer thread finished");
}

void listener::consumer_thread_function(listener_thread* thread) {
    logger->info("consumer thread started {}");

    while (thread->running_) {
        auto frame_opt = frames_.pop(1000);
        if (!frame_opt.has_value()) {
            continue;
        }

        auto frame = std::move(frame_opt.value());
        dispatch_frame(frame);
    }

    logger->info("consumer thread finished");
}

void listener::dispatch_frame(const frame::ptr& frame) {
    std::shared_lock guard(subscriber_mutex_);

    for (const auto& [_, subscriber] : subscribers_) {
        if (frame->identifier_ == subscriber.identifier_.value_or(frame->identifier_)) {
            subscriber.callback_(frame);
        }
    }
}

/* listener::raw_subscriber class */

listener::subscriber::subscriber(callback callback, std::optional<unsigned int> identifier)
    : callback_(std::move(callback)), identifier_(identifier) {}

/* listener::raw_subscriber_guard class */

listener::subscriber_guard::subscriber_guard(listener::ptr listener, quark quark)
    : listener_(std::move(listener)), quark_(quark), subscribed_(true) {
    logger->info("new subscriber [quark={}]", quark_);
}

listener::subscriber_guard::~subscriber_guard() {
    unsubscribe();
}

void listener::subscriber_guard::unsubscribe() {
    if (subscribed_) {
        subscribed_ = false;
        listener_->unsubscribe(quark_);
        logger->info("subscriber has been deleted [quark={}]", quark_);
    }
}

} /* namespace can */