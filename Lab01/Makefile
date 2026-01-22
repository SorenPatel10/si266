CC = gcc
CFLAGS = -Wall -Wextra -g

all: calc

run: 
	./calc

calc: main math
	$(CC) -o calc $(CFLAGS) main.o math.o

main: main.c math.h
	$(CC) -c main.c $(CFLAGS) -o main.o

math: math.c math.h
	$(CC) -c math.c $(CFLAGS) -o math.o

clean:
	rm -f calc *.o