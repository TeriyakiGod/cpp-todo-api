#ifndef TOOLS_H
#define TOOLS_H

#include <uuid.h>
#include <sodium.h>
#include <string>
#include <spdlog/spdlog.h>
#include <pugixml/pugixml.hpp>

#define RESOURCE_PATH "../res/"

namespace Tools
{
    class Uuid
    {
    public:
        static std::string generate()
        {
            std::random_device rd;
            auto seed_data = std::array<int, std::mt19937::state_size>{};
            std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
            std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
            std::mt19937 generator(seq);
            uuids::uuid_random_generator gen{ generator };

            return uuids::to_string(gen());
        }
    };
    class Hash
    {
    public:
        static std::string generate(std::string password)
        {
            spdlog::debug("Password: {}", password);
            if (sodium_init() < 0)
            {
                spdlog::error("Error: Unable to initialize libsodium");
                return nullptr;
            }

            unsigned char salt[crypto_pwhash_SALTBYTES];
            randombytes_buf(salt, sizeof(salt));

            std::string hashed_password(crypto_pwhash_STRBYTES, ' ');
            if (crypto_pwhash_str(
                hashed_password.data(),
                password.c_str(),
                password.length(),
                crypto_pwhash_OPSLIMIT_INTERACTIVE,
                crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
            {
                spdlog::error("Error: Unable to hash password");
                return nullptr;
            }
            return hashed_password;
        }
    };
    class Resource {
    public:
        static std::string load_string(std::string res, std::string name) {
            pugi::xml_document doc;
            std::string file_path = RESOURCE_PATH + res + ".xml";
            pugi::xml_parse_result result = doc.load_file(file_path.c_str());
            if (!result) {
                spdlog::error("Error: Unable to load file {}", res + ".xml");
                return nullptr;
            }
            for (pugi::xml_node string_node = doc.child(res.c_str()).first_child(); string_node; string_node = string_node.next_sibling()) {
                if (std::string(string_node.attribute("name").value()) == name) {
                    return string_node.text().as_string();
                }
            }
            spdlog::error("Error: Unable to find node {}", res + "/" + name);
            return nullptr;
        }
    };
}

#endif // TOOLS_H