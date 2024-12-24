#include "worker.h"

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <thread>
#include <zmq.hpp>
#include <cstdlib>
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/prctl.h>

void Worker(int id) {
    // Устанавливаем, чтобы воркер получал SIGTERM при смерти родителя (SIGKILL родителя)
    if (prctl(PR_SET_PDEATHSIG, SIGTERM) == -1) {
        std::cerr << "Failed to set PDEATHSIG\n";
        exit(EXIT_FAILURE);
    }
    // Проверяем, не умер ли родитель раньше
    if (getppid() == 1) {
        std::cerr << "Parent died before prctl\n";
        exit(EXIT_FAILURE);
    }

    // Локальный контекст ZeroMQ для воркера
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);

    // Привязываем socket к tcp://127.0.0.1:(5555 + id)
    try {
        socket.bind("tcp://127.0.0.1:" + std::to_string(5555 + id));
    } catch (zmq::error_t&) {
        context.close();
        exit(-1);
    }

    // "словарь" для exec
    std::map<std::string, int> dictionary;

    while (true) {
        zmq::message_t request;
        // Получаем сообщение (std::optional<size_t>)
        auto recvRes = socket.recv(request, zmq::recv_flags::none);
        if (!recvRes.has_value()) {
            // Ошибка/прерывание
            continue;
        }

        // Парсим сообщение
        std::string msg(static_cast<char*>(request.data()), request.size());
        std::istringstream iss(msg);
        std::string command;
        iss >> command;

        if (command == "exec") {
            // exec <name> [value]
            std::string name;
            iss >> name;

            if (!iss.eof()) {
                // запишем в словарь
                int value;
                iss >> value;
                dictionary[name] = value;
                socket.send(zmq::buffer("Ok:" + std::to_string(id)), zmq::send_flags::none);
            } else {
                // ищем name
                auto it = dictionary.find(name);
                if (it != dictionary.end()) {
                    socket.send(
                        zmq::buffer("Ok:" + std::to_string(id) + ": " + std::to_string(it->second)),
                        zmq::send_flags::none
                    );
                } else {
                    socket.send(
                        zmq::buffer("Ok:" + std::to_string(id) + ": '" + name + "' not found"),
                        zmq::send_flags::none
                    );
                }
            }
        }
        else if (command == "ping") {
            // отвечаем "Ok"
            socket.send(zmq::buffer("Ok"), zmq::send_flags::none);
        }
        else if (command == "exit") {
            // завершаемся
            socket.send(zmq::buffer("Ok"), zmq::send_flags::none);
            socket.close();
            context.close();
            exit(0);
        }
        else {
            // неизвестная команда
            socket.send(zmq::buffer("!" + command + "!"), zmq::send_flags::none);
        }
    }
}
