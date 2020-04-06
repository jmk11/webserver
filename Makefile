CC	= gcc
CFLAGS	= -Wall -pedantic -g
OPENSSLFLAGS = -lssl -lcrypto


.PHONY:	all
all:	webserver http

webserver:			src/main.c src/helpers.c src/headers.c src/custom.c src/ssl.c src/common.c
					$(CC) $(CFLAGS) $(OPENSSLFLAGS) -o webserver src/main.c src/helpers.c src/headers.c src/custom.c src/ssl.c src/common.c

http:				src/http.c src/helpers.c src/common.c src/headers.c
					$(CC) $(CFLAGS) src/http.c src/helpers.c src/common.c src/headers.c -o http
.PHONY: clean
clean:
	-rm -f webserver http
