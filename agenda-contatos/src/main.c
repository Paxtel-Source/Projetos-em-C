#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "agenda.h"

/* Lê uma linha do teclado, removendo o '\n'. Retorna 0 em EOF. */
static int ler_linha(const char *rotulo, char *destino, size_t tamanho)
{
    printf("%s", rotulo);
    fflush(stdout);

    if (fgets(destino, (int)tamanho, stdin) == NULL) {
        return 0;
    }

    if (strchr(destino, '\n') == NULL) {
        /* Linha maior que o buffer: descarta o restante */
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    } else {
        destino[strcspn(destino, "\n")] = '\0';
    }
    return 1;
}

static void menu(void)
{
    printf("\n====== AGENDA DE CONTATOS ======\n");
    printf("1. Adicionar contato\n");
    printf("2. Listar contatos\n");
    printf("3. Buscar contato\n");
    printf("4. Remover contato\n");
    printf("0. Sair\n");
}

static void opcao_adicionar(Agenda *agenda)
{
    Contato c = {0}; /* Campos vazios caso a entrada termine no meio (EOF) */

    if (!ler_linha("Nome: ", c.nome, sizeof c.nome) || c.nome[0] == '\0') {
        printf("Nome nao pode ficar vazio.\n");
        return;
    }
    ler_linha("Telefone: ", c.telefone, sizeof c.telefone);
    ler_linha("E-mail: ", c.email, sizeof c.email);

    switch (agenda_adicionar(agenda, &c)) {
    case 0:
        printf("Contato adicionado!\n");
        break;
    case -1:
        printf("Agenda cheia (limite de %d contatos).\n", MAX_CONTATOS);
        break;
    case -2:
        printf("Ja existe um contato com esse nome.\n");
        break;
    case -3:
        printf("Nome nao pode ficar vazio.\n");
        break;
    case -4:
        printf("Os campos nao podem conter '%c'.\n", SEPARADOR);
        break;
    }
}

static void opcao_buscar(const Agenda *agenda)
{
    char termo[TAM_NOME];
    if (!ler_linha("Buscar por (nome, telefone ou e-mail): ", termo, sizeof termo)) {
        return;
    }
    if (agenda_buscar(agenda, termo) == 0) {
        printf("Nenhum contato encontrado.\n");
    }
}

static void opcao_remover(Agenda *agenda)
{
    char nome[TAM_NOME];
    if (!ler_linha("Nome do contato a remover: ", nome, sizeof nome)) {
        return;
    }
    if (agenda_remover(agenda, nome) == 0) {
        printf("Contato removido.\n");
    } else {
        printf("Contato nao encontrado.\n");
    }
}

/*
 * Guarda o arquivo corrompido como "<arquivo>.bak" para que ele não seja
 * sobrescrito ao sair. Retorna 1 se for seguro gravar no arquivo original.
 */
static int guardar_arquivo_corrompido(const char *arquivo)
{
    char backup[FILENAME_MAX];

    if (snprintf(backup, sizeof backup, "%s.bak", arquivo) < (int)sizeof backup) {
        remove(backup);
        if (rename(arquivo, backup) == 0) {
            fprintf(stderr, "Aviso: '%s' esta corrompido. Uma copia foi guardada em '%s' "
                            "e a agenda comeca vazia.\n", arquivo, backup);
            return 1;
        }
    }
    fprintf(stderr, "Aviso: '%s' esta corrompido e nao foi possivel copia-lo. "
                    "Nada sera gravado ao sair, para nao perder os dados.\n", arquivo);
    return 0;
}

int main(int argc, char *argv[])
{
    /* Permite escolher outro arquivo: ./bin/agenda meus_contatos.csv */
    const char *arquivo = (argc > 1) ? argv[1] : ARQUIVO_PADRAO;

    Agenda *agenda = malloc(sizeof *agenda);
    if (agenda == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        return EXIT_FAILURE;
    }

    int pode_salvar = 1;
    if (agenda_carregar(agenda, arquivo) != 0) {
        pode_salvar = guardar_arquivo_corrompido(arquivo);
    }

    char entrada[16];
    int rodando = 1;

    while (rodando) {
        menu();
        if (!ler_linha("Opcao: ", entrada, sizeof entrada)) {
            break;
        }

        switch (entrada[0]) {
        case '1':
            opcao_adicionar(agenda);
            break;
        case '2':
            agenda_listar(agenda);
            break;
        case '3':
            opcao_buscar(agenda);
            break;
        case '4':
            opcao_remover(agenda);
            break;
        case '0':
            rodando = 0;
            break;
        default:
            printf("Opcao invalida.\n");
        }
    }

    int status = EXIT_SUCCESS;
    if (!pode_salvar) {
        printf("Ate mais!\n");
    } else if (agenda_salvar(agenda, arquivo) != 0) {
        fprintf(stderr, "Erro ao salvar em '%s'.\n", arquivo);
        status = EXIT_FAILURE;
    } else {
        printf("Contatos salvos em '%s'. Ate mais!\n", arquivo);
    }

    free(agenda);
    return status;
}
