#ifndef TESTE_H
#define TESTE_H

#include <stdio.h>

/*
 * Mini framework de testes, sem bibliotecas externas.
 * VERIFICAR(condicao) conta um acerto ou mostra onde falhou.
 */

extern int testes_total;
extern int testes_falhos;

#define VERIFICAR(condicao)                                                   \
    do {                                                                      \
        testes_total++;                                                       \
        if (!(condicao)) {                                                    \
            testes_falhos++;                                                  \
            fprintf(stderr, "  FALHOU %s:%d  %s\n", __FILE__, __LINE__, #condicao); \
        }                                                                     \
    } while (0)

#define GRUPO(nome) printf("[%s]\n", nome)

void testar_lista(void);
void testar_pilha(void);
void testar_fila(void);
void testar_arvore(void);
void testar_hash(void);

#endif /* TESTE_H */
