#include "../header_files/client.h"

SOCKET client_socket;
SOCKET clients[CLIENTS];
struct sockaddr_in client_address;
socklen_t clientLen = sizeof(client_address);
pthread_t game_thread;

int player_num = 0;

/*******************************************************************************************************
    CLIENT HANDLING
*/
void* client_handling(void* client_args) {
    struct client *p_client = client_args;
    int num, response, temp, result;
    char recvbuf[BUFFER];
    int recvbuflen = BUFFER;

    //  Info messages for clients
    char* text = "Joined chatroom";
    char* notice_text = malloc(sizeof(unsigned short) + strlen(text) + 10);
    sprintf(notice_text, "[%hu]: %s\n\r", ntohs(client_address.sin_port), text);
    char* wait_others_message = "Waiting for other participants...\n\r";
    char* answer = NULL;
    char* clear_message = "\033[2J\033[H\033[3J"; // Clear putty terminal and scrollback
    char* welcome_message = "==============================================================\n\r"
                            "*                       WELCOME TO CHAT                      *\n\r"
                            "*                                                            *\n\r"
                            "*                       (1)       Play                       *\n\r"
                            "*                       (!quit)   Quit                       *\n\r"
                            "*                                                            *\n\r"
                            "==============================================================\n\r";
    
    send(*p_client->socket, clear_message, strlen(clear_message), 0);
    send(*p_client->socket, welcome_message, strlen(welcome_message), 0);

    printf("clients: %d\n", client_num);

    if(client_num == 1) {
        send(*p_client->socket, wait_others_message, strlen(wait_others_message), 0);
    }
    else {
        for(int i = 0; i < CLIENTS; i++) {
            send(clients[i], clear_message, strlen(clear_message), 0);
            send(clients[i], welcome_message, strlen(welcome_message), 0);
            send(clients[i], notice_text, strlen(notice_text), 0);
        }
    }

    //  Prompt action from client
    do {
        result = recv(*p_client->socket, recvbuf, recvbuflen, 0);  
    
        if(result > 0) {
            recvbuf[result] = '\0';
            getpeername(*p_client->socket, (struct sockaddr*) &client_address, &clientLen);
            broadcast(ntohs(client_address.sin_port), recvbuf); // UTILITY LIBRARY FUNCTION [ BROADCAST ]
            answer = recvbuf; 
            num = atoi(answer); // cast answer to integer
            
            switch (num)
            {
                case 1:  
                    // Create game thread 
                    if(player_num < GAMERS) {
                        clients[*p_client->index] = 0;
                    
                        if(client_num == CLIENTS - 1 && *p_client->index == CLIENTS - 1) {
                            *p_client->index = 0;
                        }
                        array_index = *p_client->index;
                        client_num--;
                    
                        if(client_num == 1) {
                            for(int i=0; i<CLIENTS; i++) {
                                if(clients[i] != 0) {
                                    send(clients[i], wait_others_message, strlen(wait_others_message), 0);
                                }
                            }
                        }   

                        player_num++;
                        printf("PLAYER_NUM:%d\n", player_num);
                        response = pthread_create(&game_thread, NULL, (void*)game_handling, p_client);
                        if(response) {
                            printf("Unable to create thread");
                            pthread_exit(NULL);
                            player_num--;
                        }
                        //  Game thread return
                        pthread_join(game_thread, NULL);
                        player_num--;
                        printf("P_quit: %d\n", player_quit);
                        printf("player_num:%d\n", player_num);

                        if(player_quit != 1) {
                            if(client_num != CLIENTS) {  
                                for(int i=0; i<CLIENTS; i++) {
                                    if(clients[i] == 0) {
                                        clients[i] = *p_client->socket;
                                        *p_client->index = i;
                                        break;
                                    }
                                }
                                array_index = *p_client->index;
                                array_index++;
                                client_num++;

                                printf("CLIENTS: %d\n", client_num);

                                for(int i = 0; i < CLIENTS; i++) {
                                    send(clients[i], clear_message, strlen(clear_message), 0);
                                    send(clients[i], welcome_message, strlen(welcome_message), 0);
                                    if(client_num == 1) {
                                        send(clients[i], wait_others_message, strlen(wait_others_message), 0);
                                    }
                                    else {
                                        send(clients[i], notice_text, strlen(notice_text), 0);
                                    }
                                }
                                break;
                            }
                            else {
                                printf("Chat was full\n");   
                                shutdown(*p_client->socket, SD_BOTH);
                                closesocket(*p_client->socket);
                            }    
                        }
                        else {
                            player_quit = 0;
                            printf("player quitted game\n");   
                            shutdown(*p_client->socket, SD_BOTH);
                            closesocket(*p_client->socket);
                        }       
                    }           
            }
          
            // Check if quit command is typed
            if(!memcmp(recvbuf, "!quit", 5 * sizeof(char))) {
                getpeername(*p_client->socket, (struct sockaddr*) &client_address, &clientLen);
                printf("Client quitted at: %s: %d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                break;
            }
        }   
        else if(result == 0) {
            getpeername(*p_client->socket, (struct sockaddr*) &client_address, &clientLen);
            broadcast(ntohs(client_address.sin_port), "disconnected\n");
            printf("Client disconnected at: %s: %d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            break;
        }
    } while(1);
    
    // Shutdown client connection
    shutdown(*p_client->socket, SD_BOTH);
    closesocket(*p_client->socket);
    clients[*p_client->index] = 0;

    if(client_num == CLIENTS - 1 && *p_client->index == CLIENTS - 1) {
        *p_client->index = 0;
    }
    array_index = *p_client->index;
    client_num--;
   
    if(client_num == 1) {
        for(int i=0; i<CLIENTS; i++) {
            if(clients[i] != 0) {
                send(clients[i], wait_others_message, strlen(wait_others_message), 0);
            }
        }
    }

    printf("CLIENTIT LOPUSSA:%d\n", client_num);
    //  Free dynamic allocated memory
    free(notice_text);
    free(p_client->socket);
    free(p_client->index);
    free(p_client);
    p_client = NULL;
}
