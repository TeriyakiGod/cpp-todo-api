#ifndef AUTH_H
#define AUTH_H

#include <jwt-cpp/jwt.h>
#include <httplib.h>
#include "../models/user.h"
#include "../controllers/user.h"
#include "../src/tools.h"
#include <spdlog/spdlog.h>
#include <json.hpp>
using json = nlohmann::json;

#define SECRET_KEY_FILE "../secret.key"
#define JWT_TOKEN_EXPIRATION 10

namespace Controller {
    class Auth
    {
    public:
        Auth(httplib::Server& svr) {
            svr.Post("/auth/signup", [&](const httplib::Request& req, httplib::Response& res) {
                json j = json::parse(req.body);
                auto new_user = j.template get<Model::User>();
                res.set_header("Access-Control-Allow-Origin", Tools::Resource::load_string("config", "server_url").c_str());
                res.set_content(sign_up(new_user), "text/plain");
                });

            svr.Post("/auth/signin", [&](const httplib::Request& req, httplib::Response& res) {
                json j = json::parse(req.body);
                //TODO: Add credential model
                auto email = j.at("email").get<std::string>();
                auto password = j.at("password").get<std::string>();
                res.set_header("Access-Control-Allow-Origin", Tools::Resource::load_string("config", "server_url").c_str());
                res.set_content(sign_in(email, password), "text/plain");
                });

        }
    private:
        static std::string sign_up(Model::User user) {
            if (!user.check_fields()) {
                return "Invalid name, email or password, please correct and try again.";
            }
            return Controller::User::create_user(user);
        }
        static std::string sign_in(std::string email, std::string password) {
            auto user = Controller::User::get_user_by_email(email);
            if (user.user_id.empty()) {
                return "Wrong email, please try again.";
            }
            if (user.validate_password(password)) {
                return generate_token(user);
            }
            else {
                return "Invalid password, please try again.";
            }
        }

        static std::string generate_token(Model::User user) {
            std::ifstream file(SECRET_KEY_FILE);
            std::string secret_key((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            std::string token = jwt::create()
                .set_issuer("auth0")
                .set_type("JWS")
                .set_payload_claim("user_id", jwt::claim(user.user_id))
                .set_payload_claim("name", jwt::claim(user.name))
                .set_payload_claim("email", jwt::claim(user.email))
                .set_payload_claim("password", jwt::claim(user.password))
                //.set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{ 10 })
                .sign(jwt::algorithm::hs256{ secret_key });
            return token;
        }
    };
} // namespace API

#endif // AUTH_H