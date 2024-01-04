#include "controllers/auth.hpp"
#include "controllers/todo.hpp"
#include "controllers/user.hpp"
#include "db.hpp"
#include <httplib.h>
#include <iostream>
#include <spdlog/spdlog.h>

int main(int, char **) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting server...");
    httplib::Server svr;

    svr.set_logger([](const auto &req, const auto &res) {
        spdlog::info("{} {} {}", req.method, req.path, res.status);
    });

    Controller::Todo todoController(svr);
    Controller::User userController(svr);
    Controller::Auth authController(svr);

    spdlog::info("Server running on port {}", 8080);
    svr.listen("0.0.0.0", 8080);
}