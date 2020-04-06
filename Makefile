CC	= gcc
CFLAGS	= -Wall -pedantic -g


.PHONY:	all
all:	webserver

webserver:			src/main.c src/helpers.c src/headers.c src/custom.c src/openssl.c
					$(CC) $(CFLAGS) -pthread -lssl -lcrypto -o webserver src/main.c src/helpers.c src/headers.c src/custom.c src/openssl.c

.PHONY: clean
clean:
	-rm -f webserver webserver.o

