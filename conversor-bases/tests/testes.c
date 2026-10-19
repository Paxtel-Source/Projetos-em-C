#include <stdio.h>
#include <string.h>

#include "conversor.h"

static int falhas = 0;

#define CONFERIR(cond)                                                   \
    do {                                                                 \
        if (!(cond)) {                                                   \
            fprintf(stderr, "FALHOU %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            falhas++;                                                    \
        }                                                                \
    } while (0)

static void testar_leitura(void)
{
    uint64_t v = 0;

    CONFERIR(texto_para_numero("255", 10, &v) && v == 255);
    CONFERIR(texto_para_numero("FF", 16, &v) && v == 255);
    CONFERIR(texto_para_numero("0xff", 16, &v) && v == 255);
    CONFERIR(texto_para_numero("0b1010", 2, &v) && v == 10);
    CONFERIR(texto_para_numero("0o17", 8, &v) && v == 15);
    CONFERIR(texto_para_numero("18446744073709551615", 10, &v) && v == UINT64_MAX);

    CONFERIR(!texto_para_numero("18446744073709551616", 10, &v)); /* overflow */
    CONFERIR(!texto_para_numero("2", 2, &v));
    CONFERIR(!texto_para_numero("8", 8, &v));
    CONFERIR(!texto_para_numero("G", 16, &v));
    CONFERIR(!texto_para_numero("", 10, &v));
    CONFERIR(!texto_para_numero("0x", 16, &v));
    CONFERIR(!texto_para_numero("10", 7, &v));
}

static void testar_conversoes(void)
{
    char s[TAM_BUFFER];

    para_binario(10, s, sizeof s);
    CONFERIR(strcmp(s, "1010") == 0);
    para_binario(0, s, sizeof s);
    CONFERIR(strcmp(s, "0") == 0);
    para_octal(8, s, sizeof s);
    CONFERIR(strcmp(s, "10") == 0);
    para_hexadecimal(255, s, sizeof s);
    CONFERIR(strcmp(s, "FF") == 0);
    para_decimal(UINT64_MAX, s, sizeof s);
    CONFERIR(strcmp(s, "18446744073709551615") == 0);

    /* 64 dígitos binários precisam caber no buffer */
    para_binario(UINT64_MAX, s, sizeof s);
    CONFERIR(strlen(s) == 64);
}

int main(void)
{
    testar_leitura();
    testar_conversoes();

    if (falhas) {
        printf("conversor-bases: %d teste(s) falharam\n", falhas);
        return 1;
    }
    printf("conversor-bases: todos os testes passaram\n");
    return 0;
}
