#include "Client.h"
#include "Tools.h"
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

void run_client() {
    // Open the shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error opening shared memory: " << strerror(errno) << "\n";
        return;
    }

    // Map the shared memory
    GameData* game_data = (GameData*)mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (game_data == MAP_FAILED) {
        std::cerr << "Error mapping shared memory: " << strerror(errno) << "\n";
        close(shm_fd);
        return;
    }

    // Register the player
    char login[32];
    std::cout << "Enter your login: ";
    std::cin >> login;

    int player_number = 0;
    if (strlen(game_data->player1_login) == 0) {
        strncpy(game_data->player1_login, login, 32);
        player_number = 1;
        std::cout << "You are registered as Player 1.\n";
    } else if (strlen(game_data->player2_login) == 0) {
        strncpy(game_data->player2_login, login, 32);
        player_number = 2;
        std::cout << "You are registered as Player 2.\n";
    } else {
        std::cout << "Game is full. Try again later.\n";
        munmap(game_data, SHM_SIZE);
        close(shm_fd);
        return;
    }

    // Place ships
    if (player_number == 1) {
        place_ships(game_data->board1);
        game_data->player1_ships_placed = 1;
    } else if (player_number == 2) {
        place_ships(game_data->board2);
        game_data->player2_ships_placed = 1;
    }

    // Wait for the game to start
    while (!game_data->game_started && !game_data->game_over) {
        std::cout << "[DEBUG] Player " << player_number << ": Waiting for the game to start...\n";
        sleep(1);
    }

    // Check if the game is over
    if (game_data->game_over) {
        std::cout << "[DEBUG] Player " << player_number << ": Game over detected while waiting.\n";
        if (player_number == 1 && check_win(game_data->board2)) {
            std::cout << "You won!\n";
        } else if (player_number == 2 && check_win(game_data->board1)) {
            std::cout << "You won!\n";
        } else {
            std::cout << "You lost!\n";
        }
        munmap(game_data, SHM_SIZE);
        close(shm_fd);
        return;
    }

    // Play the game
    play_game(game_data, player_number);

    // Cleanup
    munmap(game_data, SHM_SIZE);
    close(shm_fd);
}