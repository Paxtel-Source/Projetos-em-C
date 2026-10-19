#include "recorde.h"

#include <stdio.h>
#include <stdlib.h>

#define NOME_ARQUIVO ".snake-recorde"

/* Monta o caminho $HOME/.snake-recorde (ou só o nome, se não houver HOME) */
static void caminho(char *destino, size_t tamanho)
{
    const char *home = getenv("HOME");
    if (home == NULL) home = getenv("USERPROFILE");   /* Windows */
    if (home != NULL) {
        snprintf(destino, tamanho, "%s/%s", home, NOME_ARQUIVO);
    } else {
        snprintf(destino, tamanho, "%s", NOME_ARQUIVO);
    }
}

int recorde_ler(void)
{
    char arquivo[1024];
    caminho(arquivo, sizeof arquivo);

    FILE *fp = fopen(arquivo, "r");
    if (fp == NULL) return 0;

    int pontos = 0;
    if (fscanf(fp, "%d", &pontos) != 1 || pontos < 0) pontos = 0;
    fclose(fp);
    return pontos;
}

void recorde_salvar(int pontos)
{
    char arquivo[1024];
    caminho(arquivo, sizeof arquivo);

    FILE *fp = fopen(arquivo, "w");
    if (fp == NULL) return;   /* sem permissão: o recorde só não fica salvo */
    fprintf(fp, "%d\n", pontos);
    fclose(fp);
}
