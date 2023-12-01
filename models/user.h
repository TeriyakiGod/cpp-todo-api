#ifndef MODEL_USER_H
#define MODEL_USER_H

#include <regex>
#include <string>
#include <json.hpp>
#include <sodium.h>
#include <spdlog/spdlog.h>
using json = nlohmann::json;
using namespace std;

namespace Model
{
    struct User
    {
        string user_id;
        string name;
        string email;
        string password;

        bool check_name() const
        {
            // Name should be a string and not empty (or null) and not too long (max 255) and not too short (min 3) and not contain any special characters (only letters, numbers)
            if (name.empty() || name.length() > 255 || name.length() < 3 || !regex_match(name, regex("^[a-zA-Z0-9]*$")))
            {
                return false;
            }
            return true;
        }

        bool check_email() const
        {
            // Email should just pass valid email regex
            if (!regex_match(email, regex("^[a-zA-Z0-9+_.-]+@[a-zA-Z0-9.-]+$")))
            {
                return false;
            }
            return true;
        }

        bool check_password() const
        {
            // Password should be a string and not empty (or null) and not too long (max 255) and not too short (min 8) and should contain at least one uppercase letter, one lowercase letter, one number and one special character
            if (password.empty() || password.length() > 255 || password.length() < 8 || !regex_match(password, regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[!@#$%^&*()_+\\-=\\[\\]{};':\"\\\\|,.<>\\/?]).{8,}$")))
            {
                return false;
            }
            return true;
        }

        bool check_fields() const
        {
            // Check if all the fields have correct values using regex
            // ID should be fine cause it's generated
            if (!check_name() || !check_email() || !check_password())
            {
                return false;
            }
            return true;
        }

        bool validate_password(const std::string& entered_password)
        {
            if (sodium_init() < 0)
            {
                spdlog::error("Error: Unable to initialize libsodium");
                return false;
            }

            int result = crypto_pwhash_str_verify(
                password.c_str(),
                entered_password.c_str(),
                entered_password.length());

            return result == 0;
        }
    };

    void from_json(const json& j, User& u)
    {
        j.at("user_id").get_to(u.user_id);
        j.at("name").get_to(u.name);
        j.at("email").get_to(u.email);
        j.at("password").get_to(u.password);
    }

    void to_json(json& j, const User& u)
    {
        j = json{
            {"user_id", u.user_id},
            {"name", u.name},
            {"email", u.email},
            {"password", u.password}
        };
    }
} // namespace Model

#endif // MODEL_USER_H
