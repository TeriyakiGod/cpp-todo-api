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

//TODO: add user authentication
//TODO: add sql query files for user

#define SQL_INIT "sql/initUser.sql"
#define SQL_CREATE "sql/createUser.sql"
#define SQL_UPDATE "sql/updateUser.sql"
#define SQL_DELETE "sql/deleteUser.sql"
#define SQL_GET "sql/getUser.sql"
#define SQL_GETS "sql/getUsers.sql"

class UserAPI {
public:
    static void expose(httplib::Server& svr) {

        std::ifstream file(SQL_INIT);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        SQLite::Database::executeQuery(sql);

        svr.Get("/user", [&](const Request& req, Response& res) {
            std::ifstream file(SQL_GETS);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = SQLite::Database::executeQuery(sql);
            res.set_content(result.dump(), "application/json");
            });

        svr.Get("/user/:string", [&](const Request& req, Response& res) {
            std::ifstream file(SQL_GET);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = SQLite::Database::executeQuery(sql, req.path_params.at("string"));
            res.set_content(result.dump(), "application/json");
            });

        svr.Post("/user", [&](const Request& req, Response& res) {
            json body = json::parse(req.body);

            Model::User newUser = Model::fromJson<Model::User>(body);

            std::ifstream file(SQL_CREATE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            SQLite::Database::executeQuery(sql, newUser.id, newUser.name, newUser.email, newUser.password);
            res.set_content("new user created", "text/plain");
            });

        svr.Put("/user", [&](const Request& req, Response& res) {
            json body = json::parse(req.body);

            Model::User newUser = Model::fromJson<Model::User>(body);

            std::ifstream file(SQL_UPDATE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, newUser.name, newUser.email, newUser.password, newUser.id);
            res.set_content("user updated", "text/plain");
            });

        svr.Delete("/user/:string", [&](const Request& req, Response& res) {
            std::ifstream file(SQL_DELETE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, req.path_params.at("string"));
            res.set_content("user deleted", "text/plain");
            });
    }
};

#endif // TODO_H
