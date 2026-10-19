#include <stdio.h>

#include "teste.h"

int testes_total = 0;
int testes_falhos = 0;

int main(void)
{
    testar_lista();
    testar_pilha();
    testar_fila();
    testar_arvore();
    testar_hash();

    printf("\n%d verificações, %d falha(s)\n", testes_total, testes_falhos);
    return testes_falhos == 0 ? 0 : 1;
}
