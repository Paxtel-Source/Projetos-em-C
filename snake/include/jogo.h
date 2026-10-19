#ifndef JOGO_H
#define JOGO_H

#include <stdbool.h>

/*
 * Regras do jogo, sem nada de tela.
 * Separar a lógica da interface permite testar o jogo sem ncurses.
 */

#define LARGURA_MAX 80
#define ALTURA_MAX  40
#define MAX_CORPO   (LARGURA_MAX * ALTURA_MAX)

typedef struct {
    int x, y;
} Ponto;

typedef enum { CIMA, BAIXO, ESQUERDA, DIREITA } Direcao;

typedef enum { JOGANDO, PAUSADO, FIM_DE_JOGO } Estado;

/*
 * O corpo da cobra é uma fila circular de posições:
 * a cabeça entra no fim da fila e a cauda sai do começo.
 */
typedef struct {
    Ponto corpo[MAX_CORPO];
    int   inicio;          /* índice da cauda */
    int   tamanho;
} Cobra;

typedef struct {
    int     largura, altura;   /* área jogável, sem contar a borda */
    Cobra   cobra;
    Direcao direcao;
    Direcao proximas[2];       /* curvas digitadas esperando a vez */
    int     curvas_pendentes;
    Ponto   comida;
    int     pontos;
    int     comidas;
    Estado  estado;
    unsigned semente;          /* gerador de números aleatórios próprio */
} Jogo;

#define PONTOS_POR_COMIDA 10
#define TAMANHO_INICIAL   4

void  jogo_iniciar(Jogo *jogo, int largura, int altura, unsigned semente);

/* Registra uma curva; ignora meia-volta (ir para trás mataria a cobra) */
void  jogo_virar(Jogo *jogo, Direcao nova);

/* Avança um passo. Devolve true se a cobra comeu nesse passo. */
bool  jogo_passo(Jogo *jogo);

void  jogo_alternar_pausa(Jogo *jogo);

Ponto cobra_cabeca(const Cobra *cobra);
Ponto cobra_parte(const Cobra *cobra, int i);   /* 0 = cauda */
bool  cobra_ocupa(const Cobra *cobra, Ponto p);

/* Tempo entre passos em milissegundos: diminui conforme a cobra come */
int   jogo_intervalo_ms(const Jogo *jogo);

#endif /* JOGO_H */
