#include <iostream>
#include <fstream>

using namespace std;

const int MAX_MAP_ROWS = 10;
const int MAX_MAP_COLS = 15;

bool load_map (const char *filepath, char map[]) {
	ifstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	for (int row = 0; row < MAX_MAP_ROWS; row++) {
		for (int col = 0; col < MAX_MAP_COLS; col++) {
			map[row * MAX_MAP_COLS + col] = file.get();
		}
		if (file.get() != '\n') {
			file.close();
			return false;
		}
	}

	file.close();
	return true;
}

void print_map(const char *map) {
	for (int row = 0; row < MAX_MAP_ROWS; row++) {
		for (int col = 0; col < MAX_MAP_COLS; col++) {
			cout << map[row * MAX_MAP_COLS + col];
		}
		cout << endl;
	}
}

int main () {
	char map[MAX_MAP_ROWS * MAX_MAP_COLS];
	if (!load_map("map1.txt", map))
	  return -1;

	print_map(map);

	return 0;
}
