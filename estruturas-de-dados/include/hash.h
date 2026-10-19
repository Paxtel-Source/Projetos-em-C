#ifndef HASH_H
#define HASH_H

#include <stdbool.h>
#include <stddef.h>

#include "comum.h"

/*
 * Tabela Hash (dicionário de texto -> valor).
 * A função hash djb2 transforma a chave em uma posição do vetor.
 * Colisões são resolvidas com encadeamento (uma lista em cada posição),
 * e a tabela dobra de tamanho quando passa de 75% de ocupação.
 * Inserir, buscar e remover custam O(1) em média.
 */
typedef struct Hash Hash;

/* A tabela guarda uma cópia própria de cada chave */
Hash  *hash_criar(FuncaoLiberar liberar_valor);
void   hash_destruir(Hash *hash);

/* Insere ou substitui o valor da chave (o valor antigo é liberado) */
bool   hash_inserir(Hash *hash, const char *chave, void *valor);
void  *hash_obter(const Hash *hash, const char *chave);   /* NULL se não existir */
bool   hash_contem(const Hash *hash, const char *chave);
bool   hash_remover(Hash *hash, const char *chave);

/* Visita cada par; a ordem não é definida */
typedef void (*FuncaoVisitarPar)(const char *chave, void *valor, void *contexto);
void   hash_para_cada(const Hash *hash, FuncaoVisitarPar visitar, void *contexto);

size_t hash_tamanho(const Hash *hash);
size_t hash_capacidade(const Hash *hash);

/* Exposta para os testes e para quem quiser estudar a distribuição */
unsigned long hash_djb2(const char *texto);

#endif /* HASH_H */
