#include "Server.h"
#include "Tools.h"
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

void run_server() {
    // Create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error creating shared memory: " << strerror(errno) << "\n";
        return;
    }

    // Set the size of the shared memory
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        std::cerr << "Error setting shared memory size: " << strerror(errno) << "\n";
        close(shm_fd);
        return;
    }

    // Map the shared memory
    GameData* game_data = (GameData*)mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (game_data == MAP_FAILED) {
        std::cerr << "Error mapping shared memory: " << strerror(errno) << "\n";
        close(shm_fd);
        return;
    }

    // Initialize game data
    memset(game_data, 0, SHM_SIZE);
    initialize_board(game_data->board1);
    initialize_board(game_data->board2);
    game_data->current_turn = 1;
    game_data->game_over = 0;
    game_data->players_ready = 0;
    game_data->game_started = 0;
    game_data->player1_ships_placed = 0;
    game_data->player2_ships_placed = 0;

    std::cout << "Server started. Waiting for players...\n";

    // Main server loop
    while (true) {
        if (strlen(game_data->player1_login) > 0 && strlen(game_data->player2_login) > 0) {
            if (game_data->player1_ships_placed && game_data->player2_ships_placed) {
                if (!game_data->players_ready) {
                    game_data->players_ready = 1;
                    game_data->game_started = 1;
                    std::cout << "Both players are ready. Game started.\n";
                }

                if (game_data->game_over) {
                    std::cout << "Game over. Waiting for players to finish...\n";
                    sleep(5); // Wait for clients to process the game over state
                    std::cout << "Restarting...\n";
                    // Reset game state for a new game
                    memset(game_data, 0, SHM_SIZE);
                    initialize_board(game_data->board1);
                    initialize_board(game_data->board2);
                    game_data->current_turn = 1;
                    game_data->game_over = 0;
                    game_data->players_ready = 0;
                    game_data->player1_ships_placed = 0;
                    game_data->player2_ships_placed = 0;
                } else {
                    // Check for win conditions
                    if (check_win(game_data->board1)) {
                        std::cout << "[DEBUG] Server: Player 2 wins!\n";
                        game_data->game_over = 1;
                        msync(game_data, SHM_SIZE, MS_SYNC); // Sync shared memory
                    } else if (check_win(game_data->board2)) {
                        std::cout << "[DEBUG] Server: Player 1 wins!\n";
                        game_data->game_over = 1;
                        msync(game_data, SHM_SIZE, MS_SYNC); // Sync shared memory
                    }
                }
            } else {
                // Wait for both players to place their ships
                sleep(1);
            }
        } else {
            // Wait for both players to register
            sleep(1);
        }
    }

    // Cleanup
    munmap(game_data, SHM_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);
}