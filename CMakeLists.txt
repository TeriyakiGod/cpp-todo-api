cmake_minimum_required(VERSION 3.0.0)
project(cpp-todo-app VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(SQLite3 REQUIRED)
find_package(spdlog REQUIRED)

include_directories(include)

add_executable(cpp-todo-app main.cpp db.h controllers/todo.h controllers/user.h models/todo.h models/user.h)

target_link_libraries(cpp-todo-app PRIVATE SQLite::SQLite3)
target_link_libraries(cpp-todo-app PRIVATE spdlog::spdlog)
target_link_libraries(cpp-todo-app PRIVATE sodium)