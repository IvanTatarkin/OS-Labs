#include "GameData.h"
#include <iostream> // Add this line

const char* SHM_NAME = "/battleship_shm";
const int SHM_SIZE = sizeof(GameData);

void initialize_board(char board[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            board[i][j] = '~'; // Water
        }
    }
}

bool check_win(char board[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (board[i][j] == 'S') { // If there are still ships
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