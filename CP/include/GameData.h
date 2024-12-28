#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

struct GameData {
    char player1_login[32];
    char player2_login[32];
    int player1_wins;
    int player1_losses;
    int player2_wins;
    int player2_losses;
    char board1[10][10];
    char board2[10][10];
    int current_turn;
    int game_over;
    int players_ready;
    int game_started;
    int player1_ships_placed;
    int player2_ships_placed;
};

extern const char* SHM_NAME;
extern const int SHM_SIZE;

void initialize_board(char board[10][10]);
bool check_win(char board[10][10]);
void print_board(char board[10][10]);

#endif // GAMEDATA_H