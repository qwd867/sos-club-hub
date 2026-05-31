#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "json.hpp"
#include <string>

class HttpResponse {
public:
    static nlohmann::json success(const nlohmann::json& data = {});
    static nlohmann::json error(const std::string& message, int code = 400);
};

#endif
