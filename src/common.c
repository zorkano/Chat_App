#include "common.h"

char buffer[1024];
void send_to(SOCKET sock){
    printf("You: ");
    fgets(buffer, sizeof(buffer), stdin);
    send(sock, buffer, strlen(buffer), 0);

}

void receive(SOCKET sock){
    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);
}

