#include "jogo.h"

/* ---------- Números aleatórios ---------- */

/* Gerador próprio (LCG): mesma semente = mesmo jogo, útil nos testes */
static int sortear(Jogo *jogo, int limite)
{
    jogo->semente = jogo->semente * 1103515245u + 12345u;
    return (int)((jogo->semente >> 16) % (unsigned)limite);
}

/* ---------- Cobra (fila circular) ---------- */

Ponto cobra_parte(const Cobra *cobra, int i)
{
    return cobra->corpo[(cobra->inicio + i) % MAX_CORPO];
}

Ponto cobra_cabeca(const Cobra *cobra)
{
    return cobra_parte(cobra, cobra->tamanho - 1);
}

bool cobra_ocupa(const Cobra *cobra, Ponto p)
{
    for (int i = 0; i < cobra->tamanho; i++) {
        Ponto parte = cobra_parte(cobra, i);
        if (parte.x == p.x && parte.y == p.y) return true;
    }
    return false;
}

static void cobra_crescer_para(Cobra *cobra, Ponto nova_cabeca)
{
    cobra->corpo[(cobra->inicio + cobra->tamanho) % MAX_CORPO] = nova_cabeca;
    cobra->tamanho++;
}

static void cobra_perder_cauda(Cobra *cobra)
{
    cobra->inicio = (cobra->inicio + 1) % MAX_CORPO;
    cobra->tamanho--;
}

/* ---------- Comida ---------- */

static void sortear_comida(Jogo *jogo)
{
    int livres = jogo->largura * jogo->altura - jogo->cobra.tamanho;
    if (livres <= 0) {
        jogo->estado = FIM_DE_JOGO;   /* a cobra encheu o tabuleiro: vitória total */
        return;
    }
    /* Sorteia entre as casas livres, então nunca cai em cima da cobra */
    int alvo = sortear(jogo, livres);
    for (int y = 0; y < jogo->altura; y++) {
        for (int x = 0; x < jogo->largura; x++) {
            Ponto p = {x, y};
            if (cobra_ocupa(&jogo->cobra, p)) continue;
            if (alvo-- == 0) {
                jogo->comida = p;
                return;
            }
        }
    }
}

/* ---------- Jogo ---------- */

void jogo_iniciar(Jogo *jogo, int largura, int altura, unsigned semente)
{
    if (largura > LARGURA_MAX) largura = LARGURA_MAX;
    if (altura > ALTURA_MAX) altura = ALTURA_MAX;

    jogo->largura = largura;
    jogo->altura = altura;
    jogo->direcao = DIREITA;
    jogo->curvas_pendentes = 0;
    jogo->pontos = 0;
    jogo->comidas = 0;
    jogo->estado = JOGANDO;
    jogo->semente = semente;

    /* Cobra deitada no meio do tabuleiro, olhando para a direita */
    jogo->cobra.inicio = 0;
    jogo->cobra.tamanho = 0;
    int meio_y = altura / 2;
    int x0 = largura / 2 - TAMANHO_INICIAL;
    for (int i = 0; i < TAMANHO_INICIAL; i++) {
        cobra_crescer_para(&jogo->cobra, (Ponto){x0 + i, meio_y});
    }
    sortear_comida(jogo);
}

static bool opostas(Direcao a, Direcao b)
{
    return (a == CIMA && b == BAIXO) || (a == BAIXO && b == CIMA) ||
           (a == ESQUERDA && b == DIREITA) || (a == DIREITA && b == ESQUERDA);
}

void jogo_virar(Jogo *jogo, Direcao nova)
{
    if (jogo->estado != JOGANDO || jogo->curvas_pendentes == 2) return;

    /* Compara com a última curva pedida, não só com a direção atual.
     * Assim duas teclas rápidas (cima + esquerda) funcionam, mas uma
     * meia-volta escondida (cima + baixo) é recusada. */
    Direcao referencia = jogo->curvas_pendentes
        ? jogo->proximas[jogo->curvas_pendentes - 1]
        : jogo->direcao;

    if (nova == referencia || opostas(nova, referencia)) return;
    jogo->proximas[jogo->curvas_pendentes++] = nova;
}

bool jogo_passo(Jogo *jogo)
{
    if (jogo->estado != JOGANDO) return false;

    /* Aplica uma curva por passo */
    if (jogo->curvas_pendentes > 0) {
        jogo->direcao = jogo->proximas[0];
        jogo->proximas[0] = jogo->proximas[1];
        jogo->curvas_pendentes--;
    }

    Ponto cabeca = cobra_cabeca(&jogo->cobra);
    switch (jogo->direcao) {
    case CIMA:     cabeca.y--; break;
    case BAIXO:    cabeca.y++; break;
    case ESQUERDA: cabeca.x--; break;
    case DIREITA:  cabeca.x++; break;
    }

    /* Bateu na parede */
    if (cabeca.x < 0 || cabeca.y < 0 || cabeca.x >= jogo->largura || cabeca.y >= jogo->altura) {
        jogo->estado = FIM_DE_JOGO;
        return false;
    }

    bool comeu = (cabeca.x == jogo->comida.x && cabeca.y == jogo->comida.y);

    /* A cauda sai antes de checar a colisão: a cabeça pode ocupar
     * a casa que a cauda acabou de deixar */
    if (!comeu) cobra_perder_cauda(&jogo->cobra);

    if (cobra_ocupa(&jogo->cobra, cabeca)) {
        jogo->estado = FIM_DE_JOGO;
        return false;
    }
    cobra_crescer_para(&jogo->cobra, cabeca);

    if (comeu) {
        jogo->comidas++;
        jogo->pontos += PONTOS_POR_COMIDA;
        sortear_comida(jogo);
    }
    return comeu;
}

void jogo_alternar_pausa(Jogo *jogo)
{
    if (jogo->estado == JOGANDO)      jogo->estado = PAUSADO;
    else if (jogo->estado == PAUSADO) jogo->estado = JOGANDO;
}

int jogo_intervalo_ms(const Jogo *jogo)
{
    /* Começa em 140 ms e acelera 6 ms a cada 3 comidas, até 60 ms */
    int intervalo = 140 - (jogo->comidas / 3) * 6;
    return intervalo < 60 ? 60 : intervalo;
}
