#pragma once

#include <memory>
#include <unordered_set>
#include <string>
#include <zmq.hpp>

extern zmq::context_t globalContext;

struct Node {
    int id;
    int pid;
    zmq::socket_t socket;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    Node(int id, int pid);
};

extern std::shared_ptr<Node> root;

std::shared_ptr<Node> FindNode(std::shared_ptr<Node> root, int id);

bool InsertNode(std::shared_ptr<Node>& root, int id, int pid);

void PingNodes(const std::shared_ptr<Node>& node, std::unordered_set<int> &unavailable_nodes);

void TerminateNodes(const std::shared_ptr<Node>& node);
