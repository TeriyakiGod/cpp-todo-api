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
#include "../models.h"

#define SQL_INIT "sql/todo/initTodo.sql"
#define SQL_CREATE "sql/todo/createTodo.sql"
#define SQL_UPDATE "sql/todo/updateTodo.sql"
#define SQL_DELETE "sql/todo/deleteTodo.sql"
#define SQL_GET "sql/todo/getTodo.sql"
#define SQL_GETS "sql/todo/getTodos.sql"

class TodoAPI {
public:
    static void expose(httplib::Server& svr) {

        std::ifstream file(SQL_INIT);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        SQLite::Database::executeQuery(sql);

        svr.Get("/todo", [&](const Request& req, Response& res) {
            std::ifstream file(SQL_GETS);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = SQLite::Database::executeQuery(sql);
            res.set_content(result.dump(), "application/json");
            });

        svr.Get("/todo/:string", [&](const Request& req, Response& res) {
            std::ifstream file(SQL_GET);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = SQLite::Database::executeQuery(sql, req.path_params.at("string"));
            res.set_content(result.dump(), "application/json");
            });

        svr.Post("/todo", [&](const Request& req, Response& res) {
            json json = json::parse(req.body);

            Model::Todo newTodo = Model::fromJson<Model::Todo>(json);

            std::ifstream file(SQL_CREATE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, newTodo.id, newTodo.title, newTodo.description, newTodo.status);
            res.set_content("new todo created", "text/plain");
            });

        svr.Put("/todo", [&](const Request& req, Response& res) {
            json json = json::parse(req.body);

            Model::Todo newTodo = Model::fromJson<Model::Todo>(json);

            std::ifstream file(SQL_UPDATE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, newTodo.title, newTodo.description, newTodo.status, newTodo.id);
            res.set_content("todo updated", "text/plain");
            });

        svr.Delete("/todo/:string", [&](const Request& req, Response& res) {
            std::ifstream file(SQL_DELETE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, req.path_params.at("string"));
            res.set_content("todo deleted", "text/plain");
            });
    }
};

#endif // TODO_H
