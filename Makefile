.POSIX:
.SUFFIXES:

CC = cc
CFLAGS = -std=c17 -O3 -s -flto -march=native -MMD \
	-Wall -Wextra -Werror -pedantic -Wconversion
CPPFLAGS += -Iinclude
LDFLAGS += -Llib
LDLIBS += -ldlx

.PHONY: all
all: lib/libdlx.a

# library
lib/libdlx.a: obj/dlx.o | lib
	ar rcs $@ $^

obj/dlx.o: src/dlx.c | obj
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# examples
.PHONY: example
example: lib/libdlx.a bin/simple_example bin/nqueens bin/sudoku

bin/%: obj/%.o | bin
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

obj/%.o: examples/%.c | obj
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# folders
bin:
	mkdir -p bin

lib:
	mkdir -p lib

obj:
	mkdir -p obj

.PHONY: clean
clean:
	rm -rf lib obj bin

.PHONY: fmt
fmt:
	clang-format -i src/*.c src/*.h examples/*.c

-include $(OBJ:.o=.d)

