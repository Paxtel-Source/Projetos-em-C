# Compila ou limpa todos os projetos de uma vez
PROJETOS := calculadora agenda-contatos jogo-da-velha conversor-bases

.PHONY: all test clean $(PROJETOS)

all: $(PROJETOS)

$(PROJETOS):
	$(MAKE) -C $@

test:
	for p in $(PROJETOS); do $(MAKE) -C $$p test || exit 1; done

clean:
	for p in $(PROJETOS); do $(MAKE) -C $$p clean; done
