# Compila, testa ou limpa todos os projetos de uma vez
PROJETOS := calculadora agenda-contatos jogo-da-velha conversor-bases sistema-escolar compactador-rle \
            estruturas-de-dados mini-shell snake

# Projetos que têm testes automatizados ("make test")
COM_TESTES := compactador-rle estruturas-de-dados mini-shell snake

.PHONY: all test clean $(PROJETOS)

all: $(PROJETOS)

$(PROJETOS):
	$(MAKE) -C $@

test:
	for p in $(COM_TESTES); do $(MAKE) -C $$p test || exit 1; done

clean:
	for p in $(PROJETOS); do $(MAKE) -C $$p clean; done
