#include "http_response.h"

nlohmann::json HttpResponse::success(const nlohmann::json& data) {
    nlohmann::json res;
    res["code"] = 200;
    res["message"] = "success";
    res["data"] = data;
    return res;
}

nlohmann::json HttpResponse::error(const std::string& message, int code) {
    nlohmann::json res;
    res["code"] = code;
    res["message"] = message;
    res["data"] = nullptr;
    return res;
}
