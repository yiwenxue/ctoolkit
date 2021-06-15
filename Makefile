CC=clang

PROJECT=main

all: main hashfile genfile editor rsa_encrypt
	@echo "Done!"

main: main.c comments.h comments.c config.h
	${CC} main.c comments.c -o main 

SSL_LIBS=-L/usr/local/lib -lssl -lcrypto
SSL_FLAGS=-I/usr/local/include

hashfile: sha.c config.h
	${CC} sha.c -o hashfile  ${SSL_FLAGS} ${SSL_LIBS}

rsa_encrypt: rsa_encrypt.c config.h
	${CC} rsa_encrypt.c -o rsa_encrypt  ${SSL_FLAGS} ${SSL_LIBS}

genfile: genfile.c config.h
	${CC} genfile.c -o genfile

editor: editor.c config.h
	${CC} editor.c -o editor

.PHONY: clean

clean:
	rm -rf main hashfile genfile editor
