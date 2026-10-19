#include "lista.h"

#include <stdlib.h>

typedef struct No {
    void      *dado;
    struct No *anterior;
    struct No *proximo;
} No;

struct Lista {
    No           *inicio;
    No           *fim;
    size_t        tamanho;
    FuncaoLiberar liberar;
};

/* ---------- Auxiliares ---------- */

static No *novo_no(void *dado)
{
    No *no = malloc(sizeof *no);
    if (no != NULL) {
        no->dado = dado;
        no->anterior = no->proximo = NULL;
    }
    return no;
}

/* Desliga o nó da lista e devolve o dado dele (sem liberar o dado) */
static void *desligar(Lista *lista, No *no)
{
    if (no->anterior) no->anterior->proximo = no->proximo;
    else              lista->inicio = no->proximo;

    if (no->proximo) no->proximo->anterior = no->anterior;
    else             lista->fim = no->anterior;

    void *dado = no->dado;
    free(no);
    lista->tamanho--;
    return dado;
}

/* ---------- Ciclo de vida ---------- */

Lista *lista_criar(FuncaoLiberar liberar)
{
    Lista *lista = calloc(1, sizeof *lista);
    if (lista != NULL) {
        lista->liberar = liberar;
    }
    return lista;
}

void lista_destruir(Lista *lista)
{
    if (lista == NULL) return;

    No *atual = lista->inicio;
    while (atual != NULL) {
        No *proximo = atual->proximo;
        if (lista->liberar) lista->liberar(atual->dado);
        free(atual);
        atual = proximo;
    }
    free(lista);
}

/* ---------- Inserção ---------- */

bool lista_inserir_inicio(Lista *lista, void *dado)
{
    No *no = novo_no(dado);
    if (no == NULL) return false;

    no->proximo = lista->inicio;
    if (lista->inicio) lista->inicio->anterior = no;
    else               lista->fim = no;
    lista->inicio = no;
    lista->tamanho++;
    return true;
}

bool lista_inserir_fim(Lista *lista, void *dado)
{
    No *no = novo_no(dado);
    if (no == NULL) return false;

    no->anterior = lista->fim;
    if (lista->fim) lista->fim->proximo = no;
    else            lista->inicio = no;
    lista->fim = no;
    lista->tamanho++;
    return true;
}

/* ---------- Remoção ---------- */

void *lista_remover_inicio(Lista *lista)
{
    return lista->inicio ? desligar(lista, lista->inicio) : NULL;
}

void *lista_remover_fim(Lista *lista)
{
    return lista->fim ? desligar(lista, lista->fim) : NULL;
}

bool lista_remover(Lista *lista, const void *procurado, FuncaoComparar comparar)
{
    for (No *no = lista->inicio; no != NULL; no = no->proximo) {
        if (comparar(no->dado, procurado) == 0) {
            void *dado = desligar(lista, no);
            if (lista->liberar) lista->liberar(dado);
            return true;
        }
    }
    return false;
}

/* ---------- Consulta ---------- */

void *lista_obter(const Lista *lista, size_t posicao)
{
    if (posicao >= lista->tamanho) return NULL;

    /* Começa pela ponta mais próxima: no máximo metade da lista é percorrida */
    No *no;
    if (posicao < lista->tamanho / 2) {
        no = lista->inicio;
        for (size_t i = 0; i < posicao; i++) no = no->proximo;
    } else {
        no = lista->fim;
        for (size_t i = lista->tamanho - 1; i > posicao; i--) no = no->anterior;
    }
    return no->dado;
}

void *lista_buscar(const Lista *lista, const void *procurado, FuncaoComparar comparar)
{
    for (No *no = lista->inicio; no != NULL; no = no->proximo) {
        if (comparar(no->dado, procurado) == 0) return no->dado;
    }
    return NULL;
}

void lista_para_cada(const Lista *lista, FuncaoVisitar visitar, void *contexto)
{
    for (No *no = lista->inicio; no != NULL; no = no->proximo) {
        visitar(no->dado, contexto);
    }
}

void lista_inverter(Lista *lista)
{
    No *atual = lista->inicio;
    while (atual != NULL) {
        No *proximo = atual->proximo;
        atual->proximo = atual->anterior;
        atual->anterior = proximo;
        atual = proximo;
    }
    No *antigo_inicio = lista->inicio;
    lista->inicio = lista->fim;
    lista->fim = antigo_inicio;
}

size_t lista_tamanho(const Lista *lista)
{
    return lista->tamanho;
}

bool lista_vazia(const Lista *lista)
{
    return lista->tamanho == 0;
}
