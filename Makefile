CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -Isrc

FONTES = src/main.c src/saida.c src/busca.c src/globo.c src/labirinto.c
CABECALHOS = $(wildcard src/*.h)

all: bin/labirinto

bin/labirinto: $(FONTES) $(CABECALHOS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(FONTES) -o $@

gerador: bin/gerador_mapas

bin/gerador_mapas: src/gerador_mapas.c
	@mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf bin

.PHONY: all gerador clean
