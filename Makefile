# TODO: make single makefile

CC=gcc
CFLAGS+=-O2 -Wall

all:
	$(CC) -o example $(CFLAGS) example.c
	$(CC) -o server  $(CFLAGS) server-example.c
	$(CC) -o fs      $(CFLAGS) filesystem-example.c

esp8266:
	$(MAKE) -C ./esp8266/

clean:
	rm -f example
	rm -f server

.PHONY: all esp8266 clean
