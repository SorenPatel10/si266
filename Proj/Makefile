CC = gcc
CFLAGS = -Wall -Wextra -g
THREAD = -pthread

all: gatherer

gatherer: gatherer.c
	$(CC) $(CFLAGS) gatherer.c -o gatherer $(THREAD)

run: gatherer
	./gatherer

clean:
	rm -f gatherer
