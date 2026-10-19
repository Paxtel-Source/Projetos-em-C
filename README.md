# 💻 Projetos em C

[![CI](https://github.com/Paxtel-Source/projetos-em-c/actions/workflows/ci.yml/badge.svg)](https://github.com/Paxtel-Source/projetos-em-c/actions/workflows/ci.yml)

Coleção de projetos de terminal em **linguagem C** para praticar os fundamentos da linguagem: entrada e saída, validação de dados, structs, arquivos, matrizes e operadores bit a bit.

| Projeto | Descrição | Conceitos principais |
|---------|-----------|----------------------|
| [🧮 Calculadora](calculadora/) | Operações `+ - * /` via terminal, com validação e tratamento de divisão por zero | `argc/argv`, `strtod`, `switch`, `enum` |
| [📒 Agenda de Contatos](agenda-contatos/) | CRUD de contatos salvos em arquivo de texto | `struct`, `fopen`, `fgets`, `fprintf`, `fclose` |
| [❌⭕ Jogo da Velha](jogo-da-velha/) | Jogo para dois jogadores no terminal, com placar | Matrizes, vetores, laços, lógica condicional |
| [🔢 Conversor de Bases](conversor-bases/) | Converte entre decimal, binário, octal e hexadecimal | Operadores bit a bit (`&`, `>>`), `uint64_t` |

## Organização

Todos os projetos seguem a mesma estrutura:

```
projeto/
├── include/     # Cabeçalhos (.h)
├── src/         # Código-fonte (.c)
├── tests/       # Testes (make test)
├── Makefile     # Compila com um simples "make"
├── README.md    # O que faz e como usar
└── .gitignore   # Ignora binários e arquivos gerados
```

## Requisitos

- GCC (ou Clang) com suporte a C11
- `make`
- **Windows:** use [MSYS2](https://www.msys2.org/), MinGW ou WSL

## Como compilar

Compilar **todos** os projetos de uma vez, a partir da raiz:

```bash
make
```

Ou compilar apenas um deles:

```bash
cd calculadora
make
./bin/calculadora 10 + 5
```

Para rodar os testes de todos os projetos:

```bash
make test
```

O GitHub Actions compila tudo com GCC e Clang (avisos tratados como erro) e roda os testes a cada push.

Para limpar todos os arquivos compilados:

```bash
make clean
```

Os executáveis ficam na pasta `bin/` de cada projeto. Veja o README de cada pasta para instruções detalhadas.
