#ifndef MODELS_H
#define MODELS_H

#include <string>
#include "json.hpp"
using json = nlohmann::json;

namespace Models
{
    struct Todo
    {
        std::string id;
        std::string title;
        std::string description;
        std::string status;

        //TODO: FIX THIS
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Todo, id, title, description, status);
    };

    struct User
    {
        std::string id;
        std::string name;
        std::string email;
        std::string password;

        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(User, id, name, email, password);
    };

} // namespace Models

#endif // MODELS_H