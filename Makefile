CC=clang

PROJECT=main

all: main hashfile genfile
	@echo "Done!"

main: main.c comments.h comments.c config.h
	${CC} main.c comments.c -o main 

SSL_LIBS=-L/usr/local/lib -lssl -lcrypto
SSL_FLAGS=-I/usr/local/include

hashfile: sha.c config.h
	${CC} sha.c -o hashfile  ${SSL_FLAGS} ${SSL_LIBS}

genfile: genfile.c config.h
	${CC} genfile.c -o genfile


.PHONY: clean

clean:
	rm -rf main hashfile genfile
