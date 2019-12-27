SRC_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS += -Iinclude
CFLAGS += -O3 -s -Wall -Wextra -Werror -pedantic -flto -fomit-frame-pointer
LDFLAGS += -Llib
LDLIBS += -ldlx

.PHONY: all clean 

all: libdlx 8queens example

libdlx: $(OBJ_DIR)/dlx.o
	ar rcs $(LIB_DIR)/$@.a $^

8queens: $(OBJ_DIR)/8queens.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $(BIN_DIR)/$@

example: $(OBJ_DIR)/example.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $(BIN_DIR)/$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)
