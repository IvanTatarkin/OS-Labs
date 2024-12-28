#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

struct GameData {
    char player1_login[32];
    char player2_login[32];
    int player1_wins;      // Количество побед Player 1
    int player1_losses;    // Количество поражений Player 1
    int player2_wins;      // Количество побед Player 2
    int player2_losses;    // Количество поражений Player 2
    char board1[10][10];   // Поле игрока 1
    char board2[10][10];   // Поле игрока 2
    int current_turn;      // 1 или 2
    int game_over;         // 0 - игра продолжается, 1 - игра завершена
    int players_ready;     // 0 - игроки не готовы, 1 - игроки готовы
    int game_started;      // 0 - игра не началась, 1 - игра началась
    int player1_ships_placed; // Корабли Player 1 размещены
    int player2_ships_placed; // Корабли Player 2 размещены
};
extern const char* SHM_NAME;
extern const int SHM_SIZE;

void initialize_board(char board[10][10]);
bool check_win(char board[10][10]);
void print_board(char board[10][10]);

#endif // GAMEDATA_H