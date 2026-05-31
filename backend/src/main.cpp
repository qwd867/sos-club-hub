#include "server.h"
#include <csignal>

Server* g_server = nullptr;

void signal_handler(int sig) {
    (void)sig;
    if (g_server) {
        g_server->stop();
    }
}

int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    Server server("0.0.0.0", 8080);
    g_server = &server;

    if (!server.start()) {
        return 1;
    }
    return 0;
}
