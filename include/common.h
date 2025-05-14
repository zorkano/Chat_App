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



int initWinsock();

int getServerIpPort(socket_t sockfd, char* ipStr, size_t ipStrLen, int* outPort);

void close_socket(int sockfd);


#endif /* COMMON_H */