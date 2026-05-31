#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "user.h"
#include "json.hpp"
#include <sqlite3.h>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

class DatabaseManager {
public:
    static DatabaseManager& get_instance();

    bool initialize(const std::string& db_path = "auth.db");

    bool create_user(const std::string& email, const std::string& password_hash,
                     const std::string& username);
    std::optional<User> find_user_by_email(const std::string& email);
    std::optional<User> find_user_by_id(int64_t id);
    bool update_user_password(int64_t id, const std::string& new_password_hash);

    // Quest related methods
    bool add_quest_points(int64_t user_id, int points);
    bool ensure_user_quests_initialized(int64_t user_id);
    std::vector<nlohmann::json> get_user_quests(int64_t user_id);
    std::vector<nlohmann::json> get_all_quests();
    bool update_quest_progress(int64_t user_id, int quest_id, int increment);
    bool claim_quest_reward(int64_t user_id, int quest_id, int points_reward);
    bool complete_quest(int64_t user_id, int quest_id);
    int get_user_points(int64_t user_id);
    std::string get_user_daily_brainstorm(int64_t user_id);
    bool set_user_daily_brainstorm(int64_t user_id, const std::string& task_id);
    std::string get_user_last_reset_date(int64_t user_id);
    bool unlock_quest(int64_t user_id, int quest_id);
    bool reset_daily_quests(int64_t user_id);

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool execute(const std::string& sql);
    bool create_tables();
    bool migrate_users_table();
    bool create_quest_tables();
    bool insert_quest_seed_data();
    bool column_exists(const std::string& table, const std::string& column);

    sqlite3* db_;
    std::mutex mutex_;
};

#endif
