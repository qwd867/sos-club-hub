#ifndef ROUTER_H
#define ROUTER_H

#include "auth_service.h"
#include "httplib.h"

class Router {
public:
    explicit Router(AuthService& auth_service);

    void setup(httplib::Server& server);

private:
    AuthService& auth_service_;

    void handle_register(const httplib::Request& req, httplib::Response& res);
    void handle_login(const httplib::Request& req, httplib::Response& res);
    void handle_forgot_password(const httplib::Request& req,
                                 httplib::Response& res);
    void handle_logout(const httplib::Request& req, httplib::Response& res);
    void handle_demo(const httplib::Request& req, httplib::Response& res);

    static void set_json_response(httplib::Response& res,
                                   const std::string& json_str);
    static bool extract_token(const httplib::Request& req,
                               std::string& token);
};

#endif
