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
#include "auth.h"

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

            svr.Get("/todo", get_todos_handler());
            svr.Get("/todo/:string", get_todo_by_id_handler());
            svr.Post("/todo", post_todo_handler());
            svr.Put("/todo", put_todo_handler());
            svr.Delete("/todo/:string", delete_todo_by_id_handler());
        }

    private:

        static Server::Handler get_todos_handler() {
            return [&](const Request& req, Response& res) {
                std::string token = req.get_header_value("Authorization");
                std::string user_id = Tools::Jwt::verify_user(token);
                if (user_id.empty())
                {
                    res.set_content("Invalid token", "text/plain");
                    return;
                }
                res.set_content(get_todos(user_id), "application/json");
                };
        }

        static Server::Handler get_todo_by_id_handler() {
            return [&](const Request& req, Response& res) {
                std::string todo_id = req.path_params.at("string");
                std::string token = req.get_header_value("Authorization");
                std::string user_id = Tools::Jwt::verify_user(token);
                if (user_id.empty())
                {
                    res.set_content("Invalid token", "text/plain");
                    return;
                }
                res.set_content(get_todo_by_id(todo_id, user_id), "application/json");
                };
        }

        static Server::Handler post_todo_handler() {
            return [&](const Request& req, Response& res) {
                json j = json::parse(req.body);
                auto new_todo = j.template get<Model::Todo>();
                new_todo.todo_id = Tools::Uuid::generate();
                std::string token = req.get_header_value("Authorization");
                std::string user_id = Tools::Jwt::verify_user(token);
                if (user_id.empty())
                {
                    res.set_content("Invalid token", "text/plain");
                    return;
                }
                new_todo.user_id = user_id;
                res.set_content(create_todo_from_request(new_todo), "text/plain");
                };
        }

        static Server::Handler put_todo_handler() {
            return [&](const Request& req, Response& res) {
                json j = json::parse(req.body);
                auto new_todo = j.template get<Model::Todo>();
                new_todo.todo_id = Tools::Uuid::generate();
                std::string token = req.get_header_value("Authorization");
                std::string user_id = Tools::Jwt::verify_user(token);
                if (user_id.empty())
                {
                    res.set_content("Invalid token", "text/plain");
                    return;
                }
                new_todo.user_id = user_id;
                res.set_content(update_todo_from_request(new_todo), "text/plain");
                };
        }

        static Server::Handler delete_todo_by_id_handler() {
            return [&](const Request& req, Response& res) {
                std::string token = req.get_header_value("Authorization");
                std::string user_id = Tools::Jwt::verify_user(token);
                std::string todo_id = req.path_params.at("string");
                if (user_id.empty())
                {
                    res.set_content("Invalid token", "text/plain");
                    return;
                }
                res.set_content(delete_todo_by_id(todo_id, user_id), "text/plain");
                };
        }

        static void prepare_todo_table() {
            std::ifstream file(SQL_CREATE_TODO_TABLE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Sqlite::Database::execute_query(sql);
        }

        static std::string get_todos(std::string user_id) {
            std::ifstream file(SQL_GET_TODOS);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = Sqlite::Database::execute_query(sql, user_id);
            return result.dump();
        }

        static std::string get_todo_by_id(const std::string& id, const std::string& user_id) {
            std::ifstream file(SQL_GET_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = Sqlite::Database::execute_query(sql, id, user_id);
            return result.dump();
        }

        static std::string create_todo_from_request(const Model::Todo& new_todo) {
            std::ifstream file(SQL_CREATE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Sqlite::Database::execute_query(sql, new_todo.todo_id, new_todo.user_id, new_todo.title, new_todo.description, new_todo.status);
            return "new todo created";
        }

        static std::string update_todo_from_request(const Model::Todo& new_todo) {
            std::ifstream file(SQL_UPDATE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Sqlite::Database::execute_query(sql, new_todo.title, new_todo.description, new_todo.status, new_todo.todo_id);
            return "todo updated";
        }

        static std::string delete_todo_by_id(const std::string& id, const std::string& user_id) {
            std::ifstream file(SQL_DELETE_TODO);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            Sqlite::Database::execute_query(sql, id);
            return "todo deleted";
        }
    };
}// namespace Controller

#endif // TODO_H
