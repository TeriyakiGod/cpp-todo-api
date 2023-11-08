#include "todo.h"

void TodoAPI::init(crow::SimpleApp& app) {
    getAll(app);
    getById(app);
    create(app);
    update(app);
    remove(app);
}

void TodoAPI::getAll(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/todos")
        .methods("GET"_method)([](crow::response& res)
            {
                crow::json::wvalue result;
                std::string sql = "SELECT * FROM todos";
                SQLite::Database::executeQuery(sql, SQLite::Callback::getJson, result);
                res.write(result.dump());
                res.end();
            });
}

void TodoAPI::getById(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/todos/<string>")
        .methods("GET"_method)([](crow::response& res, std::string id)
            {
                crow::json::wvalue result;
                std::string sql = "SELECT * FROM todos WHERE id = '" + id + "'";
                SQLite::Database::executeQuery(sql, SQLite::Callback::getJson, result);
                res.write(result.dump());
                res.end();
            });
}

void TodoAPI::create(crow::SimpleApp& app) {
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
}

void TodoAPI::update(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/todos")
        .methods("PUT"_method)([](const crow::request& req, crow::response& res)
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
}

void TodoAPI::remove(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/todos/<string>")
        .methods("DELETE"_method)([](crow::response& res, std::string id)
            {
                crow::json::wvalue result;
                std::string sql = "DELETE FROM todos WHERE id = '" + id + "'";
                SQLite::Database::executeQuery(sql, nullptr, result);
                res.write(result.dump());
                res.end();
            });
}