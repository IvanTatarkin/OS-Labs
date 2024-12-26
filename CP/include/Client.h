#ifndef CLIENT_H
#define CLIENT_H

#include "GameData.h"

void run_client();
void place_ships(char board[10][10]);
void play_game(GameData* game_data, int player_number);

#endif // CLIENT_H