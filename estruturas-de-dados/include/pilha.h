#ifndef PILHA_H
#define PILHA_H

#include <stdbool.h>
#include <stddef.h>

#include "comum.h"

/*
 * Pilha (LIFO: o último a entrar é o primeiro a sair).
 * Implementada com um vetor que dobra de tamanho quando enche,
 * então empilhar custa O(1) em média.
 */
typedef struct Pilha Pilha;

Pilha *pilha_criar(FuncaoLiberar liberar);
void   pilha_destruir(Pilha *pilha);

bool   pilha_empilhar(Pilha *pilha, void *dado);
void  *pilha_desempilhar(Pilha *pilha);   /* NULL se vazia */
void  *pilha_topo(const Pilha *pilha);    /* NULL se vazia */

size_t pilha_tamanho(const Pilha *pilha);
bool   pilha_vazia(const Pilha *pilha);

#endif /* PILHA_H */
