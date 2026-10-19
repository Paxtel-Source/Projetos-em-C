#define _POSIX_C_SOURCE 200809L

#include "historico.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *linhas[MAX_HISTORICO];
static int   total = 0;   /* quantos comandos já foram adicionados desde o início */

void historico_adicionar(const char *linha)
{
    /* Não repete o mesmo comando duas vezes seguidas */
    if (total > 0 && strcmp(linhas[(total - 1) % MAX_HISTORICO], linha) == 0) {
        return;
    }
    int pos = total % MAX_HISTORICO;
    free(linhas[pos]);   /* sobrescreve o mais antigo quando o vetor enche */
    linhas[pos] = strdup(linha);
    total++;
}

int historico_quantidade(void)
{
    return total < MAX_HISTORICO ? total : MAX_HISTORICO;
}

int historico_primeiro_numero(void)
{
    return total - historico_quantidade() + 1;
}

const char *historico_obter(int numero)
{
    if (numero < historico_primeiro_numero() || numero > total) return NULL;
    return linhas[(numero - 1) % MAX_HISTORICO];
}

void historico_listar(void)
{
    for (int n = historico_primeiro_numero(); n <= total; n++) {
        printf("%5d  %s\n", n, historico_obter(n));
    }
}

void historico_limpar(void)
{
    for (int i = 0; i < MAX_HISTORICO; i++) {
        free(linhas[i]);
        linhas[i] = NULL;
    }
    total = 0;
}
