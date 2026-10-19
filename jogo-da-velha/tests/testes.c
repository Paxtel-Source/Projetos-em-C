#include <stdio.h>

#include "tabuleiro.h"

static int falhas = 0;

#define CONFERIR(cond)                                                   \
    do {                                                                 \
        if (!(cond)) {                                                   \
            fprintf(stderr, "FALHOU %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            falhas++;                                                    \
        }                                                                \
    } while (0)

/* Joga as posições em sequência, alternando X e O, começando por X */
static EstadoJogo jogar(char tab[TAM][TAM], const int *posicoes, int n)
{
    char jogador = 'X';
    EstadoJogo estado = EM_ANDAMENTO;

    tabuleiro_iniciar(tab);
    for (int i = 0; i < n; i++) {
        tabuleiro_jogar(tab, posicoes[i], jogador);
        estado = tabuleiro_verificar(tab, jogador);
        jogador = (jogador == 'X') ? 'O' : 'X';
    }
    return estado;
}

static void testar_jogadas(void)
{
    char tab[TAM][TAM];

    tabuleiro_iniciar(tab);
    CONFERIR(tabuleiro_jogar(tab, 1, 'X'));
    CONFERIR(!tabuleiro_jogar(tab, 1, 'O')); /* casa ocupada */
    CONFERIR(!tabuleiro_jogar(tab, 0, 'O'));
    CONFERIR(!tabuleiro_jogar(tab, 10, 'O'));
    CONFERIR(tab[0][0] == 'X');
}

static void testar_resultados(void)
{
    char tab[TAM][TAM];

    const int linha[] = {1, 4, 2, 5, 3};
    const int coluna[] = {1, 2, 4, 5, 7};
    const int diagonal[] = {1, 2, 5, 3, 9};
    const int diagonal_sec[] = {3, 1, 5, 2, 7};
    const int andamento[] = {1, 5};
    /* X O X / X O O / O X X */
    const int velha[] = {1, 2, 3, 5, 4, 6, 8, 7, 9};

    CONFERIR(jogar(tab, linha, 5) == VITORIA);
    CONFERIR(jogar(tab, coluna, 5) == VITORIA);
    CONFERIR(jogar(tab, diagonal, 5) == VITORIA);
    CONFERIR(jogar(tab, diagonal_sec, 5) == VITORIA);
    CONFERIR(jogar(tab, andamento, 2) == EM_ANDAMENTO);
    CONFERIR(jogar(tab, velha, 9) == EMPATE);
}

int main(void)
{
    testar_jogadas();
    testar_resultados();

    if (falhas) {
        printf("jogo-da-velha: %d teste(s) falharam\n", falhas);
        return 1;
    }
    printf("jogo-da-velha: todos os testes passaram\n");
    return 0;
}
