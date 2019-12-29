DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS = -pg -g3 -gdwarf-2 -DDEBUG
	LDFLAGS += -pg
else
	CFLAGS= -DNDEBUG -std=c11 -O3 -s -Wall -Wextra -Werror -pedantic -flto -fomit-frame-pointer
endif

BIN_DIR = bin
LIB_DIR = lib
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS += -Iinclude
LDFLAGS += -Llib
LDLIBS += -ldlx

.PHONY: all clean

all: $(LIB_DIR)/libdlx.a $(BIN_DIR)/nqueens $(BIN_DIR)/example

$(LIB_DIR)/libdlx.a: $(OBJ_DIR)/dlx.o
	ar rcs $@ $^

$(BIN_DIR)/nqueens: $(OBJ_DIR)/nqueens.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR)/example: $(OBJ_DIR)/example.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)
