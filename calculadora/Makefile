# ===== Configuração =====
TARGET  := calculadora
CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude $(EXTRA_CFLAGS)
LDLIBS  := -lm

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

# Os testes usam todo o código, menos o main.c
LIB_OBJS := $(filter-out $(OBJ_DIR)/main.o,$(OBJS))
TEST_BIN := $(BIN_DIR)/testes$(EXE)

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

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(OBJ_DIR)/testes.o $(LIB_OBJS) | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDLIBS)

$(OBJ_DIR)/testes.o: tests/testes.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
