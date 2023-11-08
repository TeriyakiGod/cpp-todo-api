#include "todo.h"

void TodoAPI::init(crow::SimpleApp& app) {

    std::ifstream file(SQL_INIT_TODO);
    std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    SQLite::Database::executeQuery(sql);

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
                std::ifstream file(SQL_GET_TODOS);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                crow::json::wvalue result = SQLite::Database::executeQuery(sql);
                res.end(result.dump());
            });
}

void TodoAPI::getById(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/todos/<string>")
        .methods("GET"_method)([](crow::response& res, std::string id)
            {
                std::ifstream file(SQL_GET_TODO);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                crow::json::wvalue result = SQLite::Database::executeQuery(sql, id);
                res.end(result.dump());
            });
}

void TodoAPI::create(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/todos")
        .methods("POST"_method)([](const crow::request& req, crow::response& res)
            {
                crow::json::rvalue body = crow::json::load(req.body);

                if (!body.has("id") || !(body["id"].t() == crow::json::type::String) ||
                    !body.has("title") || !(body["title"].t() == crow::json::type::String) ||
                    !body.has("description") || !(body["description"].t() == crow::json::type::String) ||
                    (!body.has("status") || !(body["status"].t() == crow::json::type::String))) {
                    res.code = 400;
                    res.write("Invalid JSON object.");
                    res.end();
                    return;
                }

                todo newTodo{
                    body["id"].s(),
                    body["title"].s(),
                    body["description"].s(),
                    body["status"].s()
                };

                std::ifstream file(SQL_CREATE_TODO);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                SQLite::Database::executeQuery(sql, newTodo.id, newTodo.title, newTodo.description, newTodo.status);
                res.end();
            });
}

//TODO: Fix this endpoint
void TodoAPI::update(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/todos")
        .methods("PUT"_method)([](const crow::request& req, crow::response& res)
            {
                crow::json::rvalue body = crow::json::load(req.body);

                todo newTodo{
                    body["id"].s(),
                    body["title"].s(),
                    body["description"].s(),
                    body["status"].s()
                };

                std::ifstream file(SQL_UPDATE_TODO);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                SQLite::Database::executeQuery(sql, newTodo.title, newTodo.description, newTodo.status, newTodo.id);
                res.end();
            });
}

void TodoAPI::remove(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/todos/<string>")
        .methods("DELETE"_method)([](crow::response& res, std::string id)
            {
                std::ifstream file(SQL_DELETE_TODO);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                SQLite::Database::executeQuery(sql, id);
                res.end();
            });
}