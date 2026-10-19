#include <stdio.h>

#include "calculadora.h"

static int falhas = 0;

#define CONFERIR(cond)                                                   \
    do {                                                                 \
        if (!(cond)) {                                                   \
            fprintf(stderr, "FALHOU %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            falhas++;                                                    \
        }                                                                \
    } while (0)

static void testar_operacoes(void)
{
    double r = 0;

    CONFERIR(calcular(2, '+', 3, &r) == CALC_OK && r == 5);
    CONFERIR(calcular(2, '-', 3, &r) == CALC_OK && r == -1);
    CONFERIR(calcular(4, '*', 2.5, &r) == CALC_OK && r == 10);
    CONFERIR(calcular(4, 'x', 2, &r) == CALC_OK && r == 8);
    CONFERIR(calcular(4, 'X', 2, &r) == CALC_OK && r == 8);
    CONFERIR(calcular(10, '/', 4, &r) == CALC_OK && r == 2.5);
}

static void testar_erros(void)
{
    double r = 0;

    CONFERIR(calcular(1, '/', 0, &r) == CALC_ERRO_DIVISAO_ZERO);
    CONFERIR(calcular(1, '/', -0.0, &r) == CALC_ERRO_DIVISAO_ZERO);
    CONFERIR(calcular(1, '%', 2, &r) == CALC_ERRO_OPERADOR_INVALIDO);
}

static void testar_leitura(void)
{
    double v = 0;

    CONFERIR(ler_numero("3.5", &v) && v == 3.5);
    CONFERIR(ler_numero("-12", &v) && v == -12);
    CONFERIR(ler_numero("1e3", &v) && v == 1000);
    CONFERIR(!ler_numero("", &v));
    CONFERIR(!ler_numero(NULL, &v));
    CONFERIR(!ler_numero("12abc", &v));
    CONFERIR(!ler_numero("1e999", &v)); /* overflow */
    CONFERIR(!ler_numero("nan", &v));
    CONFERIR(!ler_numero("inf", &v));
}

int main(void)
{
    testar_operacoes();
    testar_erros();
    testar_leitura();

    if (falhas) {
        printf("calculadora: %d teste(s) falharam\n", falhas);
        return 1;
    }
    printf("calculadora: todos os testes passaram\n");
    return 0;
}
