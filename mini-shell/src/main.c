#define _POSIX_C_SOURCE 200809L

#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "executor.h"
#include "historico.h"
#include "parser.h"

/* Cores ANSI do prompt (só usadas quando a saída é um terminal) */
#define VERDE    "\033[1;32m"
#define AZUL     "\033[1;34m"
#define VERMELHO "\033[1;31m"
#define NORMAL   "\033[0m"

static int interativo;

/* Mostra a pasta atual, trocando o começo por ~ quando estiver dentro da HOME */
static void mostrar_prompt(void)
{
    char pasta[PATH_MAX];
    if (getcwd(pasta, sizeof pasta) == NULL) strcpy(pasta, "?");

    const char *home = getenv("HOME");
    const char *exibir = pasta;
    char abreviada[PATH_MAX + 2];
    size_t n = home ? strlen(home) : 0;
    if (n > 0 && strncmp(pasta, home, n) == 0 && (pasta[n] == '/' || pasta[n] == '\0')) {
        snprintf(abreviada, sizeof abreviada, "~%s", pasta + n);
        exibir = abreviada;
    }

    if (ultimo_status != 0) {
        printf(VERMELHO "[%d] " NORMAL, ultimo_status);
    }
    printf(VERDE "minishell" NORMAL ":" AZUL "%s" NORMAL "$ ", exibir);
    fflush(stdout);
}

/*
 * Troca "!!" pelo último comando e "!N" pelo comando N do histórico.
 * Devolve uma nova string (alocada) ou NULL se a referência não existir.
 */
static char *expandir_historico(const char *linha)
{
    const char *anterior = NULL;
    if (strcmp(linha, "!!") == 0) {
        anterior = historico_obter(historico_primeiro_numero() + historico_quantidade() - 1);
    } else if (linha[0] == '!' && linha[1] != '\0') {
        char *fim;
        long numero = strtol(linha + 1, &fim, 10);
        if (*fim == '\0') anterior = historico_obter((int)numero);
    } else {
        return strdup(linha);
    }

    if (anterior == NULL) {
        fprintf(stderr, "%s: não existe no histórico\n", linha);
        return NULL;
    }
    if (interativo) printf("%s\n", anterior);   /* mostra o que vai rodar */
    return strdup(anterior);
}

/* Remove espaços do começo e do fim (sem alocar) */
static char *aparar(char *texto)
{
    while (*texto == ' ' || *texto == '\t') texto++;
    char *fim = texto + strlen(texto);
    while (fim > texto && (fim[-1] == ' ' || fim[-1] == '\t' || fim[-1] == '\n' || fim[-1] == '\r')) {
        *--fim = '\0';
    }
    return texto;
}

int main(void)
{
    /* Sem terminal (ex.: comandos vindos de um arquivo), não mostra prompt */
    interativo = isatty(STDIN_FILENO);

    /* Ctrl+C interrompe o comando em execução, não o shell */
    signal(SIGINT, SIG_IGN);

    if (interativo) {
        puts("Mini Shell em C. Digite \"ajuda\" para ver os comandos ou \"sair\" para encerrar.");
    }

    char *linha = NULL;
    size_t capacidade = 0;

    while (!deve_sair) {
        if (interativo) mostrar_prompt();

        if (getline(&linha, &capacidade, stdin) == -1) {
            if (interativo) putchar('\n');   /* Ctrl+D */
            break;
        }

        char *texto = aparar(linha);
        if (*texto == '\0' || *texto == '#') continue;   /* vazio ou comentário */

        char *comando = expandir_historico(texto);
        if (comando == NULL) {
            ultimo_status = 1;
            continue;
        }
        historico_adicionar(comando);

        Pipeline pipeline;
        const char *erro = analisar_linha(comando, &pipeline);
        if (erro) {
            fprintf(stderr, "minishell: erro de sintaxe: %s\n", erro);
            ultimo_status = 2;
        } else {
            executar_pipeline(&pipeline);
            liberar_pipeline(&pipeline);
        }
        free(comando);
    }

    free(linha);
    historico_limpar();
    return ultimo_status;
}
