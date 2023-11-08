#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <crow.h>
#include <sqlite3.h>

#include <string>

#define DB_FILE_NAME "todo.db"

namespace SQLite {
  typedef int (*SQLiteCallback)(void*, int, char**, char**);

  class Callback {
  public:
    static int getJson(void* data, int argc, char** argv, char** azColName);
  };

  class Database {
  public:
    static void init();
    static bool executeQuery(const std::string& sql, SQLiteCallback callback, crow::json::wvalue& result);
    static bool executeQuery(const std::string& sql);
  private:
    static bool openDatabase();
    static bool closeDatabase();
  };
}  // namespace SQLite

#endif  // SQLITEDATABASE_H
