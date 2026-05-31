#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "user.h"
#include <sqlite3.h>
#include <mutex>
#include <optional>
#include <string>

class DatabaseManager {
public:
    static DatabaseManager& get_instance();

    bool initialize(const std::string& db_path = "auth.db");

    bool create_user(const std::string& email, const std::string& password_hash,
                     const std::string& username);
    std::optional<User> find_user_by_email(const std::string& email);
    std::optional<User> find_user_by_id(int64_t id);
    bool update_user_password(int64_t id, const std::string& new_password_hash);

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool execute(const std::string& sql);
    bool create_tables();

    sqlite3* db_;
    std::mutex mutex_;
};

#endif
