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
        string id;
        string name;
        string email;
        string password;
    };

    void from_json(const json& j, User& u)
    {
        u.id = Tools::UUID::generate();
        j.at("Name").get_to(u.name);
        j.at("Email").get_to(u.email);
        j.at("Password").get_to(u.password);
    }

    void to_json(json& j, const User& u)
    {
        j = json{
            {"UserID", u.id},
            {"Name", u.name},
            {"Email", u.email},
            {"Password", u.password}
        };
    }
} // namespace Models

#endif // MODELS_H