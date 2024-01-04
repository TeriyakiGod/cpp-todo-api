#include "controllers/auth-controller.hpp"
#include "controllers/todo-controller.hpp"
#include "controllers/user-controller.hpp"
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
    spdlog::set_level(spdlog::level::info);

    Tools::Jwt::generate_secret_key(32);

    spdlog::info("Starting server...");
    httplib::Server svr;

    svr.set_logger([](const auto &req, const auto &res) {
        spdlog::info("{} {} {}", req.method, req.path, res.status);
    });

    auto ret = svr.set_mount_point("/", "doc/html");
    if (!ret) {
        spdlog::error("Failed to set mount point");
    }

    Controller::Todo todoController(svr);
    Controller::User userController(svr);
    Controller::Auth authController(svr);

    spdlog::info("Server running on port {}", 8080);
    svr.listen("0.0.0.0", 8080);
}