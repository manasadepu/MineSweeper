#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <sstream>

using namespace std;

class WindowInfo{
public:
    int num_rows;
    int num_columns;
    int mine_count;
    int tile_count;
    int height;
    int width;
    string current_time_mintens;
    string current_time_minones;
    string current_time_sectens;
    string current_time_secones;
    WindowInfo(string file_directory){
        fstream config_file;
        config_file.open(file_directory, ios_base::in);
        if (config_file.is_open()){
            config_file >> num_columns;
            config_file >> num_rows;
            config_file >> mine_count;
            tile_count = num_rows * num_columns;
            width = num_columns * 32;
            height = (num_rows * 32) + 100;

        }
        else{
            cout << "File is not open";
        }
    }
};

class Tile{
public:
    sf::Sprite sprite;
    bool has_mine;
    bool has_flag;
    bool revealed;
    int neighboring_mines;
    vector<Tile*> adjacent_tiles;

    Tile(sf::Sprite &Sprite_){
        sprite = Sprite_;
        has_mine = false;
        has_flag = false;
        revealed = false;
        neighboring_mines = 0;
    }
};

void setText(sf::Text &text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
                   textRect.top + textRect.height / 2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

void insert_cursor(sf::String &mystring){
    mystring.insert(mystring.getSize(), "|");
}

void load_texture_and_sprite(map<string, sf::Sprite> &button_map, string filename, float position_x, float position_y) {
    sf::Texture texture;
    texture.loadFromFile(filename);
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setPosition(position_x, position_y);
    button_map[filename.substr(13)] = sprite;
}

bool is_valid_index(int row_index, int col_index, WindowInfo &base_window){
    if ((row_index >= base_window.num_rows) || (col_index >= base_window.num_columns) || (row_index < 0) || (col_index < 0)){
        return false;
    }
    else{
        return true;
    }
}

void divide_digits(map<string, sf::Sprite> &digit_sprite_map, map<string, sf::Texture> &texture_dict){
    vector<string> digit_names = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9","negative"};

    for (int i = 0; i < digit_names.size(); i++){
        sf::Sprite sprite;
        sprite.setTexture(texture_dict.at("digits.png"));
        sprite.setTextureRect(sf::IntRect(i * 21,0,21,32));
        digit_sprite_map[digit_names.at(i)] = sprite;
    }
}

void set_welcome_window(sf::RenderWindow &window, WindowInfo &base_window){
    window.clear(sf::Color(0,0,255));
    sf::Font myfont;
    myfont.loadFromFile("files/font.ttf");
    sf::Text welcometext("WELCOME TO MINESWEEPER", myfont);
    welcometext.setCharacterSize(24);
    welcometext.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(welcometext,base_window.width/2.0f,base_window.height/2.0f - 150);
    window.draw(welcometext);

    sf::Text input_prompt_text("Enter your name:", myfont);
    input_prompt_text.setCharacterSize(20);
    input_prompt_text.setStyle(sf::Text::Bold);
    setText(input_prompt_text,base_window.width/2.0f,base_window.height/2.0f - 75);
    window.draw(input_prompt_text);
}

map<string, sf::Sprite> set_button_map(WindowInfo &base_window){
    map<string, sf::Sprite> sprite_map;

    load_texture_and_sprite(sprite_map, "files/images/debug.png",
                                                             ((base_window.num_columns) * 32) - 304, 32 * ((base_window.num_rows)+0.5f));

    load_texture_and_sprite(sprite_map, "files/images/pause.png", ((base_window.num_columns) * 32) - 240,
                                                             32 * ((base_window.num_rows)+0.5f));
    load_texture_and_sprite(sprite_map, "files/images/play.png", ((base_window.num_columns) * 32) - 240,
                                                            32 * ((base_window.num_rows)+0.5f));
    load_texture_and_sprite(sprite_map, "files/images/face_happy.png",
                                                           (((base_window.num_columns) / 2.0) * 32) - 32, 32 *((base_window.num_rows)+0.5f));
    load_texture_and_sprite(sprite_map, "files/images/face_lose.png",
                                                          (((base_window.num_columns) / 2.0) * 32) - 32, 32 *((base_window.num_rows)+0.5f));
    load_texture_and_sprite(sprite_map, "files/images/face_win.png",
                                                         (((base_window.num_columns) / 2.0) * 32) - 32, 32 *((base_window.num_rows)+0.5f));
    load_texture_and_sprite(sprite_map, "files/images/leaderboard.png",
                                                                   ((base_window.num_columns) * 32) - 176, 32 * ((base_window.num_rows)+0.5f));
    return sprite_map;
}

vector<vector<Tile>> set_tiles(WindowInfo &base_window, map<string, sf::Texture> &texture_map){
    vector<vector<Tile>> tiles;

    float pos_x;
    float pos_y = 0;
    tiles.resize(base_window.num_rows);
    for (int i = 0; i < base_window.num_rows; i++){
        pos_x = 0;
        for (int b = 0; b < base_window.num_columns; b++){
            sf::Sprite sprite;
            sprite.setTexture(texture_map.at("tile_hidden.png"));
            sprite.setPosition(pos_x, pos_y);
            pos_x += 32;
            Tile my_tile(sprite);
            tiles[i].push_back(my_tile);
        }
        pos_y += 32;
    }

    srand((unsigned) time(NULL));
    int mine_count = base_window.mine_count;
    while (mine_count > 0){
        int rand_col = rand() % base_window.num_columns;
        int rand_row = rand() % base_window.num_rows;
        if (tiles[rand_row].at(rand_col).has_mine){
            continue;
        }
        else{
            tiles[rand_row].at(rand_col).has_mine = true;
            mine_count -= 1;
        }
    }

    for (int i = 0; i < base_window.num_rows; i++) {
        for (int b = 0; b < base_window.num_columns; b++) {
            if (tiles[i].at(b).has_mine){
                continue;
            }
            if (is_valid_index(i, b - 1, base_window)) {
                if (tiles[i].at(b - 1).has_mine) {
                    tiles[i].at(b).neighboring_mines += 1;
                }
                tiles[i].at(b).adjacent_tiles.push_back(&tiles[i].at(b - 1));
            }
            if (is_valid_index(i, b + 1, base_window)) {
                if (tiles[i].at(b + 1).has_mine) {
                    tiles[i].at(b).neighboring_mines += 1;
                }
                tiles[i].at(b).adjacent_tiles.push_back(&tiles[i].at(b + 1));

            }
            if (is_valid_index(i - 1, b - 1, base_window)) {
                if (tiles[i - 1].at(b - 1).has_mine) {
                    tiles[i].at(b).neighboring_mines += 1;
                }
                tiles[i].at(b).adjacent_tiles.push_back(&tiles[i - 1].at(b - 1));
            }
            if (is_valid_index(i - 1, b, base_window)) {
                if (tiles[i - 1].at(b).has_mine) {
                    tiles[i].at(b).neighboring_mines += 1;
                }
                tiles[i].at(b).adjacent_tiles.push_back(&tiles[i - 1].at(b));
            }
            if (is_valid_index(i - 1, b + 1, base_window)) {
                if (tiles[i - 1].at(b + 1).has_mine) {
                    tiles[i].at(b).neighboring_mines += 1;
                }
                tiles[i].at(b).adjacent_tiles.push_back(&tiles[i - 1].at(b + 1));
            }
            if (is_valid_index(i + 1, b - 1, base_window)) {
                if (tiles[i + 1].at(b - 1).has_mine) {
                    tiles[i].at(b).neighboring_mines += 1;
                }
                tiles[i].at(b).adjacent_tiles.push_back(&tiles[i + 1].at(b - 1));
            }
            if (is_valid_index(i + 1, b, base_window)) {
                if (tiles[i + 1].at(b).has_mine) {
                    tiles[i].at(b).neighboring_mines += 1;
                }
                tiles[i].at(b).adjacent_tiles.push_back(&tiles[i + 1].at(b));
            }
            if (is_valid_index(i + 1, b + 1, base_window)) {
                if (tiles[i + 1].at(b + 1).has_mine) {
                    tiles[i].at(b).neighboring_mines += 1;
                }
                tiles[i].at(b).adjacent_tiles.push_back(&tiles[i + 1].at(b + 1));
            }
        }
    }
    return tiles;
}

void set_game_window(vector<vector<Tile>> &tiles, sf::RenderWindow &window, WindowInfo &base_window, map<string, sf::Sprite> &sprite_map, map<string, sf::Texture> &texture_map,
                     bool leader_board_condition, bool debug_condition, bool pause_condition, string game_condition, map<string, sf::Sprite> &digits_map, int time_elapsed){
    window.clear(sf::Color::White);
    int flag_counter = base_window.mine_count;
    if (leader_board_condition || pause_condition){
        for (int i = 0; i < tiles.size(); i++) {
            for (int b = 0; b < base_window.num_columns; b++) {
                const sf::Texture* current_texture = tiles[i].at(b).sprite.getTexture();
                tiles[i].at(b).sprite.setTexture(texture_map.at("tile_revealed.png"));
                window.draw(tiles[i].at(b).sprite);
                tiles[i].at(b).sprite.setTexture(*current_texture);
                if (tiles[i].at(b).has_flag){
                    flag_counter -= 1;
                }
            }
        }
    }
    else{
        for (int i = 0; i < tiles.size(); i++){
            for (int b = 0; b < base_window.num_columns; b++){
                    window.draw(tiles[i].at(b).sprite);
                    if (tiles[i].at(b).has_flag){
                        sf::Sprite flag_sprite;
                        sf::Vector2f coords = tiles[i].at(b).sprite.getPosition();
                        flag_sprite.setTexture(texture_map.at("flag.png"));
                        flag_sprite.setPosition(coords.x, coords.y);
                        window.draw(flag_sprite);
                        flag_counter -= 1;
                    }
                    if (tiles[i].at(b).revealed) {
                        int mines_around = tiles[i].at(b).neighboring_mines;
                        sf::Vector2f coords = tiles[i].at(b).sprite.getPosition();
                        sf::Sprite num_sprite;
                        if (mines_around == 1) {
                            num_sprite.setTexture(texture_map.at("number_1.png"));
                            num_sprite.setPosition(coords.x, coords.y);
                        } else if (mines_around == 2) {
                            num_sprite.setTexture(texture_map.at("number_2.png"));
                            num_sprite.setPosition(coords.x, coords.y);
                        } else if (mines_around == 3) {
                            num_sprite.setTexture(texture_map.at("number_3.png"));
                            num_sprite.setPosition(coords.x, coords.y);
                        } else if (mines_around == 4) {
                            num_sprite.setTexture(texture_map.at("number_4.png"));
                            num_sprite.setPosition(coords.x, coords.y);
                        } else if (mines_around == 5) {
                            num_sprite.setTexture(texture_map.at("number_5.png"));
                            num_sprite.setPosition(coords.x, coords.y);
                        } else if (mines_around == 6) {
                            num_sprite.setTexture(texture_map.at("number_6.png"));
                            num_sprite.setPosition(coords.x, coords.y);
                        } else if (mines_around == 7) {
                            num_sprite.setTexture(texture_map.at("number_7.png"));
                            num_sprite.setPosition(coords.x, coords.y);
                        } else if (mines_around == 8) {
                            num_sprite.setTexture(texture_map.at("number_8.png"));
                            num_sprite.setPosition(coords.x, coords.y);
                        } else {
                            continue;
                        }
                        window.draw(num_sprite);
                    }
                if (game_condition == "lost"){
                    if (tiles[i].at(b).has_mine){
                        sf::Sprite mine_sprite;
                        sf::Vector2f coords = tiles[i].at(b).sprite.getPosition();
                        mine_sprite.setTexture(texture_map.at("mine.png"));
                        mine_sprite.setPosition(coords.x, coords.y);
                        window.draw(mine_sprite);
                    }
                }
                if (debug_condition){
                    if (tiles[i].at(b).has_mine){
                        sf::Sprite mine_sprite;
                        sf::Vector2f coords = tiles[i].at(b).sprite.getPosition();
                        mine_sprite.setTexture(texture_map.at("mine.png"));
                        mine_sprite.setPosition(coords.x, coords.y);
                        window.draw(mine_sprite);
                    }
                }
            }
        }
    }

    if (game_condition == "running"){
        sprite_map.at("face_happy.png").setTexture(texture_map.at("face_happy.png"));
        window.draw(sprite_map.at("face_happy.png"));
    }
    if (game_condition == "lost"){
        sprite_map.at("face_lose.png").setTexture(texture_map.at("face_lose.png"));
        window.draw(sprite_map.at("face_lose.png"));
    }
    if (game_condition == "won"){
        sprite_map.at("face_win.png").setTexture(texture_map.at("face_win.png"));
        window.draw(sprite_map.at("face_win.png"));
    }
    if (pause_condition){
        sprite_map.at("play.png").setTexture(texture_map.at("play.png"));
        window.draw(sprite_map.at("play.png"));
    }
    else{
        sprite_map.at("pause.png").setTexture(texture_map.at("pause.png"));
        window.draw(sprite_map.at("pause.png"));
    }
    sprite_map.at("debug.png").setTexture(texture_map.at("debug.png"));
    window.draw(sprite_map.at("debug.png"));

    sprite_map.at("leaderboard.png").setTexture(texture_map.at("leaderboard.png"));
    window.draw(sprite_map.at("leaderboard.png"));


    float seconds = time_elapsed;
    int num_minutes = (int)seconds / 60;
    int num_seconds = (int)seconds % 60;
    string tens_dig_min;
    string ones_dig_min;
    string tens_dig_sec;
    string ones_dig_sec;
    if (num_minutes < 10){
        tens_dig_min = "0";
    }
    else {
        tens_dig_min = to_string(num_minutes / 10 % 10);
    }
    if (num_seconds < 10){
        tens_dig_sec = "0";
    }
    else{
        tens_dig_sec = to_string(num_seconds / 10 % 10);
    }
    ones_dig_min = to_string(num_minutes % 10);
    ones_dig_sec = to_string(num_seconds % 10);

    if ((game_condition == "lost") || (game_condition == "won" || pause_condition)){
        digits_map.at(base_window.current_time_mintens).setTexture(texture_map.at("digits.png"));
        digits_map.at(base_window.current_time_mintens).setPosition(((base_window.num_columns)*32)-97, 32*((base_window.num_rows)+0.5f)+16);
        window.draw(digits_map.at(base_window.current_time_mintens));
        digits_map.at(base_window.current_time_minones).setTexture(texture_map.at("digits.png"));
        digits_map.at(base_window.current_time_minones).setPosition(((base_window.num_columns)*32)-76, 32*((base_window.num_rows)+0.5f)+16);
        window.draw(digits_map.at(base_window.current_time_minones));
        digits_map.at(base_window.current_time_sectens).setTexture(texture_map.at("digits.png"));
        digits_map.at(base_window.current_time_sectens).setPosition(((base_window.num_columns)*32)-54, 32*((base_window.num_rows)+0.5f)+16);
        window.draw(digits_map.at(base_window.current_time_sectens));
        digits_map.at(base_window.current_time_secones).setTexture(texture_map.at("digits.png"));
        digits_map.at(base_window.current_time_secones).setPosition(((base_window.num_columns)*32)-33, 32*((base_window.num_rows)+0.5f)+16);
        window.draw(digits_map.at(base_window.current_time_secones));
    }
    else{
        base_window.current_time_minones = ones_dig_min;
        base_window.current_time_mintens = tens_dig_min;
        base_window.current_time_secones = ones_dig_sec;
        base_window.current_time_sectens = tens_dig_sec;
    map<string, sf::Sprite>::iterator timeiter;
    for (timeiter = digits_map.begin(); timeiter != digits_map.end(); timeiter++){
        if (tens_dig_min == timeiter->first){
            timeiter->second.setTexture(texture_map.at("digits.png"));
            timeiter->second.setPosition(((base_window.num_columns)*32)-97, 32*((base_window.num_rows)+0.5f)+16);
            window.draw(timeiter->second);
        }
        if (ones_dig_min == timeiter->first){
            timeiter->second.setTexture(texture_map.at("digits.png"));
            timeiter->second.setPosition(((base_window.num_columns)*32)-76, 32*((base_window.num_rows)+0.5f)+16);
            window.draw(timeiter->second);
        }
        if (tens_dig_sec == timeiter->first){
            timeiter->second.setTexture(texture_map.at("digits.png"));
            timeiter->second.setPosition(((base_window.num_columns)*32)-54, 32*((base_window.num_rows)+0.5f)+16);
            window.draw(timeiter->second);
        }
        if (ones_dig_sec == timeiter->first) {
            timeiter->second.setTexture(texture_map.at("digits.png"));
            timeiter->second.setPosition(((base_window.num_columns) * 32) - 33,
                                         32 * ((base_window.num_rows) + 0.5f) + 16);
            window.draw(timeiter->second);
        }
        }
    }



    if (flag_counter < 0) {
        digits_map.at("negative").setTexture(texture_map.at("digits.png"));
        digits_map.at("negative").setPosition(12, 32 * ((base_window.num_rows) + 0.5f) + 16);
        window.draw(digits_map.at("negative"));
        flag_counter = ::abs(flag_counter);
    }
    string ones_digit = to_string(flag_counter % 10);
    string tens_digit = to_string(flag_counter / 10 % 10);
    string hundreds_digit;
    if (flag_counter < 100) {
        hundreds_digit = "0";
    }
    else{
        hundreds_digit = to_string(flag_counter /100 % 10);
    }
    map<string, sf::Sprite>::iterator myiter;
    for (myiter = digits_map.begin(); myiter != digits_map.end(); myiter++){
            if (hundreds_digit == myiter->first){
                myiter->second.setTexture(texture_map.at("digits.png"));
                myiter->second.setPosition(33, 32 * ((base_window.num_rows) + 0.5f) + 16);
                window.draw(myiter->second);
            }
            if (tens_digit == myiter->first){
                myiter->second.setTexture(texture_map.at("digits.png"));
                myiter->second.setPosition(54, 32 * ((base_window.num_rows) + 0.5f) + 16);
                window.draw(myiter->second);
            }
            if (ones_digit == myiter->first){
                myiter->second.setTexture(texture_map.at("digits.png"));
                myiter->second.setPosition(75, 32 * ((base_window.num_rows) + 0.5f) + 16);
                window.draw(myiter->second);
            }
    }
}

vector<string> read_leaderboard_file(){
    vector<string> leaders;
    fstream file("files/leaderboard.txt", ios_base::in);
    string leader_info;
    if (file.is_open()){
        for (int i = 0; i < 5; i++){
            getline(file, leader_info);
            leaders.push_back(leader_info);
        }
    }
    return leaders;
}

void setLeaderboard_window(sf::RenderWindow &leader_window, vector<string> &leaders){
    leader_window.clear(sf::Color::Blue);

    sf::Font myfont;
    myfont.loadFromFile("files/font.ttf");
    sf::Text leaderboard_title("LEADERBOARD", myfont);
    leaderboard_title.setCharacterSize(20);
    leaderboard_title.setFillColor(sf::Color::White);
    leaderboard_title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(leaderboard_title,leader_window.getSize().x/2.0f,leader_window.getSize().y/2.0f - 120);
    leader_window.draw(leaderboard_title);

    string temp_string = leaders.at(0);
    string substring_time = temp_string.substr(0,5);
    string substring_name = temp_string.substr(6);
    string leaderboard_string = (string)"1." + "\t" + substring_time + "\t" + substring_name + "\n\n";
    for (int i = 1; i < 5; i++){
        string temp2_string = leaders.at(i);
        string substring2_time = temp2_string.substr(0,5);
        string substring2_name = temp2_string.substr(6);
        stringstream current_rank;
        current_rank << i + 1;
        string string_current_rank;
        current_rank >> string_current_rank;
        // ADD Functionality of Asterisk for UserName
        leaderboard_string += string_current_rank + "." + "\t" + substring2_time + "\t" + substring2_name + "\n\n";
    }
    sf::Text leaderboard_rankings(leaderboard_string, myfont);
    leaderboard_rankings.setCharacterSize(18);
    leaderboard_rankings.setFillColor(sf::Color::White);
    leaderboard_rankings.setStyle(sf::Text::Bold);
    setText(leaderboard_rankings,leader_window.getSize().x/2.0f,leader_window.getSize().y/2.0f + 20);
    leader_window.draw(leaderboard_rankings);
}

map<string, sf::Texture> load_textures_into_map(){
    map<string, sf::Texture> map_oftextures;

    vector<string> file_names{"files/images/digits.png","files/images/mine.png", "files/images/flag.png", "files/images/number_1.png",
                              "files/images/number_2.png", "files/images/number_3.png", "files/images/number_4.png", "files/images/number_5.png",
                              "files/images/number_6.png", "files/images/number_7.png", "files/images/number_8.png", "files/images/tile_hidden.png",
                              "files/images/tile_revealed.png", "files/images/debug.png", "files/images/pause.png", "files/images/play.png",
                              "files/images/face_happy.png", "files/images/face_lose.png", "files/images/face_win.png", "files/images/leaderboard.png"};
    for (int i = 0; i < file_names.size(); i++){
        string texture_name = file_names.at(i).substr(13);
        sf::Texture texture;
        texture.loadFromFile(file_names.at(i));
        map_oftextures.emplace(texture_name, texture);
    }
    return map_oftextures;
}

void tile_revealed_func(Tile &tile_, map<string, sf::Texture> &texture_dict) {
        if (tile_.has_mine){}
        else if (tile_.has_flag){}
        else if (tile_.neighboring_mines > 0) {
            tile_.revealed = true;
            tile_.has_flag = false;
            tile_.sprite.setTexture(texture_dict.at("tile_revealed.png"));

        }else if (tile_.revealed == false && tile_.neighboring_mines == 0) {
            tile_.revealed = true;
            tile_.has_flag = false;
            tile_.sprite.setTexture(texture_dict.at("tile_revealed.png"));
            for (int i = 0; i < tile_.adjacent_tiles.size(); i++){
                tile_revealed_func(*tile_.adjacent_tiles.at(i), texture_dict);
            }
        }
    }

bool check_for_win(vector<vector<Tile>> &tiles, WindowInfo &base_window){
    int count = 0;
        for (int i = 0; i < tiles.size(); i++) {
            for (int b = 0; b < base_window.num_columns; b++) {
                if (!tiles[i].at(b).has_mine){
                    if (tiles[i].at(b).revealed || tiles[i].at(b).has_flag){
                        count += 1;
                    }
                    else{
                        return false;
                    }
                }
            }
        }

        if ((count + base_window.mine_count) == base_window.tile_count){
            for (int i = 0; i < tiles.size(); i++) {
                for (int b = 0; b < base_window.num_columns; b++) {
                    if (tiles[i].at(b).has_mine){
                        tiles[i].at(b).has_flag = true;
                    }
                }
                }
            return true;
        }
        else{
            return false;
        }
}
