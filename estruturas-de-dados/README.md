# 🧱 Biblioteca de Estruturas de Dados

As cinco estruturas de dados mais usadas em programação, implementadas do zero em C: **lista ligada, pilha, fila, árvore binária de busca e tabela hash**. Cada uma tem seu próprio `.h` e `.c`, guarda qualquer tipo de dado (`void *`), tem testes automatizados e é verificada contra vazamento de memória com o **Valgrind**.

## As estruturas

| Estrutura | Arquivo | Como funciona | Custo das operações |
|-----------|---------|---------------|---------------------|
| Lista duplamente ligada | `lista.c` | Nós que apontam para o anterior e o próximo | Inserir/remover nas pontas: O(1) |
| Pilha (LIFO) | `pilha.c` | Vetor que dobra de tamanho quando enche | Empilhar/desempilhar: O(1) |
| Fila (FIFO) | `fila.c` | Vetor circular: início e fim "dão a volta" | Enfileirar/desenfileirar: O(1) |
| Árvore binária de busca | `arvore.c` | Menores à esquerda, maiores à direita | Buscar/inserir/remover: O(log n) em média |
| Tabela hash | `hash.c` | Função djb2 + encadeamento, cresce acima de 75% de ocupação | Inserir/buscar/remover: O(1) em média |

## Programas de exemplo

**`demo`**: um problema real resolvido com cada estrutura.

```
=== Pilha: parênteses balanceados ===
  (a + b) * [c - d]    ok
  ([)]                 desbalanceado

=== Fila: atendimento do banco ===
  Chegou: Ana (fila com 1)
  ...
  Atendendo: Ana

=== Árvore binária de busca: nomes em ordem ===
  Inseridos: Marcos Beatriz Tiago Ana Helena Rafael Zeca
  Em ordem:  Ana Beatriz Helena Marcos Rafael Tiago Zeca

=== Tabela hash: capitais ===
  MG -> Belo Horizonte
  AM -> (não cadastrado)
```

**`contador-palavras`**: lê um arquivo de texto e mostra as palavras mais frequentes, usando a tabela hash para contar.

```
$ ./bin/contador-palavras exemplos/texto.txt 5
129 palavras, 75 diferentes

  1. a                    10
  2. uma                  8
  3. o                    5
  4. e                    4
  5. é                    4
```

## Estrutura do projeto

```
estruturas-de-dados/
├── include/          # Uma interface (.h) por estrutura + comum.h
├── src/              # As implementações (.c)
├── apps/             # demo.c e contador.c
├── tests/            # Testes automatizados (mini framework próprio)
├── exemplos/         # Texto de exemplo para o contador
├── Makefile
└── README.md
```

## Dependências

- GCC (ou Clang) com suporte a C11 e `make`
- Valgrind (opcional, só para `make valgrind`, disponível no Linux e no WSL)
- No Windows: [MSYS2](https://www.msys2.org/), MinGW ou WSL

## Como compilar e executar

```bash
cd estruturas-de-dados
make                                          # compila demo e contador
./bin/demo
./bin/contador-palavras exemplos/texto.txt 10
make test                                     # roda os 66 testes
make valgrind                                 # roda os testes procurando vazamento de memória
make clean
```

## Como usar em outro programa

Todas as estruturas seguem o mesmo padrão: `criar`, operações e `destruir`. Quem cria diz como liberar os elementos (ou `NULL` para não liberar):

```c
#include "hash.h"

Hash *idades = hash_criar(free);          /* free libera cada valor ao destruir */

int *idade = malloc(sizeof *idade);
*idade = 19;
hash_inserir(idades, "Gabriel", idade);

int *encontrada = hash_obter(idades, "Gabriel");   /* NULL se não existir */
hash_destruir(idades);                             /* libera tudo de uma vez */
```

## Detalhes técnicos

- **Genéricas com `void *`:** as estruturas guardam ponteiros e recebem funções de comparação e de liberação (`FuncaoComparar`, `FuncaoLiberar`), como a `qsort` da biblioteca padrão.
- **Tipos opacos:** os `.h` só declaram `typedef struct Lista Lista;`. Os campos ficam escondidos no `.c`, então ninguém mexe nos ponteiros por fora.
- **Ponteiro para ponteiro:** a inserção e a remoção na árvore e na hash usam `No **link`, o que elimina o caso especial da raiz ou do primeiro elemento.
- **Remoção na árvore:** trata os três casos (folha, um filho e dois filhos, usando o sucessor).
- **Redimensionamento da hash:** ao passar de 75% de ocupação, a tabela dobra e redistribui as entradas sem copiar chaves nem valores.
- **Sem vazamentos:** `make valgrind` falha se sobrar qualquer byte alocado, e roda automaticamente no GitHub Actions.

## Conceitos praticados

Ponteiros e ponteiros para ponteiros, alocação dinâmica (`malloc`, `realloc`, `free`), ponteiros para funções, `void *`, tipos opacos, recursão, análise de complexidade (notação O), testes automatizados e Valgrind.
