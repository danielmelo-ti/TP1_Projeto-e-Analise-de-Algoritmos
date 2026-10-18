#include "saida.h"

#include <stdio.h>
#include <stdlib.h>

void saida_imprimir_sem_solucao(void) {
    printf("Não há um caminho possível para a entrada de dados fornecida\n");
}

void saida_imprimir_caminho(const Caminho *caminho) {
    size_t i;

    for (i = 0; i < caminho->tamanho; i++) {
        printf("[%zu,%zu]", caminho->posicoes[i].linha,
               caminho->posicoes[i].coluna);
        if (i + 1 < caminho->tamanho) {
            printf(",");
        }
    }
    printf("\n");
}

//Seta do movimento de a para b. Na passagem pela borda (modo globo) a 
// diferenca entre as coordenadas e maior que 1, e o sentido se inverte
static char seta(Posicao a, Posicao b) {
    long dl = (long)b.linha - (long)a.linha;
    long dc = (long)b.coluna - (long)a.coluna;

    if (dl != 0) {
        if (dl == 1 || dl < -1) {
            return 'v';
        }
        return '^';
    }
    if (dc == 1 || dc < -1) {
        return '>';
    }
    return '<';
}

void saida_imprimir_mapa(const Labirinto *labirinto, const Caminho *caminho) {
    size_t i, j;
    size_t colunas = labirinto->colunas;
    char *grade = malloc(labirinto->linhas * colunas);

    if (grade == NULL) {
        fprintf(stderr, "Sem memoria para desenhar o mapa.\n");
        return;
    }

    //Copia o mapa trocando 0 por '.' e 1 por '#'
    for (i = 0; i < labirinto->linhas; i++) {
        for (j = 0; j < colunas; j++) {
            char c = labirinto->celulas[i][j];
            if (c == '0') {
                c = '.';
            } else if (c == '1') {
                c = '#';
            }
            grade[i * colunas + j] = c;
        }
    }

    //Marca o caminho: salas livres viram setas; A, C e X continuam
    for (i = 0; i + 1 < caminho->tamanho; i++) {
        Posicao atual = caminho->posicoes[i];
        char *celula = &grade[atual.linha * colunas + atual.coluna];
        if (*celula == '.') {
            *celula = seta(atual, caminho->posicoes[i + 1]);
        }
    }

    for (i = 0; i < labirinto->linhas; i++) {
        for (j = 0; j < colunas; j++) {
            printf("%c", grade[i * colunas + j]);
            if (j + 1 < colunas) {
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("Legenda: A inicio | C chave | X bau | # parede | . sala livre | "
           "^ > v < caminho\n");
    free(grade);
}

void saida_imprimir_todos(const Labirinto *labirinto,
                          const ResultadoBusca *resultado,
                          size_t limite, int mostrar_mapa) {
    size_t i;
    size_t mostrar = resultado->quantidade;

    if (limite > 0 && limite < mostrar) {
        mostrar = limite;
    }

    printf("Total de caminhos validos: %zu\n", resultado->quantidade);
    for (i = 0; i < mostrar; i++) {
        printf("Caminho %zu (%zu salas):\n", i + 1,
               resultado->caminhos[i].tamanho);
        saida_imprimir_caminho(&resultado->caminhos[i]);
        if (mostrar_mapa) {
            saida_imprimir_mapa(labirinto, &resultado->caminhos[i]);
            printf("\n");
        }
    }
    if (mostrar < resultado->quantidade) {
        printf("(mostrando %zu de %zu caminhos; use --limite 0 para mostrar "
               "todos)\n", mostrar, resultado->quantidade);
    }
}

void saida_imprimir_analise(const Labirinto *labirinto,
                            const ResultadoBusca *resultado,
                            int todos_caminhos, int modo_globo,
                            double segundos) {
    printf("\n=== MODO ANALISE ===\n");
    printf("Mapa: %zux%zu, %zu chave(s)\n", labirinto->linhas,
           labirinto->colunas, labirinto->quantidade_chaves);
    printf("Modo globo: %s | Todos os caminhos: %s\n",
           modo_globo ? "sim" : "nao", todos_caminhos ? "sim" : "nao");
    printf("Chamadas recursivas: %zu\n", resultado->chamadas_recursivas);
    printf("Nivel maximo de recursao: %zu\n", resultado->nivel_maximo);
    printf("Caminhos encontrados: %zu\n", resultado->quantidade);
    printf("Tempo de busca: %.6f s\n", segundos);
}
