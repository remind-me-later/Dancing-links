BIN_DIR = bin
LIB_DIR = lib
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS= -DNDEBUG -std=c11 -O3 -s -Wall -Wextra -Werror -pedantic -flto -fomit-frame-pointer
CPPFLAGS += -Iinclude
LDFLAGS += -Llib
LDLIBS += -ldlx

.PHONY: all clean

all: $(LIB_DIR)/libdlx.a $(BIN_DIR)/nqueens $(BIN_DIR)/example

debug: CFLAGS = -O0 -g3 -gdwarf-2 -DDEBUG
debug: all

profile: CFLAGS = -O0 -pg -g3 -gdwarf-2 -DDEBUG
profile: LDFLAGS += -pg
profile: all

clean:
	$(RM) $(OBJ)

$(LIB_DIR)/libdlx.a: $(OBJ_DIR)/dlx.o
	ar rcs $@ $^

$(BIN_DIR)/nqueens: $(OBJ_DIR)/nqueens.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR)/example: $(OBJ_DIR)/example.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
