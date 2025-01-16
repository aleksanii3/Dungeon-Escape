#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

const int MAX_MAP_ROWS = 10;
const int MAX_MAP_COLS = 15;

struct Game {
	char map[MAX_MAP_ROWS * MAX_MAP_COLS];
	int lives;
	int coins;
	int player_x;
	int player_y;
};

bool load_map (Game *game, const char *filepath) {
	ifstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	for (int row = 0; row < MAX_MAP_ROWS; row++) {
		for (int col = 0; col < MAX_MAP_COLS; col++) {
			char tile = file.get();
			game->map[row * MAX_MAP_COLS + col] = tile;
			if (tile == '@') {
				game->player_x = col;
				game->player_y = row;
			}
		}
		if (file.get() != '\n') {
			file.close();
			return false;
		}
	}

	file.close();
	return true;
}

void print_lives(int lives) {
	cout << "Lives: ";
	for (int i = 0; i < lives; i++) {
		cout << '*';
	}
	cout << endl;
}

void print_coins(int coins) {
	cout << "Coins: " << coins << endl;
}

void print_map(Game *game) {
	for (int row = 0; row < MAX_MAP_ROWS; row++) {
		for (int col = 0; col < MAX_MAP_COLS; col++) {
			if (game->player_x == col && game->player_y == row) {
				cout << '@';
			}
			else {
				char tile = game->map[row * MAX_MAP_COLS + col];
				if (tile == '@') {
					cout << ' ';
				}
				else {
					cout << tile;
				}
			}
		}
		cout << endl;
	}
}

void move_player(Game *game) {
	char input;
	cin >> input;

	int new_x = game->player_x;
	int new_y = game->player_y;

	if (input == 'a' && new_x > 0) {
		new_x--;
	}
	else if (input == 'd' && new_x < MAX_MAP_COLS - 1) {
		new_x++;
	}
	else if (input == 'w' && new_y > 0) {
		new_y--;
	}
	else if (input == 's' && new_y < MAX_MAP_ROWS - 1) {
		new_y++;
	}

	char tile = game->map[new_y * MAX_MAP_COLS + new_x];
	if (tile == '#') {
		game->lives--;
		new_x = game->player_x;
		new_y = game->player_y;
	}
	else if (tile == 'C') {
		game->coins++;
		game->map[new_y * MAX_MAP_COLS + new_x] = ' ';
	}

	game->player_x = new_x;
	game->player_y = new_y;
}

int main () {
	Game game = {};
	game.lives = 3;

	if (!load_map(&game, "map1.txt"))
	  return -1;

	while (true) {
		system("cls");
		print_lives(game.lives);
		print_coins(game.coins);
		print_map(&game);

		if (game.lives <= 0) {
			cout << "Game Over!" << endl;
			break;
		}

		move_player(&game);
	}

	return 0;
}
