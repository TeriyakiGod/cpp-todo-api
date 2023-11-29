#ifndef MODEL_USER_H
#define MODEL_USER_H

#include <string>
#include "../tools.h"
#include <json.hpp>
using json = nlohmann::json;
using namespace std;

namespace Model
{
    struct User
    {
        string user_id;
        string name;
        string email;
        string password;
    };

    void from_json(const json& j, User& u)
    {
        u.user_id = Tools::UUID::generate();
        j.at("name").get_to(u.name);
        j.at("email").get_to(u.email);
        j.at("password").get_to(u.password);
    }

    void to_json(json& j, const User& u)
    {
        j = json{
            {"userID", u.user_id},
            {"name", u.name},
            {"email", u.email},
            {"password", u.password}
        };
    }
} // namespace Models

#endif // MODELS_H