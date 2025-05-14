#include "common.h"
#include "client.h"

int client(char server_ip[100], int server_port){
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[1024];

    // Initialize Winsock
    WSAStartup(MAKEWORD(2,2), &wsa);

    server.sin_addr.s_addr = inet_addr(server_ip); 
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
        return 1;

    return 0;    

}