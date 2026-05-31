#ifndef SERVER_H
#define SERVER_H

#include "auth_service.h"
#include "database_manager.h"
#include "quest_service.h"
#include "router.h"
#include "httplib.h"
#include <string>

class Server {
public:
    Server(const std::string& host, int port);

    bool start();
    void stop();

private:
    std::string host_;
    int port_;
    httplib::Server svr_;
    DatabaseManager& db_;
    AuthService auth_service_;
    QuestService quest_service_;
    Router router_;
};

#endif
