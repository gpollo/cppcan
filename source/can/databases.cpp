#include <mutex>

#include "can/databases.hpp"
#include "can/log.hpp"

namespace can {

databases::ptr create() {
    return std::make_shared<databases>();
}

quark databases::add(database::const_ptr subdatabase) {
    std::unique_lock guard(mutex_);

    auto new_quark = utils::quark::get_next();
    subdatabases_.insert({new_quark, subdatabase});
    return new_quark;
}

void databases::remove(quark subdatabase) {
    std::unique_lock guard(mutex_);

    if (subdatabases_.contains(subdatabase)) {
        subdatabases_.erase(subdatabase);
    }
}

std::vector<database::message::const_ptr> databases::get_messages() const {
    std::shared_lock guard(mutex_);

    std::vector<database::message::const_ptr> messages;
    for (auto [quark, subdatabase] : subdatabases_) {
        auto subdatabase_messages = subdatabase->get_messages();
        messages.insert(messages.end(), subdatabase_messages.begin(), subdatabase_messages.end());
    }

    return messages;
}

database::message::const_ptr databases::get_message(unsigned int identifier) const {
    std::shared_lock guard(mutex_);

    database::message::const_ptr message = nullptr;
    for (auto [quark, subdatabase] : subdatabases_) {
        auto subdatabase_message = subdatabase->get_message(identifier);
        if (subdatabase_message == nullptr) {
            continue;
        }

        if (message == nullptr) {
            message = subdatabase_message;
        } else {
            logger->error("message with identifier {} is already provided by another database", identifier);
        }
    }

    return message;
}

database::message::const_ptr databases::get_message(const std::string& name) const {
    std::shared_lock guard(mutex_);

    database::message::const_ptr message = nullptr;
    for (auto [quark, subdatabase] : subdatabases_) {
        auto subdatabase_message = subdatabase->get_message(name);
        if (subdatabase_message == nullptr) {
            continue;
        }

        if (message == nullptr) {
            message = subdatabase_message;
        } else {
            logger->error("message with name '{}' is already provided by another database", name);
        }
    }

    return message;
}

} /* namespace can */