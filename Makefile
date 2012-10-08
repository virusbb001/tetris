CC = gcc-4.7

all:tetris

tetris:main.c
	$(CC) -o tetris main.c -W -Wall -lncurses
