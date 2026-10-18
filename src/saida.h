#ifndef SAIDA_H
#define SAIDA_H

#include <stddef.h>

#include "busca.h"
#include "labirinto.h"

void saida_imprimir_sem_solucao(void);

void saida_imprimir_caminho(const Caminho *caminho);

void saida_imprimir_mapa(const Labirinto *labirinto, const Caminho *caminho);

void saida_imprimir_todos(const Labirinto *labirinto, const ResultadoBusca *resultado, size_t limite, int mostrar_mapa);

void saida_imprimir_analise(const Labirinto *labirinto, const ResultadoBusca *resultado, int todos_caminhos, int modo_globo, double segundos);

#endif
