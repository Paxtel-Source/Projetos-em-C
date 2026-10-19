# 📒 Agenda de Contatos (CRUD em Arquivo)

Sistema de terminal em C para **adicionar, listar, buscar e remover** contatos. Os dados ficam salvos em um **arquivo de texto** (`contatos.csv`), então continuam lá na próxima vez que o programa for aberto.

## Funcionalidades

- **Adicionar** contato (nome, telefone, e-mail) — bloqueia nomes duplicados
- **Listar** todos os contatos em formato de tabela
- **Buscar** por trecho do nome, telefone ou e-mail (sem diferenciar maiúsculas)
- **Remover** contato pelo nome
- Salvamento automático ao sair, com `fopen`, `fprintf` e `fclose`, gravando primeiro num arquivo temporário para não corromper o original
- Carregamento ao iniciar, com `fopen`, `fgets` e `fclose`
- Se o arquivo estiver corrompido, ele é guardado como `contatos.csv.bak` antes de qualquer gravação

## Estrutura

```
agenda-contatos/
├── include/
│   └── agenda.h    # Struct Contato e declarações do CRUD
├── src/
│   ├── agenda.c    # CRUD e leitura/gravação do arquivo
│   └── main.c      # Menu e interação com o usuário
├── tests/
│   └── testes.c    # Testes do CRUD e do arquivo (make test)
├── Makefile
└── README.md
```

## Dependências

- Compilador C (GCC ou Clang) com suporte a C11
- `make`
- No Windows: [MSYS2](https://www.msys2.org/), MinGW ou WSL

## Como compilar e executar

1. Entre na pasta do projeto:
   ```bash
   cd agenda-contatos
   ```
2. Compile:
   ```bash
   make
   ```
3. Execute:
   ```bash
   ./bin/agenda
   ```
   Opcionalmente, informe outro arquivo de dados:
   ```bash
   ./bin/agenda meus_contatos.csv
   ```
4. Para rodar os testes:
   ```bash
   make test
   ```
5. Para apagar os arquivos compilados:
   ```bash
   make clean
   ```

## Exemplo de uso

```
====== AGENDA DE CONTATOS ======
1. Adicionar contato
2. Listar contatos
3. Buscar contato
4. Remover contato
0. Sair
Opcao: 2

  #  Nome                      | Telefone        | E-mail
---------------------------------------------------------------------
  1. Maria Silva               | 84 99999-0000   | maria@email.com

Total: 1 contato(s)
```

## Formato do arquivo

O `contatos.csv` tem um contato por linha, com os campos separados por `;`:

```
Maria Silva;84 99999-0000;maria@email.com
```

Por isso nenhum campo pode conter `;`. Como é texto, o arquivo funciona em qualquer sistema e compilador, e dá para abrir no editor ou no Excel. Ele é ignorado pelo Git (veja o `.gitignore`), pois são dados pessoais de cada usuário.

## Conceitos praticados

`struct`, vetores de structs, manipulação de arquivos de texto (`fopen`, `fgets`, `fprintf`, `fclose`, `rename`), `memmove`, alocação dinâmica com `malloc`/`free`, leitura segura com `fgets`.
