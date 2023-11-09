#ifndef TODO_H
#define TODO_H

#include <fstream>
#include <string>
#include "../httplib.h"
using namespace httplib;
#include "../json.hpp"
using json = nlohmann::json;
#include <spdlog/spdlog.h>
#include "../db.h"
#include "../models/todo.h"

#define SQL_INIT_TODO "sql/initTodo.sql"
#define SQL_CREATE_TODO "sql/createTodo.sql"
#define SQL_UPDATE_TODO "sql/updateTodo.sql"
#define SQL_DELETE_TODO "sql/deleteTodo.sql"
#define SQL_GET_TODO "sql/getTodo.sql"
#define SQL_GET_TODOS "sql/getTodos.sql"

class TodoAPI {
public:
    static void expose(httplib::Server& svr) {

        std::ifstream file(SQL_INIT_TODO);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        SQLite::Database::executeQuery(sql);

        svr.Get("/todos", [&](const Request& req, Response& res) {
            std::ifstream file(SQL_GET_TODOS);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = SQLite::Database::executeQuery(sql);
            res.set_content(result.dump(), "application/json");
            });

        svr.Get("/todos/:string", [&](const Request& req, Response& res) {
            std::ifstream file(SQL_GET_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = SQLite::Database::executeQuery(sql, req.path_params.at("string"));
            res.set_content(result.dump(), "application/json");
            });

        svr.Post("/todos", [&](const Request& req, Response& res) {
            json body = json::parse(req.body);

            todo newTodo{
                body["id"].get<std::string>(),
                body["title"].get<std::string>(),
                body["description"].get<std::string>(),
                body["status"].get<std::string>()
            };

            std::ifstream file(SQL_CREATE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            SQLite::Database::executeQuery(sql, newTodo.id, newTodo.title, newTodo.description, newTodo.status);
            res.set_content("New todo created", "text/plain");
            });

        svr.Put("/todos", [&](const Request& req, Response& res) {
            json body = json::parse(req.body);

            todo newTodo{
                body["id"].get<std::string>(),
                body["title"].get<std::string>(),
                body["description"].get<std::string>(),
                body["status"].get<std::string>()
            };

            std::ifstream file(SQL_UPDATE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, newTodo.title, newTodo.description, newTodo.status, newTodo.id);
            res.set_content("Todo updated", "text/plain");
            });

        svr.Delete("/todos/:string", [&](const Request& req, Response& res) {
            std::ifstream file(SQL_DELETE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, req.path_params.at("string"));
            res.set_content("Todo deleted", "text/plain");
            });
    }
};

#endif // TODO_H
