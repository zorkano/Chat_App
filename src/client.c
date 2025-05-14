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

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
        return 1;
    else{
        return 0;    
    }
}

int createClientSocket(socket_t* sockfd) {
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return 1;
    }
    return 0;
}


int connectToServer(socket_t sockfd, const char *server_ip, int port, const char *client_username, char *server_username, size_t username_size){
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        return 1;
    }

    // Send client username
    if (send(sockfd, client_username, strlen(client_username) + 1, 0) < 0) {
        return 1;
    }

    // Receive server username
    if (recv(sockfd, server_username, username_size, 0) <= 0) {
        return 1;
    }

    return 0;
}

