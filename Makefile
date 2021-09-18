CFLAGS=-O2 -Wall -Wextra -lX11 -lpci
PREFIX=$(HOME)/.local

all: timer install

timer: timer.c
	$(CC) timer.c -o timer $(CFLAGS) -lncurses
	strip timer

install: timer
	mkdir -p $(PREFIX)/bin
	install ./timer $(PREFIX)/bin/timer
