CC := gcc
CFLAGS := -std=c99 -Wall -Werror -pedantic -Wextra -Isrc

test : test.exe
	./test.exe

test.exe : ./src/callocs.c ./test/main.c
	$(CC) $(CFLAGS) -o $@ $^
