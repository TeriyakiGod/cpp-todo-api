#ifndef MODELS_H
#define MODELS_H
#include <string>
#include "spdlog/spdlog.h"
#include "json.hpp"
using json = nlohmann::json;

class Model {
public:
    typedef struct {
        std::string id;
        std::string title;
        std::string description;
        std::string status;
    } Todo;

    typedef struct {
        std::string id;
        std::string name;
        std::string email;
        std::string password;
    } User;

    template<typename T>
    static T fromJson(nlohmann::json& body) {
        T model;
        for (auto& [key, value] : body.items()) {
            try {
                model.value(key, value);
            }
            catch (const std::exception& e) {
                spdlog::error("Error parsing json: {}", e.what());
            }
        }
        return model;
    }

};

#endif // MODELS_H