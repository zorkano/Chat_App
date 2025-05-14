#ifndef SERVER_H
#define SERVER_H

int server(char server_ip[100], int server_port);

int createServerSocket(socket_t* out_sockfd);

int startListening(socket_t sockfd, int backlog);

int acceptClient(socket_t server_sockfd, socket_t* out_client_sockfd, struct sockaddr_in* out_client_addr,
                 const char *server_username, char *client_username, size_t username_size);

#endif /* SERVER_H */