CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-unused-variable \
 -Wno-unused-but-set-variable -Werror -O0 -ggdb3 -I../contrib/libsir

all: sample ../contrib/libsir/build/lib/libsir_s.a

run: sample
	./sample

rebuild: clean sample

sample: main.o ../contrib/libsir/build/lib/libsir_s.a
	$(CC) -o sample main.o -L../contrib/libsir/build/lib -lsir_s -lpthread

../contrib/libsir/build/lib/libsir_s.a:
	cd ../contrib/libsir ; make static

main.o: main.c
	$(CC) $(CFLAGS) -o main.o -c main.c

clean:
	rm -f main.o
	rm -rf ../contrib/libsir/build
