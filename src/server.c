#include "common.h"
#include "server.h"

int server(char server_ip[100], int server_port){
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int client_len;
    char buffer[1024];

     // Initialize Winsock
    WSAStartup(MAKEWORD(2,2), &wsa);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_port = htons(server_port);

    bind(server_socket, (struct sockaddr *)&server, sizeof(server));
    listen(server_socket, 1);
    client_len = sizeof(struct sockaddr_in);
    if (client_socket = accept(server_socket, (struct sockaddr *)&client, &client_len) < 0 )
        return 1;
    
    return 0 ;

}