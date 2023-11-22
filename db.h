#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <string>
#include <sqlite3.h>
#include "spdlog/spdlog.h"
#include "json.hpp"
using json = nlohmann::json;

#define DB_FILE_NAME "todo.db"

namespace SQLite {
  class Database {
  public:
    template<typename... Args>
    static json executeQuery(const std::string& sql, Args... args) {
      spdlog::debug("Executing query: {}", sql);
      sqlite3* db = openDatabase();
      if (db == nullptr)
        return nullptr;

      sqlite3_stmt* stmt;
      int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

      if (rc != SQLITE_OK) {
        spdlog::error("Can't prepare statement: {}", sqlite3_errmsg(db));
        return nullptr;
      }

      bindParams(stmt, 1, args...);

      json result = json::array();

      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
      json row;
      getJson(stmt, row);
      result.push_back(row);
      }

      switch (rc) {
      case SQLITE_BUSY:
        spdlog::error("The database file is locked.");
        break;
      case SQLITE_DONE:
        spdlog::info("Query executed successfully.");
        break;
      case SQLITE_ERROR:
        spdlog::error("SQL error: {}", sqlite3_errmsg(db));
        break;
      case SQLITE_MISUSE:
        spdlog::error("SQLite misuse: {}", sqlite3_errmsg(db));
        break;
      }

      sqlite3_finalize(stmt);

      if (!closeDatabase(db))
        return nullptr;

      if (result.size() == 1 || result.size() == 0)
        return result[0];
      else
        return result;
    }

  private:
    static void getJson(sqlite3_stmt* stmt, json& result) {
      int columnCount = sqlite3_column_count(stmt);
      for (int i = 0; i < columnCount; i++) {
        const char* columnName = sqlite3_column_name(stmt, i);
        switch (sqlite3_column_type(stmt, i)) {
        case SQLITE_INTEGER:
          result[columnName] = sqlite3_column_int(stmt, i);
          break;
        case SQLITE_FLOAT:
          result[columnName] = sqlite3_column_double(stmt, i);
          break;
        case SQLITE_TEXT:
          result[columnName] = (const char*)sqlite3_column_text(stmt, i);
          break;
        case SQLITE_BLOB:
          result[columnName] = (const char*)sqlite3_column_blob(stmt, i);
          break;
        case SQLITE_NULL:
          result[columnName] = nullptr;
          break;
        }
      }
    }

    template<typename T, typename... Args>
    static void bindParams(sqlite3_stmt* stmt, int index, T value, Args... args) {
      if constexpr (std::is_integral_v<T>) {
        sqlite3_bind_int(stmt, index, value);
      }
      else if constexpr (std::is_floating_point_v<T>) {
        sqlite3_bind_double(stmt, index, value);
      }
      else if constexpr (std::is_same_v<T, std::string>) {
        sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
      }
      bindParams(stmt, index + 1, args...);
    }

    static void bindParams(sqlite3_stmt* stmt, int index) {
      // No-op for base case
    }

    static sqlite3* openDatabase() {
      sqlite3* db;
      spdlog::info("Opening database...");
      int rc = sqlite3_open(DB_FILE_NAME, &db);
      if (rc) {
        spdlog::error("Can't open database: {}", sqlite3_errmsg(db));
        sqlite3_close(db);
        return nullptr;
      }
      spdlog::info("Database opened successfully.");
      return db;
    }

    static bool closeDatabase(sqlite3* db) {
      int rc = sqlite3_close(db);
      if (rc) {
        spdlog::error("Can't close database: {}", sqlite3_errmsg(db));
        return false;
      }
      spdlog::info("Database closed successfully.");
      return true;
    }
  };
}  // namespace SQLite

#endif  // SQLITEDATABASE_H
