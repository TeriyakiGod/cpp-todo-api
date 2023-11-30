#ifndef TOOLS_H
#define TOOLS_H

#include <uuid.h>
#include <sodium.h>
#include <string>

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
            spdlog::debug("Hashed password: {}", hashed_password);
            return hashed_password;
        }
    };
}

#endif // TOOLS_H