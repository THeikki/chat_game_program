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
    int mine_locations[HARD_ROWS][HARD_COLUMNS];
    int free_locations[HARD_ROWS][HARD_COLUMNS];
    int taken_locations[HARD_ROWS][HARD_COLUMNS];
    int grid_array[HARD_ROWS][HARD_COLUMNS];
};

static struct arg {
    int* socket;
    int* index;
    struct array level;
}arguments;

int initialize_winsock();
int create_server();

#endif