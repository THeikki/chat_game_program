#include "../header_files/game.h"

char* new_line = "\n\r";
char* grid_left_empty = "   ";
char* vertical_bar = "| ";
char* horizontal_bar = "- ";
char* clear_message = "\033[2J\033[H\033[3J"; // Clear putty terminal and scrollback
char* welcome_message = "==============================================================\n\r"
                        "*                       MINE SWEEPER                         *\n\r"
                        "*                                                            *\n\r"
                        "==============================================================\n\r";

char* grid_numbers;
char* vertical_numbers;
char* grid_arr;

int x;
int y;
int show_prompt;
int result;
int player_quit;

//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
/*******************************************************************************************************
    GAME HANDLING FUNCTION
*/
void* game_handling(void* p_args) {
    struct arg *p = p_args;
   
    int difficulty = 0;
    int row_settings = 0;
    int column_settings = 0;
    int mine_settings = 0; 
    show_prompt = 0;
    player_quit = 0;

    send(*p->socket, clear_message, strlen(clear_message), 0);
    send(*p->socket, welcome_message, strlen(welcome_message), 0);
    difficulty = choose_difficulty_level(p);

    if(difficulty == 1) {
        row_settings = EASY_ROWS;
        column_settings = EASY_COLUMNS;
        mine_settings = EASY_MINES;
       
        p->level.mine_numbers[EASY_MINES];
        p->level.mine_locations[EASY_ROWS][EASY_COLUMNS];
        p->level.free_locations[EASY_ROWS][EASY_COLUMNS];
        p->level.taken_locations[EASY_ROWS][EASY_COLUMNS];
        p->level.grid_array[EASY_ROWS][EASY_COLUMNS];
    }
    else if(difficulty == 2) {
        row_settings = MEDIUM_ROWS;
        column_settings = MEDIUM_COLUMNS;
        mine_settings = MEDIUM_MINES;
        p->level.mine_numbers[MEDIUM_MINES];
        p->level.mine_locations[MEDIUM_ROWS][MEDIUM_COLUMNS];
        p->level.free_locations[MEDIUM_ROWS][MEDIUM_COLUMNS];
        p->level.taken_locations[MEDIUM_ROWS][MEDIUM_COLUMNS];
        p->level.grid_array[MEDIUM_ROWS][MEDIUM_COLUMNS];
    }
    else if(difficulty == 3) {
        row_settings = HARD_ROWS;
        column_settings = HARD_COLUMNS;
        mine_settings = HARD_MINES;
        p->level.mine_numbers[HARD_MINES];
        p->level.mine_locations[HARD_ROWS][HARD_COLUMNS];
        p->level.free_locations[HARD_ROWS][HARD_COLUMNS];
        p->level.taken_locations[HARD_ROWS][HARD_COLUMNS];
        p->level.grid_array[HARD_ROWS][HARD_COLUMNS];
    }
    //  Initialize struct arrays to zero
    for(int i = 0; i < mine_settings; i++) {
        p->level.mine_numbers[i] = 0;
    }
    for(int i = 0; i < row_settings; i++) {
        for(int j = 0; j < column_settings; j++) {
            p->level.mine_locations[i][j] = 0;
            p->level.free_locations[i][j] = 0;
            p->level.taken_locations[i][j] = 0;
            p->level.grid_array[i][j] = 0;
        }
    }

    send(*p->socket, clear_message, strlen(clear_message), 0);
    send(*p->socket, welcome_message, strlen(welcome_message), 0);

    print_grid(p, row_settings, column_settings);
    get_random_mine_locations(p, column_settings -1, 0, mine_settings);
    setup_mines(p, row_settings, column_settings, mine_settings);
   
    do {
        //pthread_mutex_lock(&lock);
        get_player_input(p, column_settings, difficulty);
        //pthread_mutex_unlock(&lock);
        check_move(p, row_settings, column_settings);
        check_if_win(p, row_settings, column_settings);  
    } while (1);
}
/**********************************************************************************************************************
    PROMPT USER TO SELECT DIFFICULTY LEVEL
*/
int choose_difficulty_level(void* p_arg) {
    struct arg *p = p_arg;
    int difficulty_num;
    char difficulty_input[10];
    char* message = "Choose difficulty level:\n\r"
                    "[1] = Easy              \n\r"
                    "[2] = Average           \n\r"
                    "[3] = Hard              \n\r"
                    "                        \n\r";

    while (*difficulty_input != '1' && *difficulty_input != '2' && *difficulty_input != '3')
    {
        send(*p->socket, clear_message, strlen(clear_message), 0);
        send(*p->socket, welcome_message, strlen(welcome_message), 0);
        send(*p->socket, message, strlen(message), 0);
        result = recv(*p->socket, difficulty_input, 200, 0);

        if(result <0) {
            player_quit = 1;
            pthread_exit(NULL);
        }
    }
    send(*p->socket, clear_message, strlen(clear_message), 0);
    send(*p->socket, welcome_message, strlen(welcome_message), 0);
    difficulty_num = atoi(difficulty_input);
   
    return difficulty_num;
}
/**********************************************************************************************************************
    PRINTS GRID
*/
void print_grid(void* p_arg, int row_settings, int column_settings) {
    struct arg *p = p_arg;
    char* horizontal_numbers = malloc(sizeof(int) * column_settings);
    char* vertical_numbers = malloc(sizeof(int) * column_settings);
   
    for(int i = 0; i < column_settings; i++) {
        if(i == 0) {
            send(*p->socket, grid_left_empty, strlen(grid_left_empty), 0);
        }
        sprintf(horizontal_numbers, "%d ", i);
        send(*p->socket, horizontal_numbers, strlen(horizontal_numbers), 0);
    }
  
    send(*p->socket, new_line, strlen(new_line), 0);
    for (int i = 0; i < column_settings; i++)
    {
        if(i == 0) {
            send(*p->socket, grid_left_empty, strlen(grid_left_empty), 0);
        }
        send(*p->socket, vertical_bar, strlen(vertical_bar), 0);
    }
    send(*p->socket, new_line, strlen(new_line), 0);

    for(int i = 0; i < row_settings; i++) {
        for(int j = 0; j < column_settings; j++) {
            if(j == 0) {
                sprintf(vertical_numbers, "%d- ", i);
                send(*p->socket, vertical_numbers, strlen(vertical_numbers), 0);
            }
            send(*p->socket, horizontal_bar, strlen(horizontal_bar), 0);      
        }
        send(*p->socket, new_line, strlen(new_line), 0);    
    }
    free(horizontal_numbers);
    free(vertical_numbers); 
    horizontal_numbers = NULL;
    vertical_numbers = NULL; 
}
/**********************************************************************************************************************
    GETS MINE RANDOM NUMBERS
*/
void get_random_mine_locations(void* p_arg, int max, int min, int count) {
    struct arg *p = p_arg;
    srand(time(NULL));

    for(int i = 0; i < count; i++) {
        *(p->level.mine_numbers + i) = rand() % (max - min);
    }
}
/**********************************************************************************************************************
    GETS PLAYER INPUT
*/
void get_player_input(void* p_arg, int column_settings, int difficulty) {
    struct arg *p = p_arg;  
    char* x_input_prompt = malloc(sizeof(int) + 30);
    char x_input[10];
    char* y_input_prompt = malloc(sizeof(int) + 30);
    char y_input[10];
    sprintf(x_input_prompt,"Give (X) [0-%d]: ", column_settings - 1);
    sprintf(y_input_prompt,"Give (Y) [0-%d]: ", column_settings - 1);
   
    if(show_prompt != 1) {
        if(difficulty == 1) {
            while (*x_input != '0' && *x_input != '1' && *x_input != '2') {
   
                send(*p->socket, x_input_prompt, strlen(x_input_prompt), 0);
                result = recv(*p->socket, x_input, 200, 0); 
                
                if(result == 0) {
                    player_quit = 1;
                    clear_mine_sweeper();
                    pthread_exit(NULL);
                }
            }
        }
        else if(difficulty == 2) {
            while (*x_input != '0' && *x_input != '1' && *x_input != '2' && *x_input != '3'
                               && *x_input != '4' && *x_input != '5') {
   
                send(*p->socket, x_input_prompt, strlen(x_input_prompt), 0);
                result = recv(*p->socket, x_input, 200, 0); 
                
                if(result == 0) {
                    player_quit = 1;
                    clear_mine_sweeper();
                    pthread_exit(NULL);
                }
            }
        }
        else if(difficulty == 3) {
            while (*x_input != '0' && *x_input != '1' && *x_input != '2' && *x_input != '3'
                               && *x_input != '4' && *x_input != '5' && *x_input != '6'
                               && *x_input != '7' && *x_input != '8') {
   
                send(*p->socket, x_input_prompt, strlen(x_input_prompt), 0);
                result = recv(*p->socket, x_input, 200, 0); 
                
                if(result == 0) {
                    player_quit = 1;
                    clear_mine_sweeper();
                    pthread_exit(NULL);
                }
            }
        }
        x = atoi(x_input);
        free(x_input_prompt);
        x_input_prompt = NULL;

        if(difficulty == 1) {
            while (*y_input != '0' && *y_input != '1' && *y_input != '2' && *y_input != '3' 
                               && *y_input != '4' && *y_input != '5' && *y_input != '6' 
                               && *y_input != '7') {

                send(*p->socket, y_input_prompt, strlen(y_input_prompt), 0);
                result = recv(*p->socket, y_input, 200, 0); 
                
                if(result == 0) {
                    player_quit = 1;
                    clear_mine_sweeper();
                    pthread_exit(NULL);
                }
            }
        }
        else if(difficulty == 2) {
            while (*y_input != '0' && *y_input != '1' && *y_input != '2' && *y_input != '3' 
                               && *y_input != '4' && *y_input != '5') {

                send(*p->socket, y_input_prompt, strlen(y_input_prompt), 0);
                result = recv(*p->socket, y_input, 200, 0); 
                
                if(result == 0) {
                    player_quit = 1;
                    clear_mine_sweeper();
                    pthread_exit(NULL);
                }
            }
        }
        else if(difficulty == 3) {
            while (*y_input != '0' && *y_input != '1' && *y_input != '2' && *y_input != '3' 
                               && *y_input != '4' && *y_input != '5' && *y_input != '6' 
                               && *y_input != '7' && *y_input != '8') {

                send(*p->socket, y_input_prompt, strlen(y_input_prompt), 0);
                result = recv(*p->socket, y_input, 200, 0); 
                
                if(result == 0) {
                    player_quit = 1;
                    clear_mine_sweeper();
                    pthread_exit(NULL);
                }
            }
        }
        
        y = atoi(y_input);
        free(y_input_prompt);
        y_input_prompt = NULL;
    }
}
/**********************************************************************************************************************
    SETUP MINES IN ARRAY
*/
void setup_mines(void* p_arg, int row_settings, int column_settings, int mine_settings) {
    struct arg *p = p_arg;
    int *a = p->level.mine_numbers;
    int mine_num = 0;
    int mine_count;
    
    //  Set mine locations to array
    for(int i = 0; i < row_settings; i++) {
        for(int j = 0; j < column_settings; j++) {
            if(j == *a && mine_num != mine_settings ) {
                p->level.mine_locations[i][j] = 9;
                p->level.grid_array[i][j] = p->level.mine_locations[i][j];
                *a++;
                mine_num++;
            }
            else {
                p->level.free_locations[i][j] = 1;
                p->level.grid_array[i][j] = p->level.mine_locations[i][j];
            }
        }
    }

    // Arrange mines and neighbours in array
    for (int i = 0; i < row_settings; i++)
    {
        for (int j = 0; j < column_settings; j++)
        {
            if(p->level.grid_array[i][j] != 9) {
                mine_count = 0;
                for (int ii = -1; ii <= 1; ii++)
                {
                    if((i + ii) >= 0 && (i + ii) < row_settings) {
                        
                        for (int jj = -1; jj <= 1; jj++)
                        {
                            if((j + jj) >= 0 && (j + jj) < column_settings) {
                               
                                if(p->level.grid_array[i + ii][j + jj] == 9) {
                                    mine_count++;
                                    p->level.grid_array[i][j] = mine_count;                                          
                                }
                            }
                        }           
                    }
                }
            }   
        }     
    }
}
/**********************************************************************************************************************
    GAMEOVER ACTION PROMPT
*/
void gameover_action_prompt(void* p_arg, int status) {
    struct arg *p = p_arg;
    char* win_message = "YOU WIN!";
    char* lose_message = "YOU LOSE!";
    char a_input[10];
    char* a_message = "Press (Q) to quit, (R) to retry, (C) to return chat: ";
    char* b_message = "Press (Q) to quit, (R) to retry: ";

    // Send message to player according to game end status and further action prompt
    send(*p->socket, clear_message, strlen(clear_message), 0);
    while(1) {
        send(*p->socket, clear_message, strlen(clear_message), 0);
        if(status == 1) {
            send(*p->socket, lose_message, strlen(lose_message), 0);
            send(*p->socket, new_line, strlen(new_line), 0);
        }
        else {
            send(*p->socket, win_message, strlen(win_message), 0);
            send(*p->socket, new_line, strlen(new_line), 0);
        }   
        send(*p->socket, new_line, strlen(new_line), 0);
        if(client_num != CLIENTS) {
            send(*p->socket, a_message, strlen(a_message), 0);
            result = recv(*p->socket, a_input, 200, 0); 

            if(result > 0) {
                if(*a_input == 'Q' || *a_input == 'q') {
                    player_quit = 1;
                    clear_mine_sweeper();
                    pthread_exit(NULL);
                }
                else if(*a_input == 'R' || *a_input == 'r') {
                    clear_mine_sweeper();
                    game_handling(p);
                }
                else if(*a_input == 'C' || *a_input == 'c') {
                    clear_mine_sweeper();
                    pthread_exit(NULL);
                }
            }
            else if(result == 0) {
                player_quit = 1;
                clear_mine_sweeper();
                pthread_exit(NULL);
            }
        }
        else {
            send(*p->socket, b_message, strlen(b_message), 0);
            result = recv(*p->socket, a_input, 200, 0); 

            if(result > 0) {
                if(*a_input == 'Q' || *a_input == 'q') {
                    player_quit = 1;
                    clear_mine_sweeper();
                    pthread_exit(NULL);
                }
                else if(*a_input == 'R' || *a_input == 'r') {
                    clear_mine_sweeper();
                    game_handling(p);
                }
            }
            else if(result == 0) {
                player_quit = 1;
                clear_mine_sweeper();
                pthread_exit(NULL);
            }
        }
    }
}
/**********************************************************************************************************************
    CHECK PLAYER`S MOVE
*/
void check_move(void* p_arg, int row_settings, int column_settings) {
    struct arg *p = p_arg;

    grid_numbers = malloc(sizeof(int) * column_settings);
    vertical_numbers = malloc(sizeof(int) * column_settings);
    grid_arr = malloc(sizeof(int) * column_settings * row_settings);
    int mine_count, lose;
    
    //  Check if players guess hit mine
    if(p->level.grid_array[x][y] == 9) {
        show_prompt = 1;
        lose = 1;
        gameover_action_prompt(p, lose); 
    }
    
    else{ 
        //  Check how many mines are near taken coordinates
        for (int i = 0; i < row_settings; i++)
        {
            for (int j = 0; j < column_settings; j++)
            {
                if(p->level.grid_array[x][y] != 9) {
                    mine_count = 0;
                    for (int ii = -1; ii <= 1; ii++)
                    {
                        if((x + ii) >= 0 && (x + ii) < row_settings) {
                            for (int jj = -1; jj <= 1; jj++)
                            {
                                if((y + jj) >= 0 && (y + jj) < column_settings) {
                                    if(p->level.grid_array[x + ii][y + jj] == 9) {
                                        mine_count++;
                                    }
                                }
                            }         
                        }
                    }
                    p->level.grid_array[x][y] = mine_count;                 
                }
            }        
        }

        //  Set guessed coordinates to 
        p->level.taken_locations[x][y] = p->level.free_locations[x][y];

        //  Reveal all zero spots
        if(mine_count == 0) {    
            reveal_zero_numbers(p, x, y, row_settings, column_settings);
        }
        //  Clear console
        send(*p->socket, clear_message, strlen(clear_message), 0);
        send(*p->socket, welcome_message, strlen(welcome_message), 0);
       
        //  Debug mode print
        if(DEBUG_MODE) {
            for(int i = 0; i < row_settings; i++) {
                for(int j = 0; j < column_settings; j++) {
                    sprintf(grid_arr,"%d ", p->level.grid_array[i][j]);
                    send(*p->socket, grid_arr, strlen(grid_arr), 0);
                }
                send(*p->socket, new_line, strlen(new_line), 0);
            }
            send(*p->socket, new_line, strlen(new_line), 0);
        }

        //  Print array and taken guesses   
        for(int i = 0; i < column_settings; i++) {
            if(i == 0) {
                send(*p->socket, grid_left_empty, strlen(grid_left_empty), 0);
            }
            sprintf(grid_numbers, "%d ", i);
            send(*p->socket, grid_numbers, strlen(grid_numbers), 0);
        }
        send(*p->socket, new_line, strlen(new_line), 0);
        for (int i = 0; i < column_settings; i++)
        {
            if(i == 0) {
                send(*p->socket, grid_left_empty, strlen(grid_left_empty), 0);
            }
            send(*p->socket, vertical_bar, strlen(vertical_bar), 0);
        }
        send(*p->socket, new_line, strlen(new_line), 0);

        //  Taken guesses
        for(int i = 0; i < row_settings; i++) {
            for(int j = 0; j < column_settings; j++) {
                if(j == 0) {
                    sprintf(vertical_numbers, "%d- ", i);
                    send(*p->socket, vertical_numbers, strlen(vertical_numbers), 0);
                }
                if(p->level.taken_locations[i][j]) {
                    sprintf(grid_arr,"%d ", p->level.grid_array[i][j]);
                    send(*p->socket, grid_arr, strlen(grid_arr), 0);   
                }
                else {
                    send(*p->socket, horizontal_bar, strlen(horizontal_bar), 0);
                }     
            }
            send(*p->socket, new_line, strlen(new_line), 0);
        }
        show_prompt = 0;
    }
}
/**********************************************************************************************************************
    CHECK IF PLAYER HAS WON
*/
void check_if_win(void* p_arg, int row_settings, int column_settings) {
    struct arg *p = p_arg;
    int response, win;
    response = memcmp(p->level.free_locations, p->level.taken_locations, row_settings * column_settings * sizeof(int));
    
    if (response == 0) {
        show_prompt = 1;
        win = 0;
        gameover_action_prompt(p, win);
    }
}
/**********************************************************************************************************************
    REVEAL ALL ZERO SPOTS IN GRID
*/
void reveal_zero_numbers(void* p_arg, int x, int y, int row_settings, int column_settings) { 
    struct arg *p = p_arg; 
    for (int i = - (row_settings - 1); i <= (row_settings - 1); i++)
    {
        if((x + i) >= 0 && (x + i) < row_settings) {
            for (int j = - (column_settings - 1); j <= (column_settings - 1); j++)
            {
                if((y + j) >= 0 && (y + j) < column_settings) {
                    
                    if(p->level.grid_array[x + i][y + j] != 9 && p->level.grid_array[x + i][y + j] == 0) {
                        p->level.taken_locations[x + i][y + j] = p->level.free_locations[x][y];    
                    }
                }
            }         
        }
    }
} 