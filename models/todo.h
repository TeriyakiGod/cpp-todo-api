#ifndef MODEL_TODO_H
#define MODEL_TODO_H

#include <string>
#include "../tools.h"
#include <json.hpp>
using json = nlohmann::json;
using namespace std;

namespace Model
{
    struct Todo
    {
        string todo_id;
        string user_id;
        string title;
        string description;
        int status;
    };

    void from_json(const json& j, Todo& t)
    {
        t.todo_id = Tools::UUID::generate();
        j.at("user_id").get_to(t.user_id);
        j.at("title").get_to(t.title);
        j.at("description").get_to(t.description);
        j.at("status").get_to(t.status);
    }

    void to_json(json& j, const Todo& t)
    {
        j = json{
            {"todo_id", t.todo_id},
            {"user_id", t.user_id},
            {"title", t.title},
            {"description", t.description},
            {"status", t.status}
        };
    }

}

#endif // MODEL_TODO_H