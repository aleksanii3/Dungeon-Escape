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
	int has_key;
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

void print_key(bool has_key) {
	if (has_key) {
		cout << "Key found" << endl;
	}
	else {
		cout << "Key not found" << endl;
	}
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

void find_next_portal(const Game *game, int new_x, int new_y, int& portal_x, int& portal_y) {
	int first_portal_x = -1;
	int first_portal_y = -1;
	bool after_first_portal = true;
	bool after_current_portal = false;

	for (int row = 0; row < MAX_MAP_ROWS; row++) {
		for (int col = 0; col < MAX_MAP_COLS; col++) {
			if (game->map[row * MAX_MAP_COLS + col] == '%') {
				if (after_first_portal) {
					after_first_portal = false;
					first_portal_x = col;
					first_portal_y = row;
				}

				if (col == new_x && row == new_y) {
					after_current_portal = true;
				}
				else if (after_current_portal) {
					portal_x = col;
					portal_y = row;
					return;
				}
			}
		}
	}

	portal_x = first_portal_x;
	portal_y = first_portal_y;
}

void move_player(Game *game) {
	char input;
	cin >> input;

	int new_x = game->player_x;
	int new_y = game->player_y;

	if ((input == 'a' || input == 'A') && new_x > 0) {
		new_x--;
	}
	else if ((input == 'd' || input == 'D') && new_x < MAX_MAP_COLS - 1) {
		new_x++;
	}
	else if ((input == 'w' || input == 'W') && new_y > 0) {
		new_y--;
	}
	else if ((input == 's' || input == 'S') && new_y < MAX_MAP_ROWS - 1) {
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
	else if (tile == '&') {
		game->has_key = true;
		game->map[new_y * MAX_MAP_COLS + new_x] = ' ';
	}
	else if (tile == '%') {
		int portal_x, portal_y;
		find_next_portal(game, new_x, new_y, portal_x, portal_y);
		new_x = portal_x;
		new_y = portal_y;
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
		print_key(game.has_key);
		print_map(&game);

		if (game.lives <= 0) {
			cout << "Game Over!" << endl;
			break;
		}

		move_player(&game);
	}

	return 0;
}
