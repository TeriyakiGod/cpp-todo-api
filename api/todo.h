#ifndef TODO_H
#define TODO_H

#include <string>
#include <crow.h>
#include "../db.h"
#include "../models/todo.h"

class TodoAPI {
public:
    static void init(crow::SimpleApp& app);
    static void getAll(crow::SimpleApp& app);
    static void getById(crow::SimpleApp& app);
    static void create(crow::SimpleApp& app);
    static void update(crow::SimpleApp& app);
    static void remove(crow::SimpleApp& app);
};

#endif // TODO_H
