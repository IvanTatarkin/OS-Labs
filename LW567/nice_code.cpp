#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <set>
#include <queue>

// Structure to store node information
struct Node {
    int id;
    int parent_id;
    std::vector<int> children;
    pid_t pid;
    std::chrono::steady_clock::time_point last_heartbeat;
    bool available;
};

// Global variables
std::unordered_map<int, Node> nodes;
std::unordered_map<pid_t, int> pid_to_id;
std::vector<int> dead_nodes; // Updated to store dead node IDs
std::atomic<int> heartbeat_interval_ms(1000);
std::mutex nodes_mutex;
std::atomic<bool> monitoring_active(false);
std::queue<pid_t> child_exit_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;

// Variables storage
std::unordered_map<int, std::unordered_map<std::string, std::string>> node_variables;

// Function prototypes
void start_heartbit_monitoring(int interval_ms);
void ping_node(int id);
void collect_descendants(int id, std::set<int>& descendants);
void mark_descendants_unavailable(int id);
void create_node(int id, int parent_id);
void terminate_descendants(int id);
void terminate_all_nodes();
void print_dead_nodes(); // Declaration added
void exec_command(int id, const std::string& varName, const std::string& value = "");

// Function to recursively collect all descendants of a node
void collect_descendants(int id, std::set<int>& descendants) {
    auto it = nodes.find(id);
    if (it != nodes.end()) {
        for (int child_id : it->second.children) {
            descendants.insert(child_id);
            collect_descendants(child_id, descendants);
        }
    }
}

// Function to mark a node and its descendants as unavailable
void mark_descendants_unavailable(int id) {
    std::set<int> descendants;
    collect_descendants(id, descendants);

    for (int descendant_id : descendants) {
        auto it = nodes.find(descendant_id);
        if (it != nodes.end() && it->second.available) {
            it->second.available = false;
            dead_nodes.push_back(descendant_id); // Update dead_nodes with the ID of the dead node
            //std::cout << "Heartbit: node " << descendant_id << " is unavailable now" << std::endl;
        }
    }

    auto it = nodes.find(id);
    if (it != nodes.end() && it->second.available) {
        it->second.available = false;
        dead_nodes.push_back(id); // Update dead_nodes with the ID of the dead node
        //std::cout << "Heartbit: node " << id << " is unavailable now" << std::endl;
    }
}

// Function to recursively terminate a node and its descendants
void terminate_descendants(int id) {
    std::set<int> descendants;
    collect_descendants(id, descendants);

    for (int descendant_id : descendants) {
        auto it = nodes.find(descendant_id);
        if (it != nodes.end()) {
            kill(it->second.pid, SIGKILL);
            nodes.erase(it);
            pid_to_id.erase(it->second.pid);
        }
    }

    auto it = nodes.find(id);
    if (it != nodes.end()) {
        kill(it->second.pid, SIGKILL);
        nodes.erase(it);
        pid_to_id.erase(it->second.pid);
    }
}

// Function to terminate all nodes
void terminate_all_nodes() {
    std::lock_guard<std::mutex> lock(nodes_mutex);
    for (auto& [id, node] : nodes) {
        kill(node.pid, SIGKILL);
    }
    nodes.clear();
    pid_to_id.clear();
    std::cout << "All nodes terminated." << std::endl;
}

// Signal handler
void signal_handler(int sig, siginfo_t* siginfo, void* context) {
    if (sig == SIGCHLD) {
        pid_t pid;
        int status;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            std::lock_guard<std::mutex> lock(queue_mutex);
            child_exit_queue.push(pid);
            queue_cv.notify_one();
        }
    } else if (sig == SIGUSR1) {
        pid_t sender_pid = siginfo->si_pid;
        std::lock_guard<std::mutex> lock(nodes_mutex);
        auto it = pid_to_id.find(sender_pid);
        if (it != pid_to_id.end()) {
            int node_id = it->second;
            auto node_it = nodes.find(node_id);
            if (node_it != nodes.end()) {
                node_it->second.last_heartbeat = std::chrono::steady_clock::now();
            }
        }
    } else if (sig == SIGINT) {
        std::cout << "SIGINT received. Terminating all nodes..." << std::endl;
        terminate_all_nodes();
        exit(0);
    }
}

// Heartbit monitoring thread function
void monitor_heartbits() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    while (monitoring_active.load()) {
        auto next_wakeup = std::chrono::steady_clock::now() + std::chrono::milliseconds(heartbeat_interval_ms.load());
        queue_cv.wait_until(lock, next_wakeup);

        auto now = std::chrono::steady_clock::now();
        for (auto& [id, node] : nodes) {
            if (node.available) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - node.last_heartbeat).count();
                if (elapsed > 4 * heartbeat_interval_ms.load()) {
                    node.available = false;
                    dead_nodes.push_back(id); // Update dead_nodes with the ID of the dead node
                    // std::cout << "Heartbit: node " << id << " is unavailable now" << std::endl;
                    mark_descendants_unavailable(id);
                }
            }
        }

        // Process exited children
        while (!child_exit_queue.empty()) {
            pid_t pid = child_exit_queue.front();
            child_exit_queue.pop();
            auto it = pid_to_id.find(pid);
            if (it != pid_to_id.end()) {
                int node_id = it->second;
                auto node_it = nodes.find(node_id);
                if (node_it != nodes.end() && node_it->second.available) {
                    node_it->second.available = false;
                    dead_nodes.push_back(node_id); // Update dead_nodes with the ID of the dead node
                    // std::cout << "Heartbit: node " << node_id << " is unavailable now" << std::endl;
                    mark_descendants_unavailable(node_id);
                }
                nodes.erase(node_id);
                pid_to_id.erase(it);
            }
        }
    }
}

// Function to create a node
void create_node(int id, int parent_id) {
    std::lock_guard<std::mutex> lock(nodes_mutex);

    if (nodes.find(id) != nodes.end()) {
        std::cerr << "Error: Node with id " << id << " already exists" << std::endl;
        return;
    }

    Node node;
    node.id = id;
    node.parent_id = parent_id;
    node.available = true;
    node.last_heartbeat = std::chrono::steady_clock::now();

    if (parent_id != -1) {
        auto it = nodes.find(parent_id);
        if (it == nodes.end()) {
            std::cerr << "Error: Parent node " << parent_id << " not found" << std::endl;
            return;
        }
        it->second.children.push_back(id);
    }

    std::cout << "About to fork for node " << id << std::endl;
    pid_t pid = fork();
    if (pid == 0) {
        std::cout << "Child process for node " << id << " started" << std::endl;
        pid_t ppid = getppid();
        while (true) {
            kill(ppid, SIGUSR1);
            std::this_thread::sleep_for(std::chrono::milliseconds(heartbeat_interval_ms.load()));
        }
        exit(0);
    } else if (pid > 0) {
        std::cout << "Parent process, child pid: " << pid << std::endl;
        node.pid = pid;
        nodes[id] = node;
        pid_to_id[pid] = id;
        std::cout << "Ok: " << pid << std::endl;
    } else {
        perror("fork");
    }
}

// Function to start heartbit monitoring
void start_heartbit_monitoring(int interval_ms) {
    heartbeat_interval_ms.store(interval_ms);

    if (monitoring_active.load()) {
        std::cout << "Monitoring already active." << std::endl;
        return;
    }

    monitoring_active.store(true);
    std::thread monitor_thread(monitor_heartbits);
    monitor_thread.detach();
    std::cout << "Ok" << std::endl;
}

// Function to ping a node
void ping_node(int id) {
    std::lock_guard<std::mutex> lock(nodes_mutex);

    auto it = nodes.find(id);
    if (it == nodes.end()) {
        std::cout << "Error: Node " << id << " not found" << std::endl;
        return;
    }

    Node& node = it->second;

    if (node.available && kill(node.pid, 0) == 0) {
        std::cout << "Ok: 1" << std::endl;
    } else {
        if (node.available) {
            node.available = false;
            dead_nodes.push_back(id); // Update dead_nodes with the ID of the dead node
            //std::cout << "Heartbit: node " << id << " is unavailable now" << std::endl;
            mark_descendants_unavailable(id);
        }
        std::cout << "Ok: 0" << std::endl;
    }
}

// Function to print IDs of all dead nodes
void print_dead_nodes() {
    std::lock_guard<std::mutex> lock(nodes_mutex); // Защита от одновременного доступа

    // Обновляем список мертвых узлов
    for (auto& [id, node] : nodes) {
        if (node.available && kill(node.pid, 0) != 0) { // Проверка доступности узла
            node.available = false;
            dead_nodes.push_back(id);
            //std::cout << "Heartbit: node " << id << " is unavailable now" << std::endl;
            mark_descendants_unavailable(id); // Помечаем потомков как недоступных
        }
    }

    // Формируем строку с ID всех мертвых узлов
    std::set<int> unique_dead_nodes(dead_nodes.begin(), dead_nodes.end()); // Убираем дубли
    std::stringstream ss;
    for (auto it = unique_dead_nodes.begin(); it != unique_dead_nodes.end(); ++it) {
        if (it != unique_dead_nodes.begin()) {
            ss << ";";
        }
        ss << *it;
    }

    std::cout << "Ok: " << ss.str() << std::endl;
}

// Function to handle exec command
void exec_command(int id, const std::string& varName, const std::string& value) {
    std::lock_guard<std::mutex> lock(nodes_mutex);

    auto node_it = nodes.find(id);
    if (node_it == nodes.end()) {
        std::cerr << "Error: Node with id " << id << " not found" << std::endl;
        return;
    }

    // Check if the node is available
    if (!node_it->second.available) {
        std::cerr << "Error: Node with id " << id << " is not available" << std::endl;
        return;
    }

    // Set or get the variable
    if (!value.empty()) {
        // Set variable
        node_variables[id][varName] = value;
        std::cout << "Ok" << std::endl;
    } else {
        // Get variable
        auto var_it = node_variables[id].find(varName);
        if (var_it != node_variables[id].end()) {
            std::cout << "Ok: " << var_it->second << std::endl;
        } else {
            std::cerr << "Error: Variable " << varName << " not found for node " << id << std::endl;
        }
    }
}

int main() {
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP | SA_SIGINFO;

    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        perror("sigaction SIGCHLD");
        exit(1);
    }
    if (sigaction(SIGUSR1, &sa, nullptr) == -1) {
        perror("sigaction SIGUSR1");
        exit(1);
    }

    std::string line;
    while (std::cout << "> ", std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "create") {
            int id, parent_id = -1;
            iss >> id;
            if (iss >> parent_id) {
                create_node(id, parent_id);
            } else {
                create_node(id, -1);
            }
        } else if (cmd == "heartbit") {
            int interval;
            iss >> interval;
            start_heartbit_monitoring(interval);
        } else if (cmd == "ping") {
            int id;
            iss >> id;
            ping_node(id);
        } else if (cmd == "pingall") {
            print_dead_nodes(); // Print IDs of all dead nodes
        } else if (cmd == "exec") {
            int id;
            std::string varName, value;
            iss >> id >> varName;
            if (iss >> value) {
                exec_command(id, varName, value);
            } else {
                exec_command(id, varName, "");
            }
        } else {
            std::cerr << "Error: Unknown command" << std::endl;
        }
    }

    terminate_all_nodes();
    return 0;
}