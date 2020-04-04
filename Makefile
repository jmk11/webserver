CC	= gcc
CFLAGS	= -Wall -pedantic -g


.PHONY:	all
all:	webserver

webserver:			main.c helpers.c
					$(CC) $(CFLAGS) -pthread -o webserver main.c helpers.c

.PHONY: clean
clean:
	-rm -f webserver webserver.o

