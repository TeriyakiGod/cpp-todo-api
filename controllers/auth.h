#ifndef AUTH_H
#define AUTH_H

#include <httplib.h>
#include "../models/user.h"
#include "../controllers/user.h"
#include "../src/tools.h"
#include <spdlog/spdlog.h>
#include <json.hpp>
using json = nlohmann::json;

namespace Controller {
    class Auth
    {
    public:
        Auth(httplib::Server& svr) {
            svr.Post("/auth/signup", sign_up_handler());
            svr.Post("/auth/signin", sign_in_handler());
        }

    private:
        static Server::Handler sign_up_handler() {
            return [&](const httplib::Request& req, httplib::Response& res) {
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
            return [&](const httplib::Request& req, httplib::Response& res) {
                std::string token = sign_in(req.body);
                if (token.empty()) {
                    res.status = 401;
                    res.set_content("Invalid credentials", "text/plain");
                    return;
                }
                res.set_content(token, "text/plain");
                };
        }

        static std::string sign_up(const std::string& body) {
            json j = json::parse(body);
            auto new_user = j.template get<Model::User>();
            if (!new_user.check_fields()) {
                return "";
            }
            return Controller::User::create_user(new_user);
        }

        static std::string sign_in(const std::string& body) {
            json j = json::parse(body);
            auto email = j.at("email").get<std::string>();
            auto password = j.at("password").get<std::string>();
            auto user = Controller::User::get_user_by_email(email);
            if (user.user_id.empty()) {
                return "";
            }
            if (user.validate_password(password)) {
                return Tools::Jwt::generate_token(user.user_id);
            }
            else {
                return "";
            }
        }
    };
} // namespace API

#endif // AUTH_H