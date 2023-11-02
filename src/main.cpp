#include <iostream>
#include "crow.h"

int main(int, char**){
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([](){
        return "Hello world";
    });

    app.port(18080).multithreaded().run();
}