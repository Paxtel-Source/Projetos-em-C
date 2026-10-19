#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conversor.h"

static void mostrar_uso(const char *programa)
{
    printf("Uso:\n");
    printf("  %s <numero> <base>   (modo direto)\n", programa);
    printf("  %s                   (modo interativo)\n\n", programa);
    printf("Bases aceitas: 2 (binario), 8 (octal), 10 (decimal), 16 (hexadecimal)\n");
    printf("Exemplos:  %s 255 10    |    %s FF 16    |    %s 0b1010 2\n",
           programa, programa, programa);
}

/* Converte e exibe o número em todas as bases. Retorna 0 em caso de sucesso. */
static int converter_e_exibir(const char *texto, const char *texto_base)
{
    char *fim;
    long base = strtol(texto_base, &fim, 10);
    uint64_t valor;

    if (*fim != '\0' || (base != 2 && base != 8 && base != 10 && base != 16)) {
        fprintf(stderr, "Erro: base invalida. Use 2, 8, 10 ou 16.\n");
        return 1;
    }
    if (!texto_para_numero(texto, (int)base, &valor)) {
        fprintf(stderr, "Erro: '%s' nao e um numero valido na base %ld (ou excede 64 bits).\n",
                texto, base);
        return 1;
    }

    char bin[TAM_BUFFER], oct[TAM_BUFFER], dec[TAM_BUFFER], hex[TAM_BUFFER];
    para_binario(valor, bin, sizeof bin);
    para_octal(valor, oct, sizeof oct);
    para_decimal(valor, dec, sizeof dec);
    para_hexadecimal(valor, hex, sizeof hex);

    printf("\n  Decimal     : %s\n", dec);
    printf("  Binario     : %s\n", bin);
    printf("  Octal       : %s\n", oct);
    printf("  Hexadecimal : %s\n\n", hex);
    return 0;
}

static void modo_interativo(void)
{
    char linha[128], numero[96], base[16], extra[2];

    printf("====== CONVERSOR DE BASES NUMERICAS ======\n");
    printf("Digite: <numero> <base>   (ex: FF 16)  ou 'sair'\n\n");

    for (;;) {
        printf("> ");
        fflush(stdout);
        if (fgets(linha, sizeof linha, stdin) == NULL) {
            break;
        }
        if (strchr(linha, '\n') == NULL && !feof(stdin)) {
            /* Linha maior que o buffer: descarta o restante para não virar outra entrada */
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            fprintf(stderr, "Linha muito longa (maximo %d caracteres).\n", (int)sizeof linha - 2);
            continue;
        }
        linha[strcspn(linha, "\n")] = '\0';

        if (strcmp(linha, "sair") == 0) {
            break;
        }
        if (linha[0] == '\0') {
            continue;
        }
        if (sscanf(linha, "%95s %15s %1s", numero, base, extra) != 2) {
            fprintf(stderr, "Formato invalido. Use: <numero> <base>\n");
            continue;
        }
        converter_e_exibir(numero, base);
    }
    printf("Ate mais!\n");
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        modo_interativo();
        return EXIT_SUCCESS;
    }
    if (argc == 3) {
        return converter_e_exibir(argv[1], argv[2]);
    }

    mostrar_uso(argv[0]);
    return EXIT_FAILURE;
}
