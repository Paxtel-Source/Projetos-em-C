#ifndef HISTORICO_H
#define HISTORICO_H

#define MAX_HISTORICO 100

/* Guarda os últimos comandos digitados (um vetor circular) */
void        historico_adicionar(const char *linha);
int         historico_quantidade(void);

/* Número do comando: 1 é o mais antigo ainda guardado. NULL se não existir. */
const char *historico_obter(int numero);
int         historico_primeiro_numero(void);

void        historico_listar(void);
void        historico_limpar(void);

#endif /* HISTORICO_H */
