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

#define SQL_CREATE_USER_TABLE "../sql/user/create_user_table.sql"
#define SQL_CHECK_IF_USER_EXISTS "../sql/user/check_if_user_exists.sql"
#define SQL_CREATE_USER "../sql/user/create_user.sql"
#define SQL_UPDATE_USER "../sql/user/update_user.sql"
#define SQL_DELETE_USER "../sql/user/delete_user.sql"
#define SQL_GET_USERS "../sql/user/get_users.sql"
#define SQL_GET_USER "../sql/user/get_user.sql"

namespace Controller {
    class User {
    public:
        User(httplib::Server& svr) {
            prepare_user_table();

            svr.Get("/user", [&](const Request& req, Response& res) {
                res.set_content(get_users(), "application/json");
                });

            svr.Get("/user/:string", [&](const Request& req, Response& res) {
                res.set_content(get_user(req.path_params.at("string")), "application/json");
                });

            svr.Post("/user", [&](const Request& req, Response& res) {
                json j = json::parse(req.body);
                auto new_user = j.template get<Model::User>();
                res.set_content(create_user(new_user), "text/plain");
                });

            svr.Put("/user", [&](const Request& req, Response& res) {
                json j = json::parse(req.body);
                auto new_user = j.template get<Model::User>();
                res.set_content(update_user(new_user), "text/plain");
                });

            svr.Delete("/user/:string", [&](const Request& req, Response& res) {
                res.set_content(delete_user(req.path_params.at("string")), "text/plain");
                });
        }

        static void prepare_user_table() {
            std::ifstream file(SQL_CREATE_USER_TABLE);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            sqlite::database::execute_query(sql);
        }

        static std::string get_users() {
            std::ifstream file(SQL_GET_USERS);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = sqlite::database::execute_query(sql);
            return result.dump();
        }

        static std::string get_user(const std::string& param) {
            std::ifstream file(SQL_GET_USER);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            json result = sqlite::database::execute_query(sql, param);
            return result.dump();
        }

        static std::string create_user(const Model::User& new_user) {
            std::ifstream file1(SQL_CHECK_IF_USER_EXISTS);
            std::string sql1((std::istreambuf_iterator<char>(file1)), std::istreambuf_iterator<char>());

            json result = sqlite::database::execute_query(sql1, new_user.email);
            bool does_email_exist = result["user_exists"].template get<int>();
            spdlog::debug("does_email_exist: {}", does_email_exist);
            if (!does_email_exist) {
                std::ifstream file2(SQL_CREATE_USER);
                std::string sql2((std::istreambuf_iterator<char>(file2)), std::istreambuf_iterator<char>());

                sqlite::database::execute_query(sql2, new_user.user_id, new_user.name, new_user.email, new_user.password);
                return "New user created";
            }
            else {
                return "User with this email already exists, please use another email";
            }
        }

        static std::string update_user(const Model::User& new_user) {
            std::ifstream file(SQL_UPDATE_USER);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            sqlite::database::execute_query(sql, new_user.name, new_user.email, new_user.password, new_user.user_id);
            return "User updated";
        }

        static std::string delete_user(const std::string& param) {
            std::ifstream file(SQL_DELETE_USER);
            std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            sqlite::database::execute_query(sql, param);
            return "User deleted";
        }
    };

} // namespace API

#endif // USER_H
