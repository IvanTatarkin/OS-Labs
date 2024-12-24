#pragma once

#include <memory>
#include <unordered_set>
#include <string>
#include <zmq.hpp>

/*
 * Глобальный контекст ZeroMQ, используемый родительским процессом
 * для взаимодействия с дочерними (через сокеты).
 */
extern zmq::context_t globalContext;

/*
 * Структура Node хранит:
 *  - id: уникальный идентификатор узла
 *  - pid: PID запущенного воркера (дочерний процесс)
 *  - socket: ZeroMQ (req) для общения с воркером
 *  - left, right: указатели на левый и правый узел (бинарное дерево)
 */
struct Node {
    int id;
    int pid;
    zmq::socket_t socket;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    Node(int id, int pid);
};

/*
 * Глобальный корень дерева.
 */
extern std::shared_ptr<Node> root;

/*
 * Поиск узла по id (рекурсивно). Возвращает узел или nullptr.
 */
std::shared_ptr<Node> FindNode(std::shared_ptr<Node> root, int id);

/*
 * Вставка узла (id, pid) в дерево.
 * Возвращает false, если узел с таким id уже существует.
 */
bool InsertNode(std::shared_ptr<Node>& root, int id, int pid);

/*
 * Отправить "ping" всем узлам в дереве.
 * Узлы, которые не ответили "Ok", попадают в множество unavailable_nodes.
 */
void PingNodes(const std::shared_ptr<Node>& node, std::unordered_set<int> &unavailable_nodes);

/*
 * Корректно завершить все узлы:
 *   - отправить "exit"
 *   - если узел не ответил "Ok", убить SIGKILL
 *   - дождаться завершения процессов (waitpid)
 */
void TerminateNodes(const std::shared_ptr<Node>& node);
