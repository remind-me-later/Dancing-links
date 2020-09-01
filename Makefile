BIN_DIR = bin
LIB_DIR = lib
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests
PROF_DIR = prof

OUT_DIR = $(OBJ_DIR) $(LIB_DIR)

CWARN = -Wall -Wextra -Werror -pedantic -std=c11
CFLAGS = -DNDEBUG -Ofast -s -flto
CPPFLAGS += -Iinclude
LDFLAGS +=
LDLIBS +=

.PHONY: all dir debug profile clean example

all: dir $(LIB_DIR)/libdlx.a 

debug: CFLAGS = $(CWARN) -DDEBUG -O0 -g3 -fsanitize=address -fsanitize=undefined
debug: LDFLAGS += -fsanitize=address -fsanitize=undefined
debug: clean all

profile: CFLAGS = -pg -fno-inline -fprofile-generate=$(PROF_DIR)
profile: LDFLAGS += -pg -lgcov --coverage
profile: clean all

clean:
	$(RM) $(OBJ)

example: all
	$(MAKE) -C examples

dir: | $(OUT_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(LIB_DIR)/libdlx.a: $(OBJ_DIR)/dlx.o 
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
