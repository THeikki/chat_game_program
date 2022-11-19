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
/*******************************************************************************************************
    GAME HANDLING FUNCTION
*/
void* game_handling(void* client_args) {
    struct client *p_client = client_args;
   
    int difficulty = 0;
    int zeros_revealed = 0;
    int row_settings = 0;
    int column_settings = 0;
    int mine_settings = 0; 
    show_prompt = 0;
    player_quit = 0;

    send(*p_client->socket, clear_message, strlen(clear_message), 0);
    send(*p_client->socket, welcome_message, strlen(welcome_message), 0);
    difficulty = choose_difficulty_level(p_client);

    if(difficulty == 1) {
        p_client->level.moves_left = 6;
        p_client->level.is_blocked = 0;
        row_settings = EASY_ROWS;
        column_settings = EASY_COLUMNS;
        p_client->level.mine_locations[EASY_ROWS][EASY_COLUMNS];
        p_client->level.free_locations[EASY_ROWS][EASY_COLUMNS];
        p_client->level.locked_locations[EASY_ROWS][EASY_COLUMNS];
        p_client->level.taken_locations[EASY_ROWS][EASY_COLUMNS];
        p_client->level.grid_array[EASY_ROWS][EASY_COLUMNS];
    }
    else if(difficulty == 2) {
        p_client->level.moves_left = 24;
        p_client->level.is_blocked = 0;
        row_settings = MEDIUM_ROWS;
        column_settings = MEDIUM_COLUMNS;
        p_client->level.mine_locations[MEDIUM_ROWS][MEDIUM_COLUMNS];
        p_client->level.locked_locations[MEDIUM_ROWS][MEDIUM_COLUMNS];
        p_client->level.free_locations[MEDIUM_ROWS][MEDIUM_COLUMNS];
        p_client->level.taken_locations[MEDIUM_ROWS][MEDIUM_COLUMNS];
        p_client->level.grid_array[MEDIUM_ROWS][MEDIUM_COLUMNS];
    }
    else if(difficulty == 3) {
        p_client->level.moves_left = 54;
        p_client->level.is_blocked = 0;
        row_settings = HARD_ROWS;
        column_settings = HARD_COLUMNS;
        p_client->level.mine_locations[HARD_ROWS][HARD_COLUMNS];
        p_client->level.locked_locations[HARD_ROWS][HARD_COLUMNS];
        p_client->level.free_locations[HARD_ROWS][HARD_COLUMNS];
        p_client->level.taken_locations[HARD_ROWS][HARD_COLUMNS];
        p_client->level.grid_array[HARD_ROWS][HARD_COLUMNS];
    }

    //  Initialize struct arrays to zero
    for(int i = 0; i < row_settings; i++) {
        for(int j = 0; j < column_settings; j++) {
            p_client->level.mine_locations[i][j] = 0;
            p_client->level.locked_locations[i][j] = 0;
            p_client->level.free_locations[i][j] = 0;
            p_client->level.taken_locations[i][j] = 0;
            p_client->level.grid_array[i][j] = 0;
        }
    }

    send(*p_client->socket, clear_message, strlen(clear_message), 0);
    send(*p_client->socket, welcome_message, strlen(welcome_message), 0);

    print_number_of_mines_grid(p_client);
    print_grid(p_client, row_settings, column_settings);
    setup_mines(p_client, row_settings, column_settings, mine_settings, difficulty);
   
    do {
        get_player_input(p_client, column_settings, difficulty);
        check_move(p_client, row_settings, column_settings);
        check_if_win(p_client, row_settings, column_settings);  
    } while (1);
}
/**********************************************************************************************************************
    PROMPT USER TO SELECT DIFFICULTY LEVEL
*/
int choose_difficulty_level(void* client_args) {
    struct client *p_client = client_args;
    int difficulty_num;
    char difficulty_input[10];
    char* message = "Choose difficulty level:\n\r"
                    "[1] = Easy              \n\r"
                    "[2] = Average           \n\r"
                    "[3] = Hard              \n\r"
                    "                        \n\r";

    while (*difficulty_input != '1' && *difficulty_input != '2' && *difficulty_input != '3')
    {
        send(*p_client->socket, clear_message, strlen(clear_message), 0);
        send(*p_client->socket, welcome_message, strlen(welcome_message), 0);
        send(*p_client->socket, message, strlen(message), 0);
        result = recv(*p_client->socket, difficulty_input, 200, 0);

        if(result <0) {
            player_quit = 1;
            pthread_exit(NULL);
        }
    }
    send(*p_client->socket, clear_message, strlen(clear_message), 0);
    send(*p_client->socket, welcome_message, strlen(welcome_message), 0);
    difficulty_num = atoi(difficulty_input);
   
    return difficulty_num;
}
/**********************************************************************************************************************
    PRINTS GRID
*/
void print_grid(void* client_args, int row_settings, int column_settings) {
    struct client *p_client = client_args;
    char* horizontal_numbers = malloc(sizeof(int) * column_settings);
    char* vertical_numbers = malloc(sizeof(int) * column_settings);
   
    for(int i = 0; i < column_settings; i++) {
        if(i == 0) {
            send(*p_client->socket, grid_left_empty, strlen(grid_left_empty), 0);
        }
        sprintf(horizontal_numbers, "%d ", i);
        send(*p_client->socket, horizontal_numbers, strlen(horizontal_numbers), 0);
    }
  
    send(*p_client->socket, new_line, strlen(new_line), 0);
    for (int i = 0; i < column_settings; i++)
    {
        if(i == 0) {
            send(*p_client->socket, grid_left_empty, strlen(grid_left_empty), 0);
        }
        send(*p_client->socket, vertical_bar, strlen(vertical_bar), 0);
    }
    send(*p_client->socket, new_line, strlen(new_line), 0);

    for(int i = 0; i < row_settings; i++) {
        for(int j = 0; j < column_settings; j++) {
            if(j == 0) {
                sprintf(vertical_numbers, "%d- ", i);
                send(*p_client->socket, vertical_numbers, strlen(vertical_numbers), 0);
            }
            send(*p_client->socket, horizontal_bar, strlen(horizontal_bar), 0);      
        }
        send(*p_client->socket, new_line, strlen(new_line), 0);    
    }
    free(horizontal_numbers);
    free(vertical_numbers); 
    horizontal_numbers = NULL;
    vertical_numbers = NULL; 
}
/**********************************************************************************************************************
    GETS PLAYER INPUT
*/
void get_player_input(void* client_args, int column_settings, int difficulty) {
    struct client *p_client = client_args;  
    char* x_input_prompt = malloc(sizeof(int) + 30);
    char x_input[10];
    char* y_input_prompt = malloc(sizeof(int) + 30);
    char y_input[10];
    sprintf(x_input_prompt,"Give (X) [0-%d]: ", column_settings - 1);
    sprintf(y_input_prompt,"Give (Y) [0-%d]: ", column_settings - 1);
   
    if(show_prompt != 1) {
        if(difficulty == 1) {
            while (*x_input != '0' && *x_input != '1' && *x_input != '2') {
   
                send(*p_client->socket, x_input_prompt, strlen(x_input_prompt), 0);
                result = recv(*p_client->socket, x_input, 200, 0); 
                
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
   
                send(*p_client->socket, x_input_prompt, strlen(x_input_prompt), 0);
                result = recv(*p_client->socket, x_input, 200, 0); 
                
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
   
                send(*p_client->socket, x_input_prompt, strlen(x_input_prompt), 0);
                result = recv(*p_client->socket, x_input, 200, 0); 
                
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
            while (*y_input != '0' && *y_input != '1' && *y_input != '2') {

                send(*p_client->socket, y_input_prompt, strlen(y_input_prompt), 0);
                result = recv(*p_client->socket, y_input, 200, 0); 
                
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

                send(*p_client->socket, y_input_prompt, strlen(y_input_prompt), 0);
                result = recv(*p_client->socket, y_input, 200, 0); 
                
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

                send(*p_client->socket, y_input_prompt, strlen(y_input_prompt), 0);
                result = recv(*p_client->socket, y_input, 200, 0); 
                
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
void setup_mines(void* client_args, int row_settings, int column_settings, int mine_settings, int difficulty) {
    struct client *p_client = client_args;
    int mine_count;
    int mine = 0, mine2 = 0, mine3 = 0;
    int max = 0;
    int min = 0;
   
    srand(time(NULL));
    
    if(difficulty == 1) {
        min = 0;
        max = 3;
        for (int i = 0; i < row_settings; i++)
        { 
            mine = rand() % (max - min);  
            for (int j = 0; j < column_settings; j++)
            {
                if(j == mine) { 
                    p_client->level.mine_locations[i][j] = 9;
                    p_client->level.grid_array[i][j] = p_client->level.mine_locations[i][j];
                }
                else {
                    p_client->level.free_locations[i][j] = 1;
                    p_client->level.grid_array[i][j] = p_client->level.mine_locations[i][j];
                } 
            }             
        }
    }

    else if(difficulty == 2) {
        min = 0;
        max = 6;
        for (int i = 0; i < row_settings; i++)
        { 
            mine = rand() % (max - min);  
            while (mine2 == mine)
            {
                mine2 = rand() % (max - min);
            }
            for (int j = 0; j < column_settings; j++)
            {
                if(j == mine || j == mine2) { 
                    p_client->level.mine_locations[i][j] = 9;
                    p_client->level.grid_array[i][j] = p_client->level.mine_locations[i][j];
                }
                else {
                    p_client->level.free_locations[i][j] = 1;
                    p_client->level.grid_array[i][j] = p_client->level.mine_locations[i][j];
                } 
            }             
        }
    }

    else if(difficulty == 3) {
        min = 0;
        max = 9;
        for (int i = 0; i < row_settings; i++)
        { 
            mine = rand() % (max - min);  
            while (mine2 == mine)
            {
                mine2 = rand() % (max - min);
            }
            while (mine3 == mine2 || mine3 == mine)
            {
                mine3 = rand() % (max - min);
            }
            for (int j = 0; j < column_settings; j++)
            {
                if(j == mine || j == mine2 || j == mine3) {
                    p_client->level.mine_locations[i][j] = 9;
                    p_client->level.grid_array[i][j] = p_client->level.mine_locations[i][j];
                }
                else {
                    p_client->level.free_locations[i][j] = 1;
                    p_client->level.grid_array[i][j] = p_client->level.mine_locations[i][j];
                } 
            }             
        }
    }
   
    // Arrange neighbour numbers of mines in array
    for (int i = 0; i < row_settings; i++)
    {
        for (int j = 0; j < column_settings; j++)
        {
            if(p_client->level.grid_array[i][j] != 9) {
                mine_count = 0;
                for (int ii = -1; ii <= 1; ii++)
                {
                    if((i + ii) >= 0 && (i + ii) < row_settings) {
                        
                        for (int jj = -1; jj <= 1; jj++)
                        {
                            if((j + jj) >= 0 && (j + jj) < column_settings) {
                               
                                if(p_client->level.grid_array[i + ii][j + jj] == 9) {
                                    mine_count++;
                                    p_client->level.grid_array[i][j] = mine_count;                                          
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
void gameover_action_prompt(void* client_args, int status) {
    struct client *p_client = client_args;
    char* win_message = "YOU WIN!";
    char* lose_message = "YOU LOSE!";
    char a_input[10];
    char* a_message = "Press (Q) to quit, (R) to retry, (C) to return chat: ";
    char* b_message = "Press (Q) to quit, (R) to retry: ";

    // Send message to player according to game end status and further action prompt
    send(*p_client->socket, clear_message, strlen(clear_message), 0);
    while(1) {
        send(*p_client->socket, clear_message, strlen(clear_message), 0);
        if(status == 1) {
            send(*p_client->socket, lose_message, strlen(lose_message), 0);
            send(*p_client->socket, new_line, strlen(new_line), 0);
        }
        else {
            send(*p_client->socket, win_message, strlen(win_message), 0);
            send(*p_client->socket, new_line, strlen(new_line), 0);
        }   
        send(*p_client->socket, new_line, strlen(new_line), 0);
        if(client_num != CLIENTS) {
            send(*p_client->socket, a_message, strlen(a_message), 0);
            result = recv(*p_client->socket, a_input, 200, 0); 

            if(result > 0) {
                if(*a_input == 'Q' || *a_input == 'q') {
                    player_quit = 1;
                    clear_mine_sweeper();
                    pthread_exit(NULL);
                }
                else if(*a_input == 'R' || *a_input == 'r') {
                    clear_mine_sweeper();
                    game_handling(p_client);
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
            send(*p_client->socket, b_message, strlen(b_message), 0);
            result = recv(*p_client->socket, a_input, 200, 0); 

            if(result > 0) {
                if(*a_input == 'Q' || *a_input == 'q') {
                    player_quit = 1;
                    clear_mine_sweeper();
                    pthread_exit(NULL);
                }
                else if(*a_input == 'R' || *a_input == 'r') {
                    clear_mine_sweeper();
                    game_handling(p_client);
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
void check_move(void* client_args, int row_settings, int column_settings) {
    struct client *p_client = client_args;

    grid_numbers = malloc(sizeof(int) * column_settings);
    vertical_numbers = malloc(sizeof(int) * column_settings);
    grid_arr = malloc(sizeof(int) * column_settings * row_settings);
    int mine_count, lose;
    
    //  Check if players guess hit mine
    if(p_client->level.grid_array[x][y] == 9) {
        show_prompt = 1;
        lose = 1;
        gameover_action_prompt(p_client, lose); 
    }
    
    else{ 
    
        //  Check how many mines are near given coordinates
        for (int i = 0; i < row_settings; i++)
        {
            for (int j = 0; j < column_settings; j++)
            {
                if(p_client->level.grid_array[x][y] != 9) {
                    mine_count = 0;
                    for (int ii = -1; ii <= 1; ii++)
                    {
                        if((x + ii) >= 0 && (x + ii) < row_settings) {
                            for (int jj = -1; jj <= 1; jj++)
                            {
                                if((y + jj) >= 0 && (y + jj) < column_settings) {
                                    if(p_client->level.grid_array[x + ii][y + jj] == 9) {
                                        mine_count++;
                                    }
                                }
                            }         
                        }
                    }
                    p_client->level.grid_array[x][y] = mine_count;         
                }
            }        
        }

        //  Set guessed coordinates to taken array 
        p_client->level.taken_locations[x][y] = p_client->level.free_locations[x][y];

        //  Reveal all zero spots
        if(mine_count == 0 && p_client->level.is_blocked != 1) {  
            reveal_zero_numbers(p_client, x, y, row_settings, column_settings);
        }
        // Check if gues is already taken
        else if(p_client->level.grid_array[x][y] != p_client->level.locked_locations[x][y]){
            p_client->level.moves_left--;
        }
        
        //  Clear console
        send(*p_client->socket, clear_message, strlen(clear_message), 0);
        send(*p_client->socket, welcome_message, strlen(welcome_message), 0);
    
        //  Debug mode print
        if(DEBUG_MODE) {
            for(int i = 0; i < row_settings; i++) {
                for(int j = 0; j < column_settings; j++) {
                    sprintf(grid_arr,"%d ", p_client->level.grid_array[i][j]);
                    send(*p_client->socket, grid_arr, strlen(grid_arr), 0);
                }
                send(*p_client->socket, new_line, strlen(new_line), 0);
            }
            send(*p_client->socket, new_line, strlen(new_line), 0);
        }

        // Print number of mines in level
        print_number_of_mines_grid(p_client);

        //  Print mine array   
        for(int i = 0; i < column_settings; i++) {
            if(i == 0) {
                send(*p_client->socket, grid_left_empty, strlen(grid_left_empty), 0);
            }
            sprintf(grid_numbers, "%d ", i);
            send(*p_client->socket, grid_numbers, strlen(grid_numbers), 0);
        }
        send(*p_client->socket, new_line, strlen(new_line), 0);
        for (int i = 0; i < column_settings; i++)
        {
            if(i == 0) {
                send(*p_client->socket, grid_left_empty, strlen(grid_left_empty), 0);
            }
            send(*p_client->socket, vertical_bar, strlen(vertical_bar), 0);
        }
        send(*p_client->socket, new_line, strlen(new_line), 0);

        //  Print taken guesses
        for(int i = 0; i < row_settings; i++) {
            for(int j = 0; j < column_settings; j++) {
                if(j == 0) {
                    sprintf(vertical_numbers, "%d- ", i);
                    send(*p_client->socket, vertical_numbers, strlen(vertical_numbers), 0);
                }
                if(p_client->level.taken_locations[i][j]) {
                    sprintf(grid_arr,"%d ", p_client->level.grid_array[i][j]);
                    send(*p_client->socket, grid_arr, strlen(grid_arr), 0);   
                }
                else {
                    send(*p_client->socket, horizontal_bar, strlen(horizontal_bar), 0);
                }     
            }
            send(*p_client->socket, new_line, strlen(new_line), 0);
        }
        show_prompt = 0;

        // Set guessed location to locked array
        p_client->level.locked_locations[x][y] = p_client->level.grid_array[x][y];
    }
}
/**********************************************************************************************************************
    PRINT NUMBER OF MINES IN GRID
*/
void print_number_of_mines_grid(void* client_args) {
    struct client *p_client = client_args;
    char* level_mine_count = malloc(sizeof(int) + sizeof(char) * 20);
    
    sprintf(level_mine_count, "SAFE MOVES LEFT: %d", p_client->level.moves_left);
    send(*p_client->socket, level_mine_count, strlen(level_mine_count), 0);
    send(*p_client->socket, new_line, strlen(new_line), 0);
    send(*p_client->socket, new_line, strlen(new_line), 0);
    free(level_mine_count);
    level_mine_count = NULL;
}
/**********************************************************************************************************************
    CHECK IF PLAYER HAS WON
*/
void check_if_win(void* client_args, int row_settings, int column_settings) {
    struct client *p_client = client_args;
    int win;
    
    if (p_client->level.moves_left == 0) {
        show_prompt = 1;
        win = 0;
        gameover_action_prompt(p_client, win);
    }
}
/**********************************************************************************************************************
    REVEAL ALL ZERO SPOTS IN GRID
*/
void reveal_zero_numbers(void* client_args, int x, int y, int row_settings, int column_settings) { 
    struct client *p_client = client_args; 
    int zero_nums = 0;
    for (int i = - (row_settings - 1); i <= (row_settings - 1); i++)
    {
        if((x + i) >= 0 && (x + i) < row_settings) {
            for (int j = - (column_settings - 1); j <= (column_settings - 1); j++)
            {
                if((y + j) >= 0 && (y + j) < column_settings) {
                    
                    if(p_client->level.grid_array[x + i][y + j] != 9 && p_client->level.grid_array[x + i][y + j] == 0) {
                        p_client->level.taken_locations[x + i][y + j] = p_client->level.free_locations[x][y]; 
                        zero_nums++;
                    }
                }
            }         
        }
    }
    printf("ZEROS:%d\n", zero_nums);
    p_client->level.moves_left = p_client->level.moves_left - zero_nums;
    p_client->level.is_blocked = 1;
} 