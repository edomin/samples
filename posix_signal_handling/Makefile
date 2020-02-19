CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Werror -O0 -ggdb3

all: sample so.so

run: sample so.so
	./sample

rebuild: clean sample so.so

sample: main.o
	$(CC) -o sample main.o -ldl

so.so: so.o
	$(CC) -shared -o so.so so.o

main.o: main.c
	$(CC) $(CFLAGS) -o main.o -c main.c

so.o: so.c
	$(CC) $(CFLAGS) -fPIC -o so.o -c so.c

clean:
	rm -f main.o
	rm -f so.o
