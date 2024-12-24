#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "tools.h"

struct SharedMemory {
    std::mutex mutex;
    std::vector<User> users;
    std::vector<Message> messages;
    // Добавьте дополнительные поля для состояния игры
};

// Функция для сервера
void run_server(const std::string& memory_map_path, size_t memory_map_size, std::mutex& print_mutex) {
    std::cout << "Запуск сервера..." << std::endl;
    // Создание или открытие memory-mapped файла
    int fd = shm_open(memory_map_path.c_str(), O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Ошибка при вызове shm_open");
        Tools::safe_print("Не удалось создать memory-mapped файл.", print_mutex);
        return;
    }

    std::cout << "Файл разделяемой памяти открыт с fd: " << fd << std::endl;

    // Установка размера memory-mapped файла
    if (ftruncate(fd, memory_map_size) == -1) {
        perror("Ошибка при вызове ftruncate");
        Tools::safe_print("Не удалось установить размер memory-mapped файла.", print_mutex);
        close(fd);
        return;
    }

    std::cout << "Размер memory-mapped файла установлен: " << memory_map_size << std::endl;

    // Отображение файла в память
    void* ptr = mmap(nullptr, memory_map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Ошибка при вызове mmap");
        Tools::safe_print("Не удалось отобразить memory-mapped файл.", print_mutex);
        close(fd);
        return;
    }

    std::cout << "Memory-mapped файл успешно отображен в память." << std::endl;

    // Инициализация общей структуры
    SharedMemory* shared = new(ptr) SharedMemory();
    std::cout << "Инициализация общей памяти прошла успешно." << std::endl;

    Tools::safe_print("Сервер запущен и готов принимать клиентов.", print_mutex);

    // Основной цикл сервера
    while (true) {
        // Обработка сообщений от клиентов
        {
            std::lock_guard<std::mutex> lock(shared->mutex);
            // Логика обработки сообщений
        }

        // Пауза для предотвращения высокой загрузки CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Очистка
    munmap(ptr, memory_map_size);
    close(fd);
    shm_unlink(memory_map_path.c_str());
    std::cout << "Сервер завершил работу." << std::endl;
}

// Функция для клиента
void run_client(const std::string& memory_map_path, size_t memory_map_size, std::mutex& print_mutex) {
    std::cout << "Запуск клиента..." << std::endl;
    // Открытие существующего memory-mapped файла
    int fd = shm_open(memory_map_path.c_str(), O_RDWR, 0666);
    if (fd == -1) {
        perror("Ошибка при вызове shm_open (клиент)");
        Tools::safe_print("Не удалось открыть memory-mapped файл. Убедитесь, что сервер запущен.", print_mutex);
        return;
    }

    std::cout << "Файл разделяемой памяти открыт с fd: " << fd << std::endl;

    // Отображение файла в память
    void* ptr = mmap(nullptr, memory_map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Ошибка при вызове mmap (клиент)");
        Tools::safe_print("Не удалось отобразить memory-mapped файл.", print_mutex);
        close(fd);
        return;
    }

    std::cout << "Memory-mapped файл успешно отображен в память." << std::endl;

    SharedMemory* shared = static_cast<SharedMemory*>(ptr);

    // Аутентификация пользователя
    std::string username;
    std::cout << "Введите ваш логин: ";
    std::cin >> username;

    {
        std::lock_guard<std::mutex> lock(shared->mutex);
        bool user_exists = false;
        for (auto& user : shared->users) {
            if (user.username == username) {
                user_exists = true;
                break;
            }
        }
        if (!user_exists) {
            shared->users.push_back(User{username, 0, 0});
            Tools::safe_print("Пользователь зарегистрирован.", print_mutex);
        } else {
            Tools::safe_print("Пользователь найден. Добро пожаловать!", print_mutex);
        }
    }

    // Основной цикл клиента
    while (true) {
        std::cout << "\nВыберите действие:\n1. Посмотреть статистику\n2. Выйти\n> ";
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            std::lock_guard<std::mutex> lock(shared->mutex);
            for (const auto& user : shared->users) {
                if (user.username == username) {
                    std::cout << "Статистика для " << user.username << ":\n";
                    std::cout << "Победы: " << user.wins << "\nПоражения: " << user.losses << std::endl;
                    break;
                }
            }
        } else if (choice == 2) {
            Tools::safe_print("Выход из клиента.", print_mutex);
            break;
        } else {
            Tools::safe_print("Неверный выбор. Попробуйте снова.", print_mutex);
        }

        // Добавьте здесь дополнительные действия, такие как участие в игре
    }

    // Очистка
    munmap(ptr, memory_map_size);
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " [server|client]" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string memory_map_path;
    size_t memory_map_size;
    int server_port;
    int max_players;

    if (!Tools::load_config("config.json", memory_map_path, memory_map_size, server_port, max_players)) {
        return 1;
    }

    std::cout << "Конфигурация загружена:\n";
    std::cout << "Путь к разделяемой памяти: " << memory_map_path << "\n";
    std::cout << "Размер разделяемой памяти: " << memory_map_size << "\n";
    std::cout << "Порт сервера: " << server_port << "\n";
    std::cout << "Максимальное количество игроков: " << max_players << "\n";

    std::mutex print_mutex;

    if (mode == "server") {
        run_server(memory_map_path, memory_map_size, print_mutex);
    } else if (mode == "client") {
        run_client(memory_map_path, memory_map_size, print_mutex);
    } else {
        std::cerr << "Неверный режим. Используйте 'server' или 'client'." << std::endl;
        return 1;
    }

    return 0;
}
