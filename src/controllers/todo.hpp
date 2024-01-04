#ifndef TODO_H
#define TODO_H

#include <fstream>
#include <httplib.h>
#include <string>
using namespace httplib;
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "../db.hpp"
#include "../models/todo.hpp"
#include "../tools.hpp"
#include "auth.hpp"
#include <spdlog/spdlog.h>

#define SQL_CREATE_TODO_TABLE "../res/sql/todo/createTodoTable.sql"
#define SQL_CREATE_TODO "../res/sql/todo/createTodo.sql"
#define SQL_UPDATE_TODO "../res/sql/todo/updateTodo.sql"
#define SQL_DELETE_TODO "../res/sql/todo/deleteTodo.sql"
#define SQL_GET_TODO "../res/sql/todo/getTodo.sql"
#define SQL_GET_TODOS "../res/sql/todo/getTodos.sql"

namespace Controller {
/// @brief This class handles all the todo related routes
class Todo {
public:
    /// @brief This will register all the routes related to todo
    /// @param svr The server object
    Todo(httplib::Server &svr) {
        prepare_todo_table();

        svr.Get("/todo", get_todos_handler());
        svr.Get("/todo/:string", get_todo_by_id_handler());
        svr.Post("/todo", post_todo_handler());
        svr.Put("/todo", put_todo_handler());
        svr.Delete("/todo/:string", delete_todo_by_id_handler());
    }

private:
    /// @brief This will return the handler for getting all the todos
    /// @return Server::Handler Returns the handler for getting all the todos
    static Server::Handler get_todos_handler() {
        return [&](const Request &req, Response &res) {
            std::string user_id = res.get_header_value("user_id");
            res.set_content(get_todos(user_id), "application/json");
        };
    }

    /// @brief This will return the handler for getting a todo by id
    /// @return Server::Handler Returns the handler for getting a todo by id
    static Server::Handler get_todo_by_id_handler() {
        return [&](const Request &req, Response &res) {
            std::string todo_id = req.path_params.at("string");
            std::string user_id = res.get_header_value("user_id");
            res.set_content(get_todo_by_id(todo_id, user_id), "application/json");
        };
    }

    /// @brief This will return the handler for creating a todo
    /// @return Server::Handler Returns the handler for creating a todo
    static Server::Handler post_todo_handler() {
        return [&](const Request &req, Response &res) {
            json j = json::parse(req.body);
            auto new_todo = j.template get<Model::Todo>();
            std::string user_id = res.get_header_value("user_id");
            new_todo.user_id = user_id;
            res.set_content(create_todo(new_todo), "text/plain");
        };
    }

    /// @brief This will return the handler for updating a todo
    /// @return Server::Handler Returns the handler for updating a todo
    static Server::Handler put_todo_handler() {
        return [&](const Request &req, Response &res) {
            json j = json::parse(req.body);
            auto new_todo = j.template get<Model::Todo>();
            std::string user_id = res.get_header_value("user_id");
            new_todo.user_id = user_id;
            res.set_content(update_todo(new_todo), "text/plain");
        };
    }

    /// @brief This will return the handler for deleting a todo by id
    /// @return Server::Handler Returns the handler for deleting a todo by id
    static Server::Handler delete_todo_by_id_handler() {
        return [&](const Request &req, Response &res) {
            std::string todo_id = req.path_params.at("string");
            std::string user_id = res.get_header_value("user_id");
            res.set_content(delete_todo(todo_id, user_id), "text/plain");
        };
    }

    /// @brief This will prepare the todo table
    static void prepare_todo_table() {
        std::ifstream file(SQL_CREATE_TODO_TABLE);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Sqlite::Database::execute_query(sql);
    }

    /// @brief This will return all the todos
    /// @param user_id The user_id of the user
    /// @return std::string Returns the todos in json format as string
    static std::string get_todos(std::string user_id) {
        std::ifstream file(SQL_GET_TODOS);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        json result = Sqlite::Database::execute_query(sql, user_id);
        return result.dump();
    }

    /// @brief This will return a todo by id
    /// @param id The id of the todo
    /// @param user_id The user_id of the user
    /// @return std::string Returns the todo in json format as string
    static std::string get_todo_by_id(const std::string &id, const std::string &user_id) {
        std::ifstream file(SQL_GET_TODO);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        json result = Sqlite::Database::execute_query(sql, id, user_id);
        return result.dump();
    }

    /// @brief This will create a new todo
    /// @param new_todo The todo object
    /// @return std::string Returns the message
    static std::string create_todo(const Model::Todo &new_todo) {
        std::ifstream file(SQL_CREATE_TODO);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Sqlite::Database::execute_query(
            sql, Tools::Uuid::generate(), new_todo.user_id, new_todo.title, new_todo.description,
            new_todo.status);
        return "New todo created";
    }

    /// @brief This will update a todo
    /// @param new_todo The todo object
    /// @return std::string Returns the message
    static std::string update_todo(const Model::Todo &new_todo) {
        std::ifstream file(SQL_UPDATE_TODO);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Sqlite::Database::execute_query(
            sql, new_todo.title, new_todo.description, new_todo.status, new_todo.todo_id);
        return "Todo updated";
    }

    /// @brief This will delete a todo
    /// @param id The id of the todo
    /// @param user_id The user_id of the user
    /// @return std::string Returns the message
    static std::string delete_todo(const std::string &id, const std::string &user_id) {
        std::ifstream file(SQL_DELETE_TODO);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Sqlite::Database::execute_query(sql, id);
        return "Todo deleted";
    }
};
} // namespace Controller

#endif // TODO_H
