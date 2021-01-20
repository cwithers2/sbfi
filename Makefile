CC=cc
CFLAGS=-Ofast
SH=/bin/sh

build: main.c
	$(CC) -o brainfuck main.c $(CFLAGS)
test: build
	$(SH) ./test.sh
.PHONY: clean

clean:
	rm -f ./brainfuck
