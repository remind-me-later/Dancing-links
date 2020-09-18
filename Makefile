CWARN = -Wall -Wextra -Werror -pedantic
CFLAGS = -std=c11 -O3 -s -flto -march=native -MMD $(CWARN)
CPPFLAGS += -Iinclude
LDFLAGS += -Llib
LDLIBS += -ldlx

all: lib/libdlx.a 

# library
# -------
lib/libdlx.a: obj/dlx.o | lib
	ar rcs $@ $^

obj/dlx.o: src/dlx.c | obj
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# examples
# --------
example: lib/libdlx.a bin/example bin/nqueens bin/sudoku 

bin/%: obj/%.o | bin
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

obj/%.o: examples/%.c | obj
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# folders
# -------
bin:
	mkdir -p bin

lib:
	mkdir -p lib

obj:
	mkdir -p obj

clean:
	$(RM) lib/libdlx.a obj/* bin/*

-include $(OBJ:.o=.d)

.PHONY: all clean example
