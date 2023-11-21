#ifndef MODEL_TODO_H
#define MODEL_TODO_H

#include <string>
#include "../json.hpp"
using json = nlohmann::json;

namespace Model
{
    struct Todo
    {
        std::string id;
        std::string title;
        std::string description;
        std::string status;
    };

    void from_json(const json& j, Todo& t)
    {
        j.at("id").get_to(t.id);
        j.at("title").get_to(t.title);
        j.at("description").get_to(t.description);
        j.at("status").get_to(t.status);
    }

    void to_json(json& j, const Todo& t)
    {
        j = json{
            {"id", t.id},
            {"title", t.title},
            {"description", t.description},
            {"status", t.status}
        };
    }
}

#endif // MODEL_TODO_H