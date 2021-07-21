#ifndef INCLUDE_CAN_DATABASES_HPP
#define INCLUDE_CAN_DATABASES_HPP

#include <shared_mutex>
#include <unordered_map>

#include "can/database.hpp"
#include "utils/quark.hpp"

namespace can {

class databases : public database {
   public:
    using ptr       = std::shared_ptr<databases>;
    using const_ptr = std::shared_ptr<const databases>;

    static ptr create();

    quark add(database::const_ptr subdatabase);
    void remove(quark subdatabase);

    /* inherited methods from can::database::message */

    [[nodiscard]] std::vector<message::const_ptr> get_messages() const override;
    [[nodiscard]] message::const_ptr get_message(unsigned int identifier) const override;
    [[nodiscard]] message::const_ptr get_message(const std::string& name) const override;

   private:
    std::unordered_map<quark, database::const_ptr> subdatabases_;
    mutable std::shared_mutex mutex_;
};

} /* namespace can */

#endif /* INCLUDE_CAN_DATABASES_HPP */