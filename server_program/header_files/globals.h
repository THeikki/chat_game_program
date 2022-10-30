#ifndef GLOBALS_H
#define GLOBALS_H

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define PORT 8081
#define ADDRESS "127.0.0.1"
//#define ADDRESS "192.168.1.100"
#define BUFFER 1024
#define CLIENTS 2
#define GAMERS 2
#define EASY_ROWS 3
#define EASY_COLUMNS 3
#define MEDIUM_ROWS 6
#define MEDIUM_COLUMNS 6
#define HARD_ROWS 9
#define HARD_COLUMNS 9
#define DEBUG_MODE 1    //  Shows mine locations on grid [1 = on    0 = off]

#endif