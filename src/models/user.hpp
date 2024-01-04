#ifndef MODEL_USER_H
#define MODEL_USER_H

#include <nlohmann/json.hpp>
#include <regex>
#include <sodium.h>
#include <spdlog/spdlog.h>
#include <string>
using json = nlohmann::json;
using namespace std;

namespace Model {
/// @brief User roles
enum Role { ADMIN, USER, NONE };

/// @brief User model
struct User {
    string user_id;
    string name;
    string email;
    string password;
    Role role;

    /// @brief This method checks if the name is valid
    /// @return bool Returns true if the name is valid else false
    bool check_name() const {
        // Name should be a string and not empty (or null) and not too long (max 255) and not too
        // short (min 3) and not contain any special characters (only letters, numbers)
        if (name.empty() || name.length() > 255 || name.length() < 3 ||
            !regex_match(name, regex("^[a-zA-Z0-9]*$"))) {
            return false;
        }
        return true;
    }

    /// @brief This method checks if the email is valid
    /// @return bool Returns true if the email is valid else false
    bool check_email() const {
        // Email should just pass valid email regex
        if (!regex_match(email, regex("^[a-zA-Z0-9+_.-]+@[a-zA-Z0-9.-]+$"))) {
            return false;
        }
        return true;
    }

    /// @brief This method checks if the password is valid
    /// @return bool Returns true if the password is valid else false
    bool check_password() const {
        // Password should be a string and not empty (or null) and not too long (max 255) and not
        // too short (min 8) and should contain at least one uppercase letter, one lowercase letter,
        // one number and one special character
        if (password.empty() || password.length() > 255 || password.length() < 8 ||
            !regex_match(
                password, regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[!@#$%^&*()_+\\-=\\[\\]{};'"
                                ":\"\\\\|,.<>\\/?]).{8,}$"))) {
            return false;
        }
        return true;
    }

    /// @brief This method checks if all the fields are valid
    /// @return bool Returns true if all the fields are valid else false
    bool check_fields() const {
        // Check if all the fields have correct values using regex
        // ID should be fine cause it's generated
        if (!check_name() || !check_email() || !check_password()) {
            return false;
        }
        return true;
    }

    /// @brief This method compares the entered password with the stored password
    /// @param entered_password The password entered by the user
    /// @return bool Returns true if the password is correct else false
    bool validate_password(const std::string &entered_password) {
        if (sodium_init() < 0) {
            spdlog::error("Error: Unable to initialize libsodium");
            return false;
        }

        int result = crypto_pwhash_str_verify(
            password.c_str(), entered_password.c_str(), entered_password.length());

        return result == 0;
    }
};
/// @brief This converts from Json object to User object
/// @param j The Json object
/// @param u The User object
void from_json(const json &j, User &u) {
    auto it = j.find("user_id");
    if (it != j.end())
        it->get_to(u.user_id);
    it = j.find("name");
    if (it != j.end())
        it->get_to(u.name);
    it = j.find("email");
    if (it != j.end())
        it->get_to(u.email);
    it = j.find("password");
    if (it != j.end())
        it->get_to(u.password);
    it = j.find("role");
    if (it != j.end())
        it->get_to(u.role);
}

/// @brief This converts from User object to Json object
/// @param j The Json object
/// @param u The User object
void to_json(json &j, const User &u) {
    j = json{
        {"user_id", u.user_id},
        {"name", u.name},
        {"email", u.email},
        {"password", u.password},
        {"role", u.role}};
}
} // namespace Model

#endif // MODEL_USER_H
