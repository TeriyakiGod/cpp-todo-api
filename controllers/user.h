#ifndef USER_H
#define USER_H

#include <fstream>
#include <string>
#include <httplib.h>
using namespace httplib;
#include <json.hpp>
using json = nlohmann::json;
#include <spdlog/spdlog.h>
#include "../db.h"
#include "../models/user.h"

//TODO: add user authentication

#define SQL_CREATE_USER_TABLE "../sql/user/createUserTable.sql"
#define SQL_CREATE_USER "../sql/user/createUser.sql"
#define SQL_UPDATE_USER "../sql/user/updateUser.sql"
#define SQL_DELETE_USER "../sql/user/deleteUser.sql"
#define SQL_GET_USER "../sql/user/getUser.sql"
#define SQL_GET_USERS "../sql/user/getUsers.sql"
namespace Controller {
    class User {
    public:
        User(httplib::Server& svr) {

            std::ifstream file(SQL_CREATE_USER_TABLE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql);

            svr.Get("/user", [&](const Request& req, Response& res) {
                std::ifstream file(SQL_GET_USERS);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                json result = SQLite::Database::executeQuery(sql);
                res.set_content(result.dump(), "application/json");
                });

            svr.Get("/user/:string", [&](const Request& req, Response& res) {
                std::ifstream file(SQL_GET_USER);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                json result = SQLite::Database::executeQuery(sql, req.path_params.at("string"));
                res.set_content(result.dump(), "application/json");
                });

            svr.Post("/user", [&](const Request& req, Response& res) {
                json j = json::parse(req.body);

                auto newUser = j.template get<Model::User>();

                std::ifstream file(SQL_CREATE_USER);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                SQLite::Database::executeQuery(sql, newUser.id, newUser.name, newUser.email, newUser.password);
                res.set_content("new user created", "text/plain");
                });

            svr.Put("/user", [&](const Request& req, Response& res) {
                json j = json::parse(req.body);

                auto newUser = j.template get<Model::User>();

                std::ifstream file(SQL_UPDATE_USER);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                SQLite::Database::executeQuery(sql, newUser.name, newUser.email, newUser.password, newUser.id);
                res.set_content("user updated", "text/plain");
                });

            svr.Delete("/user/:string", [&](const Request& req, Response& res) {
                std::ifstream file(SQL_DELETE_USER);
                std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                SQLite::Database::executeQuery(sql, req.path_params.at("string"));
                res.set_content("user deleted", "text/plain");
                });
        }
    };

} // namespace API

#endif // USER_H
