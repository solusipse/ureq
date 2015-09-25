# TODO: make single makefile

CC=gcc
CFLAGS+=-O2 -Wall

all:
	$(CC) -o example $(CFLAGS) examples/example.c
	$(CC) -o server  $(CFLAGS) examples/server-example.c
	$(CC) -o fs      $(CFLAGS) examples/filesystem-example.c

esp8266:
	$(MAKE) -C ./esp8266/

clean:
	rm -f example
	rm -f server
	rm -f fs

.PHONY: all esp8266 clean
