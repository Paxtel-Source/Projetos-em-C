#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <stdbool.h>

#include "parser.h"

/* Código de saída do último comando (o "$?" de outros shells) */
extern int ultimo_status;

/* Pedido de encerramento feito pelo comando interno "sair"/"exit" */
extern bool deve_sair;

/* Executa o pipeline e devolve o código de saída do último comando */
int executar_pipeline(Pipeline *pipeline);

#endif /* EXECUTOR_H */
