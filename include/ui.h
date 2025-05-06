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

typedef enum CHOICE{
    SERVER = 0,
    CLIENT,
}CHOICE_t;

typedef enum CHOICE_STATE{
    CLEAR = 0,
    SET,
}CHOICE_STATE_t;


void UI_mainMenu();

void UI_serverMenu();

void UI_clientMenu();

void UI_chatMenu();



#endif /* UI_H */ 