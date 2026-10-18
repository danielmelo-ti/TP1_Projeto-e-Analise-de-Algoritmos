#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "busca.h"
#include "labirinto.h"
#include "saida.h"

#define LIMITE_PADRAO 10

typedef struct {
    const char *arquivo;
    int analise;   
    int todos;     
    int globo;    
    int mapa;      
    size_t limite; 
} Opcoes;

static void mostrar_uso(FILE *destino, const char *programa) {
    fprintf(destino, "Uso: %s ARQUIVO_DO_MAPA [opcoes]\n", programa);
    fprintf(destino, "  --analise    modo analise: mostra chamadas recursivas, nivel maximo e tempo\n");
    fprintf(destino, "  --todos      extra: busca e relata todos os caminhos validos\n");
    fprintf(destino, "  --globo      extra: bordas opostas do mapa sao vizinhas\n");
    fprintf(destino, "  --mapa       extra: desenha o labirinto com o caminho destacado\n");
    fprintf(destino, "  --limite N   com --todos: quantos caminhos imprimir (padrao %d; 0 = todos)\n", LIMITE_PADRAO);
    fprintf(destino, "  --ajuda      mostra esta mensagem\n");
}

// Le o numero que acompanha --limite. Retorna 0 em sucesso e -1 em erro
static int ler_limite(const char *texto, size_t *limite) {
    char *fim = NULL;
    long valor;

    if (texto == NULL || *texto == '\0') {
        return -1;
    }
    errno = 0;
    valor = strtol(texto, &fim, 10);
    if (errno != 0 || *fim != '\0' || valor < 0) {
        return -1;
    }
    *limite = (size_t)valor;
    return 0;
}

// Retorna 0 se esta tudo certo, 1 se pediram ajuda e -1 se ha erro de uso
static int ler_opcoes(int argc, char **argv, Opcoes *opcoes) {
    int i;

    memset(opcoes, 0, sizeof(*opcoes));
    opcoes->limite = LIMITE_PADRAO;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ajuda") == 0) {
            return 1;
        } else if (strcmp(argv[i], "--analise") == 0) {
            opcoes->analise = 1;
        } else if (strcmp(argv[i], "--todos") == 0) {
            opcoes->todos = 1;
        } else if (strcmp(argv[i], "--globo") == 0) {
            opcoes->globo = 1;
        } else if (strcmp(argv[i], "--mapa") == 0) {
            opcoes->mapa = 1;
        } else if (strcmp(argv[i], "--limite") == 0) {
            i++;
            if (i >= argc || ler_limite(argv[i], &opcoes->limite) != 0) {
                fprintf(stderr, "Erro: --limite precisa de um numero inteiro >= 0.\n");
                return -1;
            }
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Erro: opcao desconhecida: %s\n", argv[i]);
            return -1;
        } else if (opcoes->arquivo == NULL) {
            opcoes->arquivo = argv[i];
        } else {
            fprintf(stderr, "Erro: so um arquivo de mapa pode ser informado.\n");
            return -1;
        }
    }

    if (opcoes->arquivo == NULL) {
        fprintf(stderr, "Erro: informe o arquivo do mapa.\n");
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    Opcoes opcoes;
    Labirinto *labirinto = NULL;
    ResultadoBusca resultado;
    char erro[256];
    int status, achou;
    clock_t inicio, fim;
    double segundos;

    status = ler_opcoes(argc, argv, &opcoes);
    if (status == 1) {
        mostrar_uso(stdout, argv[0]);
        return 0;
    }
    if (status < 0) {
        mostrar_uso(stderr, argv[0]);
        return 2;
    }

    if (labirinto_carregar(opcoes.arquivo, &labirinto, erro, sizeof(erro)) != 0) {
        fprintf(stderr, "Erro ao ler o mapa: %s\n", erro);
        return 2;
    }

    inicio = clock();
    achou = busca_resolver_opcoes(labirinto, opcoes.todos, opcoes.globo, &resultado);
    fim = clock();
    segundos = (double)(fim - inicio) / CLOCKS_PER_SEC;

    if (achou < 0) {
        fprintf(stderr, "Erro: falha na busca (memoria insuficiente ou mapa invalido).\n");
        labirinto_liberar(labirinto);
        return 2;
    }

    if (achou == 0) {
        saida_imprimir_sem_solucao();
    } else if (opcoes.todos) {
        saida_imprimir_todos(labirinto, &resultado, opcoes.limite, opcoes.mapa);
    } else {
        saida_imprimir_caminho(&resultado.caminhos[0]);
        if (opcoes.mapa) {
            saida_imprimir_mapa(labirinto, &resultado.caminhos[0]);
        }
    }

    if (opcoes.analise) {
        saida_imprimir_analise(labirinto, &resultado, opcoes.todos,
                               opcoes.globo, segundos);
    }

    busca_resultado_liberar(&resultado);
    labirinto_liberar(labirinto);
    return 0;
}
