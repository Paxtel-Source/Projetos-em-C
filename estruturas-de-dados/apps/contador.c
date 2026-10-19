/*
 * Contador de palavras: lê um arquivo de texto e mostra as palavras
 * que mais aparecem. Usa a tabela hash para contar e qsort para ordenar.
 *
 * Uso: ./bin/contador-palavras <arquivo> [quantidade]
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

#define TAM_PALAVRA 64

typedef struct {
    const char *palavra;
    int         vezes;
} Contagem;

typedef struct {
    Contagem *itens;
    size_t    n;
} Vetor;

/* Bytes acima de 127 fazem parte de letras acentuadas em UTF-8 */
static int faz_parte_da_palavra(int c)
{
    return isalnum(c) || c >= 128 || c == '-';
}

static void copiar_para_vetor(const char *chave, void *valor, void *contexto)
{
    Vetor *v = contexto;
    v->itens[v->n].palavra = chave;
    v->itens[v->n].vezes = *(int *)valor;
    v->n++;
}

/* Quantas letras aparecem na tela (em UTF-8, "é" ocupa 2 bytes mas 1 letra) */
static int largura_na_tela(const char *texto)
{
    int largura = 0;
    for (const unsigned char *p = (const unsigned char *)texto; *p; p++) {
        if ((*p & 0xC0) != 0x80) largura++;   /* ignora bytes de continuação */
    }
    return largura;
}

static int mais_frequente_primeiro(const void *a, const void *b)
{
    const Contagem *x = a, *y = b;
    return y->vezes - x->vezes;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo> [quantidade]\n", argv[0]);
        return 1;
    }
    int quantidade = argc > 2 ? atoi(argv[2]) : 10;
    if (quantidade <= 0) quantidade = 10;

    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror(argv[1]);
        return 1;
    }

    Hash *contagem = hash_criar(free);
    char palavra[TAM_PALAVRA];
    size_t tamanho = 0;
    long total = 0;
    int c;

    /* Lê caractere por caractere; o EOF final também fecha a última palavra */
    do {
        c = fgetc(fp);
        if (c != EOF && faz_parte_da_palavra(c)) {
            if (tamanho < TAM_PALAVRA - 1) {
                palavra[tamanho++] = (char)(c < 128 ? tolower(c) : c);
            }
        } else if (tamanho > 0) {
            palavra[tamanho] = '\0';
            tamanho = 0;
            total++;

            int *vezes = hash_obter(contagem, palavra);
            if (vezes) {
                (*vezes)++;
            } else {
                vezes = malloc(sizeof *vezes);
                *vezes = 1;
                hash_inserir(contagem, palavra, vezes);
            }
        }
    } while (c != EOF);
    fclose(fp);

    Vetor v = {malloc(hash_tamanho(contagem) * sizeof(Contagem) + 1), 0};
    hash_para_cada(contagem, copiar_para_vetor, &v);
    qsort(v.itens, v.n, sizeof *v.itens, mais_frequente_primeiro);

    printf("%ld palavras, %zu diferentes\n\n", total, v.n);
    for (size_t i = 0; i < v.n && i < (size_t)quantidade; i++) {
        int espacos = 20 - largura_na_tela(v.itens[i].palavra);
        printf("%3zu. %s%*s %d\n", i + 1, v.itens[i].palavra, espacos > 0 ? espacos : 0, "",
               v.itens[i].vezes);
    }

    free(v.itens);
    hash_destruir(contagem);
    return 0;
}
