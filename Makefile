BIN_DIR = bin
LIB_DIR = lib
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests
PROF_DIR = prof

OUT_DIR = $(LIB_DIR) $(OBJ_DIR) $(BIN_DIR)

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CWARN = -Wall -Wextra -Werror -pedantic -std=c11
CFLAGS = -DNDEBUG -Ofast -s -flto
CPPFLAGS += -Iinclude
LDFLAGS += -Llib
#LDLIBS += -ldlx

.PHONY: all directories debug profile clean 

all: directories $(LIB_DIR)/libdlx.a $(BIN_DIR)/nqueens $(BIN_DIR)/example

directories: $(OUT_DIR)

debug: CFLAGS = $(CWARN) -DDEBUG -O0 -g3 -fsanitize=address -fsanitize=undefined
debug: LDFLAGS += -fsanitize=address -fsanitize=undefined
debug: clean all

profile: CFLAGS = -pg -fno-inline -fprofile-generate=$(PROF_DIR)
profile: LDFLAGS += -pg -lgcov --coverage
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
