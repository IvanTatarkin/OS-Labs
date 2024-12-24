#include "controller.h"
#include "tools.h"
#include "worker.h"

#include <iostream>
#include <string>
#include <unordered_set>
#include <memory>
#include <sstream>
#include <thread>
#include <zmq.hpp>
#include <cstdlib>
#include <sys/types.h>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>
#include <unordered_map>
#include <future>
#include <vector>
#include <chrono>

/*
 * Асинхронная функция Exec(...) для выполнения "exec <name> [value]".
 * Отправляет сообщение воркеру, ждёт ответ.
 */
std::string Exec(const std::shared_ptr<Node>& node, int id, const std::string& command_str) {
    std::string result;
    try {
        zmq::message_t message(command_str.data(), command_str.size());
        node->socket.send(message, zmq::send_flags::none);

        zmq::message_t reply;
        auto recvRes = node->socket.recv(reply, zmq::recv_flags::none);

        if (!recvRes.has_value()) {
            result = "Error:" + std::to_string(id) + ": No response\n";
        } else {
            result = reply.to_string() + "\n";
        }
    } catch (zmq::error_t&) {
        result = "Error:" + std::to_string(id) + ": Node is unavailable\n";
    } catch (std::exception& e) {
        result = "Error:" + std::to_string(id) + ": " + e.what() + "\n";
    }
    return result;
}

/*
 * Controller:
 *   - create <id>
 *   - exec <id> <name> [value]
 *   - pingall
 *   - exit
 */
void Controller(std::istream &stream, bool test) {
    std::vector<std::future<std::string>> futures;

    while (true) {
        if (!test) {
            std::cout << "command> ";
            std::cout.flush();
        }
        std::string command;
        if (!std::getline(stream, command)) {
            // EOF или ошибка чтения
            break;
        }

        std::istringstream iss(command);
        std::string cmdType;
        iss >> cmdType;

        if (cmdType == "create") {
            // create <id>
            int id;
            iss >> id;
            pid_t pid = fork();
            if (pid == 0) {
                // Дочерний
                Worker(id);
                exit(0);
            }
            // Родитель: ДАЁМ ВРЕМЯ воркеру, чтобы сделать bind(...) и быть готовым
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            if (!InsertNode(root, id, pid)) {
                std::cout << "Error: Already exists\n";
                kill(pid, SIGKILL);
                continue;
            }
            std::cout << "Ok: " << pid << "\n";
        }
        else if (cmdType == "exec") {
            // exec <id> <name> [value]
            int id;
            iss >> id;

            auto node = FindNode(root, id);
            if (!node) {
                std::cout << "Error:" << id << ": Not found\n";
                continue;
            }

            std::ostringstream oss;
            oss << "exec ";
            std::string name;
            iss >> name;
            oss << name;

            if (!iss.eof()) {
                int value;
                iss >> value;
                oss << " " << value;
            }

            try {
                // Асинхронный вызов
                futures.push_back(std::async(std::launch::async, Exec, node, id, oss.str()));
            } catch (std::exception& e) {
                std::cout << "Error:" << id << ": " << e.what() << "\n";
            }
        }
        else if (cmdType == "pingall") {
            // Проверяем доступность узлов
            std::unordered_set<int> unavailableNodes;
            PingNodes(root, unavailableNodes);

            if (unavailableNodes.empty()) {
                // все узлы ответили Ok
                std::cout << "Ok: -1\n";
            } else {
                // выводим список недоступных
                auto it = unavailableNodes.begin();
                std::cout << "Ok: " << *it;
                ++it;
                for (; it != unavailableNodes.end(); ++it) {
                    std::cout << ";" << *it;
                }
                std::cout << "\n";
            }
        }
        else if (cmdType == "exit") {
            // Завершаем все узлы
            TerminateNodes(root);
            globalContext.close();
            break;
        }
        else {
            std::cout << "Error: Unknown command\n";
        }

        // Проверяем, какие из Exec уже завершились
        for (auto it = futures.begin(); it != futures.end();) {
            if (it->wait_for(std::chrono::milliseconds(10)) == std::future_status::ready) {
                std::cout << it->get();
                it = futures.erase(it);
            } else {
                ++it;
            }
        }
    }

    // Забираем результаты незавершённых Exec
    for (auto &f : futures) {
        if (f.valid()) {
            std::cout << f.get();
        }
    }
}
