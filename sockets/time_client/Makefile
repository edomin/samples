CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-unused-parameter -O2
LD = gcc
LDFLAGS = -s

time: main.o
	$(LD) $(LDFLAGS) -o time main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

run: time
	./time

clean:
	-rm ./main.o ./time
