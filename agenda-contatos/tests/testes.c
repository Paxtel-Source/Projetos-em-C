#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "agenda.h"

#define ARQUIVO_TESTE "teste_agenda.csv"

static int falhas = 0;

#define CONFERIR(cond)                                                   \
    do {                                                                 \
        if (!(cond)) {                                                   \
            fprintf(stderr, "FALHOU %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            falhas++;                                                    \
        }                                                                \
    } while (0)

static Contato contato(const char *nome, const char *telefone, const char *email)
{
    Contato c = {0};
    snprintf(c.nome, sizeof c.nome, "%s", nome);
    snprintf(c.telefone, sizeof c.telefone, "%s", telefone);
    snprintf(c.email, sizeof c.email, "%s", email);
    return c;
}

static void escrever_arquivo(const char *conteudo)
{
    FILE *fp = fopen(ARQUIVO_TESTE, "w");
    if (fp != NULL) {
        fputs(conteudo, fp);
        fclose(fp);
    }
}

static void testar_crud(Agenda *a)
{
    a->total = 0;
    Contato maria = contato("Maria Silva", "19 99999-0000", "maria@email.com");
    Contato joao = contato("Joao", "", "");

    CONFERIR(agenda_adicionar(a, &maria) == 0);
    CONFERIR(agenda_adicionar(a, &joao) == 0);
    CONFERIR(a->total == 2);

    Contato repetido = contato("MARIA SILVA", "", "");
    CONFERIR(agenda_adicionar(a, &repetido) == -2);

    Contato vazio = contato("", "123", "");
    CONFERIR(agenda_adicionar(a, &vazio) == -3);

    Contato separador = contato("Ana;Paula", "", "");
    CONFERIR(agenda_adicionar(a, &separador) == -4);

    CONFERIR(agenda_remover(a, "maria silva") == 0);
    CONFERIR(a->total == 1 && strcmp(a->contatos[0].nome, "Joao") == 0);
    CONFERIR(agenda_remover(a, "Ninguem") == -1);
}

static void testar_arquivo(Agenda *a)
{
    a->total = 0;
    Contato maria = contato("Maria Silva", "19 99999-0000", "maria@email.com");
    Contato joao = contato("Joao", "", "");
    agenda_adicionar(a, &maria);
    agenda_adicionar(a, &joao);

    remove(ARQUIVO_TESTE);
    CONFERIR(agenda_carregar(a, ARQUIVO_TESTE) == 0 && a->total == 0); /* inexistente */

    agenda_adicionar(a, &maria);
    agenda_adicionar(a, &joao);
    CONFERIR(agenda_salvar(a, ARQUIVO_TESTE) == 0);
    CONFERIR(agenda_salvar(a, ARQUIVO_TESTE) == 0); /* sobrescrever também funciona */

    a->total = 0;
    CONFERIR(agenda_carregar(a, ARQUIVO_TESTE) == 0);
    CONFERIR(a->total == 2);
    CONFERIR(strcmp(a->contatos[0].email, "maria@email.com") == 0);
    CONFERIR(strcmp(a->contatos[1].nome, "Joao") == 0 && a->contatos[1].telefone[0] == '\0');

    /* Arquivo editado à mão no Windows, com "\r\n" e linha em branco */
    escrever_arquivo("Ana;123;ana@x.com\r\n\r\nBia;;\r\n");
    CONFERIR(agenda_carregar(a, ARQUIVO_TESTE) == 0 && a->total == 2);
    CONFERIR(strcmp(a->contatos[0].email, "ana@x.com") == 0);

    escrever_arquivo("so um campo\n");
    CONFERIR(agenda_carregar(a, ARQUIVO_TESTE) == -1 && a->total == 0);

    escrever_arquivo("a;b;c;d\n");
    CONFERIR(agenda_carregar(a, ARQUIVO_TESTE) == -1);

    escrever_arquivo(";123;sem@nome.com\n");
    CONFERIR(agenda_carregar(a, ARQUIVO_TESTE) == -1);

    /* Telefone maior que TAM_TELEFONE */
    escrever_arquivo("Ana;123456789012345678901234567890;\n");
    CONFERIR(agenda_carregar(a, ARQUIVO_TESTE) == -1);

    remove(ARQUIVO_TESTE);
}

int main(void)
{
    Agenda *a = malloc(sizeof *a);
    if (a == NULL) {
        return 1;
    }

    testar_crud(a);
    testar_arquivo(a);
    free(a);

    if (falhas) {
        printf("agenda-contatos: %d teste(s) falharam\n", falhas);
        return 1;
    }
    printf("agenda-contatos: todos os testes passaram\n");
    return 0;
}
