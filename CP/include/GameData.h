#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// Объявляем структуру GameData
struct GameData {
    char player1_login[32];
    char player2_login[32];
    int player1_wins;
    int player1_losses;
    int player2_wins;
    int player2_losses;
    char board1[10][10]; // Поле игрока 1
    char board2[10][10]; // Поле игрока 2
    int current_turn; // 1 или 2
    int game_over; // 0 - игра продолжается, 1 - игра завершена
    int players_ready; // 0 - игроки не готовы, 1 - игроки готовы
};

// Объявляем переменные как extern
extern const char* SHM_NAME;
extern const int SHM_SIZE;

// Прототипы функций
void initialize_board(char board[10][10]);
bool check_win(char board[10][10]);
void print_board(char board[10][10]);

#endif // GAMEDATA_H