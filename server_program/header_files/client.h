#ifndef CLIENT_H
#define CLIENT_H

#include "globals.h"
#include "utility.h"
#include "server.h"
#include "game.h"

extern SOCKET client_socket;
extern SOCKET clients[CLIENTS];
extern struct sockaddr_in client_address;
extern socklen_t clientLen;

extern int player_num;

void* client_handling(void* client_args);

#endif