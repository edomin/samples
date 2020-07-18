CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-unused-variable \
 -Wno-unused-but-set-variable -Werror -O0 -ggdb3 -I.

all: sample

run: sample
	./sample --foo --bar=yes -x -yyes -xzyno --bar=no

rebuild: clean sample

sample: main.o
	$(CC) -o sample main.o

main.o: main.c ketopt.h
	$(CC) $(CFLAGS) -o main.o -c main.c

ketopt.h:
	wget https://raw.githubusercontent.com/attractivechaos/klib/master/ketopt.h

clean:
	rm -f main.o sample ketopt.h
