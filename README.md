# Labirinto com Chaves e Baú

Trabalho Prático 1 — Projeto e Análise de Algoritmos (UFV — Campus Florestal).
Professor: Daniel Mendes Barbosa.

**Integrantes:** _(preencher com nome e matrícula dos quatro integrantes)_

O programa, escrito em **C**, usa **backtracking** para encontrar um caminho em um labirinto: parte da sala inicial, coleta todas as chaves necessárias e chega ao baú, andando só nas quatro direções, sem atravessar paredes e sem repetir salas. O resultado é um caminho válido ou a mensagem de que não há caminho possível.

## Estrutura do projeto

```
Makefile              compilação
README.md             este arquivo
relatorio.pdf         documentação do trabalho
src/                  código-fonte do programa
    main.c            opções de linha de comando e fluxo principal
    labirinto.c/.h    leitura do mapa e estruturas de dados
    busca.c/.h        backtracking e contadores de recursão
    globo.c/.h        cálculo da vizinhança (inclusive no modo globo)
    saida.c/.h        impressão do caminho, do mapa e do modo análise
    gerador_mapas.c   extra: gerador de mapas
entradas/
    basicos/          mapas de teste do programa básico (com e sem solução)
    globo/            mapas de teste do modo globo
    leitura/          casos de teste da leitura (válidos e inválidos)
```

## Como compilar

```bash
make            # gera bin/labirinto
make gerador    # extra: gera bin/gerador_mapas
make clean      # apaga a pasta bin/
```

## Como executar

```bash
./bin/labirinto entradas/basicos/09_enunciado_10x10.txt             # modo normal
./bin/labirinto entradas/basicos/09_enunciado_10x10.txt --analise   # modo análise
```

| Opção | Efeito |
|---|---|
| `--analise` | Modo análise: mostra chamadas recursivas, nível máximo de recursão e tempo |
| `--todos` | Extra: busca e relata todos os caminhos válidos |
| `--globo` | Extra: as bordas opostas do mapa são vizinhas |
| `--mapa` | Extra: desenha o labirinto com o caminho destacado |
| `--limite N` | Com `--todos`: quantos caminhos imprimir (padrão 10; 0 imprime todos) |
| `--ajuda` | Mostra as opções |

O programa devolve 0 quando executa (com ou sem solução) e 2 em caso de erro de uso, de arquivo ou de memória.

## Formato do arquivo de entrada

A primeira linha tem o número de linhas e de colunas; a segunda, a quantidade de chaves necessárias; as demais descrevem o mapa: `A` (início), `C` (chave), `X` (baú), `0` (sala livre) e `1` (parede).

```
10 10
2
A 0 0 0 0 0 0 0 0 C
0 1 1 1 1 0 1 1 1 0
...
```

## Gerador de mapas (extra)

```bash
./bin/gerador_mapas --linhas 8 --colunas 10 --chaves 3 --paredes 30 \
                    --saida mapa.txt --semente 42 --garantir-solucao
```

Veja `./bin/gerador_mapas --ajuda` para os parâmetros.
