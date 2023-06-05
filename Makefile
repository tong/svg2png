PREFIX ?= /usr/local
CFLAGS = -lm -O3 -std=c11 -Wall -Wextra

all: svg2png

svg2png: *.c *.h
	$(CC) main.c $(CFLAGS) -o $@

install:
	install -D -t $(PREFIX)/bin svg2png

uninstall:
	rm -f $(PREFIX)/bin/svg2png

clean:
	rm -f svg2png

.PHONY: all install uninstall clean
