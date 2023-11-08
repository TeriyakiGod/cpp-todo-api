#include <crow.h>
#include <crow/http_request.h>

#include <iostream>

#include "db.h"
#include "api/todo.h"

int main(int, char**)
{
    CROW_LOG_INFO << "Server is running on port 18080";
    crow::SimpleApp app;

    app.loglevel(crow::LogLevel::WARNING);

    CROW_ROUTE(app, "/")([]() { return "Hello world!"; });

    TodoAPI::init(app);

    app.port(18080).multithreaded().run();
}