CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-unused-variable \
 -Wno-unused-but-set-variable -Werror -O0 -ggdb3

all: sample

run: sample
	./sample

rebuild: clean sample

sample: main.o xmempool/libxmempool.a
	$(CC) -o sample main.o -Lxmempool -lxmempool

xmempool/libxmempool.a: xmempool
	cd xmempool; make static; mv xmempool.a libxmempool.a;

xmempool: xmempool.tar.gz
	mkdir xmempool
	tar -xf xmempool.tar.gz -C xmempool
	mv xmempool/`tar --list -f xmempool.tar.gz | head -n1`/* xmempool

xmempool.tar.gz:
	curl -s https://api.github.com/repos/XadillaX/xmempool/tags | \
     grep "tarball_url" | head -n 1 | cut -d" " -f6 | tr -d ",\"" | \
     wget -i - -O xmempool.tar.gz

main.o: main.c
	$(CC) $(CFLAGS) -o main.o -c main.c

clean:
	rm -rf ./main.o ./sample ./xmempool xmempool.tar.gz
