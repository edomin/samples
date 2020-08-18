CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-unused-variable \
 -Wno-unused-but-set-variable -Werror -O0 -ggdb3

all: sample

run: sample
	./sample

rebuild: clean sample

sample: main.o libkeysym2ucs.a
	$(CC) -L. -lkeysym2ucs `pkg-config --cflags --libs x11` -o sample main.o

libkeysym2ucs.a: keysym2ucs.o
	ar rcs libkeysym2ucs.a keysym2ucs.o

main.o: main.c
	$(CC) $(CFLAGS) -o main.o -c main.c

keysym2ucs.o: keysym2ucs/keysym2ucs.c keysym2ucs/keysym2ucs.h
	$(CC) $(CFLAGS) -o keysym2ucs.o -c keysym2ucs/keysym2ucs.c

keysym2ucs/keysym2ucs.c:
	-mkdir keysym2ucs
	wget "https://www.cl.cam.ac.uk/~mgk25/ucs/keysym2ucs.c" -O keysym2ucs/keysym2ucs.c

keysym2ucs/keysym2ucs.h:
	-mkdir keysym2ucs
	wget "https://www.cl.cam.ac.uk/~mgk25/ucs/keysym2ucs.h" -O keysym2ucs/keysym2ucs.h

clean:
	rm -rf sample main.o keysym2ucs.o libkeysym2ucs.a keysym2ucs
