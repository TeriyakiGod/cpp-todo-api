#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <crow.h>
#include <sqlite3.h>

#include <string>

#define DB_FILE_NAME "todo.db"

namespace SQLite {
  class Database {
  public:
    static void init();

    template<typename... Args>
    static crow::json::wvalue executeQuery(const std::string& sql, Args... args) {
      sqlite3* db = openDatabase();
      if (db == nullptr)
        return nullptr;

      sqlite3_stmt* stmt;
      int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

      if (rc != SQLITE_OK) {
        CROW_LOG_ERROR << "Failed to prepare statement: " << sqlite3_errmsg(db);
        return nullptr;
      }

      bindParams(stmt, 1, args...);

      crow::json::wvalue result;
      crow::json::wvalue::list resultList;

      while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        crow::json::wvalue row;
        getJson(stmt, row);
        resultList.push_back(row);
      }

      sqlite3_finalize(stmt);

      if (rc != SQLITE_DONE) {
        CROW_LOG_ERROR << "Execution failed: " << sqlite3_errmsg(db);
        return nullptr;
      }

      if (!closeDatabase(db))
        return nullptr;

      if (resultList.size() == 1)
        result = (crow::json::wvalue)resultList[0];
      else if (resultList.size() > 1)
        result = (crow::json::wvalue)resultList;

      CROW_LOG_INFO << "Statement executed successfully.";
      return result;
    }

  private:
    static void getJson(sqlite3_stmt* stmt, crow::json::wvalue& result) {
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
      CROW_LOG_INFO << "Opening database...";
      int rc = sqlite3_open(DB_FILE_NAME, &db);
      if (rc) {
        CROW_LOG_ERROR << "Can't open database: " << sqlite3_errmsg(db);
        sqlite3_close(db);
        return nullptr;
      }
      CROW_LOG_INFO << "Database opened successfully.";
      return db;
    }

    static bool closeDatabase(sqlite3* db) {
      int rc = sqlite3_close(db);
      if (rc) {
        CROW_LOG_ERROR << "Can't close database: " << sqlite3_errmsg(db);
        return false;
      }
      CROW_LOG_INFO << "Database closed successfully.";
      return true;
    }
  };
}  // namespace SQLite

#endif  // SQLITEDATABASE_H
