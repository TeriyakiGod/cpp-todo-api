#ifndef AUTH_H
#define AUTH_H

#include "../models/user.hpp"
#include "../src/tools.hpp"
#include "user.hpp"
#include <httplib.h>
#include <json.hpp>
#include <spdlog/spdlog.h>
using json = nlohmann::json;

#define USER_API_ACCESS_ROLE Model::Role::ADMIN

namespace Controller {
class Auth {
public:
    /// @brief This will register all the routes related to authentication
    /// @param svr The server object
    Auth(httplib::Server &svr) {
        svr.set_pre_routing_handler(authentication_handler());
        svr.Post("/auth/signup", sign_up_handler());
        svr.Post("/auth/signin", sign_in_handler());
    }

    /// @brief This will check if the user is authenticated and authorized to
    /// access the route
    /// @return Server::HandlerWithResponse Returns unhandled if authentication
    /// is successfull else returns handled
    static Server::HandlerWithResponse authentication_handler() {
        return ([](const Request &req, Response &res) {
            if (req.path == "/auth/signup" || req.path == "/auth/signin") {
                return Server::HandlerResponse::Unhandled;
            }
            std::string user_id = Auth::authenticate(req, res);
            if (user_id.empty()) {
                res.status = 403;
                res.set_content("Access denied", "text/plain");
                return Server::HandlerResponse::Handled;
            }
            res.set_header("user_id", user_id.c_str());
            if (req.path == "/user") {
                if (Auth::authorize(user_id) != USER_API_ACCESS_ROLE) {
                    res.status = 403;
                    res.set_content("Access denied", "text/plain");
                    return Server::HandlerResponse::Handled;
                }
            }
            return Server::HandlerResponse::Unhandled;
        });
    }

    /// @brief This will authenticate the user
    /// @param req The request object
    /// @param res The response object
    /// @return std::string Returns the user_id if authentication is successfull
    static std::string authenticate(const Request &req, Response &res) {
        std::string token = req.get_header_value("Authorization");
        if (token.empty()) {
            res.status = 401;
            res.set_content("Invalid token", "text/plain");
            return "";
        }
        std::string user_id = Tools::Jwt::verify_user(token);
        if (user_id.empty()) {
            res.status = 401;
            res.set_content("Access denied", "text/plain");
            return "";
        }
        return user_id;
    }

    /// @brief This authorizes the user
    /// @param user_id The user_id of the user
    /// @return Model::Role Returns the role of the user
    static Model::Role authorize(std::string user_id) {
        Model::User user = User::get_user_by_id(user_id);
        if (user.user_id.empty()) {
            return Model::Role::NONE;
        }
        return user.role;
    }

private:
    /// @brief This handles the sign up request
    /// @return Server::Handler Returns the handler for the sign up request
    static Server::Handler sign_up_handler() {
        return [&](const httplib::Request &req, httplib::Response &res) {
            auto result = sign_up(req.body);
            if (result == "Invalid fields") {
                res.status = 400;
            }
            res.set_content(result, "text/plain");
        };
    }

    /// @brief This handles the sign in request
    /// @return Server::Handler Returns the handler for the sign in request
    static Server::Handler sign_in_handler() {
        return [&](const httplib::Request &req, httplib::Response &res) {
            std::string token = sign_in(req.body);
            if (token == "No user found" || token == "Wrong password") {
                res.status = 401;
                res.set_content("Invalid credentials", "text/plain");
                return;
            }
            res.set_header("Authorization", token.c_str());
            res.set_content("Sign In successfull", "text/plain");
        };
    }

    /// @brief This creates a new user
    /// @param body The body of the request containing the user details
    /// @return std::string Returns empty string if the user is not created else
    static std::string sign_up(const std::string &body) {
        json j = json::parse(body);
        auto new_user = j.template get<Model::User>();
        if (!new_user.check_fields()) {
            return "Invalid fields";
        }
        return User::create_user(new_user);
    }

    /// @brief This signs in the user
    /// @param body The body of the request containing the user details
    /// @return std::string Returns the token if the user is signed in else
    /// returns no user found or wrong password
    static std::string sign_in(const std::string &body) {
        json j = json::parse(body);
        auto email = j.at("email").get<std::string>();
        auto password = j.at("password").get<std::string>();
        auto user = User::get_user_by_email(email);
        if (user.user_id.empty()) {
            return "No user found";
        }
        if (user.validate_password(password)) {
            return Tools::Jwt::generate_token(user.user_id);
        } else {
            return "Wrong password";
        }
    }
};
} // namespace Controller

#endif // AUTH_H