CFLAGS = -std=c99 -Wall -Wformat -Wextra -pedantic -g
CC = gcc

all: build

build: myenv

run: run_myenv

myenv: myenv.o commands.o
	$(CC) myenv.o commands.o -o myenv
myenv.o: myenv.c
	$(CC) -c $(CFLAGS) myenv.c

commands.o: commands.c commands.h
	$(CC) -c $(CFLAGS) commands.c

run_myenv :
	./myenv

kill_port:
	@lsof -ti:4444 | xargs kill -9 2>/dev/null || echo "No process on port 4444"

clean: kill_port
	rm -f myenv *.o