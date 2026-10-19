# ===== Configuração =====
TARGET  := minishell
CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -O2 -g -Iinclude
LDLIBS  :=

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin

# No Windows o executável recebe a extensão .exe
ifeq ($(OS),Windows_NT)
    EXE := .exe
endif

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
BIN  := $(BIN_DIR)/$(TARGET)$(EXE)

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

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Testes automatizados: alimenta o shell com comandos e confere a saída
test: $(BIN)
   	bash tests/testar.sh ./$(BIN)
