#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stddef.h>

#include "comum.h"

/*
 * Lista duplamente ligada.
 * Cada nó aponta para o anterior e para o próximo, então inserir e
 * remover nas duas pontas custa O(1).
 */
typedef struct Lista Lista;

Lista *lista_criar(FuncaoLiberar liberar);
void   lista_destruir(Lista *lista);

bool   lista_inserir_inicio(Lista *lista, void *dado);
bool   lista_inserir_fim(Lista *lista, void *dado);

/* Removem e devolvem o elemento (NULL se a lista estiver vazia) */
void  *lista_remover_inicio(Lista *lista);
void  *lista_remover_fim(Lista *lista);

/* Devolve o elemento na posição (0 = primeiro) ou NULL se não existir */
void  *lista_obter(const Lista *lista, size_t posicao);

/* Devolve o primeiro elemento igual a "procurado" ou NULL */
void  *lista_buscar(const Lista *lista, const void *procurado, FuncaoComparar comparar);

/* Remove (e libera) o primeiro elemento igual a "procurado" */
bool   lista_remover(Lista *lista, const void *procurado, FuncaoComparar comparar);

void   lista_para_cada(const Lista *lista, FuncaoVisitar visitar, void *contexto);
void   lista_inverter(Lista *lista);

size_t lista_tamanho(const Lista *lista);
bool   lista_vazia(const Lista *lista);

#endif /* LISTA_H */
