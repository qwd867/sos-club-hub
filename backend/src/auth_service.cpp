#include "auth_service.h"
#include "sha256.h"
#include "http_response.h"
#include <chrono>

AuthService::AuthService(DatabaseManager& db) : db_(db) {}

std::string AuthService::generate_token(int64_t user_id) {
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    std::string seed = std::to_string(user_id) + "_" + std::to_string(now);
    return Sha256::hash(seed);
}

bool AuthService::verify_token(const std::string& token, int64_t& out_user_id) {
    if (token.empty()) return false;
    try {
        out_user_id = std::stoll(token);
        auto user = DatabaseManager::get_instance().find_user_by_id(out_user_id);
        return user.has_value();
    } catch (...) {
        return false;
    }
}

nlohmann::json AuthService::register_user(const std::string& email,
                                          const std::string& password,
                                          const std::string& username) {
    if (email.empty() || password.empty()) {
        return HttpResponse::error("Email and password are required");
    }

    auto existing = db_.find_user_by_email(email);
    if (existing.has_value()) {
        return HttpResponse::error("Email already registered", 409);
    }

    std::string password_hash = Sha256::hash(password);
    if (!db_.create_user(email, password_hash, username)) {
        return HttpResponse::error("Failed to create user", 500);
    }

    auto user = db_.find_user_by_email(email);
    if (!user.has_value()) {
        return HttpResponse::error("Failed to retrieve user after creation", 500);
    }

    std::string token = std::to_string(user->get_id());
    nlohmann::json data;
    data["token"] = token;
    data["user"] = {
        {"id", user->get_id()},
        {"email", user->get_email()},
        {"username", user->get_username()}
    };
    return HttpResponse::success(data);
}

nlohmann::json AuthService::login(const std::string& email, const std::string& password) {
    if (email.empty() || password.empty()) {
        return HttpResponse::error("Email and password are required");
    }

    auto user = db_.find_user_by_email(email);
    if (!user.has_value()) {
        return HttpResponse::error("User not found", 404);
    }

    std::string password_hash = Sha256::hash(password);
    if (user->get_password_hash() != password_hash) {
        return HttpResponse::error("Invalid password", 401);
    }

    std::string token = std::to_string(user->get_id());
    nlohmann::json data;
    data["token"] = token;
    data["user"] = {
        {"id", user->get_id()},
        {"email", user->get_email()},
        {"username", user->get_username()}
    };
    return HttpResponse::success(data);
}

nlohmann::json AuthService::forgot_password(const std::string& email,
                                            const std::string& new_password) {
    if (email.empty() || new_password.empty()) {
        return HttpResponse::error("Email and new password are required");
    }

    auto user = db_.find_user_by_email(email);
    if (!user.has_value()) {
        return HttpResponse::error("User not found", 404);
    }

    std::string new_hash = Sha256::hash(new_password);
    if (!db_.update_user_password(user->get_id(), new_hash)) {
        return HttpResponse::error("Failed to update password", 500);
    }

    return HttpResponse::success({{"message", "Password updated successfully"}});
}

nlohmann::json AuthService::logout(const std::string& token) {
    return HttpResponse::success({{"message", "Logged out successfully"}});
}
