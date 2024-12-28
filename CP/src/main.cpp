#include <iostream>
#include "Client.h"
#include "Server.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <server|client>\n";
        return 1;
    }

    std::string mode(argv[1]);
    if (mode == "server") {
        run_server();
    } else if (mode == "client") {
        run_client();
    } else {
        std::cerr << "Invalid mode. Use 'server' or 'client'.\n";
        return 1;
    }

    return 0;
}