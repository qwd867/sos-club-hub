#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include "database_manager.h"
#include "json.hpp"
#include <string>

class AuthService {
public:
    explicit AuthService(DatabaseManager& db);

    nlohmann::json register_user(const std::string& email,
                                  const std::string& password,
                                  const std::string& username);
    nlohmann::json login(const std::string& email, const std::string& password);
    nlohmann::json forgot_password(const std::string& email,
                                    const std::string& new_password);
    nlohmann::json logout(const std::string& token);

    static bool verify_token(const std::string& token, int64_t& out_user_id);

private:
    DatabaseManager& db_;

    static std::string generate_token(int64_t user_id);
};

#endif
