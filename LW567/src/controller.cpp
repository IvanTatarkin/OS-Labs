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

void Controller(std::istream &stream, bool test) {
    std::vector<std::future<std::string>> futures;

    while (true) {
        if (!test) {
            std::cout << "command> ";
            std::cout.flush();
        }
        std::string command;
        if (!std::getline(stream, command)) {
            break;
        }

        std::istringstream iss(command);
        std::string cmdType;
        iss >> cmdType;

        if (cmdType == "create") {
            int id;
            iss >> id;
            pid_t pid = fork();
            if (pid == 0) {
                Worker(id);
                exit(0);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            if (!InsertNode(root, id, pid)) {
                std::cout << "Error: Already exists\n";
                kill(pid, SIGKILL);
                continue;
            }
            std::cout << "Ok: " << pid << "\n";
        }
        else if (cmdType == "exec") {
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
                futures.push_back(std::async(std::launch::async, Exec, node, id, oss.str()));
            } catch (std::exception& e) {
                std::cout << "Error:" << id << ": " << e.what() << "\n";
            }
        }
        else if (cmdType == "pingall") {
            std::unordered_set<int> unavailableNodes;
            PingNodes(root, unavailableNodes);

            if (unavailableNodes.empty()) {
                std::cout << "Ok: -1\n";
            } else {
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
            TerminateNodes(root);
            globalContext.close();
            break;
        }
        else {
            std::cout << "Error: Unknown command\n";
        }

        for (auto it = futures.begin(); it != futures.end();) {
            if (it->wait_for(std::chrono::milliseconds(10)) == std::future_status::ready) {
                std::cout << it->get();
                it = futures.erase(it);
            } else {
                ++it;
            }
        }
    }

    for (auto &f : futures) {
        if (f.valid()) {
            std::cout << f.get();
        }
    }
}
