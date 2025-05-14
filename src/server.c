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
    client_socket = accept(server_socket, (struct sockaddr *)&client, &client_len);
    if (client_socket < 0 )
        return 1;
    else{
        return 0 ;
    }
}

int createServerSocket(socket_t* out_sockfd){
    socket_t sockfd;
    struct sockaddr_in server_addr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return 1;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    server_addr.sin_port = 0;

    // Bind socket to the specified port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close_socket(sockfd);
        return 1;
    }

    *out_sockfd = sockfd;
    return 0;
}

int startListening(socket_t sockfd, int backlog) {
    if (listen(sockfd, backlog) < 0) {
        return 1;
    }
    return 0;
}

int acceptClient(socket_t server_sockfd, socket_t* out_client_sockfd, struct sockaddr_in* out_client_addr,
                 const char *server_username, char *client_username, size_t username_size){
    int addr_len = sizeof(struct sockaddr_in);
    socket_t client_sockfd = accept(server_sockfd, (struct sockaddr *)out_client_addr, &addr_len);
    if (client_sockfd < 0) {
        return 1;
    }

    // Receive client username
    if (recv(client_sockfd, client_username, username_size, 0) <= 0) {
        return 1;
    }

    // Send server username
    if (send(client_sockfd, server_username, strlen(server_username) + 1, 0) < 0) {
        return 1;
    }

    *out_client_sockfd = client_sockfd;
    return 0;
}

