BIN_DIR = bin
LIB_DIR = lib
SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS += -Iinclude
CFLAGS += -O3 -s -Wall -Wextra -Werror -pedantic -flto -fomit-frame-pointer
LDFLAGS += -Llib
LDLIBS += -ldlx

.PHONY: all clean

all: $(LIB_DIR)/libdlx.a $(BIN_DIR)/8queens $(BIN_DIR)/example

$(LIB_DIR)/libdlx.a: $(OBJ_DIR)/dlx.o
	ar rcs $@ $^

$(BIN_DIR)/8queens: $(OBJ_DIR)/8queens.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR)/example: $(OBJ_DIR)/example.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)
