#ifndef UTILITY_H
#define UTILITY_H

#include "globals.h"
#include "server.h"
#include "client.h"

int shutdown_connection();
int check(int, char*);
void broadcast(unsigned short, char*);
void clear_mine_sweeper();
void clear_array_at_start(void* p_arg);

#endif