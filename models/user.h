#ifndef MODEL_USER_H
#define MODEL_USER_H

#include <string>
#include "../json.hpp"
using json = nlohmann::json;

namespace Model
{
    struct User
    {
        std::string id;
        std::string name;
        std::string email;
        std::string password;
    };

    void from_json(const json& j, User& u)
    {
        j.at("id").get_to(u.id);
        j.at("name").get_to(u.name);
        j.at("email").get_to(u.email);
        j.at("password").get_to(u.password);
    }

    void to_json(json& j, const User& u)
    {
        j = json{
            {"id", u.id},
            {"name", u.name},
            {"email", u.email},
            {"password", u.password}
        };
    }
} // namespace Models

#endif // MODELS_H