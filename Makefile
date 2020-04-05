CC	= gcc
CFLAGS	= -Wall -pedantic -g


.PHONY:	all
all:	webserver

webserver:			src/main.c src/helpers.c src/headers.c src/custom.c
					$(CC) $(CFLAGS) -pthread -o webserver src/main.c src/helpers.c src/headers.c src/custom.c

.PHONY: clean
clean:
	-rm -f webserver webserver.o

