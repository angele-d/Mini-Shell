CFLAGS = -std=c99 -Wall -Wformat -Wextra -pedantic -g
CC = gcc

all: myenv

run: run_myenv

myenv: myenv.o commands.o
	$(CC) myenv.o commands.o -o myenv
myenv.o: myenv.c
	$(CC) -c $(CFLAGS) myenv.c

commands.o: commands.c commands.h
	$(CC) -c $(CFLAGS) commands.c

run_myenv :
	./myenv

clean:
	rm -f myenv *.o