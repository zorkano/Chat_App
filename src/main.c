#include "ui.h"
#include "common.h"
#include "client.h"
#include "server.h"

extern socket_t sockfd;

systemType_t type;

#ifdef _WIN32
    BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_CLOSE_EVENT || signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT) {
        close_socket(sockfd); // Your cleanup function
        exit(0);
    }
        return TRUE;
    }
#else
    void signalHandler(int sig) {
        close_socket(sockfd); // Your cleanup function
        exit(0);
    }
#endif

int main() {
    PROGRAM_STATE_t programState = MAIN_MENU;

    // Set up signal handling for both platforms
    #ifdef _WIN32
        SetConsoleCtrlHandler(consoleHandler, TRUE);
    #else
        signal(SIGINT, signalHandler);   // Ctrl+C
        signal(SIGTERM, signalHandler); // kill or system shutdown
    #endif

    // Your existing application logic
    while (1) {
        switch(programState) {
            case MAIN_MENU:
                type = UI_mainMenu(&programState);
                break;
            case SERVER_MENU:
                UI_serverMenu(&programState);
                break;
            case CLIENT_MENU:
                UI_clientMenu(&programState);
                break;
            case CHAT_MENU:
                UI_chatMenu(&programState, type);
                programState = MAIN_MENU;
                break;
            default:
                break;
        }
    }

    return 0;
}