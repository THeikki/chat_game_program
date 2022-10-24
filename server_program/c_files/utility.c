#include "../header_files/utility.h"

/*******************************************************************************************************
    SEND MESSAGES TO CLIENTS
*/
void broadcast(unsigned short address, char* message) {
    char* text = malloc(sizeof(unsigned short) + strlen(message) + 10);
    sprintf(text, "[%hu]: %s\n\r", address, message);

    for(int i=0; i<client_num; i++) {   
        send(clients[i], text, strlen(text), 0);
    }
    free(text);
}
/*******************************************************************************************************
    CHECK ERRORS WHEN CREATING SOCKET
*/
int check(int error_num, char* error) {
    int num = error_num;

    if(num == INVALID_SOCKET) {
        closesocket(server_socket);
        WSACleanup();
        printf(error);
        return 1;
    }
    else if(num == SOCKET_ERROR) {
        closesocket(server_socket);
        WSACleanup();
        printf(error);
        return 1;
    }
    else {
        return 0;
    }
}
/*******************************************************************************************************
    CLOSE SERVER
*/
int shutdown_connection() {
    int result;
    closesocket(server_socket);
    result = WSACleanup();
    
    if(result) {
        printf("Cleanup failed");
        return 1;
    }
    printf("Shutting down...");
}
/*******************************************************************************************************
    CLEAR MINE SWEEPER GAME 
*/
void clear_mine_sweeper() {
    
    free(grid_numbers);
    grid_numbers = NULL;
    free(vertical_numbers);
    vertical_numbers = NULL;
    free(grid_arr);
    grid_arr = NULL;
}