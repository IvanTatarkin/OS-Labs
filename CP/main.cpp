#include <iostream>
#include "Server.h"
#include "Client.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Использование: " << argv[0] << " [server|client]\n";
        return 1;
    }

    std::string mode(argv[1]);

    if (mode == "server") {
        run_server();
    } else if (mode == "client") {
        run_client();
    } else {
        std::cerr << "Неверный режим. Используйте 'server' или 'client'.\n";
        return 1;
    }

    return 0;
}