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

#define SQL_CREATE_TODO_TABLE "sql/todo/initTodoTable.sql"
#define SQL_CREATE_TODO "sql/todo/createTodo.sql"
#define SQL_UPDATE_TODO "sql/todo/updateTodo.sql"
#define SQL_DELETE_TODO "sql/todo/deleteTodo.sql"
#define SQL_GET_TODO "sql/todo/getTodo.sql"
#define SQL_GET_TODOS "sql/todo/getTodos.sql"

namespace Controller {
    class Todo {
    public:
        static void expose(httplib::Server& svr) {
            std::ifstream file(SQL_CREATE_TODO_TABLE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql);

            svr.Get("/todo", [&](const Request& req, Response& res) {
                std::ifstream file(SQL_GET_TODOS);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                json result = SQLite::Database::executeQuery(sql);
                res.set_content(result.dump(), "application/json");
                });

            svr.Get("/todo/:string", [&](const Request& req, Response& res) {
                std::ifstream file(SQL_GET_TODO);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                json result = SQLite::Database::executeQuery(sql, req.path_params.at("string"));
                res.set_content(result.dump(), "application/json");
                });

            svr.Post("/todo", [&](const Request& req, Response& res) {
                json j = json::parse(req.body);
                auto newTodo = j.template get<Model::Todo>();

                std::ifstream file(SQL_CREATE_TODO);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                SQLite::Database::executeQuery(sql, newTodo.id, newTodo.title, newTodo.description, newTodo.status);
                res.set_content("new todo created", "text/plain");
                });

            svr.Put("/todo", [&](const Request& req, Response& res) {
                json j = json::parse(req.body);

                auto newTodo = j.template get<Model::Todo>();

                std::ifstream file(SQL_UPDATE_TODO);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                SQLite::Database::executeQuery(sql, newTodo.title, newTodo.description, newTodo.status, newTodo.id);
                res.set_content("todo updated", "text/plain");
                });

            svr.Delete("/todo/:string", [&](const Request& req, Response& res) {
                std::ifstream file(SQL_DELETE_TODO);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                SQLite::Database::executeQuery(sql, req.path_params.at("string"));
                res.set_content("todo deleted", "text/plain");
                });
        }
    };

} // namespace API

#endif // TODO_H
