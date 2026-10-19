#define _POSIX_C_SOURCE 200809L

/*
 * Snake no terminal com ncurses.
 * Este arquivo só cuida da tela e do teclado; as regras ficam em jogo.c.
 */
#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "jogo.h"
#include "recorde.h"

/* Cada casa do tabuleiro ocupa 2 colunas, para ficar quadrada na tela */
#define COLUNAS_POR_CASA 2
#define LARGURA_PADRAO   30
#define ALTURA_PADRAO    18
#define LARGURA_MINIMA   12
#define ALTURA_MINIMA    8

enum { COR_CORPO = 1, COR_CABECA, COR_COMIDA, COR_BORDA, COR_DESTAQUE };

/* Canto superior esquerdo do tabuleiro na tela */
static int origem_x, origem_y;

/* ---------- Tempo ---------- */

static long agora_ms(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000L + t.tv_nsec / 1000000L;
}

/* ---------- Desenho ---------- */

static void pintar_casa(Ponto p, int cor)
{
    attron(COLOR_PAIR(cor));
    mvaddstr(origem_y + 1 + p.y, origem_x + 1 + p.x * COLUNAS_POR_CASA, "  ");
    attroff(COLOR_PAIR(cor));
}

/* Quantas colunas o texto ocupa (em UTF-8, "ã" tem 2 bytes mas ocupa 1 coluna) */
static int largura_utf8(const char *texto)
{
    int largura = 0;
    for (const unsigned char *p = (const unsigned char *)texto; *p; p++) {
        if ((*p & 0xC0) != 0x80) largura++;
    }
    return largura;
}

static void texto_centralizado(int linha, const char *texto, int atributos)
{
    attron(atributos);
    mvaddstr(linha, (COLS - largura_utf8(texto)) / 2, texto);
    attroff(atributos);
}

static void desenhar(const Jogo *jogo, int recorde)
{
    erase();
    int largura_tela = jogo->largura * COLUNAS_POR_CASA + 2;

    /* Placar acima do tabuleiro */
    mvprintw(origem_y - 1, origem_x, "Pontos: %d", jogo->pontos);
    int maior = jogo->pontos > recorde ? jogo->pontos : recorde;
    mvprintw(origem_y - 1, origem_x + largura_tela - 14, "Recorde: %5d", maior);

    /* Borda */
    attron(COLOR_PAIR(COR_BORDA));
    for (int x = 0; x < largura_tela; x++) {
        mvaddch(origem_y, origem_x + x, ACS_HLINE);
        mvaddch(origem_y + jogo->altura + 1, origem_x + x, ACS_HLINE);
    }
    for (int y = 0; y < jogo->altura + 2; y++) {
        mvaddch(origem_y + y, origem_x, ACS_VLINE);
        mvaddch(origem_y + y, origem_x + largura_tela - 1, ACS_VLINE);
    }
    mvaddch(origem_y, origem_x, ACS_ULCORNER);
    mvaddch(origem_y, origem_x + largura_tela - 1, ACS_URCORNER);
    mvaddch(origem_y + jogo->altura + 1, origem_x, ACS_LLCORNER);
    mvaddch(origem_y + jogo->altura + 1, origem_x + largura_tela - 1, ACS_LRCORNER);
    attroff(COLOR_PAIR(COR_BORDA));

    /* Comida e cobra */
    pintar_casa(jogo->comida, COR_COMIDA);
    for (int i = 0; i < jogo->cobra.tamanho - 1; i++) {
        pintar_casa(cobra_parte(&jogo->cobra, i), COR_CORPO);
    }
    pintar_casa(cobra_cabeca(&jogo->cobra), COR_CABECA);

    /* Ajuda abaixo do tabuleiro */
    mvaddstr(origem_y + jogo->altura + 2, origem_x,
             "Setas ou WASD movem   P pausa   Q sai");

    if (jogo->estado == PAUSADO) {
        texto_centralizado(origem_y + jogo->altura / 2 + 1, " PAUSADO - aperte P para continuar ",
                           A_REVERSE | A_BOLD);
    }
    refresh();
}

/* ---------- Telas de início e fim ---------- */

static int tela_inicial(int recorde)
{
    erase();
    int meio = LINES / 2;
    texto_centralizado(meio - 4, "S N A K E", COLOR_PAIR(COR_DESTAQUE) | A_BOLD);
    texto_centralizado(meio - 2, "Coma os quadrados vermelhos para crescer.", 0);
    texto_centralizado(meio - 1, "Não bata na parede nem no próprio corpo.", 0);
    char linha[64];
    snprintf(linha, sizeof linha, "Recorde: %d pontos", recorde);
    texto_centralizado(meio + 1, linha, A_BOLD);
    texto_centralizado(meio + 3, "Aperte qualquer tecla para começar (Q sai)", A_DIM);
    refresh();

    timeout(-1);
    return getch();
}

/* Devolve true se o jogador quer jogar de novo */
static bool tela_final(const Jogo *jogo, int recorde_antigo)
{
    int meio = origem_y + jogo->altura / 2 + 1;
    char linha[64];

    texto_centralizado(meio - 2, "     FIM DE JOGO     ", A_REVERSE | A_BOLD);
    snprintf(linha, sizeof linha, "  %d pontos, cobra com %d partes  ", jogo->pontos, jogo->cobra.tamanho);
    texto_centralizado(meio, linha, A_BOLD);
    if (jogo->pontos > recorde_antigo) {
        texto_centralizado(meio + 1, "  Novo recorde!  ", COLOR_PAIR(COR_DESTAQUE) | A_BOLD);
    }
    texto_centralizado(meio + 3, "  R joga de novo   Q sai  ", 0);
    refresh();

    timeout(-1);
    for (;;) {
        int tecla = getch();
        if (tecla == 'r' || tecla == 'R') return true;
        if (tecla == 'q' || tecla == 'Q') return false;
    }
}

/* ---------- Teclado ---------- */

/* Devolve false se o jogador apertou Q */
static bool tratar_tecla(Jogo *jogo, int tecla)
{
    switch (tecla) {
    case KEY_UP:    case 'w': case 'W': jogo_virar(jogo, CIMA);     break;
    case KEY_DOWN:  case 's': case 'S': jogo_virar(jogo, BAIXO);    break;
    case KEY_LEFT:  case 'a': case 'A': jogo_virar(jogo, ESQUERDA); break;
    case KEY_RIGHT: case 'd': case 'D': jogo_virar(jogo, DIREITA);  break;
    case 'p': case 'P': case ' ':       jogo_alternar_pausa(jogo);  break;
    case 'q': case 'Q':                 return false;
    }
    return true;
}

/* ---------- Partida ---------- */

/* Joga uma partida. Devolve false se o jogador quis sair no meio. */
static bool jogar(Jogo *jogo, int recorde)
{
    long proximo_passo = agora_ms() + jogo_intervalo_ms(jogo);
    desenhar(jogo, recorde);

    while (jogo->estado != FIM_DE_JOGO) {
        /* Espera uma tecla só até a hora do próximo passo */
        long restante = proximo_passo - agora_ms();
        timeout(jogo->estado == PAUSADO ? -1 : (restante > 0 ? (int)restante : 0));

        int tecla = getch();
        if (tecla != ERR) {
            if (!tratar_tecla(jogo, tecla)) return false;
            if (jogo->estado == PAUSADO) {
                desenhar(jogo, recorde);
                continue;
            }
        }

        if (jogo->estado == JOGANDO && agora_ms() >= proximo_passo) {
            jogo_passo(jogo);
            proximo_passo = agora_ms() + jogo_intervalo_ms(jogo);
            desenhar(jogo, recorde);
        } else if (tecla != ERR) {
            desenhar(jogo, recorde);   /* saiu da pausa */
            proximo_passo = agora_ms() + jogo_intervalo_ms(jogo);
        }
    }
    return true;
}

/* ---------- Programa ---------- */

static void iniciar_ncurses(void)
{
    setlocale(LC_ALL, "");   /* necessário para mostrar acentos */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(COR_CORPO, COLOR_BLACK, COLOR_GREEN);
        init_pair(COR_CABECA, COLOR_BLACK, COLOR_YELLOW);
        init_pair(COR_COMIDA, COLOR_BLACK, COLOR_RED);
        init_pair(COR_BORDA, COLOR_GREEN, -1);
        init_pair(COR_DESTAQUE, COLOR_GREEN, -1);
    }
}

int main(void)
{
    iniciar_ncurses();

    /* Ajusta o tabuleiro ao tamanho do terminal */
    int largura = (COLS - 2) / COLUNAS_POR_CASA;
    int altura = LINES - 4;
    if (largura > LARGURA_PADRAO) largura = LARGURA_PADRAO;
    if (altura > ALTURA_PADRAO) altura = ALTURA_PADRAO;

    if (largura < LARGURA_MINIMA || altura < ALTURA_MINIMA) {
        endwin();
        fprintf(stderr, "O terminal é pequeno demais. Aumente a janela (mínimo de %dx%d caracteres).\n",
                LARGURA_MINIMA * COLUNAS_POR_CASA + 2, ALTURA_MINIMA + 4);
        return 1;
    }
    origem_x = (COLS - (largura * COLUNAS_POR_CASA + 2)) / 2;
    origem_y = (LINES - (altura + 2)) / 2 + 1;

    int recorde = recorde_ler();
    int tecla = tela_inicial(recorde);

    bool continuar = !(tecla == 'q' || tecla == 'Q');
    while (continuar) {
        Jogo jogo;
        jogo_iniciar(&jogo, largura, altura, (unsigned)time(NULL));

        if (!jogar(&jogo, recorde)) break;

        int recorde_antigo = recorde;
        if (jogo.pontos > recorde) {
            recorde = jogo.pontos;
            recorde_salvar(recorde);
        }
        continuar = tela_final(&jogo, recorde_antigo);
    }

    endwin();
    printf("Até a próxima! Recorde: %d pontos.\n", recorde);
    return 0;
}
