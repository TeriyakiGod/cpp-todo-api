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
#define SQL_CHECK_IF_USER_EXISTS "../sql/user/checkIfUserExists.sql"
#define SQL_CREATE_USER "../sql/user/createUser.sql"
#define SQL_UPDATE_USER "../sql/user/updateUser.sql"
#define SQL_DELETE_USER "../sql/user/deleteUser.sql"
#define SQL_GET_USER "../sql/user/getUser.sql"
#define SQL_GET_USERS "../sql/user/getUsers.sql"
namespace Controller {
    class User {
    public:
        User(httplib::Server& svr) {
            prepareUserTable();

            svr.Get("/user", [&](const Request& req, Response& res) {
                res.set_content(getUsers(), "application/json");
            });

            svr.Get("/user/:string", [&](const Request& req, Response& res) {
                res.set_content(getUser(req.path_params.at("string")), "application/json");
            });

            svr.Post("/user", [&](const Request& req, Response& res) {
                res.set_content(createUser(req.body), "text/plain");
            });

            svr.Put("/user", [&](const Request& req, Response& res) {
                res.set_content(updateUser(req.body), "text/plain");
            });

            svr.Delete("/user/:string", [&](const Request& req, Response& res) {
                res.set_content(deleteUser(req.path_params.at("string")), "text/plain");
            });
        }

    private:
        void prepareUserTable(){
            std::ifstream file(SQL_CREATE_USER_TABLE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql);
        }

        std::string getUsers() {
            std::ifstream file(SQL_GET_USERS);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = SQLite::Database::executeQuery(sql);
            return result.dump();
        }

        std::string getUser(const std::string& param) {
            std::ifstream file(SQL_GET_USER);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = SQLite::Database::executeQuery(sql, param);
            return result.dump();
        }

        std::string createUser(const std::string& body) {
            json j = json::parse(body);
            auto newUser = j.template get<Model::User>();

            std::ifstream file1(SQL_CHECK_IF_USER_EXISTS);
            std::string sql1((std::istreambuf_iterator<char>(file1)), std::istreambuf_iterator<char>());

            json result = SQLite::Database::executeQuery(sql1, newUser.email);
            bool doesEmailExist = result["user_exists"].template get<int>();
            spdlog::debug("doesEmailExist: {}", doesEmailExist);
            if (!doesEmailExist) {
                std::ifstream file2(SQL_CREATE_USER);
                std::string sql2((std::istreambuf_iterator<char>(file2)), std::istreambuf_iterator<char>());

                SQLite::Database::executeQuery(sql2, newUser.user_id, newUser.name, newUser.email, newUser.password);
                return "New user created";
            } else {
                return "User already exists";
            }
        }

        std::string updateUser(const std::string& body) {
            json j = json::parse(body);
            auto newUser = j.template get<Model::User>();

            std::ifstream file(SQL_UPDATE_USER);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, newUser.name, newUser.email, newUser.password, newUser.user_id);
            return "User updated";
        }

        std::string deleteUser(const std::string& param) {
            std::ifstream file(SQL_DELETE_USER);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            SQLite::Database::executeQuery(sql, param);
            return "User deleted";
        }
    };

} // namespace API

#endif // USER_H
