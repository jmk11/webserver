CC	= gcc
CFLAGS	= -Wall -pedantic -g -Wextra
OPENSSLFLAGS = -lssl -lcrypto


.PHONY:	all
all:	webserver http

webserver:				src/main.c src/helpers.c src/headers.c src/custom.c src/ssl.c src/common.c build/hashtable.o
						$(CC) $(CFLAGS) $(OPENSSLFLAGS) -o webserver src/main.c src/helpers.c src/headers.c src/custom.c src/ssl.c src/common.c build/hashtable.o
build/hashtable.o:		src/hashtable.c build/hash.o
build/hash.o:			src/hash.c
						$(CC) $(CFLAGS) -c src/hash.c -o build/hash.o

http:					src/http.c src/helpers.c src/common.c src/headers.c
						$(CC) $(CFLAGS) src/http.c src/helpers.c src/common.c src/headers.c -o http
.PHONY: clean
clean:
	-rm -f webserver http *.o
