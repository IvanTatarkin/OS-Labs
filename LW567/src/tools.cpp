#include "tools.h"

#include <iostream>
#include <string>
#include <cstring>
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Глобальный контекст ZeroMQ (родительский)
zmq::context_t globalContext(1);

// Глобальный корень дерева
std::shared_ptr<Node> root = nullptr;

/*
 * Конструктор узла:
 *   - инициализируем сокет ZeroMQ (req)
 *   - подключаемся к tcp://127.0.0.1:(5555 + id)
 */
Node::Node(int id, int pid)
    : id(id),
      pid(pid),
      socket(globalContext, zmq::socket_type::req)
{
    socket.set(zmq::sockopt::rcvtimeo, 2000); // ждать ответ до 2с
    socket.set(zmq::sockopt::linger, 0);
    socket.connect("tcp://127.0.0.1:" + std::to_string(5555 + id));
}

// Рекурсивный поиск узла по id
std::shared_ptr<Node> FindNode(std::shared_ptr<Node> root, int id) {
    if (!root) {
        return nullptr;
    }
    if (root->id == id) {
        return root;
    }
    if (id < root->id) {
        return FindNode(root->left, id);
    } else {
        return FindNode(root->right, id);
    }
}

// Рекурсивная вставка узла
bool InsertNode(std::shared_ptr<Node>& root, int id, int pid) {
    if (!root) {
        try {
            root = std::make_shared<Node>(id, pid);
        } catch (zmq::error_t&) {
            return false;
        }
        return true;
    }
    if (id == root->id) {
        return false; // уже есть
    }
    if (id < root->id) {
        return InsertNode(root->left, id, pid);
    } else {
        return InsertNode(root->right, id, pid);
    }
}

// Рекурсивная функция "ping" для всех узлов
void PingNodes(const std::shared_ptr<Node>& node, std::unordered_set<int> &unavailable_nodes) {
    if (!node) {
        return;
    }

    try {
        // Создаем message_t со строкой "ping"
        static const char pingData[] = "ping";
        zmq::message_t message(pingData, sizeof(pingData) - 1);

        node->socket.send(message, zmq::send_flags::none);

        zmq::message_t reply;
        // recv(...) возвращает std::optional<size_t>
        auto recvRes = node->socket.recv(reply, zmq::recv_flags::none);

        if (!recvRes.has_value()) {
            // нет ответа => узел недоступен
            unavailable_nodes.insert(node->id);
        } else {
            // проверяем строку
            std::string replyStr = reply.to_string();
            if (replyStr != "Ok") {
                // не "Ok"
                unavailable_nodes.insert(node->id);
            }
        }
    } catch (zmq::error_t&) {
        // Ошибка сокета => узел недоступен
        unavailable_nodes.insert(node->id);
    }

    // Рекурсивно обходим left, right
    PingNodes(node->left, unavailable_nodes);
    PingNodes(node->right, unavailable_nodes);
}

// Рекурсивное завершение всех узлов
void TerminateNodes(const std::shared_ptr<Node>& node) {
    if (!node) {
        return;
    }

    bool killed = false;

    // Проверяем, жив ли процесс node->pid
    if (waitpid(node->pid, nullptr, WNOHANG) != node->pid) {
        // Процесс еще жив, отправляем "exit"
        static const char exitData[] = "exit";
        zmq::message_t message(exitData, sizeof(exitData) - 1);
        node->socket.send(message, zmq::send_flags::none);

        zmq::message_t reply;
        auto recvRes = node->socket.recv(reply, zmq::recv_flags::none);
        if (!recvRes.has_value() || reply.to_string() != "Ok") {
            // не ответил "Ok" => убьем
            killed = true;
        }
    }

    // Закрываем сокет
    try {
        node->socket.close();
    } catch (const zmq::error_t& e) {
        std::cerr << "Error closing socket for node " << node->id << ": " << e.what() << "\n";
    }

    // Если надо, убиваем процесс
    if (!killed) {
        kill(node->pid, SIGKILL);
        waitpid(node->pid, nullptr, 0);
    }

    // Рекурсивно завершаем левых и правых
    TerminateNodes(node->left);
    TerminateNodes(node->right);
}
