CC = gcc
CFLAGS = -Wall -Wextra -g
VCOMMAND = valgrind --leak-check=full --error-exitcode=1

all: part2

run: part2
	$(VCOMMAND) ./part2

part2: part2.c student.h
	$(CC) $(CFLAGS) part2.c -o part2

clean:
	rm -f part2 program.out