#include "Client.h"
#include <iostream>

void place_ships(char board[10][10]) {
    std::cout << "Разместите свои корабли.\n";
    for (int i = 0; i < 3; i++) { // Размещаем 3 корабля для примера
        int x, y;
        std::cout << "Введите координаты для корабля " << i + 1 << " (x y): ";
        std::cin >> x >> y;
        if (x >= 0 && x < 10 && y >= 0 && y < 10) {
            board[x][y] = 'S'; // 'S' обозначает корабль
        } else {
            std::cout << "Неверные координаты. Попробуйте снова.\n";
            i--;
        }
    }
}

void play_game(GameData* game_data, int player_number) {
    while (!game_data->game_over) {
        if (game_data->current_turn == player_number) {
            // Ход игрока
            int x, y;
            std::cout << "Ваш ход. Введите координаты (x y): ";
            std::cin >> x >> y;

            if (player_number == 1) {
                if (game_data->board2[x][y] == 'S') {
                    std::cout << "Попадание!\n";
                    game_data->board2[x][y] = 'X';
                } else {
                    std::cout << "Промах!\n";
                    game_data->board2[x][y] = 'O';
                }
            } else if (player_number == 2) {
                if (game_data->board1[x][y] == 'S') {
                    std::cout << "Попадание!\n";
                    game_data->board1[x][y] = 'X';
                } else {
                    std::cout << "Промах!\n";
                    game_data->board1[x][y] = 'O';
                }
            }

            game_data->current_turn = (player_number == 1) ? 2 : 1;
        } else {
            std::cout << "Ожидание хода другого игрока...\n";
        }

        sleep(1); // Ожидание
    }
}

void run_client() {
    // Открытие memory-mapped файла
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Ошибка открытия memory-mapped файла\n";
        return;
    }

    // Отображение файла в память
    GameData* game_data = (GameData*)mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (game_data == MAP_FAILED) {
        std::cerr << "Ошибка отображения файла в память\n";
        return;
    }

    // Ввод логина
    char login[32];
    std::cout << "Введите ваш логин: ";
    std::cin >> login;

    // Регистрация игрока
    int player_number = 0;
    if (strlen(game_data->player1_login) == 0) {
        strncpy(game_data->player1_login, login, 32);
        player_number = 1;
        std::cout << "Вы зарегистрированы как игрок 1.\n";
    } else if (strlen(game_data->player2_login) == 0) {
        strncpy(game_data->player2_login, login, 32);
        player_number = 2;
        std::cout << "Вы зарегистрированы как игрок 2.\n";
    } else {
        std::cout << "Игра уже начата. Попробуйте позже.\n";
        return;
    }

    // Размещение кораблей
    if (player_number == 1) {
        place_ships(game_data->board1);
    } else if (player_number == 2) {
        place_ships(game_data->board2);
    }

    // Установка флага готовности, если оба игрока подключены
    if (strlen(game_data->player1_login) > 0 && strlen(game_data->player2_login) > 0) {
        game_data->players_ready = 1;
    }

    // Основной цикл игры
    play_game(game_data, player_number);

    // Отсоединение memory-mapped файла
    munmap(game_data, SHM_SIZE);
}