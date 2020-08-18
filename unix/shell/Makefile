CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-unused-variable \
 -O0 -ggdb
LD = gcc
LDFLAGS =

shell: main.o
	$(LD) $(LDFLAGS) -o shell main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

run: shell
	./shell
