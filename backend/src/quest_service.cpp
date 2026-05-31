#include "quest_service.h"
#include "http_response.h"
#include <chrono>
#include <iomanip>
#include <sstream>

QuestService::QuestService(DatabaseManager& db) : db_(db) {
    initializeBrainstormTasks();
}

void QuestService::initializeBrainstormTasks() {
    brainstorm_tasks_ = {
        {"find_john_smith", "去寻找约翰·史密斯！", "在页面任意位置点击搜索10次", 10, "page_click"},
        {"catch_alien", "给我抓一只星际迷航里的外星人", "与长门互动5次", 5, "interact"},
        {"sunny_weather", "明天的天气必须晴朗", "签到时看到晴天提示", 1, "checkin"},
        {"endless_eight", "让世界重复15532次", "刷新页面3次", 3, "refresh"},
        {"sci_fi_movie", "给我拍一部科幻电影", "在留言墙发布一条评论", 1, "comment"},
        {"find_sos_meaning", "找到SOS团的真正含义", "点击团徽3次", 3, "page_click"},
        {"holy_war", "组织一场圣战", "在留言墙发布一条评论", 1, "comment"},
        {"prove_math", "证明1+1=3", "完成一道简单数学题", 1, "brainstorm"},
        {"bunny_girl", "给实玖瑠穿上兔女郎装", "点击实玖瑠的换装图标", 1, "interact"},
        {"glasses_gone", "调查为什么长门的眼镜会消失", "查看长门详情页面的消失段落", 1, "view"}
    };
}

std::string QuestService::getCurrentDateStr() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d");
    return ss.str();
}

std::string QuestService::getCurrentWeekStr() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%W");
    return ss.str();
}

bool QuestService::needsDailyRefresh(int64_t user_id) {
    std::string last_reset = db_.get_user_last_reset_date(user_id);
    return last_reset != getCurrentDateStr();
}

BrainstormTask QuestService::pickBrainstormTask(int64_t user_id) {
    std::hash<int64_t> hasher;
    auto now = std::chrono::system_clock::now();
    auto days_since_epoch = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count() / 24;
    size_t seed = hasher(user_id) + days_since_epoch;
    size_t index = seed % brainstorm_tasks_.size();
    return brainstorm_tasks_[index];
}

void QuestService::refreshDailyQuests(int64_t user_id) {
    db_.ensure_user_quests_initialized(user_id);
    if (needsDailyRefresh(user_id)) {
        db_.reset_daily_quests(user_id);
        BrainstormTask task = pickBrainstormTask(user_id);
        db_.set_user_daily_brainstorm(user_id, task.id);
    }
}

void QuestService::refreshWeeklyQuests(int64_t user_id) {
    db_.ensure_user_quests_initialized(user_id);
}

nlohmann::json QuestService::getUserQuests(int64_t user_id) {
    db_.ensure_user_quests_initialized(user_id);
    refreshDailyQuests(user_id);
    refreshWeeklyQuests(user_id);

    auto quests = db_.get_user_quests(user_id);
    auto brainstorm_id = db_.get_user_daily_brainstorm(user_id);

    nlohmann::json daily = nlohmann::json::array();
    nlohmann::json weekly = nlohmann::json::array();
    nlohmann::json legendary = nlohmann::json::array();

    int daily_completed = 0;
    int daily_total = 0;

    for (const auto& q : quests) {
        std::string type = q.value("type", "");
        std::string status = q.value("status", "");
        std::string qkey = q.value("quest_key", "");

        if (type == "daily") {
            daily_total++;
            if (status == "claimed") daily_completed++;
            if (qkey == "daily_brainstorm" && !brainstorm_id.empty()) {
                for (const auto& bt : brainstorm_tasks_) {
                    if (bt.id == brainstorm_id) {
                        nlohmann::json modified = q;
                        modified["description"] = bt.description;
                        modified["brainstorm_title"] = bt.title;
                        daily.push_back(modified);
                        break;
                    }
                }
                continue;
            }
            daily.push_back(q);
        } else if (type == "weekly") {
            weekly.push_back(q);
        } else if (type == "legendary") {
            legendary.push_back(q);
        }
    }

    nlohmann::json result;
    result["daily"]["reset_at"] = getCurrentDateStr() + "T00:00:00Z";
    result["daily"]["completed_count"] = daily_completed;
    result["daily"]["total_count"] = daily_total;
    result["daily"]["quests"] = daily;
    result["weekly"]["quests"] = weekly;
    result["legendary"]["quests"] = legendary;
    result["total_points"] = db_.get_user_points(user_id);

    return HttpResponse::success(result);
}

nlohmann::json QuestService::reportProgress(int64_t user_id, const std::string& quest_key, int increment) {
    db_.ensure_user_quests_initialized(user_id);
    refreshDailyQuests(user_id);
    refreshWeeklyQuests(user_id);

    auto all_quests = db_.get_all_quests();
    int quest_id = -1;
    int target_progress = 1;
    for (const auto& q : all_quests) {
        if (q.value("quest_key", "") == quest_key) {
            quest_id = q.value("id", -1);
            target_progress = q.value("target_progress", 1);
            break;
        }
    }
    if (quest_id < 0) {
        return HttpResponse::error("Quest not found", 4001);
    }

    db_.update_quest_progress(user_id, quest_id, increment);

    auto quests = db_.get_user_quests(user_id);
    nlohmann::json updated;
    for (const auto& q : quests) {
        if (q.value("id", -1) == quest_id) {
            updated = q;
            break;
        }
    }

    bool is_newly_completed = false;
    if (updated.value("status", "") == "completed" && updated.contains("completed_at")) {
        is_newly_completed = true;
        unlockDependentQuests(user_id, quest_id);
    }

    nlohmann::json data;
    data["quest_id"] = quest_id;
    data["status"] = updated.value("status", "");
    data["current_progress"] = updated.value("current_progress", 0);
    data["target_progress"] = target_progress;
    if (updated.contains("completed_at")) data["completed_at"] = updated["completed_at"];
    data["is_newly_completed"] = is_newly_completed;

    return HttpResponse::success(data);
}

nlohmann::json QuestService::claimReward(int64_t user_id, int quest_id) {
    auto quests = db_.get_user_quests(user_id);
    int points_reward = 0;
    std::string status;
    for (const auto& q : quests) {
        if (q.value("id", -1) == quest_id) {
            points_reward = q.value("points_reward", 0);
            status = q.value("status", "");
            break;
        }
    }

    if (status == "claimed") {
        return HttpResponse::error("Reward already claimed", 4004);
    }
    if (status != "completed") {
        return HttpResponse::error("Quest not completed", 4003);
    }

    if (!db_.claim_quest_reward(user_id, quest_id, points_reward)) {
        return HttpResponse::error("Quest not found", 4001);
    }

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%H:%M:%S");

    nlohmann::json data;
    data["claimed_points"] = points_reward;
    data["total_points"] = db_.get_user_points(user_id);
    data["claimed_at"] = getCurrentDateStr() + "T" + ss.str() + "Z";

    return HttpResponse::success(data);
}

nlohmann::json QuestService::getUserPoints(int64_t user_id) {
    nlohmann::json data;
    data["points"] = db_.get_user_points(user_id);
    return HttpResponse::success(data);
}

void QuestService::onUserCheckin(int64_t user_id) {
    reportProgress(user_id, "daily_checkin", 1);
    reportProgress(user_id, "weekly_checkin_streak", 1);
}

void QuestService::onUserInteract(int64_t user_id, const std::string& character_id) {
    if (character_id == "mikuru") {
        reportProgress(user_id, "daily_interact_mikuru", 1);
        reportProgress(user_id, "legend_interact_mikuru", 1);
    } else if (character_id == "yuki") {
        reportProgress(user_id, "daily_view_yuki", 1);
        reportProgress(user_id, "legend_interact_yuki", 1);
    } else if (character_id == "haruhi") {
        reportProgress(user_id, "legend_interact_haruhi", 1);
    } else if (character_id == "koizumi") {
        reportProgress(user_id, "legend_interact_koizumi", 1);
    } else if (character_id == "kyon") {
        reportProgress(user_id, "legend_interact_kyon", 1);
    }
    reportProgress(user_id, "weekly_interact_all", 1);
}

void QuestService::unlockDependentQuests(int64_t user_id, int completed_quest_id) {
    auto all_quests = db_.get_all_quests();
    for (const auto& q : all_quests) {
        int unlock_id = q.value("unlock_quest_id", 0);
        if (unlock_id == completed_quest_id) {
            int qid = q.value("id", -1);
            db_.unlock_quest(user_id, qid);
        }
    }
}
