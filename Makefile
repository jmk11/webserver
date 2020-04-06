CC	= gcc
CFLAGS	= -Wall -pedantic -g


.PHONY:	all
all:	webserver

webserver:			src/main.c src/helpers.c src/headers.c src/custom.c src/ssl.c
					$(CC) $(CFLAGS) -pthread -lssl -lcrypto -o webserver src/main.c src/helpers.c src/headers.c src/custom.c src/ssl.c

.PHONY: clean
clean:
	-rm -f webserver webserver.o

