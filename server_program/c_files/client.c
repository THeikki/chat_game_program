#include "../header_files/client.h"

SOCKET client_socket;
SOCKET clients[CLIENTS];
struct sockaddr_in client_address;
socklen_t clientLen = sizeof(client_address);
pthread_t game_thread;
pthread_t game_threads[GAMERS];
pthread_mutex_t mutex;

int player_num = 0;

/*******************************************************************************************************
    CLIENT HANDLING
*/
void* client_handling(void* p_args) {
    struct arg *p = p_args;
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
    
    send(*p->socket, clear_message, strlen(clear_message), 0);
    send(*p->socket, welcome_message, strlen(welcome_message), 0);

    printf("clients: %d\n", client_num);

    if(client_num == 1) {
        send(*p->socket, wait_others_message, strlen(wait_others_message), 0);
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
        result = recv(*p->socket, recvbuf, recvbuflen, 0);  
    
        if(result > 0) {
            recvbuf[result] = '\0';
            getpeername(*p->socket, (struct sockaddr*) &client_address, &clientLen);
            broadcast(ntohs(client_address.sin_port), recvbuf); // UTILITY LIBRARY FUNCTION [ BROADCAST ]
            answer = recvbuf; 
            num = atoi(answer); // cast answer to integer
            
            switch (num)
            {
                case 1:  
                    // Create game thread 
                    if(player_num < GAMERS) {
                        clients[*p->index] = 0;
                    
                        if(client_num == CLIENTS - 1 && *p->index == CLIENTS - 1) {
                            *p->index = 0;
                        }
                        array_index = *p->index;
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
                        response = pthread_create(&game_thread, NULL, (void*)game_handling, p);
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
                                        clients[i] = *p->socket;
                                        *p->index = i;
                                        break;
                                    }
                                }
                                array_index = *p->index;
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
                                shutdown(*p->socket, SD_BOTH);
                                closesocket(*p->socket);
                            }    
                        }
                        else {
                            player_quit = 0;
                            printf("player quitted game\n");   
                            shutdown(*p->socket, SD_BOTH);
                            closesocket(*p->socket);
                        }       
                    }           
            }
          
            // Check if quit command is typed
            if(!memcmp(recvbuf, "!quit", 5 * sizeof(char))) {
                getpeername(*p->socket, (struct sockaddr*) &client_address, &clientLen);
                printf("Client quitted at: %s: %d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                break;
            }
        }   
        else if(result == 0) {
            getpeername(*p->socket, (struct sockaddr*) &client_address, &clientLen);
            broadcast(ntohs(client_address.sin_port), "disconnected\n");
            printf("Client disconnected at: %s: %d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            break;
        }
    } while(1);
    
    // Shutdown client connection
    shutdown(*p->socket, SD_BOTH);
    closesocket(*p->socket);
    clients[*p->index] = 0;

    if(client_num == CLIENTS - 1 && *p->index == CLIENTS - 1) {
        *p->index = 0;
    }
    array_index = *p->index;
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
    free(p->socket);
    free(p->index);
    free(p);
    p = NULL;
}
