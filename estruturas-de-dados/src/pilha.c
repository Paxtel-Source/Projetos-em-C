#include "pilha.h"

#include <stdlib.h>

#define CAPACIDADE_INICIAL 8

struct Pilha {
    void        **itens;
    size_t        tamanho;
    size_t        capacidade;
    FuncaoLiberar liberar;
};

Pilha *pilha_criar(FuncaoLiberar liberar)
{
    Pilha *pilha = calloc(1, sizeof *pilha);
    if (pilha != NULL) {
        pilha->liberar = liberar;
    }
    return pilha;
}

void pilha_destruir(Pilha *pilha)
{
    if (pilha == NULL) return;
    if (pilha->liberar) {
        for (size_t i = 0; i < pilha->tamanho; i++) {
            pilha->liberar(pilha->itens[i]);
        }
    }
    free(pilha->itens);
    free(pilha);
}

bool pilha_empilhar(Pilha *pilha, void *dado)
{
    if (pilha->tamanho == pilha->capacidade) {
        size_t nova = pilha->capacidade ? pilha->capacidade * 2 : CAPACIDADE_INICIAL;
        void **itens = realloc(pilha->itens, nova * sizeof *itens);
        if (itens == NULL) return false;
        pilha->itens = itens;
        pilha->capacidade = nova;
    }
    pilha->itens[pilha->tamanho++] = dado;
    return true;
}

void *pilha_desempilhar(Pilha *pilha)
{
    return pilha->tamanho ? pilha->itens[--pilha->tamanho] : NULL;
}

void *pilha_topo(const Pilha *pilha)
{
    return pilha->tamanho ? pilha->itens[pilha->tamanho - 1] : NULL;
}

size_t pilha_tamanho(const Pilha *pilha)
{
    return pilha->tamanho;
}

bool pilha_vazia(const Pilha *pilha)
{
    return pilha->tamanho == 0;
}
