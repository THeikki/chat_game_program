#ifndef GAME_H
#define GAME_H

#include "client.h"
#include "globals.h"

extern char* new_line;
extern char* grid_left_empty;
extern char* vertical_bar;
extern char* horizontal_bar;
extern char* clear_message; 
extern char* welcome_message;

extern int x;
extern int y;
extern int show_prompt;
extern int result;
extern int player_quit;

extern char* grid_numbers;
extern char* vertical_numbers;
extern char* grid_arr;

void* game_handling(void* ptr);
int choose_difficulty_level(void* p_arg);
void print_grid(void* p_arg, int row_settings, int column_settings);
void get_random_mine_locations(void* p_arg, int max, int min, int count);
void setup_mines(void* p_arg, int row_settings, int column_settings, int mine_settings);
void get_player_input(void* p_arg, int column_settings, int difficulty);
void check_move(void* p_arg, int row_settings, int column_settings);
void check_if_win(void* p_arg, int row_settings, int column_settings);
void reveal_zero_numbers(void* p_arg, int x, int y, int row_settings, int column_settings);
void gameover_action_prompt(void* p_arg, int status);

#endif