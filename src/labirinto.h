#ifndef LABIRINTO_H
#define LABIRINTO_H

#include <stddef.h>

// Coordenadas indexadas a partir de zero: (0,0) e a primeira celula
typedef struct {
    size_t linha;
    size_t coluna;
} Posicao;

typedef struct {
    size_t linhas;
    size_t colunas;
    size_t quantidade_chaves;
    char **celulas;    
    Posicao inicio;
    Posicao bau;
    Posicao *chaves;  
} Labirinto;

/*
 * Retorno: 0 em sucesso; -1 em erro
 * Em sucesso *saida aponta para uma estrutura valida, que deve ser liberada
 * Em erro *saida permanece NULL (quando o ponteiro 'saida' for valido)
 */
int labirinto_carregar(const char *caminho, Labirinto **saida,
                       char *erro, size_t capacidade_erro);

// Aceita NULL. Libera toda a memoria pertencente ao labirinto
void labirinto_liberar(Labirinto *labirinto);

// 1 se coordenadas validas; 0 caso contrario
int labirinto_em_limites(const Labirinto *labirinto, int linha, int coluna);

// 1 se celula transitavel (qualquer simbolo diferente de '1'); 0 caso contrario
int labirinto_eh_transitavel(const Labirinto *labirinto, int linha, int coluna);

#endif
