#ifndef COMMON_H
#define COMMON_H

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
#else // linux/MacOS
    #include <unistd.h>
    #include <signal.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    typedef int socket_t;
#endif

void receive(SOCKET sock);
void send_to(SOCKET sock);

int initWinsock();

int getServerIpPort(socket_t sockfd, char* ipStr, size_t ipStrLen, int* outPort);

void close_socket(int sockfd);

int dataSend(socket_t sockfd, char* buffer, int bufferLen);

int dataRecv(socket_t sockfd, char* buffer, int bufferLen);

#endif /* COMMON_H */