#include <iostream>
#include <crow.h>
#include <crow/http_request.h>
#include "db.h"
#include "models/todo.h"

int main(int, char **)
{
    crow::SimpleApp app;

    app.loglevel(crow::LogLevel::WARNING);

    CROW_ROUTE(app, "/")
    ([](){
        return "Hello world!";
    });

    //Get all todos
    CROW_ROUTE(app, "/todos")
    .methods("GET"_method)
    ([](const crow::request &req){
        crow::json::wvalue result;
        std::string sql = "SELECT * FROM todos";
        SQLite::Database::executeQuery(sql, SQLite::Callback::getJson, result);
        return crow::response(result);
    });

    //Get todo by id
    CROW_ROUTE(app, "/todos/<int>")
    .methods("GET"_method)
    ([](const crow::request &req, int id){
        crow::json::wvalue result;
        std::string sql = "SELECT * FROM todos WHERE id = " + std::to_string(id);
        SQLite::Database::executeQuery(sql, SQLite::Callback::getJson, result);
        return crow::response(result);
    });

    //Create new todo
    CROW_ROUTE(app, "/todos")
    .methods("POST"_method)
    ([](const crow::request &req){
        crow::json::rvalue body = crow::json::load(req.body);

        todo newTodo;

        newTodo.id = body["id"].s();
        newTodo.title = body["title"].s();
        newTodo.description = body["description"].s();
        newTodo.status = body["status"].s();

        crow::json::wvalue result;
        std::string sql = 
            "CREATE TABLE IF NOT EXISTS todos ( \
            id TEXT PRIMARY KEY, \
            title TEXT, \
            description TEXT, \
            status TEXT \
            ); \
            INSERT INTO todos (id, title, description, status) \
            VALUES ('" + 
            newTodo.id +  "', '" + 
            newTodo.title + "', '" + 
            newTodo.description + "', '" + 
            newTodo.status + "')";
        SQLite::Database::executeQuery(sql, nullptr, result);
        return crow::response(result);
    });

    app.port(18080).multithreaded().run();
}