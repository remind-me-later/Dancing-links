BIN_DIR = bin
LIB_DIR = lib
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests

OUT_DIR = $(LIB_DIR) $(OBJ_DIR) $(BIN_DIR)

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS= -DNDEBUG -std=c11 -O3 -s -Wall -Wextra -Werror -pedantic -flto -fomit-frame-pointer
CPPFLAGS += -Iinclude
LDFLAGS += -Llib
#LDLIBS += -ldlx

.PHONY: all directories debug profile clean 

all: directories $(LIB_DIR)/libdlx.a $(BIN_DIR)/nqueens $(BIN_DIR)/example

directories: $(OUT_DIR)

debug: CFLAGS = -O0 -g3 -gdwarf-2 -DDEBUG
debug: clean all

profile: CFLAGS = -pg -g3 -fno-inline
profile: LDFLAGS += -pg
profile: clean all

clean:
	$(RM) $(OBJ)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(LIB_DIR)/libdlx.a: $(OBJ_DIR)/dlx.o
	ar rcs $@ $^

$(BIN_DIR)/nqueens: $(OBJ_DIR)/nqueens.o $(OBJ_DIR)/dlx.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR)/example: $(OBJ_DIR)/example.o $(OBJ_DIR)/dlx.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
