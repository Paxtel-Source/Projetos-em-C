#include "parser.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Tipos de "pedaço" (token) que a linha pode ter */
typedef enum {
    T_PALAVRA,
    T_PIPE,          /* |  */
    T_ENTRADA,       /* <  */
    T_SAIDA,         /* >  */
    T_ANEXAR,        /* >> */
    T_FIM
} TipoToken;

typedef struct {
    const char *cursor;   /* posição atual na linha */
    char       *texto;    /* texto da última palavra lida (alocado) */
    bool        aspas;    /* a última palavra tinha trechos entre aspas */
    const char *erro;
} Leitor;

/*
 * Lê o próximo token. Palavras podem misturar trechos com e sem aspas,
 * como em: echo "olá mundo"'!'  ->  olá mundo!
 */
static TipoToken proximo_token(Leitor *l)
{
    while (isspace((unsigned char)*l->cursor)) l->cursor++;

    char c = *l->cursor;
    if (c == '\0') return T_FIM;
    if (c == '|') { l->cursor++; return T_PIPE; }
    if (c == '<') { l->cursor++; return T_ENTRADA; }
    if (c == '>') {
        l->cursor++;
        if (*l->cursor == '>') { l->cursor++; return T_ANEXAR; }
        return T_SAIDA;
    }

    /* Palavra: no máximo do tamanho do resto da linha */
    char *buffer = malloc(strlen(l->cursor) + 1);
    size_t n = 0;
    l->aspas = false;
    while (*l->cursor && !isspace((unsigned char)*l->cursor) && !strchr("|<>", *l->cursor)) {
        char aspa = *l->cursor;
        if (aspa == '"' || aspa == '\'') {
            l->aspas = true;
            l->cursor++;
            while (*l->cursor && *l->cursor != aspa) {
                buffer[n++] = *l->cursor++;
            }
            if (*l->cursor != aspa) {
                free(buffer);
                l->erro = "aspas sem fechar";
                return T_FIM;
            }
            l->cursor++;   /* pula a aspa de fechamento */
        } else {
            buffer[n++] = *l->cursor++;
        }
    }
    buffer[n] = '\0';
    l->texto = buffer;
    return T_PALAVRA;
}

void liberar_pipeline(Pipeline *pipeline)
{
    for (int i = 0; i < pipeline->quantidade; i++) {
        Comando *cmd = &pipeline->comandos[i];
        for (int j = 0; j < cmd->argc; j++) free(cmd->argv[j]);
        free(cmd->entrada);
        free(cmd->saida);
    }
    pipeline->quantidade = 0;
}

/* Em caso de erro, libera o que já foi montado e devolve a mensagem */
static const char *falhar(Pipeline *pipeline, const char *mensagem)
{
    pipeline->quantidade++;   /* inclui o comando que estava sendo montado */
    liberar_pipeline(pipeline);
    return mensagem;
}

const char *analisar_linha(const char *linha, Pipeline *pipeline)
{
    memset(pipeline, 0, sizeof *pipeline);
    Leitor leitor = {linha, NULL, false, NULL};
    Comando *atual = &pipeline->comandos[0];

    for (;;) {
        TipoToken tipo = proximo_token(&leitor);
        if (leitor.erro) return falhar(pipeline, leitor.erro);

        if (tipo == T_PALAVRA) {
            if (atual->argc == MAX_ARGUMENTOS) {
                free(leitor.texto);
                return falhar(pipeline, "argumentos demais");
            }
            atual->literal[atual->argc] = leitor.aspas;
            atual->argv[atual->argc++] = leitor.texto;

        } else if (tipo == T_ENTRADA || tipo == T_SAIDA || tipo == T_ANEXAR) {
            /* Depois de <, > ou >> precisa vir o nome do arquivo */
            if (proximo_token(&leitor) != T_PALAVRA) {
                return falhar(pipeline, leitor.erro ? leitor.erro : "falta o nome do arquivo depois de <, > ou >>");
            }
            char **destino = (tipo == T_ENTRADA) ? &atual->entrada : &atual->saida;
            free(*destino);
            *destino = leitor.texto;
            if (tipo != T_ENTRADA) atual->anexar = (tipo == T_ANEXAR);

        } else if (tipo == T_PIPE) {
            if (atual->argc == 0) return falhar(pipeline, "comando vazio antes de |");
            if (pipeline->quantidade + 1 == MAX_COMANDOS) return falhar(pipeline, "pipes demais");
            pipeline->quantidade++;
            atual = &pipeline->comandos[pipeline->quantidade];

        } else { /* T_FIM */
            if (atual->argc == 0) {
                bool linha_vazia = (pipeline->quantidade == 0 && !atual->entrada && !atual->saida);
                if (linha_vazia) return NULL;
                return falhar(pipeline, "comando vazio depois de |");
            }
            pipeline->quantidade++;
            return NULL;
        }
    }
}
