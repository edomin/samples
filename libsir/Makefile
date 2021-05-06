CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-unused-variable \
 -Wno-unused-but-set-variable -Werror -O0 -ggdb3 -I../contrib/libsir

all: sample

run: sample
	./sample

rebuild: clean sample

sample: main.o libsir/build/lib/libsir_s.a
	$(CC) -o sample main.o -Llibsir/build/lib -lsir_s -lpthread

libsir/build/lib/libsir_s.a: libsir
	cd libsir ; make static

libsir: libsir.tar.gz
	mkdir libsir
	tar -xf libsir.tar.gz -C libsir
	mv libsir/`tar --list -f libsir.tar.gz | head -n1`/* libsir

libsir.tar.gz:
	curl -s https://api.github.com/repos/ryanlederman/libsir/releases/latest | \
     grep "tarball_url" | cut -d" " -f4 | tr -d ",\"" | \
     wget -i - -O libsir.tar.gz

main.o: main.c
	$(CC) $(CFLAGS) -o main.o -c main.c

clean:
	rm -rf ./main.o ./sample ./libsir libsir.tar.gz sample.log
