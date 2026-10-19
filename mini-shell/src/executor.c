#define _POSIX_C_SOURCE 200809L

#include "executor.h"

#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "historico.h"

int  ultimo_status = 0;
bool deve_sair = false;

/* ================= Comandos internos ================= */
/* Precisam rodar dentro do próprio shell: um "cd" em um processo filho
 * mudaria a pasta do filho, e não a do shell. */

static char pasta_anterior[PATH_MAX] = "";

static int interno_cd(Comando *cmd)
{
    const char *destino = cmd->argc > 1 ? cmd->argv[1] : getenv("HOME");

    if (destino != NULL && strcmp(destino, "-") == 0) {
        if (pasta_anterior[0] == '\0') {
            fprintf(stderr, "cd: ainda não existe pasta anterior\n");
            return 1;
        }
        destino = pasta_anterior;
        printf("%s\n", destino);
    }
    if (destino == NULL) {
        fprintf(stderr, "cd: variável HOME não definida\n");
        return 1;
    }

    char atual[PATH_MAX];
    if (getcwd(atual, sizeof atual) == NULL) atual[0] = '\0';

    if (chdir(destino) != 0) {
        fprintf(stderr, "cd: %s: %s\n", destino, strerror(errno));
        return 1;
    }
    strcpy(pasta_anterior, atual);
    return 0;
}

static int interno_sair(Comando *cmd)
{
    deve_sair = true;
    return cmd->argc > 1 ? atoi(cmd->argv[1]) : ultimo_status;
}

static int interno_historico(Comando *cmd)
{
    if (cmd->argc > 1 && strcmp(cmd->argv[1], "-c") == 0) {
        historico_limpar();
    } else {
        historico_listar();
    }
    return 0;
}

static int interno_ajuda(Comando *cmd)
{
    (void)cmd;
    puts("Mini Shell - comandos internos:\n"
         "  cd [pasta]        muda de pasta (cd - volta para a anterior)\n"
         "  historico [-c]    lista os comandos digitados (-c apaga)\n"
         "  !!                repete o último comando\n"
         "  !N                repete o comando de número N do histórico\n"
         "  ajuda             mostra esta mensagem\n"
         "  sair [código]     encerra o shell (exit também funciona)\n"
         "\n"
         "Também funcionam: pipes (|), redirecionamentos (<, >, >>) e aspas.\n"
         "Qualquer outro comando é procurado no PATH, como ls, grep ou gcc.");
    return 0;
}

typedef struct {
    const char *nome;
    int (*funcao)(Comando *cmd);
} Interno;

static const Interno INTERNOS[] = {
    {"cd", interno_cd},
    {"sair", interno_sair},
    {"exit", interno_sair},
    {"historico", interno_historico},
    {"history", interno_historico},
    {"ajuda", interno_ajuda},
    {"help", interno_ajuda},
};

static const Interno *buscar_interno(const char *nome)
{
    for (size_t i = 0; i < sizeof INTERNOS / sizeof *INTERNOS; i++) {
        if (strcmp(INTERNOS[i].nome, nome) == 0) return &INTERNOS[i];
    }
    return NULL;
}

/* ================= Redirecionamentos ================= */

/* Liga stdin/stdout aos arquivos de < e >. Retorna 0 ou -1 em erro. */
static int aplicar_redirecionamentos(const Comando *cmd)
{
    if (cmd->entrada) {
        int fd = open(cmd->entrada, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "%s: %s\n", cmd->entrada, strerror(errno));
            return -1;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if (cmd->saida) {
        int flags = O_WRONLY | O_CREAT | (cmd->anexar ? O_APPEND : O_TRUNC);
        int fd = open(cmd->saida, flags, 0644);
        if (fd < 0) {
            fprintf(stderr, "%s: %s\n", cmd->saida, strerror(errno));
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    return 0;
}

/* Comando interno sozinho: roda no próprio shell, guardando e
 * restaurando stdin/stdout se houver redirecionamento. */
static int executar_interno_no_shell(const Interno *interno, Comando *cmd)
{
    fflush(stdout);
    int stdin_original = dup(STDIN_FILENO);
    int stdout_original = dup(STDOUT_FILENO);

    int status = 1;
    if (aplicar_redirecionamentos(cmd) == 0) {
        status = interno->funcao(cmd);
    }

    fflush(stdout);
    dup2(stdin_original, STDIN_FILENO);
    dup2(stdout_original, STDOUT_FILENO);
    close(stdin_original);
    close(stdout_original);
    return status;
}

/* ================= Pipeline ================= */

/* ================= Curingas (* ? [ ]) ================= */

/*
 * Troca argumentos como *.c pela lista de arquivos que combinam, usando
 * glob(). Argumentos entre aspas ficam como estão. Se nada combinar, o
 * texto original é mantido (como no Bash). Só roda no processo filho,
 * então a memória alocada aqui é liberada quando o programa termina.
 */
static char **expandir_curingas(Comando *cmd)
{
    size_t capacidade = (size_t)cmd->argc + 1, n = 0;
    char **argv = malloc(capacidade * sizeof *argv);

    for (int i = 0; i < cmd->argc; i++) {
        bool tem_curinga = !cmd->literal[i] && strpbrk(cmd->argv[i], "*?[") != NULL;
        glob_t achados;

        if (tem_curinga && glob(cmd->argv[i], 0, NULL, &achados) == 0) {
            /* Abre espaço para todos os arquivos encontrados */
            capacidade += achados.gl_pathc;
            argv = realloc(argv, capacidade * sizeof *argv);
            for (size_t j = 0; j < achados.gl_pathc; j++) {
                argv[n++] = achados.gl_pathv[j];
            }
        } else {
            argv[n++] = cmd->argv[i];   /* sem curinga, entre aspas ou sem resultado */
        }
    }
    argv[n] = NULL;
    return argv;
}

/* Roda dentro do processo filho e nunca retorna */
static void executar_no_filho(Comando *cmd)
{
    /* O filho volta a reagir ao Ctrl+C normalmente */
    signal(SIGINT, SIG_DFL);

    if (aplicar_redirecionamentos(cmd) != 0) _exit(1);

    const Interno *interno = buscar_interno(cmd->argv[0]);
    if (interno) {
        int status = interno->funcao(cmd);
        fflush(stdout);
        _exit(status);
    }

    char **argv = expandir_curingas(cmd);
    execvp(argv[0], argv);

    /* Só chega aqui se o execvp falhar */
    if (errno == ENOENT) {
        fprintf(stderr, "%s: comando não encontrado\n", cmd->argv[0]);
        _exit(127);
    }
    fprintf(stderr, "%s: %s\n", cmd->argv[0], strerror(errno));
    _exit(126);
}

int executar_pipeline(Pipeline *pipeline)
{
    int n = pipeline->quantidade;
    if (n == 0) return ultimo_status;

    /* Comando interno sozinho roda sem fork (necessário para o cd) */
    if (n == 1) {
        const Interno *interno = buscar_interno(pipeline->comandos[0].argv[0]);
        if (interno) {
            ultimo_status = executar_interno_no_shell(interno, &pipeline->comandos[0]);
            return ultimo_status;
        }
    }

    pid_t pids[MAX_COMANDOS];
    int entrada_anterior = -1;   /* lado de leitura do pipe anterior */

    fflush(stdout);
    for (int i = 0; i < n; i++) {
        int canos[2] = {-1, -1};
        if (i < n - 1 && pipe(canos) != 0) {
            perror("pipe");
            n = i;   /* espera só os que já foram criados */
            break;
        }

        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            n = i;
            break;
        }

        if (pids[i] == 0) {
            /* Filho: lê do pipe anterior e escreve no próximo */
            if (entrada_anterior != -1) {
                dup2(entrada_anterior, STDIN_FILENO);
                close(entrada_anterior);
            }
            if (canos[1] != -1) {
                dup2(canos[1], STDOUT_FILENO);
                close(canos[0]);
                close(canos[1]);
            }
            executar_no_filho(&pipeline->comandos[i]);
        }

        /* Pai: fecha o que não usa mais, senão os comandos nunca terminam */
        if (entrada_anterior != -1) close(entrada_anterior);
        if (canos[1] != -1) close(canos[1]);
        entrada_anterior = canos[0];
    }
    if (entrada_anterior != -1) close(entrada_anterior);

    /* Espera todos; o código que vale é o do último comando */
    int status = 0;
    for (int i = 0; i < n; i++) {
        int s;
        while (waitpid(pids[i], &s, 0) < 0 && errno == EINTR) {
        }
        if (i == n - 1) {
            if (WIFEXITED(s))        status = WEXITSTATUS(s);
            else if (WIFSIGNALED(s)) status = 128 + WTERMSIG(s);
        }
    }
    ultimo_status = status;
    return status;
}
