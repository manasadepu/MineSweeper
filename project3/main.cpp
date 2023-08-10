#include <iostream>
#include "welcomewindow.h"
#include <chrono>
using namespace chrono;
int main() {
    map<string, sf::Texture> texture_dict = load_textures_into_map();
    map<string, sf::Sprite> digits_map;
    divide_digits(digits_map, texture_dict);
    WindowInfo base_window_info("files/board_config.cfg");
    cout << base_window_info.num_rows << endl;
    cout << base_window_info.num_columns << endl;
    cout << base_window_info.mine_count << endl;
    cout << base_window_info.width << " " << base_window_info.height;

    sf::RenderWindow window(sf::VideoMode(base_window_info.width, base_window_info.height),
                            "SFML window", sf::Style::Close);


    set_welcome_window(window, base_window_info);
    sf::Font myfont;
    myfont.loadFromFile("files/font.ttf");
    sf::String input_string;
    insert_cursor(input_string);
    sf::Text input_text(input_string, myfont);
    input_text.setCharacterSize(18);
    input_text.setStyle(sf::Text::Bold);
    input_text.setFillColor(sf::Color(255, 255, 0));
    setText(input_text, base_window_info.width / 2.0f, base_window_info.height / 2.0f - 45);
    window.draw(input_text);
    window.display();
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 10 && input_string.getSize() > 1) {
                    window.close();
                }
                if (input_string.getSize() > 10 && event.text.unicode == 8) {
                    input_string.erase(input_string.getSize() - 2);
                }
                if (input_string.getSize() > 10) {
                    continue;
                }
                if (event.text.unicode == 8) {
                    if (input_string.getSize() == 1) {
                        continue;
                    } else {
                        input_string.erase(input_string.getSize() - 2);
                    }
                } else if (isalpha(event.text.unicode)) {
                    char letter;
                    if (input_string.getSize() == 1) {
                        letter = toupper(static_cast<char>(event.text.unicode));
                    } else {
                        letter = ::tolower(static_cast<char>(event.text.unicode));
                    }
                    sf::String insert_string(letter);
                    input_string.insert(input_string.getSize() - 1, insert_string);
                }
                sf::Text text(input_string, myfont);
                text.setCharacterSize(18);
                text.setFillColor(sf::Color(255, 255, 0));
                setText(text, base_window_info.width / 2.0f, base_window_info.height / 2.0f - 45);
                set_welcome_window(window, base_window_info);
                window.draw(text);
                window.display();
            }
        }
    }

    sf::RenderWindow game_window(sf::VideoMode(base_window_info.width, base_window_info.height),
                                 "Minesweeper", sf::Style::Close);

    auto start = high_resolution_clock::now();
    auto elapse = duration_cast<seconds> (high_resolution_clock::now()-start).count();
    auto pause = duration_cast<seconds> (high_resolution_clock::now()-start).count();
    auto pauseTime = high_resolution_clock::now();
    auto pause_button_time = high_resolution_clock::now();
    bool wasPaused = false;
    int count;
    vector<string> leaders_list = read_leaderboard_file();

    map<string, sf::Sprite> sprite_map = set_button_map(base_window_info);
    vector<vector<Tile>> tiles_vector = set_tiles(base_window_info, texture_dict);
    bool debug_pressed = false;
    bool ispaused = false;
    string game_status = "running";
    set_game_window(tiles_vector, game_window, base_window_info, sprite_map, texture_dict, false, debug_pressed,ispaused, game_status, digits_map, elapse);
    game_window.display();
    while (game_window.isOpen()) {
        sf::Event event;
        while (game_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                game_window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    float x_pos = event.mouseButton.x;
                    float y_pos = event.mouseButton.y;
                    string button_pressed;
                    map<string, sf::Sprite>::iterator sprite_iter;
                    for (sprite_iter = sprite_map.begin(); sprite_iter != sprite_map.end(); sprite_iter++) {
                        const sf::Vector2f sprite_pos = sprite_iter->second.getPosition();
                        const sf::IntRect sprite_size = sprite_iter->second.getTextureRect();
                        if (x_pos >= sprite_pos.x && x_pos <= sprite_pos.x + sprite_size.width) {
                            if (y_pos >= sprite_pos.y && y_pos <= sprite_pos.y + sprite_size.height) {
                                button_pressed = sprite_iter->first;
                                break;
                            }
                        }
                    }
                    if (game_status == "lost"){
                        if (button_pressed == "face_happy.png"){
                            tiles_vector = set_tiles(base_window_info, texture_dict);
                            game_status = "running";
                            start = high_resolution_clock::now();
                        }
                        else{
                            break;
                        }
                    }
                    if (button_pressed == "leaderboard.png") {
                        // opens and runs leaderboard window
                        sf::RenderWindow leaderboard(
                                sf::VideoMode(base_window_info.width / 2, base_window_info.height / 2),
                                "Leaderboard", sf::Style::Close);
                        bool paused_before;

                        ispaused = true;
                        set_game_window(tiles_vector, game_window, base_window_info, sprite_map, texture_dict, true, debug_pressed, ispaused, game_status, digits_map, elapse);
                        game_window.display();
                        setLeaderboard_window(leaderboard, leaders_list);
                        leaderboard.display();
                        pauseTime = high_resolution_clock::now();
                        sf::Clock myclock_leaderboard;
                        while (leaderboard.isOpen()) {
                            sf::Event event;
                            while (leaderboard.pollEvent(event)) {
                                if (event.type == sf::Event::Closed) {
                                    leaderboard.close();
                                    ispaused = false;
                                    wasPaused = true;
                                    pause = duration_cast<seconds> (high_resolution_clock::now()-pauseTime).count();
                                }
                            }

                        }
                    }
                    else if (button_pressed == "debug.png" && !ispaused){
                        if (debug_pressed){
                            debug_pressed = false;
                        }
                        else{
                            debug_pressed = true;
                        }
                    }
                    else if (button_pressed == "face_happy.png"){
                        tiles_vector = set_tiles(base_window_info, texture_dict);
                        game_status = "running";
                        start = high_resolution_clock::now();
                    }
                    else if (button_pressed == "pause.png"){
                        if (ispaused){
                            wasPaused = true;
                            pause = duration_cast<seconds> (high_resolution_clock::now()-pauseTime).count();
                        }
                        else{
                            pauseTime = high_resolution_clock::now();
                        }
                        ispaused = !ispaused;
                    }
                    else {
                        if (ispaused) {

                        } else {
                            //this block checks if user clicks on a tile
                            bool tile_found = false;
                            for (int i = 0; i < tiles_vector.size(); i++) {
                                for (int b = 0; b < base_window_info.num_columns; b++) {
                                    float tile_x = tiles_vector[i][b].sprite.getPosition().x;
                                    float tile_y = tiles_vector[i][b].sprite.getPosition().y;
                                    if (x_pos >= tile_x && x_pos <= tile_x + 32) {
                                        if (y_pos >= tile_y && y_pos <= tile_y + 32) {
                                            if (tiles_vector[i][b].has_mine) {
                                                tile_found = true;
                                                game_status = "lost";
                                                break;
                                            } else if (tiles_vector[i][b].neighboring_mines == 0) {
                                                tile_revealed_func(tiles_vector[i][b], texture_dict);
                                            } else if (tiles_vector[i][b].has_flag) {
                                                tile_found = true;
                                                break;
                                            } else {
                                                tiles_vector[i][b].has_flag = false;
                                                tiles_vector[i][b].revealed = true;
                                                tiles_vector[i][b].sprite.setTexture(
                                                        texture_dict.at("tile_revealed.png"));
                                            }
                                            tile_found = true;
                                        }
                                    }

                                    if (tile_found) {
                                        break;
                                    }
                                }
                                if (tile_found) {
                                    break;
                                }
                            }

                        }
                    }

                }
                if (event.mouseButton.button == sf::Mouse::Right) {
                    if (game_status == "lost" || game_status == "won" || ispaused){
                        continue;
                    }
                    float x_pos = event.mouseButton.x;
                    float y_pos = event.mouseButton.y;
                    bool tile_found = false;
                    for (int i = 0; i < tiles_vector.size(); i++) {
                        for (int b = 0; b < base_window_info.num_columns; b++) {
                            float tile_x = tiles_vector[i][b].sprite.getPosition().x;
                            float tile_y = tiles_vector[i][b].sprite.getPosition().y;
                            if (x_pos >= tile_x && x_pos <= tile_x + 32) {
                                if (y_pos >= tile_y && y_pos <= tile_y + 32) {
                                    if (tiles_vector[i][b].has_flag){
                                        tiles_vector[i][b].has_flag = false;
                                    }
                                    else if (tiles_vector[i][b].revealed){
                                        continue;
                                    }
                                    else{
                                        tiles_vector[i][b].has_flag = true;
                                    }
                                    tile_found = true;
                                }
                            }

                            if (tile_found) {
                                break;
                            }
                        }
                        if (tile_found) {
                            break;
                        }
                    }
                }

            }
        }
        if (check_for_win(tiles_vector, base_window_info) && game_status != "won"){
            game_status = "won";
            count = 0;
        }
        if (game_status == "won" && count == 0) {
            sf::RenderWindow leaderboard(
                    sf::VideoMode(base_window_info.width / 2, base_window_info.height / 2),
                    "Leaderboard", sf::Style::Close);
            set_game_window(tiles_vector, game_window, base_window_info, sprite_map, texture_dict, false, debug_pressed,
                            ispaused, game_status, digits_map, elapse);
            game_window.display();
            setLeaderboard_window(leaderboard, leaders_list);
            leaderboard.display();
            sf::Clock myclock_leaderboard;
            while (leaderboard.isOpen()) {
                sf::Event event;
                while (leaderboard.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        leaderboard.close();
                    }
                }
            }
            count ++;
        }
        elapse = duration_cast<seconds> (high_resolution_clock::now()-start).count();
        if (wasPaused) {
            elapse = elapse - pause;
            wasPaused = !wasPaused;
            start += high_resolution_clock::now() - pauseTime;
        }
        set_game_window(tiles_vector, game_window, base_window_info, sprite_map, texture_dict, false, debug_pressed, ispaused, game_status, digits_map, elapse);
        game_window.display();
    }
    return 0;
}