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
        string id;
        string user_id;
        string title;
        string description;
        int status;
    };

    void from_json(const json& j, Todo& t)
    {
        t.id = Tools::UUID::generate();
        j.at("UserID").get_to(t.user_id);
        j.at("Title").get_to(t.title);
        j.at("Description").get_to(t.description);
        j.at("Status").get_to(t.status);
    }

    void to_json(json& j, const Todo& t)
    {
        j = json{
            {"TodoID", t.id},
            {"UserID", t.user_id},
            {"Title", t.title},
            {"Description", t.description},
            {"Status", t.status}
        };
    }

}

#endif // MODEL_TODO_H