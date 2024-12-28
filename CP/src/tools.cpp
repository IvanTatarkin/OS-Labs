#include "Tools.h"
#include "GameData.h"
#include <iostream>

void place_ships(char board[10][10]) {
    std::cout << "Place your ships.\n";
    for (int i = 0; i < 3; i++) { // Place 3 ships for simplicity
        int x, y;
        std::cout << "Enter coordinates for ship " << i + 1 << " (x y): ";
        std::cin >> x >> y;
        if (x >= 0 && x < 10 && y >= 0 && y < 10) {
            board[x][y] = 'S'; // 'S' represents a ship
        } else {
            std::cout << "Invalid coordinates. Try again.\n";
            i--;
        }
    }
}

void play_game(GameData* game_data, int player_number) {
    while (!game_data->game_over) {
        std::cout << "[DEBUG] Player " << player_number << ": Checking turn. Current turn: " << game_data->current_turn << "\n";

        if (game_data->game_started && game_data->current_turn == player_number) {
            int x, y;
            std::cout << "Your turn. Enter coordinates (x y): ";
            std::cin >> x >> y;

            bool hit = false; // Track whether the player hit a ship

            if (player_number == 1) {
                if (game_data->board2[x][y] == 'S') {
                    std::cout << "Hit!\n";
                    game_data->board2[x][y] = 'X';
                    hit = true; // Player hit a ship
                } else if (game_data->board2[x][y] == 'X' || game_data->board2[x][y] == 'O') {
                    std::cout << "Already hit here.\n";
                } else {
                    std::cout << "Miss!\n";
                    game_data->board2[x][y] = 'O';
                }
            } else if (player_number == 2) {
                if (game_data->board1[x][y] == 'S') {
                    std::cout << "Hit!\n";
                    game_data->board1[x][y] = 'X';
                    hit = true; // Player hit a ship
                } else if (game_data->board1[x][y] == 'X' || game_data->board1[x][y] == 'O') {
                    std::cout << "Already hit here.\n";
                } else {
                    std::cout << "Miss!\n";
                    game_data->board1[x][y] = 'O';
                }
            }

            // Switch turns only if the player missed
            if (!hit) {
                game_data->current_turn = (player_number == 1) ? 2 : 1;
                std::cout << "[DEBUG] Switching turn to Player " << game_data->current_turn << "\n";
            }

            // Check if the game is over
            if (check_win(game_data->board1)) {
                game_data->game_over = 1;
                msync(game_data, SHM_SIZE, MS_SYNC); // Sync shared memory
                std::cout << "[DEBUG] Game over! Player 2 wins.\n";
                if (player_number == 2) {
                    std::cout << "You won!\n";
                } else {
                    std::cout << "You lost!\n";
                }
            } else if (check_win(game_data->board2)) {
                game_data->game_over = 1;
                msync(game_data, SHM_SIZE, MS_SYNC); // Sync shared memory
                std::cout << "[DEBUG] Game over! Player 1 wins.\n";
                if (player_number == 1) {
                    std::cout << "You won!\n";
                } else {
                    std::cout << "You lost!\n";
                }
            }
        }

        // Wait for the other player's turn or game over
        while (game_data->current_turn != player_number && !game_data->game_over) {
            std::cout << "[DEBUG] Player " << player_number << ": Waiting for the other player's move...\n";
            sleep(1);

            // Check if the game is over (for the losing player)
            if (game_data->game_over) {
                std::cout << "[DEBUG] Player " << player_number << ": Detected game over.\n";
                if (player_number == 1 && check_win(game_data->board2)) {
                    std::cout << "You lost!\n";
                } else if (player_number == 2 && check_win(game_data->board1)) {
                    std::cout << "You lost!\n";
                }
                break; // Exit the game loop
            }
        }

        // Exit the game loop if the game is over
        if (game_data->game_over) {
            std::cout << "[DEBUG] Player " << player_number << ": Exiting game loop.\n";
            break;
        }
    }

    // Game over message
    std::cout << "Game over. Thank you for playing!\n";
}