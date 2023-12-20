cmake_minimum_required(VERSION 3.0.0)
project(cpp-todo-app VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(SQLite3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(OpenSSL REQUIRED)


include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB CONTROLLERS controllers/*.cpp)
file(GLOB MODELS models/*.cpp)
file(GLOB SOURCE src/*.cpp)

add_executable(cpp-todo-app ${CONTROLLERS} ${MODELS} ${SOURCE})

target_link_libraries(cpp-todo-app PRIVATE SQLite::SQLite3)
target_link_libraries(cpp-todo-app PRIVATE spdlog::spdlog)
target_link_libraries(cpp-todo-app PRIVATE sodium)
target_link_libraries(cpp-todo-app PRIVATE OpenSSL::SSL OpenSSL::Crypto)