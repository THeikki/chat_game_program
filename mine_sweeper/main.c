#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define ROWS 8
#define COLUMNS 8
#define MINES 10
#define DEBUG_MODE 1    //  Shows mine locations on grid [1 = on    0 = off]

void print_grid();
void get_random_mine_locations(int max, int min, int count);
void setup_mines(int* p_array);
void get_player_input();
void check_move();
void check_if_win();
void reveal_zero_numbers(int x, int y);

int *p_array = NULL;

int mine_numbers[MINES];
int mine_locations[ROWS][COLUMNS];
int free_locations[ROWS][COLUMNS];
int taken_locations[ROWS][COLUMNS];
int grid_array[ROWS][COLUMNS];

int x;
int y;
int gameover;
char c;
int t;

int main() { 
    p_array = mine_numbers;
    
    get_random_mine_locations(COLUMNS -1, 0, MINES);
    print_grid();
    setup_mines(p_array);
    do
    {
        get_player_input();   
        check_move();
        check_if_win();
          
    } while(1);
           
    return 0;
}
/**********************************************************************************************************************
    PRINTS GRID
*/
void print_grid() {
    
    for(int i = 0; i < COLUMNS; i++) {
        if(i == 0) {
            printf("   ");
        }
        printf("%d ", i);
    }
    printf("\n");
    for (int i = 0; i < COLUMNS; i++)
    {
        if(i == 0) {
            printf("   ");
        }
        printf("| ");
    }
    printf("\n");

    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLUMNS; j++) {
            if(j == 0) {
                printf("%d- ", i);
            }
            printf("- ");        
        }
        printf("\n");    
    }  
}
/**********************************************************************************************************************
    GETS MINE RANDOM NUMBERS
*/
void get_random_mine_locations(int max, int min, int count) {
    srand(time(NULL));
    int temp = 0;

    for(int i = 0; i < count; i++) {
        mine_numbers[i] = rand() % (max-min);
    }
}
/**********************************************************************************************************************
    GETS PLAYER INPUT
*/
void get_player_input() {

    char x_input[10];
    char y_input[10];
    
    if(gameover != 1) {
        /**/
        while (*x_input != '0' && *x_input != '1' && *x_input != '2' && *x_input != '3' 
                               && *x_input != '4' && *x_input != '5' && *x_input != '6' 
                               && *x_input != '7') {
            printf("Give (X) [0-%d]: ", COLUMNS - 1); 
            gets(x_input);    
        }
        //  Convert string input to integer
        x = atoi(x_input);
        
        while (*y_input != '0' && *y_input != '1' && *y_input != '2' && *y_input != '3' 
                               && *y_input != '4' && *y_input != '5' && *y_input != '6' 
                               && *y_input != '7') {
            printf("Give (Y) [0-%d]: ", COLUMNS - 1);
            gets(y_input);
        }
        y = atoi(y_input);
    }
}
/**********************************************************************************************************************
    SETUP MINES IN ARRAY
*/
void setup_mines(int *p_array) {
    int *a = (int*)p_array;
    int mine_num = 0;
    int mine_count;
    int zero_count;

    //  Set mine locations to array
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLUMNS; j++) {
            if(j == *a && mine_num < MINES ) {
                mine_locations[i][j] = 9;
                grid_array[i][j] = mine_locations[i][j];
                *a++;
                mine_num++;
            }
            else {
                free_locations[i][j] = 1;
                grid_array[i][j] = mine_locations[i][j];
            }
        }
    }
    
    // Arrange mines and neighbours in array
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            if(grid_array[i][j] != 9) {
                mine_count = 0;
                for (int ii = -1; ii <= 1; ii++)
                {
                    if((i + ii) >= 0 && (i + ii) < ROWS) {
                        
                        for (int jj = -1; jj <= 1; jj++)
                        {
                            if((j + jj) >= 0 && (j + jj) < COLUMNS) {
                               
                                if(grid_array[i + ii][j + jj] == 9) {
                                    mine_count++;
                                    grid_array[i][j] = mine_count;                                          
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
    CHECK PLAYER`S MOVE
*/
void check_move() {

    int mine_count;
    
    if(grid_array[x][y] == 9) {
        system("cls");
        printf("YOU LOSE!!\n");
        printf("Press [Q] to continue...");
        scanf("%c", &c);
        gameover = 1;
        
        switch (c)
        {
            case 'q':
            case 'Q':
                system("cls");
                exit(0);
            default:
                break;
        }
    }
    
    else{
        
        //  Check how many mines are near taken coordinates
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLUMNS; j++)
            {
                if(grid_array[x][y] != 9) {
                    mine_count = 0;
                    for (int ii = -1; ii <= 1; ii++)
                    {
                        if((x + ii) >= 0 && (x + ii) < ROWS) {
                            for (int jj = -1; jj <= 1; jj++)
                            {
                                if((y + jj) >= 0 && (y + jj) < COLUMNS) {
                                    if(grid_array[x + ii][y + jj] == 9) {
                                        mine_count++;
                                    }
                                }
                            }         
                        }
                    }
                    grid_array[x][y] = mine_count;                 
                }
            }        
        }

        //  Set guessed coordinates to 
        taken_locations[x][y] = free_locations[x][y];

        //  Reveal all zero spots
        if(mine_count == 0) {    
            reveal_zero_numbers(x, y);
        }
        //  Clear console
        system("cls");
       
        //  Debug mode print
        if(DEBUG_MODE) {
            for(int i = 0; i < ROWS; i++) {
                for(int j = 0; j < COLUMNS; j++) {
                    printf("%d ", grid_array[i][j]);
                }
                printf("\n");
            }
            printf("\n");
        }

        //  Print array and taken guesses   
        for(int i = 0; i < COLUMNS; i++) {
            if(i == 0) {
                printf("   ");
            }
            printf("%d ", i);
        }
        printf("\n");
        for (int i = 0; i < COLUMNS; i++)
        {
            if(i == 0) {
                printf("   ");
            }
            printf("| ");
        }
        printf("\n");

        //  Taken guesses
        for(int i = 0; i < ROWS; i++) {
            for(int j = 0; j < COLUMNS; j++) {
                if(j == 0) {
                    printf("%d- ", i);
                }
                if(taken_locations[i][j]) {
                    printf("%d ", grid_array[i][j]);   
                }
                else printf("- ");
            }
            printf("\n");
        }
        gameover = 0;
    }
}
/**********************************************************************************************************************
    CHECK IF PLAYER HAS WON
*/
void check_if_win() {
    int response;
    response = memcmp(free_locations, taken_locations, ROWS * COLUMNS * sizeof(int));

    if (response == 0) {
        gameover = 1;
        system("cls");
        printf("YOU WIN!\n");
        printf("Press [Q] to continue...\n");
        scanf("%c", &c);
        switch (c)
        {
            case 'q':
            case 'Q':
                system("cls");
                exit(0);
            default:
                break;
        }
    }
}
/**********************************************************************************************************************
    REVEAL ALL ZERO SPOTS IN GRID
*/
void reveal_zero_numbers(int x, int y) {
    
    for (int i = - (ROWS - 1); i <= (ROWS - 1); i++)
    {
        if((x + i) >= 0 && (x + i) < ROWS) {
            for (int j = - (COLUMNS - 1); j <= (COLUMNS - 1); j++)
            {
                if((y + j) >= 0 && (y + j) < COLUMNS) {
                    
                    if(grid_array[x + i][y + j] != 9 && grid_array[x + i][y + j] == 0) {
                        taken_locations[x + i][y + j] = free_locations[x][y];    
                    }
                }
            }         
        }
    }
} 