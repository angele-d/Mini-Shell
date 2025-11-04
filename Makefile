CFLAGS = -std=c99 -Wall -Wformat -Wextra -pedantic -g
CC = gcc

all: myenv

myenv: myenv.o
	$(CC) myenv.o -o myenv
myenv.o: myenv.c
	$(CC) -c $(CFLAGS) myenv.c

clean:
	rm -f myenv *.o