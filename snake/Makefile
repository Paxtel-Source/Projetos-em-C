# ===== Configuração =====
TARGET  := snake
CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -O2 -g -Iinclude
# Usa a versão com suporte a UTF-8 (ncursesw) quando estiver instalada
LDLIBS  := $(shell pkg-config --libs ncursesw 2>/dev/null || echo -lncurses)

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
BIN  := $(BIN_DIR)/$(TARGET)
TESTES := $(BIN_DIR)/testes

# ===== Regras =====
.PHONY: all run test clean

all: $(BIN)

$(BIN): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

run: $(BIN)
	./$(BIN)

# Os testes usam só as regras do jogo (jogo.c), sem ncurses
$(TESTES): tests/test_jogo.c $(SRC_DIR)/jogo.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

test: $(TESTES)
	./$(TESTES)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
