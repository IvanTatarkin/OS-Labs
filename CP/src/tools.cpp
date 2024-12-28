#include "Tools.h"
#include "GameData.h"
#include <iostream>

#include <iostream>
#include <vector>

bool can_place_ship(char board[10][10], int x, int y, int size, bool horizontal) {
    if (horizontal) {
        if (y + size > 10) return false;
        for (int i = x - 1; i <= x + 1; ++i) {
            for (int j = y - 1; j <= y + size; ++j) {
                if (i >= 0 && i < 10 && j >= 0 && j < 10 && board[i][j] == 'S') {
                    return false;
                }
            }
        }
    } else {
        if (x + size > 10) return false;
        for (int i = x - 1; i <= x + size; ++i) {
            for (int j = y - 1; j <= y + 1; ++j) {
                if (i >= 0 && i < 10 && j >= 0 && j < 10 && board[i][j] == 'S') {
                    return false;
                }
            }
        }
    }
    return true;
}

void place_ship(char board[10][10], int x, int y, int size, bool horizontal) {
    if (horizontal) {
        for (int j = y; j < y + size; ++j) {
            board[x][j] = 'S';
        }
    } else {
        for (int i = x; i < x + size; ++i) {
            board[i][y] = 'S';
        }
    }
}

#include <limits>

void place_ships(char board[10][10]) {
    std::vector<std::pair<int, int>> ships = {
        {4, 1},
        {3, 2},
        {2, 3},
        {1, 4}
    };

    for (const auto& ship : ships) {
        int size = ship.first;
        int count = ship.second;

        for (int i = 0; i < count; ++i) {
            bool placed = false;
            while (!placed) {
                std::cout << "Текущее поле:\n";
                print_board(board);

                int x, y;
                bool horizontal;
                std::cout << "Размещение " << size << "-палубного корабля (осталось " << count - i << ")\n";
                std::cout << "Введите координаты (x y) и ориентацию (0 - вертикально, 1 - горизонтально): ";

                if (!(std::cin >> x >> y >> horizontal)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Некорректный ввод. Пожалуйста, введите числа.\n";
                    continue;
                }

                if (x >= 0 && x < 10 && y >= 0 && y < 10 && can_place_ship(board, x, y, size, horizontal)) {
                    place_ship(board, x, y, size, horizontal);
                    placed = true;

                    std::cout << "Корабль размещен. Текущее поле:\n";
                    print_board(board);
                } else {
                    std::cout << "Невозможно разместить корабль здесь. Попробуйте снова.\n";
                    std::cout << "Проверьте, что:\n";
                    std::cout << "1. Координаты находятся в пределах 0-9.\n";
                    std::cout << "2. Корабль не выходит за границы поля.\n";
                    std::cout << "3. Корабль не касается других кораблей.\n";
                }
            }
        }
    }

    std::cout << "Все корабли размещены.\n";
}


bool is_ship_destroyed(char board[10][10], int x, int y) {
    for (int j = y; j >= 0 && board[x][j] != '~'; --j) {
        if (board[x][j] == 'S') return false;
    }
    for (int j = y + 1; j < 10 && board[x][j] != '~'; ++j) {
        if (board[x][j] == 'S') return false;
    }

    for (int i = x; i >= 0 && board[i][y] != '~'; --i) {
        if (board[i][y] == 'S') return false;
    }
    for (int i = x + 1; i < 10 && board[i][y] != '~'; ++i) {
        if (board[i][y] == 'S') return false;
    }

    return true;
}


void play_game(GameData* game_data, int player_number) {
    char (*my_board)[10] = (player_number == 1) ? game_data->board1 : game_data->board2;
    char (*opponent_board)[10] = (player_number == 1) ? game_data->board2 : game_data->board1;

    while (!game_data->game_over) {
        if (game_data->current_turn == player_number) {
            std::cout << "Ваш ход.\n";

            std::cout << "Поле соперника:\n";
            for (int i = 0; i < 10; ++i) {
                for (int j = 0; j < 10; ++j) {
                    if (opponent_board[i][j] == 'X' || opponent_board[i][j] == 'O') {
                        std::cout << opponent_board[i][j] << " ";
                    } else {
                        std::cout << "~ ";
                    }
                }
                std::cout << "\n";
            }

            int x, y;
            std::cout << "Введите координаты (x y): ";
            std::cin >> x >> y;

            if (x >= 0 && x < 10 && y >= 0 && y < 10) {
                if (opponent_board[x][y] == 'S') {
                    std::cout << "Попадание!\n";
                    opponent_board[x][y] = 'X';

                    if (is_ship_destroyed(opponent_board, x, y)) {
                        std::cout << "Корабль противника уничтожен!\n";
                    }

                } else if (opponent_board[x][y] == 'X' || opponent_board[x][y] == 'O') {
                    std::cout << "Вы уже стреляли сюда.\n";
                } else {
                    std::cout << "Промах!\n";
                    opponent_board[x][y] = 'O';

                    game_data->current_turn = (player_number == 1) ? 2 : 1;
                }
            } else {
                std::cout << "Некорректные координаты. Попробуйте снова.\n";
            }
        } else {
            std::cout << "Ход противника. Ожидайте...\n";
            sleep(1);
        }

        if (check_win(opponent_board)) {
            game_data->game_over = 1;
            std::cout << "Игра окончена! Вы победили!\n";
        } else if (check_win(my_board)) {
            game_data->game_over = 1;
            std::cout << "Игра окончена! Вы проиграли!\n";
        }
    }
}