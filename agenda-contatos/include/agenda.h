#ifndef AGENDA_H
#define AGENDA_H

#include <stddef.h>

#define MAX_CONTATOS  100
#define TAM_NOME      50
#define TAM_TELEFONE  20
#define TAM_EMAIL     60

#define ARQUIVO_PADRAO "contatos.csv"

/* Separa nome, telefone e e-mail em cada linha do arquivo */
#define SEPARADOR ';'

typedef struct {
    char nome[TAM_NOME];
    char telefone[TAM_TELEFONE];
    char email[TAM_EMAIL];
} Contato;

typedef struct {
    Contato contatos[MAX_CONTATOS];
    size_t  total;
} Agenda;

/*
 * Arquivo de texto, um contato por linha: nome;telefone;e-mail
 * carregar: retorna 0 (ok ou arquivo inexistente) ou -1 (arquivo corrompido).
 * salvar:   grava num arquivo temporário e só depois troca pelo original.
 */
int agenda_carregar(Agenda *agenda, const char *arquivo);
int agenda_salvar(const Agenda *agenda, const char *arquivo);

/*
 * CRUD
 * adicionar: 0 ok, -1 agenda cheia, -2 nome duplicado,
 *            -3 nome vazio, -4 algum campo contém o SEPARADOR.
 */
int  agenda_adicionar(Agenda *agenda, const Contato *novo);
void agenda_listar(const Agenda *agenda);
int  agenda_buscar(const Agenda *agenda, const char *termo);
int  agenda_remover(Agenda *agenda, const char *nome);

#endif /* AGENDA_H */
