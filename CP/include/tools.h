#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <vector>
#include <mutex>

// Структура для хранения информации о пользователе
struct User {
    std::string username;
    int wins;
    int losses;
};

// Структура для хранения информации о сообщении
struct Message {
    std::string sender;
    std::string content;
};

// Вспомогательные функции
namespace Tools {
    // Функция для загрузки конфигурации из JSON файла
    bool load_config(const std::string& filename, std::string& memory_map_path, size_t& memory_map_size, int& server_port, int& max_players);

    // Функция для сохранения статистики пользователей в JSON файл
    bool save_statistics(const std::string& filename, const std::vector<User>& users);

    // Функция для загрузки статистики пользователей из JSON файла
    bool load_statistics(const std::string& filename, std::vector<User>& users);

    // Функция для безопасного вывода сообщений с использованием mutex
    void safe_print(const std::string& message, std::mutex& print_mutex);
}

#endif // TOOLS_H
