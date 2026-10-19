#ifndef FILA_H
#define FILA_H

#include <stdbool.h>
#include <stddef.h>

#include "comum.h"

/*
 * Fila (FIFO: o primeiro a entrar é o primeiro a sair).
 * Implementada com um vetor circular: o início e o fim "dão a volta"
 * no vetor, então enfileirar e desenfileirar custam O(1).
 */
typedef struct Fila Fila;

Fila  *fila_criar(FuncaoLiberar liberar);
void   fila_destruir(Fila *fila);

bool   fila_enfileirar(Fila *fila, void *dado);
void  *fila_desenfileirar(Fila *fila);   /* NULL se vazia */
void  *fila_frente(const Fila *fila);    /* NULL se vazia */

size_t fila_tamanho(const Fila *fila);
bool   fila_vazia(const Fila *fila);

#endif /* FILA_H */
