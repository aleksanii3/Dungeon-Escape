#include <iostream>
#include <fstream>
#include <windows.h>
#include <cstdio>
using namespace std;

const int MAX_MAP_ROWS = 10;
const int MAX_MAP_COLS = 15;
const int MAX_NAME = 50;

const char* MAPS[] = {
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

bool compare_strings(const char* a, const char* b) {
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
//отговорна за запазването на текущото състояние на играта във файл
bool save_game(const Game* game) {
    char filename[60];// символен масив (стринг) с дължина 60
    snprintf(filename, sizeof(filename), "%s.txt", game->name);
//snprintf Форматира низ, който съдържа името на играча с разширение .txt.
//Пример: Ако game->name е "Aleks", файлът ще се казва Aleks.txt.
//ofstream - клас за запис на файлове
    ofstream fout(filename); // Отваря се файла за четене.
    if (!fout.is_open()) {
        cout << "Error saving game to " << filename << "!" << endl;
        return false;
    }
//fout Записва следните данни във файла
    fout << game->name << "\n"
        << game->level << "\n"
        << game->lives << "\n"
        << game->coins << "\n"
        << game->has_key << "\n"
        << game->player_x << "\n"
        << game->player_y << "\n";

    for (int i = 0; i < MAX_MAP_ROWS * MAX_MAP_COLS; i++) {
        fout << game->map[i];
    }

    fout.close();
    if (!fout.good()) {// Проверява дали е възникнала грешка по време на записа
        cout << "Error occurred while writing to file!" << endl;
        return false;
    }

    cout << "Game saved successfully at level " << game->level << "!" << endl;
    return true;
}

bool load_game(Game* game) {
    char filename[60];
    snprintf(filename, sizeof(filename), "%s.txt", game->name);

    ifstream fin(filename); // Отваря се файлът за четене (ifstream fin(filename);).
    if (!fin.is_open()) {
        cout << "No saved game found for " << game->name << "!" << endl;
        return false;
    }

    fin.getline(game->name, MAX_NAME); // getline се използва за четене на името
    fin >> game->level >> game->lives >> game->coins >> game->has_key >> game->player_x >> game->player_y;//прочита ги

    // Ensure valid level
    if (game->level < 0 || game->level >= MAPS_COUNT) {
        cout << "Invalid level in save file: " << game->level << ". Resetting to level 0." << endl;
        game->level = 0;
    }

    fin.ignore(10000, '\n');//игнорира до първия нов ред

    for (int i = 0; i < MAX_MAP_ROWS * MAX_MAP_COLS; i++) {
        char c = fin.get();//Чете един символ от файла.
        if (!fin.good()) {//Проверява дали четенето е успешно. Ако не е, затваря файла и връща false.
            fin.close();
            return false;
        }
        game->map[i] = c;//Записва символа в масива game->map на съответната позиция.
    }

    fin.close();
    cout << "Game loaded successfully at level " << game->level << "!" << endl; // Debug message
    return true;
}

bool load_map(Game* game) {
    if (game->level < 0 || game->level >= MAPS_COUNT) {
        cout << "Invalid level in save file! Resetting to level 0." << endl;
        game->level = 0;
    }

    const char* filepath = MAPS[game->level];
    ifstream file(filepath);//Отваря файла за четене.
    if (!file.is_open()) {
        cout << "Failed to load map file: " << filepath << endl;
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
    cout << "Loaded map: " << filepath << endl;//filepath съдържа името на файла с картата за текущото ниво.

    return true;
}

void input_name(Game* game) {
    cin.clear();
    cin.ignore(10000, '\n');
    cout << "Player name:" << endl;
    while (true) {
        cout << "> ";
        cin.getline(game->name, MAX_NAME + 1);
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        break;
    }
}

bool show_menu(Game* game) {
    cout << "1. New Player" << endl;
    cout << "2. Load Player" << endl;

    bool success = false;

    while (true) {
        int choice;
        cout << "> ";
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter 1 or 2." << endl;
            continue;
        }
        else if (choice == 1) {
            *game = {};// Нулиране на данните за играча
            input_name(game);
            // Try loading: if it succeeds, player name already exists
            if (load_game(game)) {
                cout << "Player name already exists!" << endl;
                success = false;
                break;
            }
            else {
                game->level = 0;
                if (!load_map(game)) {
                    cout << "Failed to initialize new game map!" << endl;
                    success = false;
                    break;
                }
                game->lives = 3;
                save_game(game);
                cout << "Welcome, " << game->name << "!" << endl;
                success = true;
                break;
            }
        }
        else if (choice == 2) {
            input_name(game);
            if (load_game(game)) {
                if (!load_map(game)) {
                    cout << "Failed to load map for level " << game->level << ". Resetting to level 0." << endl;
                    game->level = 0;
                    load_map(game);
                }
                success = true;
            }
            else {
                cout << "Player not found or save corrupted!" << endl;
                success = false;
            }
            break;
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

void print_message(const char* message) {
    cout << message << endl;
}

void print_map(Game* game) {
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
                    cout << tile;//Ако текущото поле не е позиция на играча, се извлича символът от картата (tile)
                }
            }
        }
        cout << endl;
    }
}

void find_next_portal(const Game* game, int new_x, int new_y, int& portal_x, int& portal_y) {
    int portals[MAX_MAP_ROWS * MAX_MAP_COLS][2];//Двумерен масив, който съхранява координатите на всички портали на картата.
    int portal_count = 0;

    for (int row = 0; row < MAX_MAP_ROWS; row++) {
        for (int col = 0; col < MAX_MAP_COLS; col++) {
            if (game->map[row * MAX_MAP_COLS + col] == '%') {
                portals[portal_count][0] = col;
                portals[portal_count][1] = row;//Координатите на портала (колона и ред) се записват в масива portals.
                portal_count++;
            }
        }
    }

    for (int i = 0; i < portal_count; i++) {
        if (portals[i][0] == new_x && portals[i][1] == new_y) {//Ако координатите на текущия портал (new_x, new_y) съвпадат с координатите на портал в масива, се изпълнява следният код:
            int next_index = (i + 1) % portal_count;
            portal_x = portals[next_index][0];//portal_x и portal_y се задават да бъдат координатите на следващия портал.
            portal_y = portals[next_index][1];
            return;
        }
    }

    if (portal_count > 0) {//Ако не бъде намерен портал, който съвпада с new_x и new_y, се използва първия портал
        portal_x = portals[0][0];
        portal_y = portals[0][1];
    }
}

void move_player(Game* game, bool& finished, const char*& message) {
    message = "";

    char input;
    cin >> input;

    if (input == 'Q' || input == 'q') {
        cout << "Exiting and saving game..." << endl;
        save_game(game);
        finished = true;
        return;
    }

    int new_x = game->player_x;//GAME Е ИДЕНТИФИКАТОР С КОЙТО ДОСТЪПВАМ КООРДИНАТИТЕ НА РЕД Х И КОЛОНА У
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
                save_game(game);
                load_map(game);//LOAD СЛЕДВАЩА КАРТА
                new_x = game->player_x;//LOAD СЛ. КООРДИНАТИ
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
        new_x = portal_x;//НАМИРА КООРД И ПРЕМИНАВА КЪМ СЛЕДВАЩИЯ ПОРТАЛ
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
    const char* message = "";
    const char* gameExitInfo = "Enter [Q] at any point to exit and save!";

    while (!finished) {
        system("cls");
        print_message(gameExitInfo);
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
