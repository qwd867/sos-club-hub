#include "server.h"
#include <iostream>

Server::Server(const std::string& host, int port)
    : host_(host), port_(port), db_(DatabaseManager::get_instance()),
      auth_service_(db_), quest_service_(db_), router_(auth_service_, quest_service_) {}

bool Server::start() {
    if (!db_.initialize("auth.db")) {
        std::cerr << "Failed to initialize database" << std::endl;
        return false;
    }

    router_.setup(svr_);

    std::cout << "Server starting at " << host_ << ":" << port_ << std::endl;
    return svr_.listen(host_.c_str(), port_);
}

void Server::stop() {
    svr_.stop();
}
