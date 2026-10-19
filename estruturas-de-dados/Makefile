# ===== Configuração =====
CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -O2 -g -Iinclude

OBJ_DIR := build
BIN_DIR := bin

ifeq ($(OS),Windows_NT)
    EXE := .exe
endif

# Biblioteca: as cinco estruturas
LIB_SRCS := $(wildcard src/*.c)
LIB_OBJS := $(LIB_SRCS:src/%.c=$(OBJ_DIR)/%.o)

DEMO     := $(BIN_DIR)/demo$(EXE)
CONTADOR := $(BIN_DIR)/contador-palavras$(EXE)
TESTES   := $(BIN_DIR)/testes$(EXE)

# ===== Regras =====
.PHONY: all test valgrind clean

all: $(DEMO) $(CONTADOR)

$(DEMO): $(OBJ_DIR)/demo.o $(LIB_OBJS) | $(BIN_DIR)
	$(CC) $^ -o $@

$(CONTADOR): $(OBJ_DIR)/contador.o $(LIB_OBJS) | $(BIN_DIR)
	$(CC) $^ -o $@

$(TESTES): $(OBJ_DIR)/executar.o $(OBJ_DIR)/test_estruturas.o $(LIB_OBJS) | $(BIN_DIR)
	$(CC) $^ -o $@

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: apps/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: tests/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -Itests -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Roda os testes automatizados
test: $(TESTES)
	./$(TESTES)

# Roda os testes dentro do Valgrind, que acusa qualquer vazamento de memória
valgrind: $(TESTES)
	valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all \
	         --error-exitcode=1 -q ./$(TESTES)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
