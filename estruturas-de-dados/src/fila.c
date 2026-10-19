#include "fila.h"

#include <stdlib.h>

#define CAPACIDADE_INICIAL 8

struct Fila {
    void        **itens;
    size_t        inicio;      /* posição do primeiro elemento */
    size_t        tamanho;
    size_t        capacidade;
    FuncaoLiberar liberar;
};

Fila *fila_criar(FuncaoLiberar liberar)
{
    Fila *fila = calloc(1, sizeof *fila);
    if (fila != NULL) {
        fila->liberar = liberar;
    }
    return fila;
}

void fila_destruir(Fila *fila)
{
    if (fila == NULL) return;
    if (fila->liberar) {
        for (size_t i = 0; i < fila->tamanho; i++) {
            fila->liberar(fila->itens[(fila->inicio + i) % fila->capacidade]);
        }
    }
    free(fila->itens);
    free(fila);
}

/* Dobra a capacidade, copiando os elementos para o começo do novo vetor */
static bool crescer(Fila *fila)
{
    size_t nova = fila->capacidade ? fila->capacidade * 2 : CAPACIDADE_INICIAL;
    void **itens = malloc(nova * sizeof *itens);
    if (itens == NULL) return false;

    for (size_t i = 0; i < fila->tamanho; i++) {
        itens[i] = fila->itens[(fila->inicio + i) % fila->capacidade];
    }
    free(fila->itens);
    fila->itens = itens;
    fila->inicio = 0;
    fila->capacidade = nova;
    return true;
}

bool fila_enfileirar(Fila *fila, void *dado)
{
    if (fila->tamanho == fila->capacidade && !crescer(fila)) {
        return false;
    }
    size_t fim = (fila->inicio + fila->tamanho) % fila->capacidade;
    fila->itens[fim] = dado;
    fila->tamanho++;
    return true;
}

void *fila_desenfileirar(Fila *fila)
{
    if (fila->tamanho == 0) return NULL;

    void *dado = fila->itens[fila->inicio];
    fila->inicio = (fila->inicio + 1) % fila->capacidade;
    fila->tamanho--;
    return dado;
}

void *fila_frente(const Fila *fila)
{
    return fila->tamanho ? fila->itens[fila->inicio] : NULL;
}

size_t fila_tamanho(const Fila *fila)
{
    return fila->tamanho;
}

bool fila_vazia(const Fila *fila)
{
    return fila->tamanho == 0;
}
