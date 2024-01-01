#include "../controllers/auth.hpp"
#include "../controllers/todo.hpp"
#include "../controllers/user.hpp"
#include "db.hpp"
#include <httplib.h>
#include <iostream>
#include <spdlog/spdlog.h>

int main(int, char **) {
    spdlog::set_level(spdlog::level::info);
    spdlog::info("Starting server...");
    httplib::Server svr;

    svr.set_logger([](const auto &req, const auto &res) {
        spdlog::info("{} {} {}", req.method, req.path, res.status);
    });

    std::ifstream todo_html_file("../templates/todo.html");
    std::string todo_html_code(
        (std::istreambuf_iterator<char>(todo_html_file)), std::istreambuf_iterator<char>());
    svr.Get("/", [&](const httplib::Request &, httplib::Response &res) {
        res.set_content(todo_html_code, "text/html");
    });

    Controller::Todo todoController(svr);
    Controller::User userController(svr);
    Controller::Auth authController(svr);

    spdlog::info("Server running on port {}", 8080);
    svr.listen("0.0.0.0", 8080);
}