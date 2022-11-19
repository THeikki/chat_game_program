#ifndef SERVER_H
#define SERVER_H

#include "client.h"
#include "globals.h"
#include "utility.h"

extern SOCKET server_socket;
extern struct sockaddr_in service;
extern pthread_t client_thread, game_thread;

extern int client_num;
extern int array_index;

struct array {
    int moves_left;
    int is_blocked;
    int mine_locations[HARD_ROWS][HARD_COLUMNS];
    int free_locations[HARD_ROWS][HARD_COLUMNS];
    int locked_locations[HARD_ROWS][HARD_COLUMNS];
    int taken_locations[HARD_ROWS][HARD_COLUMNS];
    int grid_array[HARD_ROWS][HARD_COLUMNS];
};

static struct client {
    int* socket;
    int* index;
    struct array level;
}client_struct;

int initialize_winsock();
int create_server();

#endif