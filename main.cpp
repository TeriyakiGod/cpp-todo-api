#include <iostream>
#include "db.h"
#include "api/todo.h"
#include "httplib.h"
#include "spdlog/spdlog.h"

int main(int, char**)
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting server...");
    httplib::Server svr;

    svr.set_logger([](const auto& req, const auto& res) {
        spdlog::info("{} {} {}", req.method, req.path, res.status);
        });

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
        });

    TodoAPI::expose(svr);

    spdlog::info("Server running on port {}", 8080);
    svr.listen("0.0.0.0", 8080);
}