CC	= gcc
CFLAGS	= -Wall -pedantic -g -Wextra
OPENSSLFLAGS = -lssl -lcrypto


.PHONY:	all
all:	webserverm httpm

webserverm:				src/main.c buildm/helpers.o buildm/headers.o src/custom.c src/ssl.c buildm/common.o src/contenttype.c src/hashtable.c src/hash.c
						$(CC) $(CFLAGS) $(OPENSSLFLAGS) -o webserverm src/main.c buildm/helpers.o buildm/common.o buildm/headers.o src/custom.c src/ssl.c src/contenttype.c src/hashtable.c src/hash.c

httpm:					src/http.c buildm/helpers.o buildm/common.o buildm/headers.o
						$(CC) $(CFLAGS) src/http.c buildm/helpers.o buildm/common.o buildm/headers.o -o httpm

buildm/helpers.o:		src/helpers.c
						$(CC) $(CFLAGS) -o buildm/helpers.o -c src/helpers.c
buildm/common.o:		src/common.c
						$(CC) $(CFLAGS) -o buildm/common.o -c src/common.c
buildm/headers.o:		src/headers.c
						$(CC) $(CFLAGS) -o buildm/headers.o -c src/headers.c

.PHONY: clean
clean:
	-rm -f webserverm httpm buildm/*.o
