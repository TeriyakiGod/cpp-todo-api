#ifndef SQLITE_DATABASE_H
#define SQLITE_DATABASE_H

#include "json.hpp"
#include "spdlog/spdlog.h"
#include <sqlite3.h>
#include <string>
using json = nlohmann::json;

#define DB_FILE_NAME "../db.sqlite3"

namespace Sqlite {
class Database {
public:
    template <typename... Args> static json execute_query(const std::string &sql, Args... args) {
        spdlog::debug("Executing query:\n{}", sql);
        sqlite3 *db = open_database();
        if (db == nullptr)
            return nullptr;

        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        if (rc != SQLITE_OK) {
            spdlog::error("Can't prepare statement: {}", sqlite3_errmsg(db));
            return nullptr;
        }

        bind_params(stmt, 1, args...);

        json result = json::array();

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            json row;
            get_json(stmt, row);
            result.push_back(row);
        }

        switch (rc) {
        case SQLITE_BUSY:
            spdlog::error("The database file is locked.");
            break;
        case SQLITE_DONE:
            spdlog::debug("Query executed successfully.");
            break;
        case SQLITE_ERROR:
            spdlog::error("SQL error: {}", sqlite3_errmsg(db));
            break;
        case SQLITE_MISUSE:
            spdlog::error("SQLite misuse: {}", sqlite3_errmsg(db));
            break;
        }

        sqlite3_finalize(stmt);

        if (!close_database(db))
            return nullptr;
        if (result.size() == (size_t)1 || result.size() == (size_t)0) {
            return result[0];
        } else {
            return result;
        }
    }

private:
    static void get_json(sqlite3_stmt *stmt, json &result) {
        int column_count = sqlite3_column_count(stmt);
        for (int i = 0; i < column_count; i++) {
            const char *column_name = sqlite3_column_name(stmt, i);
            switch (sqlite3_column_type(stmt, i)) {
            case SQLITE_INTEGER:
                result[column_name] = sqlite3_column_int(stmt, i);
                break;
            case SQLITE_FLOAT:
                result[column_name] = sqlite3_column_double(stmt, i);
                break;
            case SQLITE_TEXT:
                result[column_name] = (const char *)sqlite3_column_text(stmt, i);
                break;
            case SQLITE_BLOB:
                result[column_name] = (const char *)sqlite3_column_blob(stmt, i);
                break;
            case SQLITE_NULL:
                result[column_name] = nullptr;
                break;
            }
        }
    }

    template <typename T, typename... Args>
    static void bind_params(sqlite3_stmt *stmt, int index, T value, Args... args) {
        if constexpr (std::is_integral_v<T>) {
            sqlite3_bind_int(stmt, index, value);
        } else if constexpr (std::is_floating_point_v<T>) {
            sqlite3_bind_double(stmt, index, value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
        }
        bind_params(stmt, index + 1, args...);
    }

    static void bind_params(sqlite3_stmt *stmt, int index) {
        // No-op for base case
    }

    static sqlite3 *open_database() {
        sqlite3 *db;
        spdlog::debug("Opening database...");
        int rc = sqlite3_open(DB_FILE_NAME, &db);
        if (rc) {
            spdlog::error("Can't open database: {}", sqlite3_errmsg(db));
            sqlite3_close(db);
            return nullptr;
        }
        spdlog::debug("Database opened successfully.");
        return db;
    }

    static bool close_database(sqlite3 *db) {
        int rc = sqlite3_close(db);
        if (rc) {
            spdlog::error("Can't close database: {}", sqlite3_errmsg(db));
            return false;
        }
        spdlog::debug("Database closed successfully.");
        return true;
    }
};
} // namespace Sqlite

#endif // SQLITE_DATABASE_H
