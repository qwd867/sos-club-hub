#include "database_manager.h"
#include <iostream>
#include <vector>

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
    if (!create_tables()) return false;
    if (!migrate_users_table()) return false;
    if (!create_quest_tables()) return false;
    if (!insert_quest_seed_data()) return false;
    return true;
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
            quest_points INTEGER NOT NULL DEFAULT 0,
            created_at INTEGER DEFAULT (strftime('%s','now')),
            updated_at INTEGER DEFAULT (strftime('%s','now'))
        );
    )";
    return execute(sql);
}

bool DatabaseManager::column_exists(const std::string& table, const std::string& column) {
    const char* sql = "SELECT 1 FROM pragma_table_info(?) WHERE name = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, table.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, column.c_str(), -1, SQLITE_STATIC);
    bool exists = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt);
    return exists;
}

bool DatabaseManager::migrate_users_table() {
    if (!column_exists("users", "quest_points")) {
        return execute("ALTER TABLE users ADD COLUMN quest_points INTEGER NOT NULL DEFAULT 0;");
    }
    return true;
}

bool DatabaseManager::create_quest_tables() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS quests (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            quest_key VARCHAR(64) UNIQUE NOT NULL,
            name VARCHAR(128) NOT NULL,
            description TEXT,
            type VARCHAR(16) NOT NULL,
            category VARCHAR(32),
            target_progress INTEGER NOT NULL DEFAULT 1,
            points_reward INTEGER NOT NULL DEFAULT 0,
            unlock_quest_id INTEGER,
            sort_order INTEGER DEFAULT 0,
            is_active INTEGER DEFAULT 1,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );

        CREATE TABLE IF NOT EXISTS user_quests (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            quest_id INTEGER NOT NULL,
            status VARCHAR(16) NOT NULL DEFAULT 'available',
            current_progress INTEGER NOT NULL DEFAULT 0,
            unlocked_at DATETIME,
            completed_at DATETIME,
            claimed_at DATETIME,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            UNIQUE(user_id, quest_id)
        );

        CREATE TABLE IF NOT EXISTS user_daily_reset (
            user_id INTEGER PRIMARY KEY,
            reset_date DATE NOT NULL,
            brainstorm_task_id TEXT,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );

        CREATE INDEX IF NOT EXISTS idx_user_quests_user ON user_quests(user_id);
        CREATE INDEX IF NOT EXISTS idx_user_quests_status ON user_quests(status);
        CREATE UNIQUE INDEX IF NOT EXISTS idx_user_quests_unique ON user_quests(user_id, quest_id);
    )";
    return execute(sql);
}

bool DatabaseManager::insert_quest_seed_data() {
    const char* check_sql = "SELECT COUNT(*) FROM quests;";
    sqlite3_stmt* check_stmt = nullptr;
    if (sqlite3_prepare_v2(db_, check_sql, -1, &check_stmt, nullptr) != SQLITE_OK) return false;
    int count = 0;
    if (sqlite3_step(check_stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(check_stmt, 0);
    }
    sqlite3_finalize(check_stmt);
    if (count > 0) return true;

    const char* sql = R"(
        INSERT INTO quests (quest_key, name, description, type, target_progress, points_reward, sort_order) VALUES
        ('daily_checkin', '文艺社教室巡查', '完成每日签到', 'daily', 1, 100, 1),
        ('daily_interact_mikuru', '给实玖瑠拍照', '与实玖瑠互动3次', 'daily', 3, 150, 2),
        ('daily_view_yuki', '长门的读书时间', '查看长门角色详情页', 'daily', 1, 100, 3),
        ('daily_brainstorm', '团长的突发奇想', '完成春日团长指定的随机任务', 'daily', 1, 200, 4),
        ('weekly_checkin_streak', '无尽八月挑战', '连续7天登录签到', 'weekly', 7, 1000, 10),
        ('weekly_interact_all', 'SOS团全员集合', '与所有5位核心成员互动至少1次', 'weekly', 5, 800, 11),
        ('weekly_wall_posts', '电影拍摄周', '在留言墙发布3条以上评论', 'weekly', 3, 600, 12),
        ('weekly_view_details', '封闭空间调查', '累计查看角色详情10次', 'weekly', 10, 500, 13),
        ('legend_first_login', 'SOS团创立', '完成首次登录', 'legendary', 1, 0, 20),
        ('legend_interact_haruhi', '我对普通人类没有兴趣', '首次与春日互动', 'legendary', 1, 500, 21),
        ('legend_interact_yuki', '资讯操作', '首次与长门互动', 'legendary', 1, 500, 22),
        ('legend_interact_mikuru', '这是禁止事项', '首次与实玖瑠互动', 'legendary', 1, 500, 23),
        ('legend_interact_koizumi', '机关的试探', '首次与古泉互动', 'legendary', 1, 500, 24),
        ('legend_interact_kyon', '吐槽能量MAX', '首次与阿虚互动', 'legendary', 1, 500, 25),
        ('legend_winter_login', '凉宫春日的消失', '在冬季（12月）登录满10天', 'legendary', 10, 0, 26),
        ('legend_daily_master', '凉宫春日的惊愕', '连续30天完成所有每日委托', 'legendary', 30, 5000, 27);
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
    const char* sql = "SELECT id, email, password_hash, username, quest_points, created_at, updated_at FROM users WHERE email = ?;";
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
        user.set_quest_points(sqlite3_column_int64(stmt, 4));
        user.set_created_at(sqlite3_column_int64(stmt, 5));
        user.set_updated_at(sqlite3_column_int64(stmt, 6));
        result = user;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::optional<User> DatabaseManager::find_user_by_id(int64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = "SELECT id, email, password_hash, username, quest_points, created_at, updated_at FROM users WHERE id = ?;";
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
        user.set_quest_points(sqlite3_column_int64(stmt, 4));
        user.set_created_at(sqlite3_column_int64(stmt, 5));
        user.set_updated_at(sqlite3_column_int64(stmt, 6));
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

bool DatabaseManager::add_quest_points(int64_t user_id, int points) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = "UPDATE users SET quest_points = quest_points + ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, points);
    sqlite3_bind_int64(stmt, 2, user_id);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::ensure_user_quests_initialized(int64_t user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* check_sql = "SELECT COUNT(*) FROM user_quests WHERE user_id = ?;";
    sqlite3_stmt* check_stmt = nullptr;
    if (sqlite3_prepare_v2(db_, check_sql, -1, &check_stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(check_stmt, 1, user_id);
    int count = 0;
    if (sqlite3_step(check_stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(check_stmt, 0);
    }
    sqlite3_finalize(check_stmt);
    if (count > 0) return true;

    const char* insert_sql = R"(
        INSERT INTO user_quests (user_id, quest_id, status, unlocked_at)
        SELECT ?, id,
            CASE WHEN type = 'legendary' AND quest_key != 'legend_first_login' THEN 'locked' ELSE 'available' END,
            CASE WHEN type = 'legendary' AND quest_key != 'legend_first_login' THEN NULL ELSE datetime('now') END
        FROM quests WHERE is_active = 1;
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, insert_sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, user_id);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

std::vector<nlohmann::json> DatabaseManager::get_user_quests(int64_t user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<nlohmann::json> results;
    const char* sql = R"(
        SELECT q.id, q.quest_key, q.name, q.description, q.type, q.category,
               q.target_progress, q.points_reward, q.unlock_quest_id, q.sort_order,
               uq.status, uq.current_progress, uq.unlocked_at, uq.completed_at, uq.claimed_at
        FROM quests q
        JOIN user_quests uq ON q.id = uq.quest_id
        WHERE uq.user_id = ? AND q.is_active = 1
        ORDER BY q.sort_order;
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, user_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        nlohmann::json quest;
        quest["id"] = sqlite3_column_int(stmt, 0);
        quest["quest_key"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        quest["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const unsigned char* desc = sqlite3_column_text(stmt, 3);
        if (desc) quest["description"] = reinterpret_cast<const char*>(desc);
        quest["type"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        const unsigned char* cat = sqlite3_column_text(stmt, 5);
        if (cat) quest["category"] = reinterpret_cast<const char*>(cat);
        quest["target_progress"] = sqlite3_column_int(stmt, 6);
        quest["points_reward"] = sqlite3_column_int(stmt, 7);
        quest["unlock_quest_id"] = sqlite3_column_int(stmt, 8);
        quest["sort_order"] = sqlite3_column_int(stmt, 9);
        quest["status"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
        quest["current_progress"] = sqlite3_column_int(stmt, 11);
        const unsigned char* unlocked = sqlite3_column_text(stmt, 12);
        if (unlocked) quest["unlocked_at"] = reinterpret_cast<const char*>(unlocked);
        const unsigned char* completed = sqlite3_column_text(stmt, 13);
        if (completed) quest["completed_at"] = reinterpret_cast<const char*>(completed);
        const unsigned char* claimed = sqlite3_column_text(stmt, 14);
        if (claimed) quest["claimed_at"] = reinterpret_cast<const char*>(claimed);
        results.push_back(quest);
    }
    sqlite3_finalize(stmt);
    return results;
}

std::vector<nlohmann::json> DatabaseManager::get_all_quests() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<nlohmann::json> results;
    const char* sql = "SELECT id, quest_key, name, description, type, category, target_progress, points_reward, unlock_quest_id, sort_order FROM quests WHERE is_active = 1 ORDER BY sort_order;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        nlohmann::json quest;
        quest["id"] = sqlite3_column_int(stmt, 0);
        quest["quest_key"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        quest["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const unsigned char* desc = sqlite3_column_text(stmt, 3);
        if (desc) quest["description"] = reinterpret_cast<const char*>(desc);
        quest["type"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        const unsigned char* cat = sqlite3_column_text(stmt, 5);
        if (cat) quest["category"] = reinterpret_cast<const char*>(cat);
        quest["target_progress"] = sqlite3_column_int(stmt, 6);
        quest["points_reward"] = sqlite3_column_int(stmt, 7);
        quest["unlock_quest_id"] = sqlite3_column_int(stmt, 8);
        quest["sort_order"] = sqlite3_column_int(stmt, 9);
        results.push_back(quest);
    }
    sqlite3_finalize(stmt);
    return results;
}

bool DatabaseManager::update_quest_progress(int64_t user_id, int quest_id, int increment) {
    std::lock_guard<std::mutex> lock(mutex_);

    // 先获取 target_progress 和 current_progress
    const char* select_sql = "SELECT q.target_progress, uq.current_progress, uq.status FROM quests q JOIN user_quests uq ON q.id = uq.quest_id WHERE uq.user_id = ? AND uq.quest_id = ?;";
    sqlite3_stmt* select_stmt = nullptr;
    if (sqlite3_prepare_v2(db_, select_sql, -1, &select_stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(select_stmt, 1, user_id);
    sqlite3_bind_int(select_stmt, 2, quest_id);

    int target_progress = 1;
    int current_progress = 0;
    std::string status = "available";
    if (sqlite3_step(select_stmt) == SQLITE_ROW) {
        target_progress = sqlite3_column_int(select_stmt, 0);
        current_progress = sqlite3_column_int(select_stmt, 1);
        status = reinterpret_cast<const char*>(sqlite3_column_text(select_stmt, 2));
    }
    sqlite3_finalize(select_stmt);

    // 如果任务已锁定，先解锁
    if (status == "locked") {
        status = "available";
    }

    int new_progress = current_progress + increment;
    if (new_progress > target_progress) new_progress = target_progress;
    bool is_completed = new_progress >= target_progress;

    const char* update_sql = R"(
        UPDATE user_quests
        SET current_progress = ?,
            status = ?,
            completed_at = CASE WHEN ? AND completed_at IS NULL THEN datetime('now') ELSE completed_at END,
            unlocked_at = CASE WHEN unlocked_at IS NULL THEN datetime('now') ELSE unlocked_at END,
            updated_at = datetime('now')
        WHERE user_id = ? AND quest_id = ?;
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, update_sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, new_progress);
    sqlite3_bind_text(stmt, 2, is_completed ? "completed" : "in_progress", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, is_completed ? 1 : 0);
    sqlite3_bind_int64(stmt, 4, user_id);
    sqlite3_bind_int(stmt, 5, quest_id);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::complete_quest(int64_t user_id, int quest_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = R"(
        UPDATE user_quests
        SET status = 'completed',
            current_progress = target_progress,
            completed_at = datetime('now'),
            updated_at = datetime('now')
        WHERE user_id = ? AND quest_id = ?;
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, quest_id);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::claim_quest_reward(int64_t user_id, int quest_id, int points_reward) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = R"(
        UPDATE user_quests
        SET status = 'claimed', claimed_at = datetime('now'), updated_at = datetime('now')
        WHERE user_id = ? AND quest_id = ? AND status = 'completed';
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, quest_id);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!success || sqlite3_changes(db_) == 0) return false;

    if (points_reward > 0) {
        const char* points_sql = "UPDATE users SET quest_points = quest_points + ? WHERE id = ?;";
        sqlite3_stmt* points_stmt = nullptr;
        if (sqlite3_prepare_v2(db_, points_sql, -1, &points_stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_int(points_stmt, 1, points_reward);
        sqlite3_bind_int64(points_stmt, 2, user_id);
        bool points_success = sqlite3_step(points_stmt) == SQLITE_DONE;
        sqlite3_finalize(points_stmt);
        return points_success;
    }
    return true;
}

int DatabaseManager::get_user_points(int64_t user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = "SELECT quest_points FROM users WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_int64(stmt, 1, user_id);
    int points = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        points = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return points;
}

std::string DatabaseManager::get_user_daily_brainstorm(int64_t user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = "SELECT brainstorm_task_id FROM user_daily_reset WHERE user_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return "";
    sqlite3_bind_int64(stmt, 1, user_id);
    std::string result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        if (text) result = reinterpret_cast<const char*>(text);
    }
    sqlite3_finalize(stmt);
    return result;
}

std::string DatabaseManager::get_user_last_reset_date(int64_t user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = "SELECT reset_date FROM user_daily_reset WHERE user_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return "";
    sqlite3_bind_int64(stmt, 1, user_id);
    std::string result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        if (text) result = reinterpret_cast<const char*>(text);
    }
    sqlite3_finalize(stmt);
    return result;
}

bool DatabaseManager::unlock_quest(int64_t user_id, int quest_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = "UPDATE user_quests SET status = 'available', unlocked_at = datetime('now') WHERE user_id = ? AND quest_id = ? AND status = 'locked';";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, quest_id);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::reset_daily_quests(int64_t user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = "DELETE FROM user_quests WHERE user_id = ? AND quest_id IN (SELECT id FROM quests WHERE type = 'daily');";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    const char* insert_sql = R"(
        INSERT INTO user_quests (user_id, quest_id, status, unlocked_at)
        SELECT ?, id, 'available', datetime('now')
        FROM quests WHERE type = 'daily' AND is_active = 1;
    )";
    sqlite3_stmt* insert_stmt = nullptr;
    if (sqlite3_prepare_v2(db_, insert_sql, -1, &insert_stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(insert_stmt, 1, user_id);
    bool success = sqlite3_step(insert_stmt) == SQLITE_DONE;
    sqlite3_finalize(insert_stmt);
    return success;
}

bool DatabaseManager::set_user_daily_brainstorm(int64_t user_id, const std::string& task_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    const char* sql = R"(
        INSERT INTO user_daily_reset (user_id, reset_date, brainstorm_task_id)
        VALUES (?, date('now'), ?)
        ON CONFLICT(user_id) DO UPDATE SET
            reset_date = date('now'),
            brainstorm_task_id = excluded.brainstorm_task_id,
            updated_at = datetime('now');
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, task_id.c_str(), -1, SQLITE_STATIC);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}
