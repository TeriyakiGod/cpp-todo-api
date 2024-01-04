#ifndef MODEL_TODO_H
#define MODEL_TODO_H

#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;
using namespace std;

namespace Model {
struct Todo {
    string todo_id;
    string user_id;
    string title;
    string description;
    int status;
};

void from_json(const json &j, Todo &t) {
    auto it = j.find("todo_id");
    if (it != j.end())
        it->get_to(t.todo_id);

    it = j.find("user_id");
    if (it != j.end())
        it->get_to(t.user_id);

    it = j.find("title");
    if (it != j.end())
        it->get_to(t.title);

    it = j.find("description");
    if (it != j.end())
        it->get_to(t.description);

    it = j.find("status");
    if (it != j.end())
        it->get_to(t.status);
}

void to_json(json &j, const Todo &t) {
    j = json{
        {"todo_id", t.todo_id},
        {"user_id", t.user_id},
        {"title", t.title},
        {"description", t.description},
        {"status", t.status}};
}

} // namespace Model

#endif // MODEL_TODO_H