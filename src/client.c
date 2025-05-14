#include "common.h"
#include "client.h"

SOCKET client(char server_ip[100], int server_port){
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    // Initialize Winsock
    WSAStartup(MAKEWORD(2,2), &wsa);

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_addr.s_addr = inet_addr(server_ip); 
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR){
        printf("Failed to connect.\n");
        return INVALID_SOCKET;
    }
    else {
        return sock;
    }    

}