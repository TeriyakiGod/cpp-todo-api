#ifndef USER_H
#define USER_H

#include "../db.hpp"
#include <fstream>
#include <httplib.h>
#include <string>
using namespace httplib;
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "../models/user.hpp"
#include "../tools.hpp"
#include <spdlog/spdlog.h>

#define SQL_CREATE_USER_TABLE "../res/sql/user/createUserTable.sql"
#define SQL_CHECK_IF_USER_EXISTS "../res/sql/user/checkIfUserExists.sql"
#define SQL_CREATE_USER "../res/sql/user/createUser.sql"
#define SQL_UPDATE_USER "../res/sql/user/updateUser.sql"
#define SQL_DELETE_USER "../res/sql/user/deleteUser.sql"
#define SQL_GET_USERS "../res/sql/user/getUsers.sql"
#define SQL_GET_USER "../res/sql/user/getUser.sql"
#define SQL_GET_USER_BY_EMAIL "../res/sql/user/getUserByEmail.sql"

namespace Controller {
class User {
public:
    /// @brief This will register all the routes related to user
    /// @param svr The server object
    User(httplib::Server &svr) {
        prepare_user_table();

        svr.Get("/user", get_users_handler());
        svr.Get("/user/:string", get_user_handler());
        svr.Post("/user", post_user_handler());
        svr.Put("/user", put_user_handler());
        svr.Delete("/user/:string", delete_user_handler());
        svr.Options(R"(/user/.*$)", [](const Request &req, Response &res) {});
    }

    /// @brief This will return the handler for getting all the users
    /// @return Server::Handler Returns the handler for getting all the users
    static void prepare_user_table() {
        std::ifstream file(SQL_CREATE_USER_TABLE);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Sqlite::Database::execute_query(sql);
    }

    /// @brief This will return the handler for getting all the users
    /// @return Server::Handler Returns the handler for getting all the users
    static Server::Handler get_users_handler() {
        return [](const Request &req, Response &res) {
            res.set_content(get_users(), "application/json");
        };
    }

    /// @brief This will return the handler for getting a user
    /// @return Server::Handler Returns the handler for getting a user
    static Server::Handler get_user_handler() {
        return [](const Request &req, Response &res) {
            res.set_content(get_user(req.path_params.at("string")), "application/json");
        };
    }

    /// @brief This will return the handler for creating a user
    /// @return Server::Handler Returns the handler for creating a user
    static Server::Handler post_user_handler() {
        return [](const Request &req, Response &res) {
            json j = json::parse(req.body);
            auto new_user = j.template get<Model::User>();
            res.set_content(create_user(new_user), "text/plain");
        };
    }

    /// @brief This will return the handler for updating a user
    /// @return Server::Handler Returns the handler for updating a user
    static Server::Handler put_user_handler() {
        return [](const Request &req, Response &res) {
            json j = json::parse(req.body);
            auto new_user = j.template get<Model::User>();
            res.set_content(update_user(new_user), "text/plain");
        };
    }

    /// @brief This will return the handler for deleting a user
    /// @return Server::Handler Returns the handler for deleting a user
    static Server::Handler delete_user_handler() {
        return [](const Request &req, Response &res) {
            res.set_content(delete_user(req.path_params.at("string")), "text/plain");
        };
    }

    /// @brief This will return all the users
    /// @return std::string Returns all the users
    static std::string get_users() {
        std::ifstream file(SQL_GET_USERS);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        json result = Sqlite::Database::execute_query(sql);
        return result.dump();
    }

    /// @brief This will get all users from the database
    /// @return std::string Returns all the users
    static std::string get_user(const std::string &user_id) {
        std::ifstream file(SQL_GET_USER);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        json result = Sqlite::Database::execute_query(sql, user_id);
        return result.dump();
    }

    /// @brief This will get a user from the database
    /// @param user_id The user_id of the user
    /// @return Model::User Returns the user
    static Model::User get_user_by_id(const std::string &user_id) {
        std::ifstream file(SQL_GET_USER);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        json result = Sqlite::Database::execute_query(sql, user_id);
        auto user = result.template get<Model::User>();
        return user;
    }

    /// @brief This will get a user from the database
    /// @param email The email of the user
    /// @return Model::User Returns the user
    static Model::User get_user_by_email(const std::string &email) {
        std::ifstream file(SQL_GET_USER_BY_EMAIL);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        json result = Sqlite::Database::execute_query(sql, email);
        auto user = result.template get<Model::User>();
        return user;
    }

    /// @brief This will create a new user
    /// @param new_user The new user to be created
    /// @return std::string Returns success or failure
    static std::string create_user(const Model::User &new_user) {
        std::ifstream file1(SQL_CHECK_IF_USER_EXISTS);
        std::string sql1((std::istreambuf_iterator<char>(file1)), std::istreambuf_iterator<char>());
        json result = Sqlite::Database::execute_query(sql1, new_user.email);

        bool does_email_exist = result["user_exists"].template get<int>();
        spdlog::debug("does_email_exist: {}", does_email_exist);
        if (!does_email_exist) {
            std::ifstream file2(SQL_CREATE_USER);
            std::string sql2(
                (std::istreambuf_iterator<char>(file2)), std::istreambuf_iterator<char>());
            Sqlite::Database::execute_query(
                sql2, Tools::Uuid::generate(), new_user.name, new_user.email,
                Tools::Hash::generate(new_user.password), (int)Model::Role::USER);
            return "New user created";
        } else {
            return "User with this email already exists, please use another email";
        }
    }

    /// @brief This will update a user
    /// @param new_user The new user to be updated
    /// @return std::string Returns "User updated"
    static std::string update_user(const Model::User &new_user) {
        std::ifstream file(SQL_UPDATE_USER);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Sqlite::Database::execute_query(
            sql, new_user.name, new_user.email, Tools::Hash::generate(new_user.password),
            new_user.user_id);
        return "User updated";
    }

    /// @brief This will delete a user
    /// @param param The user_id of the user
    /// @return std::string Returns "User deleted"
    /// @todo TODO: return something elso if user not found
    static std::string delete_user(const std::string &param) {
        std::ifstream file(SQL_DELETE_USER);
        std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        Sqlite::Database::execute_query(sql, param);
        return "User deleted";
    }
};

} // namespace Controller

#endif // USER_H
