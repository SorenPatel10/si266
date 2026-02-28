CC = gcc
CFLAGS = -Wall -Wextra -g

all: gatherer

gatherer: gatherer.c
	$(CC) $(CFLAGS) gatherer.c -o gatherer

run: gatherer
	./gatherer

clean:
	rm -f gatherer
