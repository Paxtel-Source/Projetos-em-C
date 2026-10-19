#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tabuleiro.h"

/* Lê uma linha sem o '\n'. Se ela não couber no buffer, descarta o resto. Retorna 0 em EOF. */
static int ler_linha(char *destino, int tamanho)
{
    if (fgets(destino, tamanho, stdin) == NULL) {
        return 0;
    }
    if (strchr(destino, '\n') == NULL) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }
    destino[strcspn(destino, "\n")] = '\0';
    return 1;
}

/* Lê a posição digitada (1-9). Retorna -1 em EOF e 0 se a entrada for inválida. */
static int ler_posicao(char jogador)
{
    char linha[32];
    char *fim;

    printf("Jogador %c, escolha uma posicao (1-9): ", jogador);
    fflush(stdout);

    if (!ler_linha(linha, sizeof linha)) {
        return -1;
    }

    long valor = strtol(linha, &fim, 10);
    /* Confere a faixa antes de converter para int, para "4294967297" não virar 1 */
    if (fim == linha || *fim != '\0' || valor < 1 || valor > TAM * TAM) {
        return 0;
    }
    return (int)valor;
}

/* Joga uma partida completa. Retorna o vencedor ('X'/'O'), 'E' para empate ou 0 em EOF. */
static char jogar_partida(void)
{
    char tab[TAM][TAM];
    char jogador = 'X';

    tabuleiro_iniciar(tab);

    for (;;) {
        tabuleiro_exibir(tab);

        int pos = ler_posicao(jogador);
        if (pos == -1) {
            return 0;
        }
        if (!tabuleiro_jogar(tab, pos, jogador)) {
            printf("Jogada invalida! Escolha uma casa livre entre 1 e 9.\n");
            continue;
        }

        EstadoJogo estado = tabuleiro_verificar(tab, jogador);
        if (estado == VITORIA) {
            tabuleiro_exibir(tab);
            printf("*** Jogador %c venceu! ***\n", jogador);
            return jogador;
        }
        if (estado == EMPATE) {
            tabuleiro_exibir(tab);
            printf("*** Deu velha! Empate. ***\n");
            return 'E';
        }

        jogador = (jogador == 'X') ? 'O' : 'X';
    }
}

int main(void)
{
    int vitorias_x = 0, vitorias_o = 0, empates = 0;
    char resposta[16];

    printf("=========== JOGO DA VELHA ===========\n");
    printf("Dois jogadores no mesmo terminal: X comeca.\n");

    do {
        char resultado = jogar_partida();
        if (resultado == 0) {
            break;
        }
        if (resultado == 'X') vitorias_x++;
        else if (resultado == 'O') vitorias_o++;
        else empates++;

        printf("\nPlacar -> X: %d | O: %d | Empates: %d\n", vitorias_x, vitorias_o, empates);
        printf("Jogar novamente? (s/n): ");
        fflush(stdout);
    } while (ler_linha(resposta, sizeof resposta) &&
             (resposta[0] == 's' || resposta[0] == 'S'));

    printf("Obrigado por jogar!\n");
    return EXIT_SUCCESS;
}
