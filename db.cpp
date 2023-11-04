#include "db.h"

#include <crow.h>
#include <sqlite3.h>

namespace SQLite {
sqlite3 *db;

int
Callback::getJsonList(void *data, int argc, char **argv, char **azColName) {
  crow::json::wvalue::list *result = (crow::json::wvalue::list *)data;
  crow::json::wvalue row;
  for (int i = 0; i < argc; i++) {
    row[azColName[i]] = argv[i] ? argv[i] : "NULL";
  }
  result->push_back(row);
  return 0;
}

int
Callback::getJson(void *data, int argc, char **argv, char **azColName) {
  crow::json::wvalue *result = (crow::json::wvalue *)data;

  for (int i = 0; i < argc; i++) {
    result->operator[](azColName[i]) = argv[i] ? argv[i] : "NULL";
  }
  return 0;
}

bool
Database::executeQuery(const std::string &sql, SQLiteCallback callback, crow::json::wvalue &result) {
  if (!openDatabase())
    return false;

  char *errMsg = nullptr;
  result.clear();

  int rc = sqlite3_exec(db, sql.c_str(), callback, &result, &errMsg);

  if (rc != SQLITE_OK) {
    CROW_LOG_ERROR << "SQL error: " << errMsg;
    sqlite3_free(errMsg);
    return false;
  }

  if (!closeDatabase())
    return false;

  CROW_LOG_INFO << "Query executed successfully.";
  return true;
}

bool
Database::executeQuery(const std::string &sql, SQLiteCallback callback, crow::json::wvalue::list &resultList) {
  if (!openDatabase())
    return false;

  char *errMsg = nullptr;
  resultList.clear();

  int rc = sqlite3_exec(db, sql.c_str(), callback, &resultList, &errMsg);

  if (rc != SQLITE_OK) {
    CROW_LOG_ERROR << "SQL error: " << errMsg;
    sqlite3_free(errMsg);
    return false;
  }

  if (!closeDatabase())
    return false;

  CROW_LOG_INFO << "Query executed successfully.";
  return true;
}

bool
Database::openDatabase() {
  CROW_LOG_INFO << "Opening database...";
  int rc = sqlite3_open(DB_FILE_NAME, &db);
  if (rc) {
    CROW_LOG_ERROR << "Can't open database: " << sqlite3_errmsg(db);
    sqlite3_close(db);
    return false;
  }
  CROW_LOG_INFO << "Database opened successfully.";
  return true;
}

bool
Database::closeDatabase() {
  int rc = sqlite3_close(db);
  if (rc) {
    CROW_LOG_ERROR << "Can't close database: " << sqlite3_errmsg(db);
    return false;
  }
  CROW_LOG_INFO << "Database closed successfully.";
  return true;
}
};  // namespace SQLite
