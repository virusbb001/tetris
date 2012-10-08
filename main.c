#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//PC用
#include <signal.h>
#include <ncurses.h>

typedef struct{
 unsigned int LED_height;
 unsigned int LED_width;
 unsigned int LCD_height;
 unsigned int LCD_width;
}Tetris_Hard_Info;

typedef struct{
 Tetris_Hard_Info *hard;
}TetrisWorld;

void setHardStatus(Tetris_Hard_Info *thisHard);

void signalhandler(int sig);

int main(void){
 sleep(3);
 endwin();
}

void setHardStatus(Tetris_Hard_Info *thisHard){
 thisHard->LED_height=16;
 thisHard->LED_width=16;
 thisHard->LCD_height=16;
 thisHard->LCD_width=16;
}

void signalhandler(int sig){
 endwin();
}
