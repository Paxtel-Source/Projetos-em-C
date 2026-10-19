# 🧮 Calculadora de Linha de Comando

Calculadora em C que executa as quatro operações básicas (`+`, `-`, `*`, `/`) pelo terminal, **validando as entradas** e **tratando divisão por zero**.

## Funcionalidades

- Modo **direto**: passa a expressão como argumentos (`./bin/calculadora 10 / 4`)
- Modo **interativo**: digita várias expressões em sequência, até escrever `sair`
- Rejeita entradas inválidas como `12abc`, `abc` ou operadores desconhecidos
- Bloqueia divisão por zero com mensagem de erro clara
- Aceita `x` como sinônimo de `*` (o `*` sozinho é expandido pelo terminal)

## Estrutura

```
calculadora/
├── include/
│   └── calculadora.h   # Declarações das funções
├── src/
│   ├── calculadora.c   # Lógica das operações e validação
│   └── main.c          # Interface com o usuário
├── tests/
│   └── testes.c        # Testes automáticos (make test)
├── Makefile
└── README.md
```

## Dependências

- Compilador C (GCC ou Clang) com suporte a C11
- `make`
- No Windows: [MSYS2](https://www.msys2.org/), MinGW ou WSL

## Como compilar e executar

1. Clone o repositório e entre na pasta do projeto:
   ```bash
   cd calculadora
   ```
2. Compile:
   ```bash
   make
   ```
3. Execute no modo direto:
   ```bash
   ./bin/calculadora 10 + 5
   ./bin/calculadora 7 x 3
   ./bin/calculadora 9 / 0
   ```
4. Ou no modo interativo:
   ```bash
   ./bin/calculadora
   ```
5. Para rodar os testes:
   ```bash
   make test
   ```
6. Para apagar os arquivos compilados:
   ```bash
   make clean
   ```

## Exemplo de uso

```
$ ./bin/calculadora 10 / 4
10 / 4 = 2.5

$ ./bin/calculadora 9 / 0
Erro: divisao por zero nao e permitida.

$ ./bin/calculadora 12abc + 1
Erro: entrada numerica invalida.
```

## Conceitos praticados

`switch`, `enum`, `strtod` com validação via `errno`, argumentos de linha de comando (`argc`/`argv`), leitura segura com `fgets` + `sscanf`.
