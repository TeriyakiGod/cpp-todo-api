#ifndef TOOLS_H
#define TOOLS_H

#include <fstream>
#include <jwt-cpp/jwt.h>
#include <pugixml.hpp>
#include <sodium.h>
#include <spdlog/spdlog.h>
#include <string>
#include <uuid.h>

#define RESOURCE_PATH "../res/"

#define SECRET_KEY_FILE "../secret.key"
#define JWT_TOKEN_EXPIRATION 10

/// @brief Namespace for tools and utilities
namespace Tools {
/// @brief Helper class for handling JWT
class Jwt {
public:
    /// @brief Generates a JWT token
    /// @param user_id The user id to be stored in the token
    /// @return std::string Returns the generated token
    static std::string generate_token(std::string user_id) {
        std::ifstream file(SECRET_KEY_FILE);
        std::string secret_key(
            (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::string timestamp =
            std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        std::string token =
            jwt::create()
                .set_issuer("auth0")
                .set_type("JWS")
                .set_payload_claim("user_id", jwt::claim(user_id))
                .set_payload_claim("timestamp", jwt::claim(timestamp))
                .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{600})
                .sign(jwt::algorithm::hs256{secret_key});
        return token;
    }

    /// @brief Verifies a JWT token
    /// @param token The token to be verified
    static std::string verify_user(std::string token) {
        std::ifstream file(SECRET_KEY_FILE);
        std::string secret_key(
            (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        try {
            auto decoded_token = jwt::decode(token);
            auto verifier = jwt::verify()
                                .allow_algorithm(jwt::algorithm::hs256{secret_key})
                                .with_issuer("auth0");
            verifier.verify(decoded_token);
            if (decoded_token.get_expires_at() < std::chrono::system_clock::now()) {
                return "Token expired";
            }
            return decoded_token.get_payload_claim("user_id").as_string();
        } catch (const jwt::token_verification_exception &e) {
            spdlog::error("Error: Token verification failed - {}", e.what());
            return "";
        }
    }
};

/// @brief Helper class for handling UUID
class Uuid {
public:
    /// @brief Generates a UUID
    /// @return std::string Returns the generated UUID
    static std::string generate() {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size>{};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 generator(seq);
        uuids::uuid_random_generator gen{generator};

        return uuids::to_string(gen());
    }
};

/// @brief Helper class for handling hashing
class Hash {
public:
    /// @brief Hashes a password
    /// @param password The password to be hashed
    /// @return std::string Returns the hashed password
    static std::string generate(std::string password) {
        spdlog::debug("Password: {}", password);
        if (sodium_init() < 0) {
            spdlog::error("Error: Unable to initialize libsodium");
            return "";
        }

        unsigned char salt[crypto_pwhash_SALTBYTES];
        randombytes_buf(salt, sizeof(salt));

        std::string hashed_password(crypto_pwhash_STRBYTES, ' ');
        if (crypto_pwhash_str(
                hashed_password.data(), password.c_str(), password.length(),
                crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
            spdlog::error("Error: Unable to hash password");
            return "";
        }
        return hashed_password;
    }
};

/// @brief Helper class for handling resources (loading xml files)
class Resource {
public:
    /// @brief Loads a string from an xml file
    /// @param res The resource name (the xml file name)
    /// @param name The string name
    static std::string load_string(std::string res, std::string name) {
        pugi::xml_document doc;
        std::string file_path = RESOURCE_PATH + res + ".xml";
        pugi::xml_parse_result result = doc.load_file(file_path.c_str());
        if (!result) {
            spdlog::error("Error: Unable to load file {}", res + ".xml");
            return "";
        }
        for (pugi::xml_node string_node = doc.child(res.c_str()).first_child(); string_node;
             string_node = string_node.next_sibling()) {
            if (std::string(string_node.attribute("name").value()) == name) {
                return string_node.text().as_string();
            }
        }
        spdlog::error("Error: Unable to find node {}", res + "/" + name);
        return "";
    }
};
} // namespace Tools

#endif // TOOLS_H