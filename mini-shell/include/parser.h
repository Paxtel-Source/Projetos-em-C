#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#define MAX_ARGUMENTOS 64
#define MAX_COMANDOS   16

/* Um comando simples, como: grep -i erro < log.txt > saida.txt */
typedef struct {
    char *argv[MAX_ARGUMENTOS + 1];  /* termina com NULL, como o execvp exige */
    int   argc;
    bool  literal[MAX_ARGUMENTOS];   /* true se o argumento tinha aspas (não expande *) */
    char *entrada;                   /* arquivo de "<"  (ou NULL) */
    char *saida;                     /* arquivo de ">" ou ">>" (ou NULL) */
    bool  anexar;                    /* true para ">>" */
} Comando;

/* Comandos ligados por pipe: ls | grep .c | wc -l */
typedef struct {
    Comando comandos[MAX_COMANDOS];
    int     quantidade;
} Pipeline;

/*
 * Transforma a linha digitada em um Pipeline.
 * Entende aspas simples e duplas, "|", "<", ">" e ">>".
 * Retorna NULL em caso de sucesso, ou uma mensagem de erro de sintaxe.
 * Uma linha vazia resulta em quantidade == 0.
 */
const char *analisar_linha(const char *linha, Pipeline *pipeline);

/* Libera os textos alocados por analisar_linha */
void liberar_pipeline(Pipeline *pipeline);

#endif /* PARSER_H */
