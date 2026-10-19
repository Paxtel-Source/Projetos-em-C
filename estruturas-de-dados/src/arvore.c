#include "arvore.h"

#include <stdlib.h>

typedef struct No {
    void      *dado;
    struct No *esquerda;
    struct No *direita;
} No;

struct Arvore {
    No            *raiz;
    size_t         tamanho;
    FuncaoComparar comparar;
    FuncaoLiberar  liberar;
};

/* ---------- Ciclo de vida ---------- */

Arvore *arvore_criar(FuncaoComparar comparar, FuncaoLiberar liberar)
{
    Arvore *arvore = calloc(1, sizeof *arvore);
    if (arvore != NULL) {
        arvore->comparar = comparar;
        arvore->liberar = liberar;
    }
    return arvore;
}

static void destruir_nos(No *no, FuncaoLiberar liberar)
{
    if (no == NULL) return;
    destruir_nos(no->esquerda, liberar);
    destruir_nos(no->direita, liberar);
    if (liberar) liberar(no->dado);
    free(no);
}

void arvore_destruir(Arvore *arvore)
{
    if (arvore == NULL) return;
    destruir_nos(arvore->raiz, arvore->liberar);
    free(arvore);
}

/* ---------- Inserção e busca ---------- */

bool arvore_inserir(Arvore *arvore, void *dado)
{
    /* "link" aponta para o ponteiro que vai receber o novo nó.
     * Assim não é preciso tratar a raiz como caso especial. */
    No **link = &arvore->raiz;
    while (*link != NULL) {
        int c = arvore->comparar(dado, (*link)->dado);
        if (c == 0) return false;               /* já existe */
        link = (c < 0) ? &(*link)->esquerda : &(*link)->direita;
    }

    No *no = malloc(sizeof *no);
    if (no == NULL) return false;
    no->dado = dado;
    no->esquerda = no->direita = NULL;
    *link = no;
    arvore->tamanho++;
    return true;
}

void *arvore_buscar(const Arvore *arvore, const void *procurado)
{
    No *no = arvore->raiz;
    while (no != NULL) {
        int c = arvore->comparar(procurado, no->dado);
        if (c == 0) return no->dado;
        no = (c < 0) ? no->esquerda : no->direita;
    }
    return NULL;
}

/* ---------- Remoção ---------- */

bool arvore_remover(Arvore *arvore, const void *procurado)
{
    No **link = &arvore->raiz;
    while (*link != NULL) {
        int c = arvore->comparar(procurado, (*link)->dado);
        if (c == 0) break;
        link = (c < 0) ? &(*link)->esquerda : &(*link)->direita;
    }
    No *alvo = *link;
    if (alvo == NULL) return false;

    if (arvore->liberar) arvore->liberar(alvo->dado);

    if (alvo->esquerda == NULL || alvo->direita == NULL) {
        /* Caso 1 e 2: zero ou um filho, o filho toma o lugar do nó */
        *link = alvo->esquerda ? alvo->esquerda : alvo->direita;
        free(alvo);
    } else {
        /* Caso 3: dois filhos. O sucessor (menor da subárvore direita)
         * ocupa o lugar do nó removido. */
        No **link_sucessor = &alvo->direita;
        while ((*link_sucessor)->esquerda != NULL) {
            link_sucessor = &(*link_sucessor)->esquerda;
        }
        No *sucessor = *link_sucessor;
        alvo->dado = sucessor->dado;
        *link_sucessor = sucessor->direita;
        free(sucessor);
    }
    arvore->tamanho--;
    return true;
}

/* ---------- Consultas ---------- */

void *arvore_minimo(const Arvore *arvore)
{
    No *no = arvore->raiz;
    if (no == NULL) return NULL;
    while (no->esquerda) no = no->esquerda;
    return no->dado;
}

void *arvore_maximo(const Arvore *arvore)
{
    No *no = arvore->raiz;
    if (no == NULL) return NULL;
    while (no->direita) no = no->direita;
    return no->dado;
}

static void em_ordem(const No *no, FuncaoVisitar visitar, void *contexto)
{
    if (no == NULL) return;
    em_ordem(no->esquerda, visitar, contexto);
    visitar(no->dado, contexto);
    em_ordem(no->direita, visitar, contexto);
}

void arvore_em_ordem(const Arvore *arvore, FuncaoVisitar visitar, void *contexto)
{
    em_ordem(arvore->raiz, visitar, contexto);
}

static int altura(const No *no)
{
    if (no == NULL) return -1;
    int e = altura(no->esquerda);
    int d = altura(no->direita);
    return 1 + (e > d ? e : d);
}

int arvore_altura(const Arvore *arvore)
{
    return altura(arvore->raiz);
}

size_t arvore_tamanho(const Arvore *arvore)
{
    return arvore->tamanho;
}
