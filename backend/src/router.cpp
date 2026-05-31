#include "router.h"
#include "http_response.h"
#include <iostream>

Router::Router(AuthService& auth_service) : auth_service_(auth_service) {}

void Router::setup(httplib::Server& server) {
    server.Options(".*", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
    });

    server.Post("/api/auth/register", [this](const httplib::Request& req, httplib::Response& res) {
        handle_register(req, res);
    });

    server.Post("/api/auth/login", [this](const httplib::Request& req, httplib::Response& res) {
        handle_login(req, res);
    });

    server.Post("/api/auth/forgot-password", [this](const httplib::Request& req, httplib::Response& res) {
        handle_forgot_password(req, res);
    });

    server.Post("/api/auth/logout", [this](const httplib::Request& req, httplib::Response& res) {
        handle_logout(req, res);
    });

    server.Get("/api/demo", [this](const httplib::Request& req, httplib::Response& res) {
        handle_demo(req, res);
    });
}

void Router::set_json_response(httplib::Response& res, const std::string& json_str) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    res.set_content(json_str, "application/json");
}

bool Router::extract_token(const httplib::Request& req, std::string& token) {
    auto auth = req.get_header_value("Authorization");
    const std::string prefix = "Bearer ";
    if (auth.find(prefix) == 0) {
        token = auth.substr(prefix.length());
        return true;
    }
    return false;
}

void Router::handle_register(const httplib::Request& req, httplib::Response& res) {
    try {
        auto body = nlohmann::json::parse(req.body);
        auto response = auth_service_.register_user(
            body.value("email", ""),
            body.value("password", ""),
            body.value("username", "")
        );
        set_json_response(res, response.dump());
    } catch (const std::exception& e) {
        set_json_response(res, HttpResponse::error("Invalid request").dump());
    }
}

void Router::handle_login(const httplib::Request& req, httplib::Response& res) {
    try {
        auto body = nlohmann::json::parse(req.body);
        auto response = auth_service_.login(
            body.value("email", ""),
            body.value("password", "")
        );
        set_json_response(res, response.dump());
    } catch (const std::exception& e) {
        set_json_response(res, HttpResponse::error("Invalid request").dump());
    }
}

void Router::handle_forgot_password(const httplib::Request& req, httplib::Response& res) {
    try {
        auto body = nlohmann::json::parse(req.body);
        auto response = auth_service_.forgot_password(
            body.value("email", ""),
            body.value("new_password", "")
        );
        set_json_response(res, response.dump());
    } catch (const std::exception& e) {
        set_json_response(res, HttpResponse::error("Invalid request").dump());
    }
}

void Router::handle_logout(const httplib::Request& req, httplib::Response& res) {
    std::string token;
    extract_token(req, token);
    auto response = auth_service_.logout(token);
    set_json_response(res, response.dump());
}

void Router::handle_demo(const httplib::Request& req, httplib::Response& res) {
    std::string token;
    if (!extract_token(req, token)) {
        res.status = 401;
        set_json_response(res, HttpResponse::error("Unauthorized", 401).dump());
        return;
    }

    int64_t user_id = 0;
    if (!AuthService::verify_token(token, user_id)) {
        res.status = 401;
        set_json_response(res, HttpResponse::error("Invalid token", 401).dump());
        return;
    }

    nlohmann::json data;
    data["message"] = "Welcome to the demo page!";
    data["user_id"] = user_id;
    set_json_response(res, HttpResponse::success(data).dump());
}
