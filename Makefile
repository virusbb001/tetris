CC = gcc-4.7
#CC=avr-gcc


LETRO_PATH = ./Letro_main
CFLAGS=-I$(LETRO_PATH)/src -W -Wall -O0 -DHAS_CURSES -g
LDFLAGS=-L$(LETRO_PATH)/src

.c.o:
	$(CC) -c $< $(CFLAGS) 

all:tetris

tetris:main.o
	$(CC) -o tetris main.o -W -Wall -lncurses -g

clean:
	rm tetris
	rm *.o 

