CC = gcc
INIH_DEFINITIONS = -DINI_USE_STACK=0 \
 -DINI_MAX_LINE=1024 -DINI_ALLOW_REALLOC=1 -DINI_INITIAL_ALLOC=128 \
 -DINI_CALL_HANDLER_ON_NEW_SECTION=1 -DINI_INLINE_COMMENT_PREFIXES="\";\#\""
CFLAGS = -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-unused-variable \
 -Wno-unused-but-set-variable -Werror -O0 -ggdb3 $(INIH_DEFINITIONS) \
 -Iinih

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

ini.o: inih
	$(CC) $(CFLAGS) -o ini.o -c inih/ini.c

inih: inih.tar.gz
	mkdir inih
	tar -xf inih.tar.gz -C inih
	cp inih/`tar --list -f inih.tar.gz | head -n1`/ini.c inih/ini.c
	cp inih/`tar --list -f inih.tar.gz | head -n1`/ini.h inih/ini.h

inih.tar.gz:
	curl -s https://api.github.com/repos/benhoyt/inih/releases/latest | \
     grep "tarball_url" | cut -d" " -f4 | tr -d ",\"" | \
     wget -i - -O inih.tar.gz

clean:
	rm -rf ./main.o ./sample ./ini.o ./libinih.a ./inih ./inih.tar.gz
