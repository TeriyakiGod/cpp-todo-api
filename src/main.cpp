#include "controllers/auth.hpp"
#include "controllers/todo.hpp"
#include "controllers/user.hpp"
#include "db.hpp"
#include "tools.hpp"
#include <httplib.h>
#include <iostream>
#include <spdlog/spdlog.h>
/**
 * @mainpage cpp-todo-api
 *
 * @section intro_sec Introduction
 *
 * This is a simple todo API written in C++ using httplib and SQLite3.
 *
 * @section API Reference
 * @subsection Auth
 *
 * @subsection User
 *
 * @subsection Todo
 */
int main(int, char **) {
    spdlog::set_level(spdlog::level::debug);

    Tools::Jwt::generate_secret_key(32);

    spdlog::info("Starting server...");
    httplib::Server svr;

    svr.set_logger([](const auto &req, const auto &res) {
        spdlog::info("{} {} {}", req.method, req.path, res.status);
    });

    auto ret = svr.set_mount_point("/", "../doc/html");
    if (!ret) {
        spdlog::error("Failed to set mount point");
    }

    Controller::Todo todoController(svr);
    Controller::User userController(svr);
    Controller::Auth authController(svr);

    spdlog::info("Server running on port {}", 8080);
    svr.listen("0.0.0.0", 8080);
}