# ❌⭕ Jogo da Velha (Terminal)

Jogo da Velha para **dois jogadores** no mesmo terminal, escrito em C. O tabuleiro é uma **matriz 3x3** e a verificação de vitória percorre linhas, colunas e diagonais com **loops e lógica condicional**.

## Funcionalidades

- Tabuleiro desenhado no terminal, com os números das casas livres como guia
- Validação de jogadas (fora do intervalo, casa ocupada ou texto inválido)
- Detecção de vitória em linhas, colunas e nas duas diagonais
- Detecção de empate ("deu velha")
- Placar acumulado entre partidas

## Estrutura

```
jogo-da-velha/
├── include/
│   └── tabuleiro.h   # Constantes, enum de estado e declarações
├── src/
│   ├── tabuleiro.c   # Regras: jogar, exibir e verificar vencedor
│   └── main.c        # Laço do jogo, turnos e placar
├── tests/
│   └── testes.c      # Testes automáticos (make test)
├── Makefile
└── README.md
```

## Dependências

- Compilador C (GCC ou Clang) com suporte a C11
- `make`
- No Windows: [MSYS2](https://www.msys2.org/), MinGW ou WSL

## Como compilar e executar

1. Entre na pasta do projeto:
   ```bash
   cd jogo-da-velha
   ```
2. Compile:
   ```bash
   make
   ```
3. Execute:
   ```bash
   ./bin/jogo-da-velha
   ```
4. Para rodar os testes:
   ```bash
   make test
   ```
5. Para apagar os arquivos compilados:
   ```bash
   make clean
   ```

## Exemplo de partida

```
  X | O | X
 ---+---+---
  4 | X | O
 ---+---+---
  7 | 8 | X

*** Jogador X venceu! ***

Placar -> X: 1 | O: 0 | Empates: 0
Jogar novamente? (s/n):
```

## Conceitos praticados

Matrizes (`char tab[3][3]`), vetores, laços aninhados, `enum`, conversão de entrada com `strtol`, separação entre regras do jogo (`tabuleiro.c`) e interface (`main.c`).
