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

void* game_handling(void* client_args);
int choose_difficulty_level(void* client_args);
void print_grid(void* client_args, int row_settings, int column_settings);
void setup_mines(void* client_args, int row_settings, int column_settings, int mine_settings, int difficulty);
void get_player_input(void* client_args, int column_settings, int difficulty);
void check_move(void* client_args, int row_settings, int column_settings);
void print_number_of_mines_grid(void* client_args);
void check_if_win(void* client_args, int row_settings, int column_settings);
void reveal_zero_numbers(void* client_args, int x, int y, int row_settings, int column_settings);
void gameover_action_prompt(void* client_args, int status);

#endif