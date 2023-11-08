cmake_minimum_required(VERSION 3.0.0)
project(cpp-todo-app VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(SQLite3 REQUIRED)
find_package(Crow REQUIRED)

add_executable(cpp-todo-app main.cpp db.h api/todo.cpp api/todo.h )

target_link_libraries(cpp-todo-app PRIVATE Crow::Crow)
target_link_libraries(cpp-todo-app PRIVATE SQLite::SQLite3)