#include <iostream>
#include <fstream>

using namespace std;

const int MAX_MAP_ROWS = 10;
const int MAX_MAP_COLS = 15;

struct Game {
	char map[MAX_MAP_ROWS * MAX_MAP_COLS];
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
			char c = file.get();
			game->map[row * MAX_MAP_COLS + col] = c;
			if (c == '@') {
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

void print_map(Game *game) {
	for (int row = 0; row < MAX_MAP_ROWS; row++) {
		for (int col = 0; col < MAX_MAP_COLS; col++) {
			if (game->player_x == col && game->player_y == row) {
				cout << '@';
			}
			else {
				char c = game->map[row * MAX_MAP_COLS + col];
				if (c == '@') {
					cout << ' ';
				}
				else {
					cout << c;
				}
			}
		}
		cout << endl;
	}
}

void move_player(Game *game) {
	char input;
	cin >> input;

	if (input == 'a' && game->player_x > 0) {
		game->player_x--;
	}
	else if (input == 'd' && game->player_x < MAX_MAP_COLS - 1) {
		game->player_x++;
	}
	else if (input == 'w' && game->player_y > 0) {
		game->player_y--;
	}
	else if (input == 's' && game->player_y < MAX_MAP_ROWS - 1) {
		game->player_y++;
	}
}

int main () {
	Game game = {};
	if (!load_map(&game, "map1.txt"))
	  return -1;

	while (true) {
		print_map(&game);
		move_player(&game);
	}

	return 0;
}
