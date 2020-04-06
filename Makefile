CC	= gcc
CFLAGS	= -Wall -pedantic -g


.PHONY:	all
all:	webserver http

webserver:			src/main.c src/helpers.c src/headers.c src/custom.c src/ssl.c
					$(CC) $(CFLAGS) -pthread -lssl -lcrypto -o webserver src/main.c src/helpers.c src/headers.c src/custom.c src/ssl.c

http:				src/http.c src/helpers.c 
					$(CC) $(CFLAGS) -pthread src/http.c src/helpers.c -o http
.PHONY: clean
clean:
	-rm -f webserver http
