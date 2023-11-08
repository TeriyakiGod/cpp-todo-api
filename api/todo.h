#ifndef TODO_H
#define TODO_H

#include <fstream>
#include <string>
#include <crow.h>
#include "../db.h"
#include "../models/todo.h"

#define SQL_INIT_TODO "sql/initTodo.sql"
#define SQL_CREATE_TODO "sql/createTodo.sql"
#define SQL_UPDATE_TODO "sql/updateTodo.sql"
#define SQL_DELETE_TODO "sql/deleteTodo.sql"
#define SQL_GET_TODO "sql/getTodo.sql"
#define SQL_GET_TODOS "sql/getTodos.sql"

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
