#include "hash.h"

#include <stdlib.h>
#include <string.h>

#define CAPACIDADE_INICIAL 16
#define CARGA_MAXIMA       0.75

typedef struct Entrada {
    char           *chave;
    void           *valor;
    struct Entrada *proxima;
} Entrada;

struct Hash {
    Entrada     **baldes;     /* cada posição é uma lista encadeada */
    size_t        capacidade;
    size_t        tamanho;
    FuncaoLiberar liberar_valor;
};

/* ---------- Função hash ---------- */

/* djb2 (Dan Bernstein): hash = hash * 33 + caractere */
unsigned long hash_djb2(const char *texto)
{
    unsigned long hash = 5381;
    for (const unsigned char *p = (const unsigned char *)texto; *p; p++) {
        hash = ((hash << 5) + hash) + *p;
    }
    return hash;
}

static char *copiar_texto(const char *texto)
{
    size_t n = strlen(texto) + 1;
    char *copia = malloc(n);
    if (copia) memcpy(copia, texto, n);
    return copia;
}

/* ---------- Ciclo de vida ---------- */

Hash *hash_criar(FuncaoLiberar liberar_valor)
{
    Hash *hash = malloc(sizeof *hash);
    if (hash == NULL) return NULL;

    hash->baldes = calloc(CAPACIDADE_INICIAL, sizeof *hash->baldes);
    if (hash->baldes == NULL) {
        free(hash);
        return NULL;
    }
    hash->capacidade = CAPACIDADE_INICIAL;
    hash->tamanho = 0;
    hash->liberar_valor = liberar_valor;
    return hash;
}

void hash_destruir(Hash *hash)
{
    if (hash == NULL) return;
    for (size_t i = 0; i < hash->capacidade; i++) {
        Entrada *e = hash->baldes[i];
        while (e != NULL) {
            Entrada *proxima = e->proxima;
            if (hash->liberar_valor) hash->liberar_valor(e->valor);
            free(e->chave);
            free(e);
            e = proxima;
        }
    }
    free(hash->baldes);
    free(hash);
}

/* ---------- Redimensionamento ---------- */

/* Dobra o vetor e redistribui as entradas (sem copiar chaves nem valores) */
static bool redimensionar(Hash *hash)
{
    size_t nova = hash->capacidade * 2;
    Entrada **baldes = calloc(nova, sizeof *baldes);
    if (baldes == NULL) return false;

    for (size_t i = 0; i < hash->capacidade; i++) {
        Entrada *e = hash->baldes[i];
        while (e != NULL) {
            Entrada *proxima = e->proxima;
            size_t pos = hash_djb2(e->chave) % nova;
            e->proxima = baldes[pos];
            baldes[pos] = e;
            e = proxima;
        }
    }
    free(hash->baldes);
    hash->baldes = baldes;
    hash->capacidade = nova;
    return true;
}

/* ---------- Operações ---------- */

static Entrada *encontrar(const Hash *hash, const char *chave)
{
    Entrada *e = hash->baldes[hash_djb2(chave) % hash->capacidade];
    while (e != NULL && strcmp(e->chave, chave) != 0) {
        e = e->proxima;
    }
    return e;
}

bool hash_inserir(Hash *hash, const char *chave, void *valor)
{
    Entrada *existente = encontrar(hash, chave);
    if (existente != NULL) {
        if (hash->liberar_valor && existente->valor != valor) {
            hash->liberar_valor(existente->valor);
        }
        existente->valor = valor;
        return true;
    }

    /* Se a tabela ficar cheia demais, as listas crescem e tudo fica lento */
    if ((double)(hash->tamanho + 1) / hash->capacidade > CARGA_MAXIMA) {
        if (!redimensionar(hash)) return false;
    }

    Entrada *e = malloc(sizeof *e);
    char *copia = copiar_texto(chave);
    if (e == NULL || copia == NULL) {
        free(e);
        free(copia);
        return false;
    }
    size_t pos = hash_djb2(chave) % hash->capacidade;
    e->chave = copia;
    e->valor = valor;
    e->proxima = hash->baldes[pos];
    hash->baldes[pos] = e;
    hash->tamanho++;
    return true;
}

void *hash_obter(const Hash *hash, const char *chave)
{
    Entrada *e = encontrar(hash, chave);
    return e ? e->valor : NULL;
}

bool hash_contem(const Hash *hash, const char *chave)
{
    return encontrar(hash, chave) != NULL;
}

bool hash_remover(Hash *hash, const char *chave)
{
    Entrada **link = &hash->baldes[hash_djb2(chave) % hash->capacidade];
    while (*link != NULL) {
        if (strcmp((*link)->chave, chave) == 0) {
            Entrada *alvo = *link;
            *link = alvo->proxima;
            if (hash->liberar_valor) hash->liberar_valor(alvo->valor);
            free(alvo->chave);
            free(alvo);
            hash->tamanho--;
            return true;
        }
        link = &(*link)->proxima;
    }
    return false;
}

void hash_para_cada(const Hash *hash, FuncaoVisitarPar visitar, void *contexto)
{
    for (size_t i = 0; i < hash->capacidade; i++) {
        for (Entrada *e = hash->baldes[i]; e != NULL; e = e->proxima) {
            visitar(e->chave, e->valor, contexto);
        }
    }
}

size_t hash_tamanho(const Hash *hash)
{
    return hash->tamanho;
}

size_t hash_capacidade(const Hash *hash)
{
    return hash->capacidade;
}
