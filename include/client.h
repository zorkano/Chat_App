#ifndef CLIENT_H
#define CLIENT_H

int client(char server_ip[100], int server_port);

int createClientSocket(socket_t* sockfd);

int connectToServer(socket_t sockfd, const char *server_ip, int port, const char *client_username, char *server_username, size_t username_size);

#endif /* CLIENT_H */