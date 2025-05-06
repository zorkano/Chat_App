#ifndef UI_H
#define UI_H

#ifdef _WIN32
typedef enum{
    BLACK = 0,
    BLUE,
    GREEN,
    CYAN,
    RED,
    PURPLE,
    YELLOW,
    WHITE,
} t_COLOR;

#else
typedef enum{
    BLACK = 0,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE,
    CYAN,
    WHITE,
}t_COLOR;

#endif

#define CHOICE_SIZE_X 30
#define CHOICE_SIZE_Y 5



typedef enum PROGRAM_STATE{
    MAIN_MENU = 0,
    SERVER_MENU,
    CLIENT_MENU,
    CHAT_MENU,
}PROGRAM_STATE_t;

void UI_mainMenu(PROGRAM_STATE_t* programState);

void UI_serverMenu(PROGRAM_STATE_t* programState);

void UI_clientMenu(PROGRAM_STATE_t* programState);

void UI_chatMenu(PROGRAM_STATE_t* programState);



#endif /* UI_H */ 