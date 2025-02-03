#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

const int MAX_MAP_ROWS = 10;
const int MAX_MAP_COLS = 15;
const int MAX_NAME = 50;

const char *MAPS[] = {
	"map1.txt",
	"map2.txt",
	"map3.txt",
};
const int MAPS_COUNT = 3;

struct Game {
	char name[MAX_NAME + 1];
	char map[MAX_MAP_ROWS * MAX_MAP_COLS];
	int level;
	int lives;
	int coins;
	int player_x;
	int player_y;
	int has_key;
};

bool compare_strings(const char *a, const char *b) {
	int i = 0;
	while (true) {
		if (a[i] != b[i]) {
			return false;
		}
		if (a[i] == '\0') {
			return true;
		}
		i++;
	}
}

bool save_game(const Game *game) {
	cout << "Saving..." << endl;
	fstream file("players.txt", ios::binary | ios::in | ios::out);
	if (!file.is_open()) {
		file.open("players.txt", ios::binary | ios::in | ios::out | ios::trunc);
	}

	while (true) {
		Game tmp_game;
		file.read((char *) &tmp_game, sizeof(tmp_game));

		if (file.eof()) {
			file.clear();
			break;
		}

		if (compare_strings(tmp_game.name, game->name)) {
			file.seekp(file.tellg() - (streampos) sizeof(tmp_game));
			break;
		}
	}

	file.write((const char *) game, sizeof(*game));
	file.close();
	return true;
}

bool load_game(Game *game) {
	ifstream file("players.txt", std::ios::binary);
	if (!file.is_open()) {
		return false;
	}

	bool loaded = false;
	Game loaded_game;
	while (!file.eof()) {
		file.read((char *) &loaded_game, sizeof(loaded_game));
		if (compare_strings(loaded_game.name, game->name)) {
			loaded = true;
			*game = loaded_game;
			break;
		}
	}

	file.close();
	return loaded;
}

bool load_map (Game *game) {
	if (game->level >= MAPS_COUNT) {
		return false;
	}

	const char *filepath = MAPS[game->level];

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

void input_name(Game *game) {
	cin.clear();
	cin.ignore(10000, '\n');
	cout << "Player name:" << endl;
	while (true) {
		cout << "> ";
		cin.getline(game->name, MAX_NAME + 1);
		break;
	}
}

bool show_menu(Game *game) {
	cout << "1. New Player" << endl;
	cout << "2. Load Player" << endl;

	int success = false;

	while (true) {
		int choice;
		cout << "> ";
		cin >> choice;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(10000, '\n');
		}
		else if (choice == 1) {
			*game = {};
			input_name(game);
			if (load_game(game)) {
				cout << "Player name already exists!" << endl;
				success = false;
				break;
			}
			else {
				game->lives = 3;
				save_game(game);
				cout << "Welcome, " << game->name << "!" << endl;
				success = true;
				break;
			}
			break;
		}
		else if (choice == 2) {
			input_name(game);
			if (load_game(game)) {
				cout << "Game loaded!" << endl;
				success = true;
				break;
			}
			else {
				cout << "Player not found!" << endl;
				success = false;
				break;
			}
		}
	}

	return success;
}

void print_level(int level) {
	cout << "Level: " << level + 1 << endl;
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

void print_message(const char *message) {
	cout << message << endl;
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

void move_player(Game *game, bool &finished, const char *&message) {
	message = "";

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
		if (game->lives == 0) {
			cout << "You are dead!" << endl;
			game->has_key = false;
			finished = true;
			save_game(game);
		}
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
	else if (tile == 'X') {
		if (game->has_key) {
			game->has_key = false;
			game->level++;
			if (game->level >= MAPS_COUNT) {
				cout << "You win!" << endl;
				finished = true;
			}
			else {
                message="test";
				save_game(game);
				load_map(game);
				new_x = game->player_x;
				new_y = game->player_y;
			}
		}
		else {
			message = "You must find the key first.";
		}
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

int main() {
	Game game = {};

	if (!show_menu(&game)) {
		return -1;
	}

	if (game.lives == 0) {
		cout << "This player is dead!" << endl;
		return 0;
	}

	if (!load_map(&game)) {
		return -1;
	}

	bool finished = false;
	const char *message = "";

	while (!finished) {
		system("cls");
		print_level(game.level);
		print_lives(game.lives);
		print_coins(game.coins);
		print_key(game.has_key);
		print_map(&game);
		print_message(message);
		move_player(&game, finished, message);
	}

	return 0;
}

