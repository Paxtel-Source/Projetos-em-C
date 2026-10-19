/*
 * Demonstração: um problema real resolvido com cada estrutura.
 *   - Pilha: conferir se os parênteses de uma expressão estão balanceados
 *   - Fila:  simular o atendimento de uma fila de banco
 *   - Lista: montar uma playlist e tocar ao contrário
 *   - Árvore: ordenar nomes enquanto são inseridos
 *   - Hash:  dicionário de capitais
 */
#include <stdio.h>
#include <string.h>

#include "arvore.h"
#include "fila.h"
#include "hash.h"
#include "lista.h"
#include "pilha.h"

/* ---------- Pilha ---------- */

static int balanceado(const char *expressao)
{
    Pilha *p = pilha_criar(NULL);    /* guarda ponteiros para a própria string */
    int ok = 1;

    for (const char *c = expressao; *c && ok; c++) {
        if (strchr("([{", *c)) {
            pilha_empilhar(p, (void *)c);
        } else if (strchr(")]}", *c)) {
            const char *abre = pilha_desempilhar(p);
            const char *par = (*c == ')') ? "(" : (*c == ']') ? "[" : "{";
            ok = abre != NULL && *abre == *par;
        }
    }
    ok = ok && pilha_vazia(p);
    pilha_destruir(p);
    return ok;
}

static void demo_pilha(void)
{
    puts("=== Pilha: parênteses balanceados ===");
    const char *exemplos[] = {"(a + b) * [c - d]", "{[()()]}", "(a + b))", "([)]"};
    for (size_t i = 0; i < sizeof exemplos / sizeof *exemplos; i++) {
        printf("  %-20s %s\n", exemplos[i], balanceado(exemplos[i]) ? "ok" : "desbalanceado");
    }
}

/* ---------- Fila ---------- */

static void demo_fila(void)
{
    puts("\n=== Fila: atendimento do banco ===");
    Fila *f = fila_criar(NULL);
    const char *clientes[] = {"Ana", "Bruno", "Carla", "Diego"};
    for (size_t i = 0; i < 4; i++) {
        fila_enfileirar(f, (void *)clientes[i]);
        printf("  Chegou: %s (fila com %zu)\n", clientes[i], fila_tamanho(f));
    }
    while (!fila_vazia(f)) {
        printf("  Atendendo: %s\n", (const char *)fila_desenfileirar(f));
    }
    fila_destruir(f);
}

/* ---------- Lista ---------- */

static void imprimir_musica(void *dado, void *contexto)
{
    int *posicao = contexto;
    printf("  %d. %s\n", ++*posicao, (const char *)dado);
}

static void demo_lista(void)
{
    puts("\n=== Lista ligada: playlist ===");
    Lista *l = lista_criar(NULL);
    lista_inserir_fim(l, "Abertura");
    lista_inserir_fim(l, "Segunda faixa");
    lista_inserir_fim(l, "Encerramento");
    lista_inserir_inicio(l, "Introdução");

    int posicao = 0;
    lista_para_cada(l, imprimir_musica, &posicao);
    puts("  -- tocando ao contrário --");
    lista_inverter(l);
    posicao = 0;
    lista_para_cada(l, imprimir_musica, &posicao);
    lista_destruir(l);
}

/* ---------- Árvore ---------- */

static int comparar_texto(const void *a, const void *b)
{
    return strcmp(a, b);
}

static void imprimir_nome(void *dado, void *contexto)
{
    (void)contexto;
    printf(" %s", (const char *)dado);
}

static void demo_arvore(void)
{
    puts("\n=== Árvore binária de busca: nomes em ordem ===");
    Arvore *a = arvore_criar(comparar_texto, NULL);
    const char *nomes[] = {"Marcos", "Beatriz", "Tiago", "Ana", "Helena", "Rafael", "Zeca"};
    printf("  Inseridos:");
    for (size_t i = 0; i < 7; i++) {
        arvore_inserir(a, (void *)nomes[i]);
        printf(" %s", nomes[i]);
    }
    printf("\n  Em ordem: ");
    arvore_em_ordem(a, imprimir_nome, NULL);
    printf("\n  Altura da árvore: %d (com %zu nomes)\n", arvore_altura(a), arvore_tamanho(a));
    arvore_destruir(a);
}

/* ---------- Hash ---------- */

static void demo_hash(void)
{
    puts("\n=== Tabela hash: capitais ===");
    Hash *h = hash_criar(NULL);
    hash_inserir(h, "SP", "São Paulo");
    hash_inserir(h, "RJ", "Rio de Janeiro");
    hash_inserir(h, "MG", "Belo Horizonte");
    hash_inserir(h, "BA", "Salvador");

    const char *consultas[] = {"MG", "BA", "AM"};
    for (size_t i = 0; i < 3; i++) {
        const char *capital = hash_obter(h, consultas[i]);
        printf("  %s -> %s\n", consultas[i], capital ? capital : "(não cadastrado)");
    }
    hash_destruir(h);
}

int main(void)
{
    demo_pilha();
    demo_fila();
    demo_lista();
    demo_arvore();
    demo_hash();
    return 0;
}
