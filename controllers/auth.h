#ifndef AUTH_H
#define AUTH_H

#include "../models/user.h"
#include "../src/tools.h"
#include "user.h"
#include <httplib.h>
#include <json.hpp>
#include <spdlog/spdlog.h>
using json = nlohmann::json;

#define USER_API_ACCESS_ROLE Model::Role::ADMIN

namespace Controller {
class Auth {
public:
    Auth(httplib::Server &svr) {
        svr.set_pre_routing_handler(authentication_handler());
        svr.Post("/auth/signup", sign_up_handler());
        svr.Post("/auth/signin", sign_in_handler());
    }

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

    static Model::Role authorize(std::string user_id) {
        Model::User user = User::get_user_by_id(user_id);
        if (user.user_id.empty()) {
            return Model::Role::NONE;
        }
        return user.role;
    }

private:
    static Server::Handler sign_up_handler() {
        return [&](const httplib::Request &req, httplib::Response &res) {
            auto result = sign_up(req.body);
            if (result.empty()) {
                res.status = 400;
                res.set_content("Wrong email, name or password.", "text/plain");
                return;
            }
            res.set_content(result, "text/plain");
        };
    }

    static Server::Handler sign_in_handler() {
        return [&](const httplib::Request &req, httplib::Response &res) {
            std::string token = sign_in(req.body);
            if (token.empty()) {
                res.status = 401;
                res.set_content("Invalid credentials", "text/plain");
                return;
            }
            res.set_header("Authorization", token.c_str());
            res.set_content("Sign In successfull", "text/plain");
        };
    }

    static std::string sign_up(const std::string &body) {
        json j = json::parse(body);
        auto new_user = j.template get<Model::User>();
        if (!new_user.check_fields()) {
            return "";
        }
        return User::create_user(new_user);
    }

    static std::string sign_in(const std::string &body) {
        json j = json::parse(body);
        auto email = j.at("email").get<std::string>();
        auto password = j.at("password").get<std::string>();
        auto user = User::get_user_by_email(email);
        if (user.user_id.empty()) {
            return "";
        }
        if (user.validate_password(password)) {
            return Tools::Jwt::generate_token(user.user_id);
        } else {
            return "";
        }
    }
};
} // namespace Controller

#endif // AUTH_H