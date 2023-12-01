#ifndef TODO_H
#define TODO_H

#include <fstream>
#include <string>
#include <httplib.h>
using namespace httplib;
#include <json.hpp>
using json = nlohmann::json;
#include <spdlog/spdlog.h>
#include "../src/db.h"
#include "../models/todo.h"
#include "../src/tools.h"

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
            prepare_todo_table();

            svr.Get("/todo", [&](const Request& req, Response& res) {
                res.set_header("Access-Control-Allow-Origin", Tools::Resource::load_string("config", "server_url").c_str());
                res.set_content(get_todos(), "application/json");
                });

            svr.Get("/todo/:string", [&](const Request& req, Response& res) {
                res.set_header("Access-Control-Allow-Origin", Tools::Resource::load_string("config", "server_url").c_str());
                res.set_content(get_todo_by_id(req.path_params.at("string")), "application/json");
                });

            svr.Post("/todo", [&](const Request& req, Response& res) {
                json j = json::parse(req.body);
                auto new_todo = j.template get<Model::Todo>();
                res.set_header("Access-Control-Allow-Origin", Tools::Resource::load_string("config", "server_url").c_str());
                res.set_content(create_todo_from_request(new_todo), "text/plain");
                });

            svr.Put("/todo", [&](const Request& req, Response& res) {
                json j = json::parse(req.body);
                auto new_todo = j.template get<Model::Todo>();
                res.set_header("Access-Control-Allow-Origin", Tools::Resource::load_string("config", "server_url").c_str());
                res.set_content(update_todo_from_request(new_todo), "text/plain");
                });

            svr.Delete("/todo/:string", [&](const Request& req, Response& res) {
                res.set_header("Access-Control-Allow-Origin", Tools::Resource::load_string("config", "server_url").c_str());
                res.set_content(delete_todo_by_id(req.path_params.at("string")), "text/plain");
                });
        }

    private:
        void prepare_todo_table() {
            std::ifstream file(SQL_CREATE_TODO_TABLE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Sqlite::Database::execute_query(sql);
        }

        std::string get_todos() {
            std::ifstream file(SQL_GET_TODOS);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = Sqlite::Database::execute_query(sql);
            return result.dump();
        }

        std::string get_todo_by_id(const std::string& id) {
            std::ifstream file(SQL_GET_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = Sqlite::Database::execute_query(sql, id);
            return result.dump();
        }

        std::string create_todo_from_request(const Model::Todo& new_todo) {
            std::ifstream file(SQL_CREATE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Sqlite::Database::execute_query(sql, new_todo.todo_id, new_todo.user_id, new_todo.title, new_todo.description, new_todo.status);
            return "new todo created";
        }

        std::string update_todo_from_request(const Model::Todo& new_todo) {
            std::ifstream file(SQL_UPDATE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Sqlite::Database::execute_query(sql, new_todo.title, new_todo.description, new_todo.status, new_todo.todo_id);
            return "todo updated";
        }

        std::string delete_todo_by_id(const std::string& id) {
            std::ifstream file(SQL_DELETE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Sqlite::Database::execute_query(sql, id);
            return "todo deleted";
        }
    };

} // namespace API

#endif // TODO_H
