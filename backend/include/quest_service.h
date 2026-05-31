#ifndef QUEST_SERVICE_H
#define QUEST_SERVICE_H

#include "database_manager.h"
#include "json.hpp"
#include <string>
#include <vector>

struct BrainstormTask {
    std::string id;
    std::string title;
    std::string description;
    int target;
    std::string check_type;
};

class QuestService {
public:
    explicit QuestService(DatabaseManager& db);

    nlohmann::json getUserQuests(int64_t user_id);
    nlohmann::json reportProgress(int64_t user_id, const std::string& quest_key, int increment);
    nlohmann::json claimReward(int64_t user_id, int quest_id);
    nlohmann::json getUserPoints(int64_t user_id);

    void onUserCheckin(int64_t user_id);
    void onUserInteract(int64_t user_id, const std::string& character_id);

private:
    DatabaseManager& db_;
    std::vector<BrainstormTask> brainstorm_tasks_;

    void initializeBrainstormTasks();
    void refreshDailyQuests(int64_t user_id);
    void refreshWeeklyQuests(int64_t user_id);
    bool needsDailyRefresh(int64_t user_id);
    bool needsWeeklyRefresh(int64_t user_id);
    BrainstormTask pickBrainstormTask(int64_t user_id);
    std::string getCurrentDateStr();
    std::string getCurrentWeekStr();
    void unlockDependentQuests(int64_t user_id, int completed_quest_id);
    int findQuestIdByKey(const std::string& quest_key);
};

#endif
