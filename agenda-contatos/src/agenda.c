#include "agenda.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* ---------- Funções auxiliares ---------- */

/* Compara strings ignorando maiúsculas/minúsculas */
static int iguais_sem_caso(const char *a, const char *b)
{
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b;
}

/* Verifica se "termo" aparece dentro de "texto" (sem diferenciar caso) */
static int contem_sem_caso(const char *texto, const char *termo)
{
    size_t n = strlen(termo);
    if (n == 0) {
        return 1;
    }
    for (; *texto; texto++) {
        size_t i = 0;
        while (i < n && texto[i] &&
               tolower((unsigned char)texto[i]) == tolower((unsigned char)termo[i])) {
            i++;
        }
        if (i == n) {
            return 1;
        }
    }
    return 0;
}

static void imprimir_contato(size_t indice, const Contato *c)
{
    printf("%3zu. %-25s | %-15s | %s\n", indice + 1, c->nome, c->telefone, c->email);
}

/* Copia n caracteres para destino, se couberem junto com o '\0' */
static int copiar_campo(char *destino, size_t tamanho, const char *inicio, size_t n)
{
    if (n >= tamanho) {
        return 0;
    }
    memcpy(destino, inicio, n);
    destino[n] = '\0';
    return 1;
}

/* Divide "nome;telefone;email" nos campos do contato. Retorna 1 se a linha for válida. */
static int separar_campos(const char *linha, Contato *c)
{
    const char *p1 = strchr(linha, SEPARADOR);
    if (p1 == NULL) {
        return 0;
    }
    const char *p2 = strchr(p1 + 1, SEPARADOR);
    if (p2 == NULL || strchr(p2 + 1, SEPARADOR) != NULL) {
        return 0;
    }
    return copiar_campo(c->nome, sizeof c->nome, linha, (size_t)(p1 - linha)) &&
           copiar_campo(c->telefone, sizeof c->telefone, p1 + 1, (size_t)(p2 - p1 - 1)) &&
           copiar_campo(c->email, sizeof c->email, p2 + 1, strlen(p2 + 1)) &&
           c->nome[0] != '\0';
}

/* ---------- Arquivo ---------- */

int agenda_carregar(Agenda *agenda, const char *arquivo)
{
    agenda->total = 0;

    FILE *fp = fopen(arquivo, "r");
    if (fp == NULL) {
        return 0; /* Arquivo ainda não existe: agenda vazia */
    }

    /* Cabe a maior linha válida, os dois separadores e o "\r\n" */
    char linha[TAM_NOME + TAM_TELEFONE + TAM_EMAIL + 4];
    int status = 0;

    while (fgets(linha, sizeof linha, fp) != NULL) {
        size_t n = strcspn(linha, "\r\n");
        if (linha[n] == '\0' && !feof(fp)) {
            status = -1; /* Linha maior que qualquer contato válido */
            break;
        }
        linha[n] = '\0';
        if (n == 0) {
            continue; /* Ignora linhas em branco */
        }

        Contato c;
        if (agenda->total >= MAX_CONTATOS || !separar_campos(linha, &c)) {
            status = -1;
            break;
        }
        agenda->contatos[agenda->total++] = c;
    }

    if (ferror(fp)) {
        status = -1;
    }
    fclose(fp);

    if (status != 0) {
        agenda->total = 0;
    }
    return status;
}

int agenda_salvar(const Agenda *agenda, const char *arquivo)
{
    /* Grava primeiro num temporário: se algo der errado, o original fica intacto */
    char temporario[FILENAME_MAX];
    if (snprintf(temporario, sizeof temporario, "%s.tmp", arquivo) >= (int)sizeof temporario) {
        return -1;
    }

    FILE *fp = fopen(temporario, "w");
    if (fp == NULL) {
        return -1;
    }

    int ok = 1;
    for (size_t i = 0; i < agenda->total && ok; i++) {
        const Contato *c = &agenda->contatos[i];
        ok = fprintf(fp, "%s%c%s%c%s\n", c->nome, SEPARADOR, c->telefone, SEPARADOR, c->email) > 0;
    }
    if (fclose(fp) != 0) {
        ok = 0;
    }
    if (!ok) {
        remove(temporario);
        return -1;
    }

    /* No Windows, rename() falha se o destino já existe */
    remove(arquivo);
    if (rename(temporario, arquivo) != 0) {
        return -1; /* Os dados continuam salvos no .tmp */
    }
    return 0;
}

/* ---------- CRUD ---------- */

int agenda_adicionar(Agenda *agenda, const Contato *novo)
{
    if (agenda->total >= MAX_CONTATOS) {
        return -1; /* Agenda cheia */
    }
    if (novo->nome[0] == '\0') {
        return -3; /* Nome vazio */
    }
    if (strchr(novo->nome, SEPARADOR) || strchr(novo->telefone, SEPARADOR) ||
        strchr(novo->email, SEPARADOR)) {
        return -4; /* Quebraria o formato do arquivo */
    }
    for (size_t i = 0; i < agenda->total; i++) {
        if (iguais_sem_caso(agenda->contatos[i].nome, novo->nome)) {
            return -2; /* Nome duplicado */
        }
    }
    agenda->contatos[agenda->total++] = *novo;
    return 0;
}

void agenda_listar(const Agenda *agenda)
{
    if (agenda->total == 0) {
        printf("Nenhum contato cadastrado.\n");
        return;
    }
    printf("\n  #  %-25s | %-15s | %s\n", "Nome", "Telefone", "E-mail");
    printf("---------------------------------------------------------------------\n");
    for (size_t i = 0; i < agenda->total; i++) {
        imprimir_contato(i, &agenda->contatos[i]);
    }
    printf("\nTotal: %zu contato(s)\n", agenda->total);
}

int agenda_buscar(const Agenda *agenda, const char *termo)
{
    int encontrados = 0;
    for (size_t i = 0; i < agenda->total; i++) {
        const Contato *c = &agenda->contatos[i];
        if (contem_sem_caso(c->nome, termo) || contem_sem_caso(c->telefone, termo) ||
            contem_sem_caso(c->email, termo)) {
            imprimir_contato(i, c);
            encontrados++;
        }
    }
    return encontrados;
}

int agenda_remover(Agenda *agenda, const char *nome)
{
    for (size_t i = 0; i < agenda->total; i++) {
        if (iguais_sem_caso(agenda->contatos[i].nome, nome)) {
            /* Desloca os contatos seguintes uma posição para trás */
            memmove(&agenda->contatos[i], &agenda->contatos[i + 1],
                    (agenda->total - i - 1) * sizeof(Contato));
            agenda->total--;
            return 0;
        }
    }
    return -1;
}
