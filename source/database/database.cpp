#include "can/database/database.hpp"
#include "can/database/dbc/database.hpp"
#include "can/log.hpp"

namespace can::database {

database::ptr database::create(const std::filesystem::path& path) {
    if (!path.has_extension()) {
        logger->error("specified file '{}' does not have an extension", path.string());
        return nullptr;
    }

    if (path.extension().string() == ".dbc") {
        auto db = dbc::database::from_path(path);
        if (db == nullptr) {
            logger->error("failed to create database from file '{}'", path.string());
        }

        return db;
    }

    logger->error("invalid extension for file '{}', expected '*.dbc'", path.string());
    return nullptr;
}

} /* namespace can::database */