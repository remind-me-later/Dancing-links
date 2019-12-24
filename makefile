CFLAGS = -O3 -s -Wall -Wextra -Werror -pedantic -flto -fomit-frame-pointer

.PHONY: target 

target: dlx.a 8queens example

dlx.a: dlx.o
	ar rcs $@ $^

dlx.o: dlx.c dlx.h
	$(CC) $(CFLAGS) -c -o $@ $<

example: dlx_example.c dlx.a
	$(CC) $(CFLAGS) -o $@ $^

8queens: 8queens.c dlx.a
	$(CC) $(CFLAGS) -o $@ $^
