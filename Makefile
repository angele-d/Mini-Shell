CFLAGS = -std=c99 -Wall -Wformat -Wextra -pedantic -g
CC = gcc

all: myenv

myenv: myenv.o aliasCommand.o
	$(CC) myenv.o aliasCommand.o -o myenv
myenv.o: myenv.c
	$(CC) -c $(CFLAGS) myenv.c

aliasCommand.o: aliasCommand.c aliasCommand.h
	$(CC) -c $(CFLAGS) aliasCommand.c

clean:
	rm -f myenv *.o