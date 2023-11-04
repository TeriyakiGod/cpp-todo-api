#include <crow.h>
#include <crow/http_request.h>

#include <iostream>

#include "db.h"
#include "models/todo.h"

int main(int, char**)
{
    CROW_LOG_INFO << "Server is running on port 18080";
    crow::SimpleApp app;

    app.loglevel(crow::LogLevel::WARNING);

    CROW_ROUTE(app, "/")
        ([]()
            { return "Hello world!"; });

    // Get all todos
    CROW_ROUTE(app, "/todos")
        .methods("GET"_method)([](const crow::request& req, crow::response& res)
            {
                crow::json::wvalue::list result;
                std::string sql = "SELECT * FROM todos";
                SQLite::Database::executeQuery(sql, SQLite::Callback::getJsonList, result);
                for (size_t i = 0; i < result.size(); i++) {
                    res.write(result[i].dump());
                }
                res.end();
            });

    // Get todo by id
    CROW_ROUTE(app, "/todos/<string>")
        .methods("GET"_method)([](const crow::request& req, crow::response& res, std::string id)
            {
                crow::json::wvalue result;
                std::string sql = "SELECT * FROM todos WHERE id = '" + id + "'";
                SQLite::Database::executeQuery(sql, SQLite::Callback::getJson, result);
                res.write(result.dump());
                res.end();
            });

    // Create new todo
    CROW_ROUTE(app, "/todos")
        .methods("POST"_method)([](const crow::request& req, crow::response& res)
            {
                crow::json::rvalue body = crow::json::load(req.body);

                todo newTodo;

                newTodo.id = body["id"].s();
                newTodo.title = body["title"].s();
                newTodo.description = body["description"].s();
                newTodo.status = body["status"].s();

                crow::json::wvalue result;
                //TODO: Move the query to a file
                std::string sql =
                    "CREATE TABLE IF NOT EXISTS todos ( \
            id TEXT PRIMARY KEY, \
            title TEXT, \
            description TEXT, \
            status TEXT \
            ); \
            INSERT INTO todos (id, title, description, status) \
            VALUES ('" +
                    newTodo.id + "', '" +
                    newTodo.title + "', '" +
                    newTodo.description + "', '" +
                    newTodo.status + "')";
                SQLite::Database::executeQuery(sql, nullptr, result);
                res.write(result.dump());
                res.end();
            });

    // Update todo by id
    CROW_ROUTE(app, "/todos/<string>")
        .methods("PUT"_method)([](const crow::request& req, crow::response& res, std::string id)
            {
                crow::json::rvalue body = crow::json::load(req.body);

                todo newTodo;

                newTodo.id = body["id"].s();
                newTodo.title = body["title"].s();
                newTodo.description = body["description"].s();
                newTodo.status = body["status"].s();

                crow::json::wvalue result;
                //TODO: Move the query to a file
                std::string sql =
                    "UPDATE todos SET \
            title = '" + newTodo.title + "', \
            description = '" + newTodo.description + "', \
            status = '" + newTodo.status + "' \
            WHERE id = '" + newTodo.id + "'";
                SQLite::Database::executeQuery(sql, nullptr, result);
                res.write(result.dump());
                res.end();
            });

    // Delete todo by id
    CROW_ROUTE(app, "/todos/<string>")
        .methods("DELETE"_method)([](const crow::request& req, crow::response& res, std::string id)
            {
                crow::json::wvalue result;
                std::string sql = "DELETE FROM todos WHERE id = '" + id + "'";
                SQLite::Database::executeQuery(sql, nullptr, result);
                res.write(result.dump());
                res.end();
            });

    app.port(18080).multithreaded().run();
}