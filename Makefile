CC=gcc
CFLAGS+=-O2 -Wall

all:
	$(CC) -o example $(CFLAGS) example.c

clean:
	rm -f ureq

.PHONY: clean
