#!/usr/bin/env bash
# Testes do Mini Shell: cada caso envia comandos pela entrada padrão
# e compara a saída com o resultado esperado.
#
# Uso: ./tests/testar.sh ./bin/minishell

set -u
SHELL_TESTADO="$(realpath "$1")"
PASTA="$(mktemp -d)"
trap 'rm -rf "$PASTA"' EXIT
cd "$PASTA" || exit 1

acertos=0
falhas=0

# caso "nome" "comandos" "saída esperada"
caso() {
    local nome="$1" entrada="$2" esperado="$3"
    local obtido
    obtido="$(printf '%b' "$entrada" | "$SHELL_TESTADO" 2>&1)"
    if [[ "$obtido" == "$esperado" ]]; then
        acertos=$((acertos + 1))
        echo "  ok    $nome"
    else
        falhas=$((falhas + 1))
        echo "  FALHA $nome"
        echo "        esperado: $(printf '%q' "$esperado")"
        echo "        obtido:   $(printf '%q' "$obtido")"
    fi
}

echo "[comandos simples]"
caso "echo"                "echo ola mundo\n"                          "ola mundo"
caso "aspas duplas"        'echo "a   b"\n'                             "a   b"
caso "aspas simples"       "echo 'x | y > z'\n"                         "x | y > z"
caso "aspas coladas"       "echo \"ab\"'cd'ef\n"                        "abcdef"
caso "linha vazia e comentário" "\n   \n# nada\necho fim\n"             "fim"

echo "[pipes]"
caso "um pipe"             "printf 'b\\\\na\\\\nc\\\\n' | sort\n"       "$(printf 'a\nb\nc')"
caso "vários pipes"        "printf 'x\\\\ny\\\\nx\\\\n' | sort | uniq | wc -l\n" "2"
caso "pipe sem espaços"    "echo abc|tr a-z A-Z\n"                      "ABC"

echo "[redirecionamentos]"
caso "> cria arquivo"      "echo primeira > f.txt\ncat f.txt\n"          "primeira"
caso ">> anexa"            "echo um > g.txt\necho dois >> g.txt\ncat g.txt\n" "$(printf 'um\ndois')"
caso "< lê arquivo"        "echo conteudo > h.txt\ntr a-z A-Z < h.txt\n" "CONTEUDO"
caso "< e > juntos"        "echo xyz > i.txt\ntr a-z A-Z < i.txt > j.txt\ncat j.txt\n" "XYZ"
caso "arquivo inexistente" "cat < nao-existe.txt\necho depois\n"         "$(printf 'nao-existe.txt: No such file or directory\ndepois')"

echo "[curingas]"
caso "* expande"           "touch b.c a.c x.h\necho *.c\n"              "a.c b.c"
caso "* entre aspas"       "echo '*.c' \"*.h\"\n"                     "*.c *.h"
caso "* sem resultado"     "echo *.nada\n"                              "*.nada"
caso "? e [ ]"             "touch f1 f2 f3\necho f? f[13]\n"            "f1 f2 f3 f1 f3"

echo "[comandos internos]"
caso "cd e pwd"            "mkdir -p sub/pasta\ncd sub/pasta\npwd\n"     "$PASTA/sub/pasta"
caso "cd -"                "mkdir -p a\ncd a\ncd -\n"                    "$PASTA"
caso "cd inválido"         "cd /nao/existe\necho seguiu\n"               "$(printf 'cd: /nao/existe: No such file or directory\nseguiu')"
caso "historico"           "echo um\necho dois\nhistorico\n"             "$(printf 'um\ndois\n    1  echo um\n    2  echo dois\n    3  historico')"
caso "historico com pipe"  "echo a\nhistorico | wc -l\n"                 "$(printf 'a\n2')"
caso "!!"                  "echo repete\n!!\n"                           "$(printf 'repete\nrepete')"
caso "!N"                  "echo primeiro\necho segundo\n!1\n"           "$(printf 'primeiro\nsegundo\nprimeiro')"
caso "!N inexistente"      "!99\n"                                       "!99: não existe no histórico"
caso "sair para o shell"   "echo antes\nsair\necho depois\n"             "antes"

echo "[erros]"
caso "comando inexistente" "comando-que-nao-existe\n"                    "comando-que-nao-existe: comando não encontrado"
caso "pipe no começo"      "| ls\n"                                      "minishell: erro de sintaxe: comando vazio antes de |"
caso "pipe no fim"         "ls |\n"                                      "minishell: erro de sintaxe: comando vazio depois de |"
caso "> sem arquivo"       "echo oi >\n"                                 "minishell: erro de sintaxe: falta o nome do arquivo depois de <, > ou >>"
caso "aspas abertas"       "echo \"sem fim\n"                            "minishell: erro de sintaxe: aspas sem fechar"

echo "[código de saída]"
printf 'false\n' | "$SHELL_TESTADO"; s1=$?
printf 'sair 7\n' | "$SHELL_TESTADO"; s2=$?
printf 'comando-que-nao-existe\n' | "$SHELL_TESTADO" 2>/dev/null; s3=$?
if [[ $s1 == 1 && $s2 == 7 && $s3 == 127 ]]; then
    acertos=$((acertos + 1)); echo "  ok    false=1, sair 7=7, inexistente=127"
else
    falhas=$((falhas + 1)); echo "  FALHA códigos de saída: $s1 $s2 $s3"
fi

echo
echo "$acertos ok, $falhas falha(s)"
[[ $falhas -eq 0 ]]
