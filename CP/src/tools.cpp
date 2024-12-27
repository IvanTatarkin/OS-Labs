#include "GameData.h"

// Определяем переменные
const char* SHM_NAME = "/battleship_shm";
const int SHM_SIZE = sizeof(GameData);

// Реализация функций
void initialize_board(char board[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            board[i][j] = '~'; // Вода
        }
    }
}

bool check_win(char board[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (board[i][j] == 'S') { // Если остались корабли
                return false;
            }
        }
    }
    return true;
}

void print_board(char board[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            std::cout << board[i][j] << " ";
        }
        std::cout << std::endl;
    }
}