/*
 * Testes das regras do Snake (jogo.c), sem abrir a tela do ncurses.
 */
#include <stdio.h>

#include "jogo.h"

static int total = 0, falhas = 0;

#define VERIFICAR(c)                                                              \
    do {                                                                          \
        total++;                                                                  \
        if (!(c)) {                                                               \
            falhas++;                                                             \
            fprintf(stderr, "  FALHOU %s:%d  %s\n", __FILE__, __LINE__, #c);      \
        }                                                                         \
    } while (0)

/* Coloca a comida em um lugar escolhido, para controlar o teste */
static void por_comida(Jogo *j, int x, int y)
{
    j->comida = (Ponto){x, y};
}

static void teste_inicio(void)
{
    puts("[início]");
    Jogo j;
    jogo_iniciar(&j, 20, 10, 42);
    VERIFICAR(j.cobra.tamanho == TAMANHO_INICIAL);
    VERIFICAR(j.estado == JOGANDO);
    VERIFICAR(j.direcao == DIREITA);
    VERIFICAR(!cobra_ocupa(&j.cobra, j.comida));
    VERIFICAR(j.comida.x >= 0 && j.comida.x < 20 && j.comida.y >= 0 && j.comida.y < 10);
}

static void teste_movimento(void)
{
    puts("[movimento]");
    Jogo j;
    jogo_iniciar(&j, 20, 10, 1);
    por_comida(&j, 0, 0);
    Ponto antes = cobra_cabeca(&j.cobra);
    jogo_passo(&j);
    Ponto depois = cobra_cabeca(&j.cobra);
    VERIFICAR(depois.x == antes.x + 1 && depois.y == antes.y);
    VERIFICAR(j.cobra.tamanho == TAMANHO_INICIAL);   /* não cresceu */

    jogo_virar(&j, BAIXO);
    jogo_passo(&j);
    VERIFICAR(cobra_cabeca(&j.cobra).y == depois.y + 1);
}

static void teste_meia_volta(void)
{
    puts("[meia-volta]");
    Jogo j;
    jogo_iniciar(&j, 20, 10, 1);
    por_comida(&j, 0, 0);

    jogo_virar(&j, ESQUERDA);              /* oposta à direita: ignorada */
    VERIFICAR(j.curvas_pendentes == 0);

    /* Meia-volta escondida: cima e baixo antes do próximo passo */
    jogo_virar(&j, CIMA);
    jogo_virar(&j, BAIXO);
    VERIFICAR(j.curvas_pendentes == 1);

    /* Duas curvas válidas rápidas são guardadas e aplicadas uma por passo */
    jogo_iniciar(&j, 20, 10, 1);
    por_comida(&j, 0, 0);
    jogo_virar(&j, CIMA);
    jogo_virar(&j, ESQUERDA);
    VERIFICAR(j.curvas_pendentes == 2);
    jogo_passo(&j);
    VERIFICAR(j.direcao == CIMA);
    jogo_passo(&j);
    VERIFICAR(j.direcao == ESQUERDA);
    VERIFICAR(j.estado == JOGANDO);
}

static void teste_comer(void)
{
    puts("[comer]");
    Jogo j;
    jogo_iniciar(&j, 20, 10, 7);
    Ponto cabeca = cobra_cabeca(&j.cobra);
    por_comida(&j, cabeca.x + 1, cabeca.y);

    VERIFICAR(jogo_passo(&j));
    VERIFICAR(j.cobra.tamanho == TAMANHO_INICIAL + 1);
    VERIFICAR(j.pontos == PONTOS_POR_COMIDA);
    VERIFICAR(!cobra_ocupa(&j.cobra, j.comida));     /* nova comida em casa livre */
}

static void teste_parede(void)
{
    puts("[parede]");
    Jogo j;
    jogo_iniciar(&j, 12, 8, 3);
    por_comida(&j, 0, 0);
    for (int i = 0; i < 20 && j.estado == JOGANDO; i++) jogo_passo(&j);
    VERIFICAR(j.estado == FIM_DE_JOGO);
    VERIFICAR(cobra_cabeca(&j.cobra).x == 11);       /* parou na última casa */
}

static void teste_corpo(void)
{
    puts("[próprio corpo]");
    Jogo j;
    jogo_iniciar(&j, 20, 10, 5);
    /* Cobra de 5 partes dando uma volta fechada bate no próprio corpo */
    Ponto c = cobra_cabeca(&j.cobra);
    por_comida(&j, c.x + 1, c.y);
    jogo_passo(&j);                         /* come e fica com 5 partes */
    por_comida(&j, 0, 0);
    jogo_virar(&j, BAIXO);   jogo_passo(&j);
    jogo_virar(&j, ESQUERDA); jogo_passo(&j);
    jogo_virar(&j, CIMA);    jogo_passo(&j);
    VERIFICAR(j.estado == FIM_DE_JOGO);
}

static void teste_seguir_cauda(void)
{
    puts("[seguir a cauda]");
    /* Com 4 partes, dar a volta em um quadrado 2x2 é permitido:
     * a cabeça entra na casa que a cauda acabou de deixar */
    Jogo j;
    jogo_iniciar(&j, 20, 10, 5);
    por_comida(&j, 0, 0);
    jogo_virar(&j, BAIXO);    jogo_passo(&j);
    jogo_virar(&j, ESQUERDA); jogo_passo(&j);
    jogo_virar(&j, CIMA);     jogo_passo(&j);
    VERIFICAR(j.estado == JOGANDO);
}

static void teste_pausa_e_velocidade(void)
{
    puts("[pausa e velocidade]");
    Jogo j;
    jogo_iniciar(&j, 20, 10, 9);
    por_comida(&j, 0, 0);
    jogo_alternar_pausa(&j);
    Ponto antes = cobra_cabeca(&j.cobra);
    jogo_passo(&j);
    VERIFICAR(cobra_cabeca(&j.cobra).x == antes.x);  /* pausado não anda */
    jogo_alternar_pausa(&j);
    VERIFICAR(j.estado == JOGANDO);

    int inicial = jogo_intervalo_ms(&j);
    j.comidas = 30;
    VERIFICAR(jogo_intervalo_ms(&j) < inicial);
    j.comidas = 1000;
    VERIFICAR(jogo_intervalo_ms(&j) == 60);          /* tem velocidade máxima */
}

int main(void)
{
    teste_inicio();
    teste_movimento();
    teste_meia_volta();
    teste_comer();
    teste_parede();
    teste_corpo();
    teste_seguir_cauda();
    teste_pausa_e_velocidade();

    printf("\n%d verificações, %d falha(s)\n", total, falhas);
    return falhas == 0 ? 0 : 1;
}
