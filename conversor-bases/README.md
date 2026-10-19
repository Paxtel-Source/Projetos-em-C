# 🔢 Conversor de Bases Numéricas

Programa em C que converte números inteiros entre **Decimal**, **Binário**, **Octal** e **Hexadecimal**. As conversões para binário, octal e hexadecimal usam **operadores bit a bit** (`&` e `>>`), o que torna o projeto ótimo para entender como os números são representados na memória.

## Funcionalidades

- Entrada em qualquer uma das quatro bases, com saída em todas elas
- Aceita prefixos opcionais: `0b` (binário), `0o` (octal) e `0x` (hexadecimal)
- Hexadecimal sem diferenciar maiúsculas (`ff` = `FF`)
- Valida dígitos fora da base (ex.: `2` em binário) e detecta overflow acima de 64 bits
- Modo direto (argumentos) e modo interativo

## Como funciona a parte bit a bit

Como 2, 8 e 16 são potências de 2, cada dígito corresponde a um grupo fixo de bits:

| Base | Bits por dígito | Máscara |
|------|-----------------|---------|
| 2    | 1               | `0x1`   |
| 8    | 3               | `0x7`   |
| 16   | 4               | `0xF`   |

O algoritmo pega o dígito menos significativo com `valor & mascara`, descarta esses bits com `valor >>= bits` e repete até o valor chegar a zero. A conversão para decimal usa `%` e `/`, já que 10 não é potência de 2.

## Estrutura

```
conversor-bases/
├── include/
│   └── conversor.h   # Declarações das funções de conversão
├── src/
│   ├── conversor.c   # Parsing e conversões (bitwise)
│   └── main.c        # Interface com o usuário
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
   cd conversor-bases
   ```
2. Compile:
   ```bash
   make
   ```
3. Execute no modo direto (`<numero> <base>`):
   ```bash
   ./bin/conversor 255 10
   ./bin/conversor FF 16
   ./bin/conversor 0b101010 2
   ```
4. Ou no modo interativo:
   ```bash
   ./bin/conversor
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
$ ./bin/conversor 255 10

  Decimal     : 255
  Binario     : 11111111
  Octal       : 377
  Hexadecimal : FF
```

## Conceitos praticados

Operadores bit a bit (`&`, `>>`, `<<`), `uint64_t`, detecção de overflow, manipulação de strings, tabelas de consulta (`"0123456789ABCDEF"`).
