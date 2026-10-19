#include <stdio.h>
#include <string.h>

#include "calculadora.h"

#define TAM_LINHA 256

static void mostrar_uso(const char *programa)
{
    printf("Uso:\n");
    printf("  %s <numero> <operador> <numero>   (modo direto)\n", programa);
    printf("  %s                                (modo interativo)\n\n", programa);
    printf("Operadores: +  -  x  /   (use 'x' ou \"*\" com aspas para multiplicar)\n");
    printf("Exemplo:    %s 10 / 4\n", programa);
}

/* Calcula e imprime o resultado. Retorna 0 em caso de sucesso. */
static int processar(const char *txt_a, const char *txt_op, const char *txt_b)
{
    double a, b, resultado;

    if (!ler_numero(txt_a, &a) || !ler_numero(txt_b, &b)) {
        fprintf(stderr, "Erro: entrada numerica invalida.\n");
        return 1;
    }
    if (strlen(txt_op) != 1) {
        fprintf(stderr, "%s\n", calc_mensagem_erro(CALC_ERRO_OPERADOR_INVALIDO));
        return 1;
    }

    CalcStatus status = calcular(a, txt_op[0], b, &resultado);
    if (status != CALC_OK) {
        fprintf(stderr, "%s\n", calc_mensagem_erro(status));
        return 1;
    }

    printf("%g %c %g = %g\n", a, txt_op[0], b, resultado);
    return 0;
}

static void modo_interativo(void)
{
    char linha[TAM_LINHA];
    char a[TAM_LINHA], op[TAM_LINHA], b[TAM_LINHA];

    printf("=== Calculadora de Linha de Comando ===\n");
    printf("Digite uma expressao (ex: 8 * 3) ou 'sair' para encerrar.\n\n");

    for (;;) {
        printf("> ");
        if (fgets(linha, sizeof linha, stdin) == NULL) {
            break; /* EOF (Ctrl+D / Ctrl+Z) */
        }
        if (strchr(linha, '\n') == NULL && !feof(stdin)) {
            /* Linha maior que o buffer: descarta o restante para não virar outra expressão */
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            fprintf(stderr, "Linha muito longa (maximo %d caracteres).\n", TAM_LINHA - 2);
            continue;
        }
        linha[strcspn(linha, "\n")] = '\0';

        if (strcmp(linha, "sair") == 0) {
            break;
        }
        if (linha[0] == '\0') {
            continue;
        }

        char extra[2];
        if (sscanf(linha, "%255s %255s %255s %1s", a, op, b, extra) != 3) {
            fprintf(stderr, "Formato invalido. Use: <numero> <operador> <numero>\n");
            continue;
        }
        processar(a, op, b);
    }
    printf("Ate mais!\n");
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        modo_interativo();
        return 0;
    }
    if (argc == 4) {
        return processar(argv[1], argv[2], argv[3]);
    }

    mostrar_uso(argv[0]);
    return 1;
}
