CC=gcc
CFLAGS+=-O2 -Wall

all:
	$(CC) -o ureq $(CFLAGS) example.c

clean:
	rm -f ureq

.PHONY: clean
