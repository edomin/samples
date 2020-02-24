CC = gcc
INIH_DEFINITIONS = -DINI_USE_STACK=0 \
 -DINI_MAX_LINE=1024 -DINI_ALLOW_REALLOC=1 -DINI_INITIAL_ALLOC=128 \
 -DINI_CALL_HANDLER_ON_NEW_SECTION=1 -DINI_INLINE_COMMENT_PREFIXES="\";\#\""
CFLAGS = -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-unused-variable \
 -Wno-unused-but-set-variable -Werror -O0 -ggdb3 $(INIH_DEFINITIONS) \
 -I../contrib/inih

all: sample libinih.a

run: sample
	./sample

rebuild: clean sample

sample: main.o libinih.a
	$(CC) -o sample main.o -linih

libinih.a: ini.o
	ar rcs libinih.a ini.o

main.o: main.c
	$(CC) $(CFLAGS) -o main.o -c main.c

ini.o: ../contrib/inih/ini.c ../contrib/inih/ini.h
	$(CC) $(CFLAGS) -o ini.o -c ../contrib/inih/ini.c

clean:
	rm -f main.o
	rm -f ini.o
