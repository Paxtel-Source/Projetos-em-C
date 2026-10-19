# 💻 Projetos em C

![Compilar e testar](https://github.com/Paxtel-Source/Projetos-em-C/actions/workflows/compilar-e-testar.yml/badge.svg)

Coleção de projetos de terminal em **linguagem C**, dos fundamentos (entrada e saída, structs, arquivos) até temas de sistemas: estruturas de dados, processos do sistema operacional e jogos em tempo real.

## 🟢 Fundamentos

| Projeto | Descrição | Conceitos principais |
|---------|-----------|----------------------|
| [🧮 Calculadora](calculadora/) | Operações `+ - * /` via terminal, com validação e tratamento de divisão por zero | `argc/argv`, `strtod`, `switch`, `enum` |
| [📒 Agenda de Contatos](agenda-contatos/) | CRUD de contatos salvos em arquivo binário | `struct`, `fopen`, `fread`, `fwrite`, `fclose` |
| [❌⭕ Jogo da Velha](jogo-da-velha/) | Jogo para dois jogadores no terminal, com placar | Matrizes, vetores, laços, lógica condicional |
| [🔢 Conversor de Bases](conversor-bases/) | Converte entre decimal, binário, octal e hexadecimal | Operadores bit a bit (`&`, `>>`), `uint64_t` |
| [🎓 Sistema Escolar](sistema-escolar/) | CRUD completo de alunos e notas, com médias e situação | `struct`, `malloc`/`realloc`, `fread`/`fwrite`, modularização |
| [🗜️ Compactador RLE](compactador-rle/) | Compacta e descompacta arquivos com Run-Length Encoding | `fgetc`/`fputc`, ponteiros, strings, arquivos binários |

## 🟡 Intermediário

| Projeto | Descrição | Conceitos principais |
|---------|-----------|----------------------|
| [🧱 Estruturas de Dados](estruturas-de-dados/) | Lista ligada, pilha, fila, árvore binária de busca e tabela hash do zero, com testes e Valgrind | `void *`, ponteiros para funções, tipos opacos, complexidade |
| [🐚 Mini Shell](mini-shell/) | Interpretador de comandos com pipes, redirecionamentos, curingas e histórico | `fork`, `execvp`, `pipe`, `dup2`, `waitpid`, sinais |
| [🐍 Snake](snake/) | Jogo da cobrinha no terminal, com velocidade crescente e recorde salvo | `ncurses`, laço de jogo, fila circular, lógica separada da tela |

## Organização

Todos os projetos seguem a mesma estrutura:

```
projeto/
├── include/     # Cabeçalhos (.h)
├── src/         # Código-fonte (.c)
├── tests/       # Testes automatizados (nos projetos que têm)
├── Makefile     # Compila com um simples "make"
├── README.md    # O que faz e como usar
└── .gitignore   # Ignora binários e arquivos gerados
```

## Requisitos

- GCC (ou Clang) com suporte a C11 e `make`
- **Snake:** biblioteca ncurses (`sudo apt install libncurses-dev` no Ubuntu/WSL; no macOS já vem instalada)
- **Mini Shell e Snake:** Linux, macOS ou [WSL](https://learn.microsoft.com/pt-br/windows/wsl/install), porque usam recursos do sistema que não existem no Windows comum
- **Demais projetos no Windows:** [MSYS2](https://www.msys2.org/), MinGW ou WSL

## Como compilar e testar

Compilar **todos** os projetos de uma vez, a partir da raiz (Linux, macOS ou WSL):

```bash
make
```

Rodar todos os testes automatizados:

```bash
make test
```

Ou compilar apenas um projeto:

```bash
cd calculadora
make
./bin/calculadora 10 + 5
```

Para limpar todos os arquivos compilados:

```bash
make clean
```

Os executáveis ficam na pasta `bin/` de cada projeto. Veja o README de cada pasta para instruções detalhadas.

## Verificação automática

A cada envio para o GitHub, o **GitHub Actions** (veja `.github/workflows/compilar-e-testar.yml`):

1. compila todos os projetos e falha se aparecer qualquer aviso do compilador;
2. roda os testes do Compactador RLE, das Estruturas de Dados, do Mini Shell e do Snake;
3. roda as Estruturas de Dados dentro do **Valgrind**, que acusa qualquer vazamento de memória.

O selo no topo deste README fica verde quando tudo passa.
