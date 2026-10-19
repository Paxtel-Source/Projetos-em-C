#include <stdlib.h>
#include <string.h>

#include "arvore.h"
#include "fila.h"
#include "hash.h"
#include "lista.h"
#include "pilha.h"
#include "teste.h"

/* ---------- Auxiliares para guardar inteiros ---------- */

static int *novo_int(int valor)
{
    int *p = malloc(sizeof *p);
    *p = valor;
    return p;
}

static int comparar_int(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

/* Guarda os valores visitados em um vetor, para conferir a ordem */
typedef struct {
    int    valores[64];
    size_t n;
} Coleta;

static void coletar(void *dado, void *contexto)
{
    Coleta *c = contexto;
    c->valores[c->n++] = *(int *)dado;
}

/* ================= LISTA ================= */

void testar_lista(void)
{
    GRUPO("lista");
    Lista *l = lista_criar(free);
    VERIFICAR(lista_vazia(l));
    VERIFICAR(lista_remover_inicio(l) == NULL);

    lista_inserir_fim(l, novo_int(2));
    lista_inserir_fim(l, novo_int(3));
    lista_inserir_inicio(l, novo_int(1));
    VERIFICAR(lista_tamanho(l) == 3);
    VERIFICAR(*(int *)lista_obter(l, 0) == 1);
    VERIFICAR(*(int *)lista_obter(l, 2) == 3);
    VERIFICAR(lista_obter(l, 3) == NULL);

    int procurado = 2;
    VERIFICAR(*(int *)lista_buscar(l, &procurado, comparar_int) == 2);
    VERIFICAR(lista_remover(l, &procurado, comparar_int));
    VERIFICAR(!lista_remover(l, &procurado, comparar_int));
    VERIFICAR(lista_tamanho(l) == 2);

    for (int i = 4; i <= 6; i++) lista_inserir_fim(l, novo_int(i));
    lista_inverter(l);
    Coleta c = {0};
    lista_para_cada(l, coletar, &c);
    int esperado[] = {6, 5, 4, 3, 1};
    VERIFICAR(c.n == 5 && memcmp(c.valores, esperado, sizeof esperado) == 0);

    int *ultimo = lista_remover_fim(l);
    VERIFICAR(*ultimo == 1);
    free(ultimo);
    int *primeiro = lista_remover_inicio(l);
    VERIFICAR(*primeiro == 6);
    free(primeiro);
    VERIFICAR(lista_tamanho(l) == 3);

    /* Esvaziar e reutilizar */
    while (!lista_vazia(l)) free(lista_remover_inicio(l));
    lista_inserir_inicio(l, novo_int(9));
    VERIFICAR(*(int *)lista_obter(l, 0) == 9);

    lista_destruir(l);
}

/* ================= PILHA ================= */

void testar_pilha(void)
{
    GRUPO("pilha");
    Pilha *p = pilha_criar(free);
    VERIFICAR(pilha_vazia(p));
    VERIFICAR(pilha_desempilhar(p) == NULL);

    /* Mais elementos que a capacidade inicial, para testar o crescimento */
    for (int i = 0; i < 100; i++) pilha_empilhar(p, novo_int(i));
    VERIFICAR(pilha_tamanho(p) == 100);
    VERIFICAR(*(int *)pilha_topo(p) == 99);

    int ok = 1;
    for (int i = 99; i >= 50; i--) {
        int *v = pilha_desempilhar(p);
        if (*v != i) ok = 0;
        free(v);
    }
    VERIFICAR(ok);
    VERIFICAR(pilha_tamanho(p) == 50);
    pilha_destruir(p);   /* libera os 50 que sobraram */
}

/* ================= FILA ================= */

void testar_fila(void)
{
    GRUPO("fila");
    Fila *f = fila_criar(free);
    VERIFICAR(fila_vazia(f));
    VERIFICAR(fila_desenfileirar(f) == NULL);

    /* Mistura entradas e saídas para o vetor circular "dar a volta" */
    int proximo_a_sair = 0, proximo_a_entrar = 0, ok = 1;
    for (int rodada = 0; rodada < 20; rodada++) {
        for (int i = 0; i < 5; i++) fila_enfileirar(f, novo_int(proximo_a_entrar++));
        for (int i = 0; i < 3; i++) {
            int *v = fila_desenfileirar(f);
            if (*v != proximo_a_sair++) ok = 0;
            free(v);
        }
    }
    VERIFICAR(ok);
    VERIFICAR(fila_tamanho(f) == 40);
    VERIFICAR(*(int *)fila_frente(f) == proximo_a_sair);
    fila_destruir(f);
}

/* ================= ÁRVORE ================= */

void testar_arvore(void)
{
    GRUPO("arvore");
    Arvore *a = arvore_criar(comparar_int, free);
    VERIFICAR(arvore_altura(a) == -1);
    VERIFICAR(arvore_minimo(a) == NULL);

    int valores[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65};
    for (size_t i = 0; i < sizeof valores / sizeof *valores; i++) {
        VERIFICAR(arvore_inserir(a, novo_int(valores[i])));
    }
    int *repetido = novo_int(40);
    VERIFICAR(!arvore_inserir(a, repetido));   /* duplicado é recusado */
    free(repetido);

    VERIFICAR(arvore_tamanho(a) == 10);
    VERIFICAR(arvore_altura(a) == 3);
    VERIFICAR(*(int *)arvore_minimo(a) == 20);
    VERIFICAR(*(int *)arvore_maximo(a) == 80);

    int x = 65;
    VERIFICAR(arvore_buscar(a, &x) != NULL);
    x = 66;
    VERIFICAR(arvore_buscar(a, &x) == NULL);

    /* Remove folha, nó com um filho e nó com dois filhos (a raiz) */
    x = 20; VERIFICAR(arvore_remover(a, &x));
    x = 60; VERIFICAR(arvore_remover(a, &x));
    x = 50; VERIFICAR(arvore_remover(a, &x));
    x = 999; VERIFICAR(!arvore_remover(a, &x));

    Coleta c = {0};
    arvore_em_ordem(a, coletar, &c);
    int esperado[] = {30, 35, 40, 45, 65, 70, 80};
    VERIFICAR(c.n == 7 && memcmp(c.valores, esperado, sizeof esperado) == 0);
    arvore_destruir(a);
}

/* ================= HASH ================= */

static void somar_valores(const char *chave, void *valor, void *contexto)
{
    (void)chave;
    *(int *)contexto += *(int *)valor;
}

void testar_hash(void)
{
    GRUPO("hash");
    VERIFICAR(hash_djb2("") == 5381);
    VERIFICAR(hash_djb2("abc") != hash_djb2("acb"));

    Hash *h = hash_criar(free);
    VERIFICAR(hash_obter(h, "nada") == NULL);

    hash_inserir(h, "um", novo_int(1));
    hash_inserir(h, "dois", novo_int(2));
    VERIFICAR(*(int *)hash_obter(h, "dois") == 2);
    hash_inserir(h, "dois", novo_int(22));     /* substitui e libera o antigo */
    VERIFICAR(*(int *)hash_obter(h, "dois") == 22);
    VERIFICAR(hash_tamanho(h) == 2);

    /* A chave é copiada: alterar o texto original não afeta a tabela */
    char chave[] = "tres";
    hash_inserir(h, chave, novo_int(3));
    chave[0] = 'X';
    VERIFICAR(hash_contem(h, "tres"));
    VERIFICAR(!hash_contem(h, "Xres"));

    /* Força vários redimensionamentos */
    size_t capacidade_inicial = hash_capacidade(h);
    char texto[32];
    for (int i = 0; i < 1000; i++) {
        snprintf(texto, sizeof texto, "chave-%d", i);
        hash_inserir(h, texto, novo_int(i));
    }
    VERIFICAR(hash_tamanho(h) == 1003);
    VERIFICAR(hash_capacidade(h) > capacidade_inicial);
    VERIFICAR((double)hash_tamanho(h) / hash_capacidade(h) <= 0.75);
    VERIFICAR(*(int *)hash_obter(h, "chave-777") == 777);

    VERIFICAR(hash_remover(h, "chave-777"));
    VERIFICAR(!hash_remover(h, "chave-777"));
    VERIFICAR(hash_obter(h, "chave-777") == NULL);

    int soma = 0;
    hash_para_cada(h, somar_valores, &soma);
    /* 0+1+...+999 - 777 + 1 + 22 + 3 */
    VERIFICAR(soma == 999 * 1000 / 2 - 777 + 26);
    hash_destruir(h);
}
