# 🐚 Mini Shell

Um interpretador de comandos (como o Bash) escrito do zero em C. Ele lê o que você digita, separa em comandos e executa cada um em um **processo filho**, com suporte a **pipes**, **redirecionamentos**, **aspas**, **curingas** (`*.c`), **histórico** e comandos internos.

```
Mini Shell em C. Digite "ajuda" para ver os comandos ou "sair" para encerrar.
minishell:~/projetos$ ls *.c | wc -l
3
minishell:~/projetos$ echo "bom dia, mundo" > saudacao.txt
minishell:~/projetos$ tr a-z A-Z < saudacao.txt
BOM DIA, MUNDO
minishell:~/projetos$ cd /pasta/que/nao/existe
cd: /pasta/que/nao/existe: No such file or directory
[1] minishell:~/projetos$ historico
    1  ls *.c | wc -l
    2  echo "bom dia, mundo" > saudacao.txt
    3  tr a-z A-Z < saudacao.txt
    4  cd /pasta/que/nao/existe
    5  historico
```

## Funcionalidades

- **Qualquer programa do sistema:** `ls`, `grep`, `gcc`, `python3`... procurados no `PATH` com `execvp`
- **Pipes ilimitados:** `cat log.txt | grep erro | sort | uniq -c`
- **Redirecionamentos:** `<` (entrada), `>` (sobrescreve) e `>>` (acrescenta)
- **Aspas simples e duplas:** `echo "a | b"` imprime o texto, sem criar um pipe
- **Curingas:** `*`, `?` e `[ ]` viram a lista de arquivos (`ls *.c`); entre aspas (`"*.c"`) ficam como texto
- **Prompt com a pasta atual**, abreviando a pasta pessoal como `~`, e o código de erro do último comando em vermelho
- **Ctrl+C** interrompe o comando em execução, mas não fecha o shell
- **Mensagens de erro claras:** comando inexistente, arquivo não encontrado e erros de sintaxe (`| ls`, `echo >`, aspas sem fechar)

### Comandos internos

| Comando | O que faz |
|---------|-----------|
| `cd [pasta]` | Muda de pasta. Sem argumento vai para a pasta pessoal; `cd -` volta para a anterior |
| `historico` | Lista os últimos 100 comandos (`historico -c` apaga) |
| `!!` | Repete o último comando |
| `!N` | Repete o comando de número N do histórico |
| `ajuda` | Mostra a lista de comandos |
| `sair [código]` | Encerra o shell (`exit` também funciona) |

## Estrutura

```
mini-shell/
├── include/
│   ├── parser.h       # Estruturas Comando e Pipeline
│   ├── executor.h
│   └── historico.h
├── src/
│   ├── main.c         # Laço principal, prompt e expansão de !! e !N
│   ├── parser.c       # Transforma o texto em comandos (tokens, aspas, | < > >>)
│   ├── executor.c     # fork, pipe, dup2, execvp, waitpid, curingas e comandos internos
│   └── historico.c    # Vetor circular com os últimos comandos
├── tests/
│   └── testar.sh      # 32 testes automatizados
├── Makefile
└── README.md
```

## Dependências

- **Linux, macOS ou WSL.** O shell usa chamadas do sistema POSIX (`fork`, `execvp`, `pipe`), que não existem no Windows comum. No Windows, use o [WSL](https://learn.microsoft.com/pt-br/windows/wsl/install).
- GCC (ou Clang) e `make`

## Como compilar e executar

```bash
cd mini-shell
make
./bin/minishell
```

Para rodar os testes automatizados:

```bash
make test
```

## Como funciona

**1. Análise (parser).** A linha é dividida em *tokens*: palavras e os símbolos `|`, `<`, `>` e `>>`. Trechos entre aspas viram uma palavra só. O resultado é um `Pipeline`, uma lista de `Comando`s, cada um com seus argumentos e redirecionamentos.

**2. Execução.** Para `ls | grep .c | wc -l`, o shell:

1. cria um `pipe()` entre cada par de comandos;
2. faz um `fork()` para cada comando;
3. em cada filho, usa `dup2()` para ligar a entrada e a saída aos pipes certos e chama `execvp()`, que troca o programa do processo filho pelo `ls`, `grep` ou `wc`;
4. no pai, fecha as pontas dos pipes (senão o `wc` esperaria para sempre) e espera todos os filhos com `waitpid()`.

**3. Comandos internos.** O `cd` precisa rodar no próprio shell, sem `fork`: se rodasse em um filho, mudaria a pasta do filho, e não a do shell. Por isso os comandos internos são verificados antes de criar processos.

## Conceitos praticados

Processos (`fork`, `execvp`, `waitpid`), expansão de curingas (`glob`), comunicação entre processos (`pipe`), descritores de arquivo (`open`, `dup2`, `close`), sinais (`SIGINT`), análise léxica, ponteiros para funções (tabela de comandos internos), vetor circular, testes automatizados em Bash.
