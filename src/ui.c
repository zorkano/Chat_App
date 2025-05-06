#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "ui.h"

typedef enum CHOICE{
    SERVER = 0,
    CLIENT,
}CHOICE_t;

typedef enum CHOICE_STATE{
    CLEAR = 0,
    SET,
}CHOICE_STATE_t;

int columns, rows;

char* Welcome[]= {
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



#ifdef _WIN32
#include <windows.h>

void UI_maximizeConsole() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_MAXIMIZE);
    Sleep(500);
}

void UI_clearScreen() {
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

// Set text color (0–15 for standard Windows colors)
void UI_setTextColor(t_COLOR colorCode) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
}

// Set background color (0–15 for standard Windows colors)
void UI_setBackgroundColor(t_COLOR colorCode) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (colorCode << 4));
}

#else // Linux/macOS
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

void UI_maximizeConsole() {
    printf("\033[9;1t"); // Request maximize (xterm-compatible)
    usleep(500000);      // Give time to resize
}

void UI_clearScreen() {
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

// Set text color using ANSI 30–37 codes (foreground)
// Example: 31 = red, 32 = green, 33 = yellow, etc.
void UI_setTextColor(t_COLOR colorCode) {
    printf("\033[%dm", 30 + (colorCode % 8));
    fflush(stdout);
}

void UI_setBackgroundColor(t_COLOR colorCode) {
    printf("\033[%dm", 40 + (colorCode % 8));
    fflush(stdout);
}

#endif

void UI_drawBorder(int columns, int rows) {
    int startX, startY;
    UI_getCursorPosition(&startX, &startY);
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

void UI_printAscii(int x, int y, char** s) {
    int i;
    
    // Set text color for ASCII art
    UI_setTextColor(RED);

    // Print each line of the ASCII art
    for(i = 0; s[i]; i++) {
        UI_moveCursor(x, y++);
        printf("%s", s[i]);
    }

    UI_setTextColor(WHITE);
}

void UI_choiceNavigation(int x, int y, CHOICE_STATE_t state) {
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

void UI_mainMenu(PROGRAM_STATE_t* programState){
    *programState = SERVER_MENU;

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
    printf("Server");
    //draw the borders of the second choice
    UI_moveCursor(ChoiceBorderPivotX, ClientBorderPivotY);
    UI_drawBorder(CHOICE_SIZE_X, CHOICE_SIZE_Y);
    UI_moveCursor(ChoiceTextPivotX, ClientTextPivotY);
    printf("Client");

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
                    printf("Client");
                    UI_choiceNavigation(ChoiceBorderPivotX, ServerBorderPivotY, CLEAR);
                    UI_moveCursor(ChoiceTextPivotX, ServerTextPivotY);
                    UI_setTextColor(RED);
                    printf("Server");
                    *programState = CLIENT_MENU;
                }else if(CLIENT_MENU == *programState){
                    UI_choiceNavigation(ChoiceBorderPivotX, ClientBorderPivotY, CLEAR);
                    UI_moveCursor(ChoiceTextPivotX, ClientTextPivotY);
                    UI_setTextColor(RED);
                    printf("Client");
                    UI_choiceNavigation(ChoiceBorderPivotX, ServerBorderPivotY, SET);
                    UI_moveCursor(ChoiceTextPivotX + 1, ServerTextPivotY);
                    printf("Server");
                    *programState = SERVER_MENU;
                }
            }else if(VK_DOWN == virtualKeyCode && inputRecord.Event.KeyEvent.bKeyDown) {
                if(SERVER_MENU == *programState){
                    UI_choiceNavigation(ChoiceBorderPivotX, ClientBorderPivotY, SET);
                    UI_moveCursor(ChoiceTextPivotX + 1, ClientTextPivotY);
                    printf("Client");
                    UI_choiceNavigation(ChoiceBorderPivotX, ServerBorderPivotY, CLEAR);
                    UI_moveCursor(ChoiceTextPivotX, ServerTextPivotY);
                    UI_setTextColor(RED);
                    printf("Server");
                    *programState = CLIENT_MENU;
                }else if(CLIENT_MENU == *programState){
                    UI_choiceNavigation(ChoiceBorderPivotX, ClientBorderPivotY, CLEAR);
                    UI_moveCursor(ChoiceTextPivotX, ClientTextPivotY);
                    UI_setTextColor(RED);
                    printf("Client");
                    UI_choiceNavigation(ChoiceBorderPivotX, ServerBorderPivotY, SET);
                    UI_moveCursor(ChoiceTextPivotX + 1, ServerTextPivotY);
                    printf("Server");
                    *programState = SERVER_MENU;                    
                }
            }else if(VK_RETURN == virtualKeyCode && inputRecord.Event.KeyEvent.bKeyDown){
                break;
            }
        }
    }

}

void UI_serverMenu(PROGRAM_STATE_t* programState){
    UI_clearScreen();
    UI_maximizeConsole();
    UI_getConsoleSize(&columns, &rows);
    UI_drawBorder(columns, rows);

}   

void UI_clientMenu(PROGRAM_STATE_t* programState){
    UI_clearScreen();
    UI_maximizeConsole();
    UI_getConsoleSize(&columns, &rows);
    UI_drawBorder(columns, rows);

}

void UI_chatMenu(PROGRAM_STATE_t* programState){
    UI_clearScreen();
    UI_maximizeConsole();
    UI_getConsoleSize(&columns, &rows);
    UI_drawBorder(columns, rows);

}
