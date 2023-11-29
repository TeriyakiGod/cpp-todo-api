#ifndef TODO_H
#define TODO_H

#include <fstream>
#include <string>
#include <httplib.h>
using namespace httplib;
#include <json.hpp>
using json = nlohmann::json;
#include <spdlog/spdlog.h>
#include "../db.h"
#include "../models/todo.h"

#define SQL_CREATE_TODO_TABLE "../sql/todo/createTodoTable.sql"
#define SQL_CREATE_TODO "../sql/todo/createTodo.sql"
#define SQL_UPDATE_TODO "../sql/todo/updateTodo.sql"
#define SQL_DELETE_TODO "../sql/todo/deleteTodo.sql"
#define SQL_GET_TODO "../sql/todo/getTodo.sql"
#define SQL_GET_TODOS "../sql/todo/getTodos.sql"

namespace Controller {
    class Todo {
    public:
        Todo(httplib::Server& svr) {
            prepareTodoTable();

            svr.Get("/todo", [&](const Request& req, Response& res) {
                res.set_content(getTodos(), "application/json");
            });

            svr.Get("/todo/:string", [&](const Request& req, Response& res) {
                res.set_content(getTodoById(req.path_params.at("string")), "application/json");
            });

            svr.Post("/todo", [&](const Request& req, Response& res) {
                res.set_content(createTodoFromRequest(req.body), "text/plain");
            });

            svr.Put("/todo", [&](const Request& req, Response& res) {
                res.set_content(updateTodoFromRequest(req.body), "text/plain");
            });

            svr.Delete("/todo/:string", [&](const Request& req, Response& res) {
                res.set_content(deleteTodoById(req.path_params.at("string")), "text/plain");
            });
        }

    private:
        void prepareTodoTable(){
            std::ifstream file(SQL_CREATE_TODO_TABLE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql);
        }

        std::string getTodos() {
            std::ifstream file(SQL_GET_TODOS);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = SQLite::Database::executeQuery(sql);
            return result.dump();
        }

        std::string getTodoById(const std::string& id) {
            std::ifstream file(SQL_GET_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = SQLite::Database::executeQuery(sql, id);
            return result.dump();
        }

        std::string createTodoFromRequest(const std::string& requestBody) {
            json j = json::parse(requestBody);
            auto newTodo = j.template get<Model::Todo>();

            std::ifstream file(SQL_CREATE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, newTodo.todo_id, newTodo.user_id, newTodo.title, newTodo.description, newTodo.status);
            return "new todo created";
        }

        std::string updateTodoFromRequest(const std::string& requestBody) {
            json j = json::parse(requestBody);
            auto newTodo = j.template get<Model::Todo>();

            std::ifstream file(SQL_UPDATE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, newTodo.title, newTodo.description, newTodo.status, newTodo.todo_id);
            return "todo updated";
        }

        std::string deleteTodoById(const std::string& id) {
            std::ifstream file(SQL_DELETE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, id);
            return "todo deleted";
        }
    };

} // namespace API

#endif // TODO_H
