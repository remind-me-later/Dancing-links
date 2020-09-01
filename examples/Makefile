BIN_DIR = ../bin
LIB_DIR = ../lib
SRC_DIR = .
OBJ_DIR = ../obj
TEST_DIR = ../tests
PROF_DIR = ../prof

OUT_DIR = $(OBJ_DIR) $(BIN_DIR)

CWARN = -Wall -Wextra -Werror -pedantic -std=c11
CFLAGS = -DNDEBUG -O3 -s -flto
CPPFLAGS += -I../include
LDFLAGS += -L$(LIB_DIR)
LDLIBS += -ldlx

.PHONY: all debug profile clean 

all: dir $(BIN_DIR)/example $(BIN_DIR)/nqueens $(BIN_DIR)/sudoku

debug: CFLAGS = $(CWARN) -DDEBUG -O0 -g3 -fsanitize=address -fsanitize=undefined
debug: LDFLAGS += -fsanitize=address -fsanitize=undefined
debug: clean all

profile: CFLAGS = -pg -fno-inline -fprofile-generate=$(PROF_DIR)
profile: LDFLAGS += -pg -lgcov --coverage
profile: clean all

clean:
	$(RM) $(OBJ)

dir: $(OUT_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(BIN_DIR)/%: $(OBJ_DIR)/%.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
