#include "common.h"


#include <stdio.h>
#include <string.h>

socket_t sockfd;

int initWinsock() {
    #ifdef _WIN32
        WSADATA wsaData;
        return WSAStartup(MAKEWORD(2, 2), &wsaData);
    #else
        return 0;
    #endif
}

int getServerIpPort(socket_t sockfd, char* ipStr, size_t ipStrLen, int* outPort) {
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    if (getsockname(sockfd, (struct sockaddr*)&addr, &addrLen) < 0) {
        return -1;
    }

    // Retrieve the local IP address
    char hostbuffer[256];
    if (gethostname(hostbuffer, sizeof(hostbuffer)) == -1) {
        perror("gethostname");
        return -1;
    }

    struct hostent* host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL) {
        perror("gethostbyname");
        return -1;
    }

    struct in_addr** addr_list = (struct in_addr**)host_entry->h_addr_list;
    if (addr_list[0] != NULL) {
        strncpy(ipStr, inet_ntoa(*addr_list[0]), ipStrLen);
    } else {
        return -1;
    }

    *outPort = ntohs(addr.sin_port);
    return 0;
}

void close_socket(int sockfd) {

#ifdef _WIN32
    closesocket(sockfd);
    WSACleanup();
#else
    close(sockfd);
#endif
}


