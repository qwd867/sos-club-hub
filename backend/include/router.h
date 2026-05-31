#ifndef ROUTER_H
#define ROUTER_H

#include "auth_service.h"
#include "quest_service.h"
#include "httplib.h"

class Router {
public:
    Router(AuthService& auth_service, QuestService& quest_service);

    void setup(httplib::Server& server);

private:
    AuthService& auth_service_;
    QuestService& quest_service_;

    void handle_register(const httplib::Request& req, httplib::Response& res);
    void handle_login(const httplib::Request& req, httplib::Response& res);
    void handle_forgot_password(const httplib::Request& req,
                                 httplib::Response& res);
    void handle_logout(const httplib::Request& req, httplib::Response& res);
    void handle_demo(const httplib::Request& req, httplib::Response& res);

    void handle_get_quests(const httplib::Request& req, httplib::Response& res);
    void handle_report_progress(const httplib::Request& req, httplib::Response& res);
    void handle_claim_reward(const httplib::Request& req, httplib::Response& res);
    void handle_get_points(const httplib::Request& req, httplib::Response& res);

    static void set_json_response(httplib::Response& res,
                                   const std::string& json_str);
    static bool extract_token(const httplib::Request& req,
                               std::string& token);
    static bool authenticate(const httplib::Request& req, httplib::Response& res, int64_t& user_id);
};

#endif
