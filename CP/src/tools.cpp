#include "tools.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool Tools::load_config(const std::string& filename, std::string& memory_map_path, size_t& memory_map_size, int& server_port, int& max_players) {
    std::ifstream config_file(filename);
    if (!config_file.is_open()) {
        std::cerr << "Не удалось открыть файл конфигурации: " << filename << std::endl;
        return false;
    }

    json config_json;
    try {
        config_file >> config_json;
        memory_map_path = config_json.at("memory_map_path").get<std::string>();
        memory_map_size = config_json.at("memory_map_size").get<size_t>();
        server_port = config_json.at("server_port").get<int>();
        max_players = config_json.at("max_players").get<int>();
    } catch (json::exception& e) {
        std::cerr << "Ошибка при чтении конфигурации: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool Tools::save_statistics(const std::string& filename, const std::vector<User>& users) {
    json j;
    for (const auto& user : users) {
        j["users"].push_back({
            {"username", user.username},
            {"wins", user.wins},
            {"losses", user.losses}
        });
    }

    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Не удалось открыть файл для сохранения статистики: " << filename << std::endl;
        return false;
    }

    outfile << j.dump(4);
    return true;
}

bool Tools::load_statistics(const std::string& filename, std::vector<User>& users) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Не удалось открыть файл статистики: " << filename << std::endl;
        return false;
    }

    json j;
    try {
        infile >> j;
        for (const auto& item : j.at("users")) {
            User user;
            user.username = item.at("username").get<std::string>();
            user.wins = item.at("wins").get<int>();
            user.losses = item.at("losses").get<int>();
            users.push_back(user);
        }
    } catch (json::exception& e) {
        std::cerr << "Ошибка при чтении статистики: " << e.what() << std::endl;
        return false;
    }

    return true;
}

void Tools::safe_print(const std::string& message, std::mutex& print_mutex) {
    std::lock_guard<std::mutex> lock(print_mutex);
    std::cout << message << std::endl;
}
