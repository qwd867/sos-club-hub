#include "database_manager.h"
#include <iostream>

DatabaseManager::DatabaseManager() : db_(nullptr) {}

DatabaseManager::~DatabaseManager() {
    if (db_) {
        sqlite3_close(db_);
    }
}

DatabaseManager& DatabaseManager::get_instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize(const std::string& db_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    return create_tables();
}

bool DatabaseManager::execute(const std::string& sql) {
    char* err_msg = nullptr;
    if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg) != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    return true;
}

bool DatabaseManager::create_tables() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            email TEXT NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            username TEXT,
            created_at INTEGER DEFAULT (strftime('%s','now')),
            updated_at INTEGER DEFAULT (strftime('%s','now'))
        );
    )";
    return execute(sql);
}

bool DatabaseManager::create_user(const std::string& email,
                                  const std::string& password_hash,
                                  const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = "INSERT INTO users (email, password_hash, username) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password_hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

std::optional<User> DatabaseManager::find_user_by_email(const std::string& email) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = "SELECT id, email, password_hash, username, created_at, updated_at FROM users WHERE email = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User user;
        user.set_id(sqlite3_column_int64(stmt, 0));
        user.set_email(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        user.set_password_hash(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        const unsigned char* username_text = sqlite3_column_text(stmt, 3);
        if (username_text) {
            user.set_username(reinterpret_cast<const char*>(username_text));
        }
        user.set_created_at(sqlite3_column_int64(stmt, 4));
        user.set_updated_at(sqlite3_column_int64(stmt, 5));
        result = user;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::optional<User> DatabaseManager::find_user_by_id(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = "SELECT id, email, password_hash, username, created_at, updated_at FROM users WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    sqlite3_bind_int64(stmt, 1, id);

    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User user;
        user.set_id(sqlite3_column_int64(stmt, 0));
        user.set_email(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        user.set_password_hash(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        const unsigned char* username_text = sqlite3_column_text(stmt, 3);
        if (username_text) {
            user.set_username(reinterpret_cast<const char*>(username_text));
        }
        user.set_created_at(sqlite3_column_int64(stmt, 4));
        user.set_updated_at(sqlite3_column_int64(stmt, 5));
        result = user;
    }
    sqlite3_finalize(stmt);
    return result;
}

bool DatabaseManager::update_user_password(int64_t id, const std::string& new_password_hash) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = "UPDATE users SET password_hash = ?, updated_at = strftime('%s','now') WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(stmt, 1, new_password_hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}
