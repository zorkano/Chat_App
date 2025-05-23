#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "server.h"
#include "client.h"
#include "ui.h"


extern socket_t sockfd;

struct sockaddr_in out_client_addr;
typedef enum CHOICE{
    SERVER = 0,
    CLIENT,
}CHOICE_t;

typedef enum CHOICE_STATE{
    CLEAR = 0,
    SET,
}CHOICE_STATE_t;

int columns, rows;
int cursorChatPostitionX = 1;
int cursorChatPostitionY = 1;

char buffer[512] = {
    "\0"
};

char clientUsername[15] = {
    "\0"
};

char clientIpAddress[17] = {
    "\0"
};

char clientPort[15] = {
    "\0"
};

char serverUsername[15] = {
    "\0"
};

char serverIpAddress[17] = {
    "\0"
};

char serverPort[10] = {
    "\0"
};

int port;

char* Welcome[] = {
    "__          __    _                                _",
    "\\ \\        / /   | |                              | |",
    " \\ \\  /\\  / /___ | |  ___  ___   _ __ ___    ___  | |_  ___",
    "  \\ \\/  \\/ // _ \\| | / __|/ _ \\ | '_ ` _ \\  / _ \\ | __|/ _ \\",
    "   \\  /\\  /|  __/| || (__| (_) || | | | | ||  __/ | |_| (_) |",
    "    \\/  \\/  \\___||_| \\___|\\___/ |_| |_| |_| \\___|  \\__|\\___/",
    "            __  __          _____  _             _",
    "           |  \\/  |        / ____|| |           | |",
    "           | \\  / | _   _ | |     | |__    __ _ | |_",
    "           | |\\/| || | | || |     | '_ \\  / _` || __|",
    "           | |  | || |_| || |____ | | | || (_| || |_",
    "           |_|  |_| \\__, | \\_____||_| |_| \\__,_| \\__|",
    "                     __/ |",
    "                    |___/",
    "\0"
};

char* Server[] = {   
"  _____  ______  _____ __      __ ______  _____",
" / ____||  ____||  __ \\\\ \\    / /|  ____||  __ \\",
"| (___  | |__   | |__) |\\ \\  / / | |__   | |__) |",
" \\___ \\ |  __|  |  _  /  \\ \\/ /  |  __|  |  _  /",
" ____) || |____ | | \\ \\   \\  /   | |____ | | \\ \\",
"|_____/ |______||_|  \\_\\   \\/    |______||_|  \\_\\",
"\0"
};

char* Client[] = {
"  _____  _       _____  ______  _   _  _______ ",
" / ____|| |     |_   _||  ____|| \\ | ||__   __|",
"| |     | |       | |  | |__   |  \\| |   | |",
"| |     | |       | |  |  __|  | . ` |   | |",
"| |____ | |____  _| |_ | |____ | |\\  |   | |",
" \\_____||______||_____||______||_| \\_|   |_|",
"\0"
};

#ifdef _WIN32
#include <windows.h>

// Set text color (0–15 for standard Windows colors)
void UI_setTextColor(COLOR_t colorCode) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    // Mask out the background, preserve it
    WORD current = csbi.wAttributes;
    WORD newAttr = (current & 0xF0) | (colorCode & 0x0F);
    SetConsoleTextAttribute(hConsole, newAttr);
}

// Set background color (0–15 for standard Windows colors)
void UI_setBackgroundColor(COLOR_t colorCode) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    // Mask out the foreground, preserve it
    WORD current = csbi.wAttributes;
    WORD newAttr = (current & 0x0F) | ((colorCode & 0x0F) << 4);
    SetConsoleTextAttribute(hConsole, newAttr);
}

void UI_maximizeConsole() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_MAXIMIZE);
    Sleep(500);
}

void UI_clearScreen() {
    UI_setBackgroundColor(BLACK);
    UI_setTextColor(RED);
    system("cls");
}

void UI_getConsoleSize(int *columns, int *rows) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void UI_getCursorPosition(int *x, int *y) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *x = csbi.dwCursorPosition.X;
    *y = csbi.dwCursorPosition.Y;
}

// Move cursor to (x, y)
void UI_moveCursor(int x, int y) {
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

#else // Linux/macOS
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

// Set text color (0–7 for standard ANSI colors)
void UI_setTextColor(COLOR_t colorCode) {
    printf("\033[%dm", 30 + (colorCode % 8));
    fflush(stdout);
}

// Set background color (0–7 for standard ANSI colors)
void UI_setBackgroundColor(COLOR_t colorCode) {
    printf("\033[%dm", 40 + (colorCode % 8));
    fflush(stdout);
}

void UI_maximizeConsole() {
    printf("\033[9;1t"); // Request maximize (xterm-compatible)
    usleep(500000);      // Give time to resize
}

void UI_clearScreen() {
    UI_setBackgroundColor(BLACK);
    UI_setTextColor(RED);
    system("clear");
}

void UI_getConsoleSize(int *columns, int *rows) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *columns = w.ws_col;
    *rows = w.ws_row;
}

void UI_getCursorPosition(int *x, int *y) {
    struct termios saved, raw;
    char buf[32];
    unsigned int i = 0;

    // Save current terminal settings
    tcgetattr(STDIN_FILENO, &saved);
    raw = saved;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    // Request cursor position report
    printf("\033[6n");
    fflush(stdout);

    // Read response: ESC [ rows ; cols R
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, buf + i, 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &saved);

    // Parse response
    int row, col;
    if (sscanf(buf, "\033[%d;%dR", &row, &col) == 2) {
        *x = col - 1;
        *y = row - 1;
    } else {
        *x = 0;
        *y = 0;
    }
}

// Move cursor to (x, y) [1-based indexing]
void UI_moveCursor(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
    fflush(stdout);
}

#endif

int fgetsTrim(char *buffer, size_t size){
    if (fgets(buffer, size, stdin) == NULL)
        return 0; // Reading failed

    // Remove trailing newline and carriage return characters
    buffer[strcspn(buffer, "\r\n")] = '\0';
    return 1; // Success
}

void UI_drawBorder(int columns, int rows){
    int startX, startY;
    UI_getCursorPosition(&startX, &startY);
    UI_setBackgroundColor(BLACK);
    UI_setTextColor(RED);

    // Top and Bottom
    for (int x = 0; x < columns; x++) {
        UI_moveCursor(startX + x, startY); // Top
        if (x == 0 || x == columns - 1) printf("+");
        else printf("-");

        UI_moveCursor(startX + x, startY + rows - 1); // Bottom
        if (x == 0 || x == columns - 1) printf("+");
        else printf("-");
    }

    // Sides
    for (int y = 1; y < rows - 1; y++) {
        UI_moveCursor(startX, startY + y);
        printf("|");
        UI_moveCursor(startX + columns - 1, startY + y);
        printf("|");
    }

    fflush(stdout);
}

void UI_printAscii(int x, int y, char** s){
    int i;
    
    // Set text color for ASCII art
    UI_setTextColor(RED);

    // Print each line of the ASCII art
    for(i = 0; s[i]; i++) {
        UI_moveCursor(x, y++);
        printf("%s", s[i]);
    }
}

void UI_printString(char* string, COLOR_t textColor, COLOR_t backgroundColor){
    UI_setBackgroundColor(backgroundColor);
    UI_setTextColor(textColor);
    printf("%s", string);
}

void UI_choiceNavigation(int x, int y, CHOICE_STATE_t state){
    int i, j;
    // Set highlight color
    if(state){
        UI_setBackgroundColor(WHITE);    
    }else{
        UI_setBackgroundColor(BLACK);
    }

    // Draw highlight block
    for (i = 0; i < CHOICE_SIZE_Y - 2; i++) {
        for (j = 0; j < CHOICE_SIZE_X - 2; j++) {
            UI_moveCursor(x + j + 1, y + i + 1);
            printf(" "); // Fill with solid blocks
        }
    }

}

systemType_t UI_mainMenu(PROGRAM_STATE_t* programState){
    *programState = SERVER_MENU;
    systemType_t type = SYS_SERVER;


    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);

    UI_clearScreen();
    UI_maximizeConsole();
    UI_getConsoleSize(&columns, &rows);
    
    
    int ChoiceBorderPivotX = (columns - CHOICE_SIZE_X) / 2;
    int ServerBorderPivotY = (rows - CHOICE_SIZE_Y) / 2;
    int ClientBorderPivotY = (rows + CHOICE_SIZE_Y) / 2;

    int ChoiceTextPivotX = (ChoiceBorderPivotX) + ((CHOICE_SIZE_X - 8) / 2) + 1;
    int ServerTextPivotY = ServerBorderPivotY + 2;
    int ClientTextPivotY = ClientBorderPivotY + 2;
    
    //Draw the borders of the application 
    UI_drawBorder(columns, rows);

    //Draw the welcome message
    UI_printAscii(((columns - 61) / 2), 1, Welcome);

    //draw the borders of the first choice
    UI_moveCursor(ChoiceBorderPivotX, ServerBorderPivotY);
    UI_drawBorder(CHOICE_SIZE_X, CHOICE_SIZE_Y);
    UI_choiceNavigation(ChoiceBorderPivotX, ServerBorderPivotY, SET);
    UI_moveCursor(ChoiceTextPivotX + 1, ServerTextPivotY);
    UI_printString("Server", RED, WHITE);

    //draw the borders of the second choice
    UI_moveCursor(ChoiceBorderPivotX, ClientBorderPivotY);
    UI_drawBorder(CHOICE_SIZE_X, CHOICE_SIZE_Y);
    UI_moveCursor(ChoiceTextPivotX, ClientTextPivotY);
    UI_printString("Client", RED, BLACK);

    while(1){
        // Create an event record structure
        INPUT_RECORD inputRecord;
        DWORD eventsRead;

        // Read the key press event
        ReadConsoleInput(hInput, &inputRecord, 1, &eventsRead);
        
        UI_moveCursor(1,1);
        // Check if the event is a key press
        if(inputRecord.EventType == KEY_EVENT){
            WORD virtualKeyCode = inputRecord.Event.KeyEvent.wVirtualKeyCode;

            // Check for arrow keys
            if(VK_UP == virtualKeyCode && inputRecord.Event.KeyEvent.bKeyDown) {
                if(SERVER_MENU == *programState){
                    UI_choiceNavigation(ChoiceBorderPivotX, ClientBorderPivotY, SET);
                    UI_moveCursor(ChoiceTextPivotX + 1, ClientTextPivotY);
                    UI_printString("Client", RED, WHITE);
                    UI_choiceNavigation(ChoiceBorderPivotX, ServerBorderPivotY, CLEAR);
                    UI_moveCursor(ChoiceTextPivotX, ServerTextPivotY);
                    UI_setTextColor(RED);
                    UI_printString("Server", RED, BLACK);
                    *programState = CLIENT_MENU;
                    type = SYS_CLIENT;
                }else if(CLIENT_MENU == *programState){
                    UI_choiceNavigation(ChoiceBorderPivotX, ClientBorderPivotY, CLEAR);
                    UI_moveCursor(ChoiceTextPivotX, ClientTextPivotY);
                    UI_setTextColor(RED);
                    UI_printString("Client", RED, BLACK);
                    UI_choiceNavigation(ChoiceBorderPivotX, ServerBorderPivotY, SET);
                    UI_moveCursor(ChoiceTextPivotX + 1, ServerTextPivotY);
                    UI_printString("Server", RED, WHITE);
                    *programState = SERVER_MENU;
                    type = SYS_SERVER;
                }
            }else if(VK_DOWN == virtualKeyCode && inputRecord.Event.KeyEvent.bKeyDown) {
                if(SERVER_MENU == *programState){
                    UI_choiceNavigation(ChoiceBorderPivotX, ClientBorderPivotY, SET);
                    UI_moveCursor(ChoiceTextPivotX + 1, ClientTextPivotY);
                    UI_printString("Client", RED, WHITE);
                    UI_choiceNavigation(ChoiceBorderPivotX, ServerBorderPivotY, CLEAR);
                    UI_moveCursor(ChoiceTextPivotX, ServerTextPivotY);
                    UI_setTextColor(RED);
                    UI_printString("Server", RED, BLACK);
                    *programState = CLIENT_MENU;
                    type = SYS_CLIENT;
                }else if(CLIENT_MENU == *programState){
                    UI_choiceNavigation(ChoiceBorderPivotX, ClientBorderPivotY, CLEAR);
                    UI_moveCursor(ChoiceTextPivotX, ClientTextPivotY);
                    UI_setTextColor(RED);
                    UI_printString("Client", RED, BLACK);
                    UI_choiceNavigation(ChoiceBorderPivotX, ServerBorderPivotY, SET);
                    UI_moveCursor(ChoiceTextPivotX + 1, ServerTextPivotY);
                    UI_printString("Server", RED, WHITE);
                    *programState = SERVER_MENU;
                    type = SYS_SERVER;                    
                }
            }else if(VK_RETURN == virtualKeyCode && inputRecord.Event.KeyEvent.bKeyDown){
                UI_setTextColor(RED);
                UI_setBackgroundColor(BLACK);
                initWinsock();
                return type;
            }
        }
    }
}

void UI_serverMenu(PROGRAM_STATE_t* programState){    
    UI_clearScreen();
    UI_maximizeConsole();
    UI_getConsoleSize(&columns, &rows);
    UI_drawBorder(columns, rows);
    UI_printAscii(((columns - 50) / 2), 1, Server);
    
    int inputBorderPivotX = (columns - 50) / 2;
    int inputBorderPivotY = (rows - CHOICE_SIZE_Y) / 2;

    UI_moveCursor(inputBorderPivotX, inputBorderPivotY);
    UI_drawBorder(50, 7);

        // Write text to expect input
    UI_moveCursor(inputBorderPivotX + 2, inputBorderPivotY + 1);
    UI_printString("Username: ", RED, BLACK);
    UI_printString("                ", BLACK, WHITE);

    UI_moveCursor(inputBorderPivotX + 2, inputBorderPivotY + 3);
    UI_printString("IP address: ", RED, BLACK);
    UI_printString("                ", BLACK, WHITE);

    UI_moveCursor(inputBorderPivotX + 2, inputBorderPivotY + 5);
    UI_printString("Port: ", RED, BLACK);
    UI_printString("                ", BLACK, WHITE);

    UI_moveCursor(inputBorderPivotX + 12, inputBorderPivotY + 1);
    fgetsTrim(serverUsername, sizeof(serverUsername));

    createServerSocket(&sockfd);

    getServerIpPort(sockfd, serverIpAddress, sizeof(serverIpAddress), &port);
    
    itoa(port, serverPort, 10);
    // Get a free port and the device ip address and print them inplace of placeholders
    UI_moveCursor(inputBorderPivotX + 14, inputBorderPivotY + 3);
    UI_printString(serverIpAddress, BLACK, WHITE);
    UI_moveCursor(inputBorderPivotX + 8, inputBorderPivotY + 5);
    UI_printString(serverPort, BLACK, WHITE);

    startListening(sockfd, 5);
    // Wait for connection from client
    if(acceptClient(sockfd, &sockfd, &out_client_addr, serverUsername, clientUsername, sizeof(clientUsername))){
        *programState = SERVER_MENU;
    }else{

       *programState = CHAT_MENU; 
    }
}

void UI_clientMenu(PROGRAM_STATE_t* programState){
    UI_clearScreen();
    UI_maximizeConsole();
    UI_getConsoleSize(&columns, &rows);
    UI_drawBorder(columns, rows);
    UI_printAscii(((columns - 50) / 2), 1, Client);
    
    int inputBorderPivotX = (columns - 50) / 2;
    int inputBorderPivotY = (rows - CHOICE_SIZE_Y) / 2;

    UI_moveCursor(inputBorderPivotX, inputBorderPivotY);
    UI_drawBorder(50, 7);

    UI_moveCursor(inputBorderPivotX + 2, inputBorderPivotY + 1);
    UI_printString("Username: ", RED, BLACK);
    UI_printString("                ", BLACK, WHITE);

    UI_moveCursor(inputBorderPivotX + 2, inputBorderPivotY + 3);
    UI_printString("IP address: ", RED, BLACK);
    UI_printString("                ", BLACK, WHITE);

    UI_moveCursor(inputBorderPivotX + 2, inputBorderPivotY + 5);
    UI_printString("Port: ", RED, BLACK);
    UI_printString("                ", BLACK, WHITE);
    
    createClientSocket(&sockfd);
    UI_moveCursor(inputBorderPivotX + 12, inputBorderPivotY + 1);
    fgetsTrim(clientUsername, sizeof(clientUsername));
    UI_moveCursor(inputBorderPivotX + 14, inputBorderPivotY + 3);
    fgetsTrim(clientIpAddress, sizeof(clientIpAddress));
    UI_moveCursor(inputBorderPivotX + 8, inputBorderPivotY + 5);
    fgetsTrim(clientPort, sizeof(clientPort));

    port = atoi(clientPort);
    // try to connect to the server
    if(connectToServer(sockfd, clientIpAddress, port, clientUsername, serverUsername, sizeof(serverUsername))){
        *programState = CLIENT_MENU;
    }else{
       *programState = CHAT_MENU;
    }
}

void UI_chatServerMenu(PROGRAM_STATE_t* programState){
    UI_clearScreen();
    UI_maximizeConsole();
    UI_getConsoleSize(&columns, &rows);
    int inputCursorPositionX = 25 + strlen(serverUsername);
    int inputCursorPositionY = rows - 2;
    int status;

    UI_drawBorder(columns, rows);

    UI_moveCursor(0, rows - 3);
    UI_drawBorder(columns, 3);

    UI_moveCursor(1, inputCursorPositionY);
    printf("You are now talking as ");
    UI_printString(serverUsername, YELLOW, BLACK);
    printf(": ");

    // while loop for the chat
    while(1){
        memset(buffer, 0, sizeof(buffer));
        fflush(stdin);
        status = dataRecv(sockfd, buffer, sizeof(buffer));
        UI_moveCursor(cursorChatPostitionX, cursorChatPostitionY++);
        UI_printString(clientUsername, GREEN, BLACK);
        printf(": ");
        UI_printString(buffer, WHITE, BLACK);

        UI_moveCursor(1, rows - 2);
        printf("You are now talking as ");
        UI_printString(serverUsername, YELLOW, BLACK);
        printf(": ");
        
        fflush(stdin);
        
        fgetsTrim(buffer, sizeof(buffer));
        UI_moveCursor(inputCursorPositionX, inputCursorPositionY);
        UI_printString("                                                                         ", WHITE, BLACK);
        UI_moveCursor(cursorChatPostitionX, cursorChatPostitionY++);
        UI_printString(serverUsername, YELLOW, BLACK);
        printf(": ");
        UI_printString(buffer, WHITE, BLACK);
        UI_moveCursor(inputCursorPositionX, inputCursorPositionY);

        status = dataSend(sockfd, buffer, strlen(buffer));

        if(cursorChatPostitionY >= (rows - 5)){
            UI_clearScreen();
            UI_drawBorder(columns, rows);
            UI_moveCursor(0, rows - 3);
            UI_drawBorder(columns, 3);

            UI_moveCursor(1, rows - 2);
            printf("You are now talking as ");
            UI_printString(clientUsername, YELLOW, BLACK);
            printf(": ");
            cursorChatPostitionY = 1;
        }
    };
}

void UI_chatClientMenu(PROGRAM_STATE_t* programState){
    int status;
    UI_clearScreen();
    UI_maximizeConsole();
    UI_getConsoleSize(&columns, &rows);
    int inputCursorPositionX = 25 + strlen(clientUsername);
    int inputCursorPositionY = rows - 2;

    UI_drawBorder(columns, rows);

    UI_moveCursor(0, rows - 3);
    UI_drawBorder(columns, 3);


  
    // while loop for the chat
    while(1){
        fflush(stdin);
        UI_moveCursor(1, rows - 2);
        printf("You are now talking as ");
        UI_printString(clientUsername, YELLOW, BLACK);
        printf(": ");
        fgetsTrim(buffer, sizeof(buffer));
        UI_moveCursor(inputCursorPositionX, inputCursorPositionY);
        UI_printString("                                                                         ", WHITE, BLACK);
        UI_moveCursor(cursorChatPostitionX, cursorChatPostitionY++);
        UI_printString(clientUsername, YELLOW, BLACK);
        printf(": ");
        UI_printString(buffer, WHITE, BLACK);
        UI_moveCursor(inputCursorPositionX, inputCursorPositionY);

        status = dataSend(sockfd, buffer, strlen(buffer));



        memset(buffer, 0, sizeof(buffer));
        fflush(stdin);
        status = dataRecv(sockfd, buffer, sizeof(buffer));

        UI_moveCursor(cursorChatPostitionX, cursorChatPostitionY++);
        UI_printString(serverUsername, GREEN, BLACK);
        printf(": ");
        UI_printString(buffer, WHITE, BLACK);

        if(cursorChatPostitionY >= (rows - 4)){
            UI_clearScreen();
            UI_drawBorder(columns, rows);
            UI_moveCursor(0, rows - 3);
            UI_drawBorder(columns, 3);
            UI_moveCursor(1, rows - 2);
            printf("You are now talking as ");
            UI_printString(clientUsername, YELLOW, BLACK);
            printf(": ");
            cursorChatPostitionY = 1;
        }
    };
}
