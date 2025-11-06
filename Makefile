CFLAGS = -std=c99 -Wall -Wformat -Wextra -pedantic -g
CC = gcc

all: myenv

myenv: myenv.o commands.o
	$(CC) myenv.o commands.o -o myenv
myenv.o: myenv.c
	$(CC) -c $(CFLAGS) myenv.c

commands.o: commands.c commands.h
	$(CC) -c $(CFLAGS) commands.c

clean:
	rm -f myenv *.o