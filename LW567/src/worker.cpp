#include <worker.h>

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <sstream>
#include <thread>
#include <zmq.hpp>
#include <cstdlib>
#include <sys/types.h>
#include <csignal>
#include <sys/wait.h>

void Worker(int id) {
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::rep);
    try {
        socket.bind("tcp://127.0.0.1:" + std::to_string(5555 + id));
    } catch (zmq::error_t&) {
        context.close();
        exit(-1);
    }

    std::map<std::string, int> dictionary;

    while (true) {
        zmq::message_t request;
        if (socket.recv(request, zmq::recv_flags::none)) {
            std::string msg(static_cast<char*>(request.data()), request.size());

            std::istringstream iss(msg);
            std::string command;
            iss >> command;

            if (command == "exec") {
                std::string name;
                iss >> name;

                if (!iss.eof()) {
                    int value;
                    iss >> value;
                    dictionary[name] = value;
                    socket.send(zmq::buffer("Ok:" + std::to_string(id)), zmq::send_flags::none);
                } else {
                    auto it = dictionary.find(name);
                    if (it != dictionary.end()) {
                        socket.send(zmq::buffer("Ok:" + std::to_string(id) + ": " + std::to_string(it->second)), zmq::send_flags::none);
                    } else {
                        socket.send(zmq::buffer("Ok:" + std::to_string(id) + ": '" + name + "' not found"), zmq::send_flags::none);
                    }
                }
            } else if (command == "ping") {
                socket.send(zmq::buffer("Ok"), zmq::send_flags::none);
            } else if (command == "exit") {
                socket.send(zmq::buffer("Ok"), zmq::send_flags::none);
                socket.close();
                context.close();
                exit(0);
            } else {
                socket.send(zmq::buffer("!" + command + "!"), zmq::send_flags::none);
            }
        }
    }
}