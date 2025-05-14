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
} COLOR_t;

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
}COLOR_t;

#endif

#define CHOICE_SIZE_X 30
#define CHOICE_SIZE_Y 5

typedef enum PROGRAM_STATE{
    MAIN_MENU = 0,
    SERVER_MENU,
    CLIENT_MENU,
    CHAT_MENU,
}PROGRAM_STATE_t;
typedef enum {
    SYS_CLIENT, 
    SYS_SERVER
}systemType_t;

systemType_t UI_mainMenu(PROGRAM_STATE_t* programState);

void UI_serverMenu(PROGRAM_STATE_t* programState);

void UI_clientMenu(PROGRAM_STATE_t* programState);

void UI_chatMenu(PROGRAM_STATE_t* programState, systemType_t type);



#endif /* UI_H */ 