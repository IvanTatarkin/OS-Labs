#include "Server.h"
#include <iostream>

void run_server() {
    // Создание memory-mapped файла
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Ошибка создания memory-mapped файла\n";
        return;
    }

    // Установка размера файла
    ftruncate(shm_fd, SHM_SIZE);

    // Отображение файла в память
    GameData* game_data = (GameData*)mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (game_data == MAP_FAILED) {
        std::cerr << "Ошибка отображения файла в память\n";
        return;
    }

    // Инициализация данных игры
    memset(game_data, 0, SHM_SIZE);
    initialize_board(game_data->board1);
    initialize_board(game_data->board2);
    game_data->current_turn = 1;
    game_data->game_over = 0;
    game_data->players_ready = 0; // Игроки не готовы

    std::cout << "Сервер запущен. Ожидание игроков...\n";

    // Основной цикл сервера
    while (true) {
        if (strlen(game_data->player1_login) > 0 && strlen(game_data->player2_login) > 0) {
            // Оба игрока подключены
            if (game_data->game_over) {
                std::cout << "Игра завершена. Рестарт...\n";
                memset(game_data, 0, SHM_SIZE);
                initialize_board(game_data->board1);
                initialize_board(game_data->board2);
                game_data->current_turn = 1;
                game_data->game_over = 0;
                game_data->players_ready = 0; // Сброс флага готовности
            }

            // Проверка, готовы ли игроки
            if (game_data->players_ready) {
                // Проверка победы
                if (check_win(game_data->board1)) {
                    std::cout << game_data->player2_login << " выиграл!\n";
                    game_data->player2_wins++;
                    game_data->player1_losses++;
                    game_data->game_over = 1;
                } else if (check_win(game_data->board2)) {
                    std::cout << game_data->player1_login << " выиграл!\n";
                    game_data->player1_wins++;
                    game_data->player2_losses++;
                    game_data->game_over = 1;
                }
            }
        }
        sleep(1); // Ожидание
    }

    // Удаление memory-mapped файла
    munmap(game_data, SHM_SIZE);
    shm_unlink(SHM_NAME);
}