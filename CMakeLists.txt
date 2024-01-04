cmake_minimum_required(VERSION 3.0.0)
project(cpp-todo-app VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

# Set the path to the vcpkg toolchain file
set(VCPKG_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Include vcpkg toolchain file
if (EXISTS ${VCPKG_TOOLCHAIN_FILE})
    message(STATUS "Using vcpkg toolchain file: ${VCPKG_TOOLCHAIN_FILE}")
    include(${VCPKG_TOOLCHAIN_FILE})
else()
    message(FATAL_ERROR "vcpkg toolchain file not found at: ${VCPKG_TOOLCHAIN_FILE}")
endif()

find_package(httplib CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(stduuid CONFIG REQUIRED)
find_package(pugixml CONFIG REQUIRED)

find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(cpp-todo-app src/main.cpp)

target_link_libraries(cpp-todo-app PRIVATE unofficial::sqlite3::sqlite3)
target_link_libraries(cpp-todo-app PRIVATE spdlog::spdlog_header_only)
target_link_libraries(cpp-todo-app PRIVATE fmt::fmt-header-only)
target_link_libraries(cpp-todo-app PRIVATE unofficial-sodium::sodium unofficial-sodium::sodium_config_public)
target_link_libraries(cpp-todo-app PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(cpp-todo-app PRIVATE httplib::httplib)
target_link_libraries(cpp-todo-app PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(cpp-todo-app PRIVATE stduuid)
target_link_libraries(cpp-todo-app PRIVATE pugixml::static pugixml::pugixml)
target_include_directories(cpp-todo-app PRIVATE ${JWT_CPP_INCLUDE_DIRS})