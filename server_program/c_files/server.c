#include "../header_files/server.h"

SOCKET server_socket;
struct sockaddr_in service;
pthread_t client_thread;

int client_num = 0;
int array_index = 0;

/*******************************************************************************************************
    INITIALIZE WINSOCK
*/
int initialize_winsock() {
    int response;
    WSADATA wsaData;
    response = WSAStartup(MAKEWORD(2, 2), &wsaData);  

    if(response != 0) {
        printf("WSAStartup failed with error: %d\n", response);
        return 1;
    }
}
/******************************************************************************************************* 
    CREATE SERVER
*/
int create_server() {
    int response;
    struct client *p_client;
    char* info_message = "Chat is full..";
    
    // Create socket
    check(server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), "Failed to create socket\n");  // UTILITY LIBRARY FUNCTION [ CHECK ]

    //  Bind address
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ADDRESS);
    service.sin_port = htons(PORT);
    check(bind(server_socket, (struct sockaddr *) &service, sizeof (service)), "Bind failed\n");   

    //  Listen connection requests
    check(listen(server_socket, CLIENTS) == SOCKET_ERROR, "Listen failed\n");
    printf("Listening on: [%s:%d]\n", ADDRESS, PORT);
    
    do { 
        check(client_socket = accept(server_socket, NULL, NULL), "Accept failed\n");
        getpeername(client_socket, (struct sockaddr *) &client_address, &clientLen);
        printf("Client connected at: %s: %d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        if(client_num < CLIENTS) {
            p_client = malloc(sizeof(client_struct));
            p_client->socket = malloc(sizeof(int));
            p_client->index = malloc(sizeof(int));  
            *p_client->socket = client_socket;
            *p_client->index = array_index;
            clients[array_index] = *p_client->socket;
            client_num++;
            array_index++;
           
            //  Create new thread
            response = pthread_create(&client_thread, NULL, (void *)client_handling, p_client);
            if(response) {
                printf("Error:unable to create thread, %d\n", WSAGetLastError);
                free(p_client->socket);
                free(p_client->index);
                free(p_client);
                p_client->socket = NULL;
                p_client->index = NULL;
                p_client = NULL;
                exit(-1);
            }
        }
        send(client_socket, info_message, strlen(info_message), 0);    
   } while(1);
}