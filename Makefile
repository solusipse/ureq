# TODO: make single makefile

CC=gcc
CFLAGS+=-O2 -Wall
LIBS=

ifeq ($(OS),Windows_NT)
LIBS+=-lws2_32
endif

all:
	$(CC) -o example $(CFLAGS) examples/example.c $(LIBS)
	$(CC) -o server  $(CFLAGS) examples/server-example.c $(LIBS)
	$(CC) -o fs      $(CFLAGS) examples/filesystem-example.c $(LIBS)

esp8266:
	$(MAKE) -C ./esp8266/

clean:
	rm -f example
	rm -f server
	rm -f fs

.PHONY: all esp8266 clean
	