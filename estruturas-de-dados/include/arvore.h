#ifndef ARVORE_H
#define ARVORE_H

#include <stdbool.h>
#include <stddef.h>

#include "comum.h"

/*
 * Árvore Binária de Busca (BST).
 * Para cada nó, tudo à esquerda é menor e tudo à direita é maior.
 * Buscar, inserir e remover custam O(altura): O(log n) se a árvore
 * estiver equilibrada e O(n) no pior caso (dados já ordenados).
 */
typedef struct Arvore Arvore;

Arvore *arvore_criar(FuncaoComparar comparar, FuncaoLiberar liberar);
void    arvore_destruir(Arvore *arvore);

/* Devolve false se faltar memória ou se o elemento já existir */
bool    arvore_inserir(Arvore *arvore, void *dado);
void   *arvore_buscar(const Arvore *arvore, const void *procurado);
bool    arvore_remover(Arvore *arvore, const void *procurado);

void   *arvore_minimo(const Arvore *arvore);
void   *arvore_maximo(const Arvore *arvore);

/* Percorre em ordem crescente */
void    arvore_em_ordem(const Arvore *arvore, FuncaoVisitar visitar, void *contexto);

size_t  arvore_tamanho(const Arvore *arvore);
int     arvore_altura(const Arvore *arvore);   /* -1 se vazia */

#endif /* ARVORE_H */
