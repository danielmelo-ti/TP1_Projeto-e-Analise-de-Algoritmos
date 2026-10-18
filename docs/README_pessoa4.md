# Parte da Pessoa 4 — Testes, modo globo e entrega

Trabalho Prático 1 — Projeto e Análise de Algoritmos · UFV, Campus Florestal · Responsável: Pessoa 4 (nome e matrícula: preencher)

## 1. Visão geral

A Pessoa 4 é responsável por mostrar que o programa funciona: preparar os arquivos de teste (com e sem solução), conferir as saídas, registrar as medidas de recursão e documentar os resultados. Também implementou o extra do modo globo e organizou a entrega final.

| Item | Onde | Nível de detalhe |
|---|---|---|
| Mapas de teste do programa básico | `entradas/basicos/` | Detalhado |
| Mapas de teste do modo globo | `entradas/globo/` | Detalhado |
| Conferência das saídas (programa verificador) | ferramenta auxiliar, fora da entrega | Detalhado |
| Contagem independente de caminhos | ferramenta auxiliar, fora da entrega | Resumido |
| Módulo de vizinhança e modo globo | `src/globo.c`, `src/globo.h` | Detalhado |
| Resultados e análise das medidas | seções 7 e 8 | Detalhado |
| Casos de teste da leitura | `entradas/leitura/` | Resumido |
| Organização e conferência da entrega | seção 10 | Resumido |

## 2. Estratégia de testes

Cada mapa de teste foi pensado para exercitar **uma regra do enunciado ou um risco específico** da implementação. Os mapas pequenos permitem conferir o resultado à mão; os maiores servem para observar como as medidas de recursão crescem. Há mapas com solução e sem solução, já que os dois resultados precisam estar corretos.

### 2.1 O que é verificado em cada saída com solução

1. O caminho **começa em `A`**.
2. O caminho **termina em `X`**.
3. Cada passo é para uma sala **vizinha nas quatro direções** (cima, baixo, esquerda, direita), sem diagonais e sem saltos.
4. O caminho **nunca pisa em uma parede** (`1`).
5. **Nenhuma sala se repete** no caminho.
6. Ao chegar ao baú, **todas as chaves exigidas** foram coletadas.

Para os casos sem solução, a saída deve conter **exatamente** a mensagem exigida e nenhuma coordenada.

### 2.2 Como cada teste foi executado

Cada mapa foi executado no **modo análise** (`--analise`). A saída foi guardada, conferida segundo as regras acima, e as medidas (chamadas recursivas e nível máximo) foram registradas. Os mapas do modo globo foram executados com `--globo`.

## 3. Os mapas de teste e o que cada um verifica

### 3.1 Programa básico (`entradas/basicos/`)

| Mapa | Dimensão | Chaves | Esperado | O que verifica |
|---|---|---|---|---|
| 01_uma_chave | 1x5 | 1 | com solução | Caso mais simples: corredor com uma chave antes do baú |
| 02_varias_chaves | 3x5 | 3 | com solução | Várias chaves em pontos que exigem desvio; coleta de todas as chaves |
| 03_becos_sem_saida | 3x5 | 1 | com solução | Becos sem saída ao lado do caminho: exige o retorno do backtracking |
| 04_bau_inacessivel | 3x3 | 1 | sem solução | Baú cercado por paredes; a chave é alcançável, mas isso não basta |
| 05_chave_inacessivel | 3x4 | 1 | sem solução | Baú alcançável, mas a chave está isolada: não dá para coletar todas |
| 06_exige_repeticao | 3x3 | 2 | sem solução | As duas chaves ficam em lados opostos da sala central, que teria de ser pisada duas vezes: proibido |
| 07_apenas_diagonal | 3x3 | 1 | sem solução | A única ligação entre as salas é diagonal: proibido |
| 08_corredor_longo | 1x20 | 2 | com solução | Recursão profunda sem ramificação; duas chaves no corredor |
| 09_enunciado_10x10 | 10x10 | 2 | com solução | Mapa da Figura 2 do enunciado (possui 6 caminhos válidos) |
| 10_grade_aberta_sem_solucao | 5x5 | 1 | sem solução | Grade aberta com baú isolado: obriga a explorar todas as alternativas |
| 11_grade_aberta_com_solucao | 5x5 | 3 | com solução | Grade aberta com 3 chaves: muitos caminhos válidos (3757) |
| 12_minimo_sem_chaves | 1x2 | 0 | com solução | Caso mínimo: `A` ao lado de `X`, sem chaves |
| 13_serpente | 7x7 | 3 | com solução | Corredor em serpente: um único caminho longo, de 31 salas |
| 14_grade_6x6_sem_solucao | 6x6 | 1 | sem solução | Como o 10, mas maior: mostra o crescimento do custo da busca |

### 3.2 Modo globo (`entradas/globo/`)

| Mapa | Dimensão | Sem `--globo` | Com `--globo` | O que verifica |
|---|---|---|---|---|
| g01_borda_horizontal | 1x6 | sem solução | com solução | Passagem pela borda horizontal: de `A` vai à chave, depois sai pela esquerda e chega ao baú na outra ponta |
| g02_borda_vertical | 5x1 | sem solução | com solução | O mesmo, na vertical |
| g03_parede_na_borda | 1x6 | sem solução | sem solução | A volta cairia em uma parede, e paredes continuam valendo no globo |

## 4. Como as saídas foram conferidas

### 4.1 O programa verificador (ferramenta auxiliar)

Para não depender de conferir caminhos à mão, foi escrito em C um programa verificador, separado do projeto. Ele **não faz parte da entrega**, pois não é exigido pelo enunciado, e por isso não está incluído no ZIP. Sua lógica está descrita aqui porque é o que sustenta a confiança nos resultados.

**Entradas:** o arquivo do mapa, o arquivo com a saída do programa, o resultado esperado (`com_solucao` ou `sem_solucao`) e, opcionalmente, a palavra `globo`.

**Funcionamento, em ordem:**

1. **Leitura do mapa.** Lê as dimensões, a quantidade de chaves e a matriz. Recusa símbolos fora de `A`, `C`, `X`, `0`, `1`, dimensões fora de 1 a 100, e mapas que não tenham exatamente um `A`, um `X` e a quantidade de chaves declarada.
2. **Leitura da saída do programa.** Procura, no texto, trechos no formato `[linha,coluna]`. **Tudo o que está fora dos colchetes é ignorado**, e por isso o bloco do modo análise pode ficar na mesma saída sem atrapalhar. Uma coordenada malformada é falha.
3. **Caso sem solução.** A saída precisa conter uma linha igual à mensagem exigida (ignorando espaços nas pontas) e **nenhuma** coordenada.
4. **Caso com solução.** A saída não pode conter a mensagem de falta de solução e precisa ter ao menos uma coordenada. Depois, cada coordenada é conferida em ordem, com uma matriz de salas já visitadas:
   - dentro do mapa; não é parede; não foi visitada antes;
   - o primeiro passo está em `A`;
   - a partir do segundo passo, a sala anterior e a atual são vizinhas: a diferença de linhas mais a de colunas é exatamente 1. Com a opção `globo`, também são vizinhas as salas em bordas opostas (diferença igual à dimensão menos 1, na mesma linha ou coluna);
   - cada sala `C` pisada é contada como chave coletada.
5. **Verificações finais.** A última sala tem de ser `X`, e o número de chaves coletadas tem de ser igual ao exigido.

**Resultado:** o programa imprime uma mensagem (por exemplo, `APROVADO: caminho valido com 47 salas e 2 chaves.` ou `FALHA: sala repetida no passo 26.`) e devolve o código 0 (aprovado), 1 (falha na resposta) ou 2 (erro de arquivo ou de mapa).

**Uma propriedade importante:** o verificador aceita **qualquer** caminho válido, e não apenas um caminho específico. Isso é necessário porque um mapa pode ter várias soluções e a ordem das direções na busca determina qual delas aparece primeiro. Assim, os testes não dependem dessa escolha.

### 4.2 Contagem independente de caminhos (ferramenta auxiliar)

Para confirmar se cada mapa tem ou não solução, e quantos caminhos válidos possui, foi usada uma busca exaustiva escrita à parte, sem aproveitar o código da busca do programa. Os totais do programa no modo `--todos` coincidiram com essa contagem:

| Mapa | Modo normal | Modo globo |
|---|---|---|
| 02_varias_chaves | 25 | 1294 |
| 09_enunciado_10x10 | 6 | 122919 |
| 10_grade_aberta_sem_solucao | 0 | 12590 |
| 11_grade_aberta_com_solucao | 3757 | 443811 |

No mapa 12 em modo globo a contagem independente dava 2 e o programa dá 1. A diferença é intencional e explicada na seção 6.4: numa dimensão de 2 salas, a volta pela borda levaria à mesma sala já alcançada diretamente, e o programa não conta esse caminho duas vezes.

## 5. Decisões de interpretação

### 5.1 O baú encerra o caminho

A busca trata a sala `X` como o **fim** de um caminho: ao entrar nela, ou a solução está completa (todas as chaves coletadas) ou aquele ramo é abandonado. Não se perde nenhuma solução com isso. Para abrir o baú é preciso estar nele com todas as chaves, e como nenhuma sala pode ser visitada duas vezes, quem passasse pelo baú sem as chaves não poderia voltar a ele depois.

### 5.2 O exemplo de saída do enunciado

O caminho impresso como exemplo no enunciado, para o mapa 10x10 da Figura 2, **contradiz as restrições escritas** no próprio enunciado:

- repete salas: de `[2,3]` até `[2,6]` (primeiro no trecho que vai para a esquerda, depois no que volta), e também os trechos `[0,7]` a `[0,9]` e `[1,9]` a `[5,9]`;
- passa por paredes: `[1,7]` e `[8,9]` são `1` no mapa;
- não coleta a chave de `[8,0]`.

O verificador rejeita esse exemplo com a mensagem `FALHA: sala repetida no passo 26.` Foi adotada a interpretação das **regras escritas** (não repetir salas, não atravessar paredes, coletar todas as chaves). Para o mesmo mapa, o programa encontra 6 caminhos válidos; o mostrado na seção 8 tem 47 salas. Sugere-se confirmar essa interpretação com o professor.

## 6. Módulo de vizinhança e modo globo (`globo.c`, `globo.h`)

### 6.1 A ideia

No modo globo, o mapa se comporta como no jogo da cobrinha: sair pela borda direita leva à coluna 0; sair pela esquerda leva à última coluna; o mesmo vale para as linhas. A passagem só acontece se a sala do outro lado **não for parede**, e as demais regras (não repetir sala, não andar na diagonal) continuam valendo.

### 6.2 A interface

```c
#define GLOBO_NUM_DIRECOES 4

int globo_vizinho(const Labirinto *lab, int linha, int coluna, int direcao,
                  int modo_globo, int *nova_linha, int *nova_coluna);
```

| Parâmetro | Significado |
|---|---|
| `lab` | O labirinto carregado |
| `linha`, `coluna` | Posição atual |
| `direcao` | 0 = cima, 1 = direita, 2 = baixo, 3 = esquerda |
| `modo_globo` | 0 = mapa normal; 1 = modo globo |
| `nova_linha`, `nova_coluna` | Saída: posição do vizinho, preenchida apenas se ele existir |

**Retorno:** 1 se o vizinho existe e não é parede (e a posição é gravada nos dois ponteiros); 0 em qualquer outro caso, inclusive para ponteiros nulos e direção inválida.

O módulo não faz a busca: apenas responde «qual é o vizinho nesta direção?». A busca da Pessoa 2 chama essa função em vez de somar as coordenadas diretamente (integração descrita na documentação da Pessoa 3).

### 6.3 A implementação, passo a passo

```c
int globo_vizinho(const Labirinto *lab, int linha, int coluna, int direcao,
                  int modo_globo, int *nova_linha, int *nova_coluna) {
    int linhas, colunas;
    int l = linha;
    int c = coluna;

    if (lab == NULL || nova_linha == NULL || nova_coluna == NULL) {
        return 0;
    }
    linhas = (int)lab->linhas;
    colunas = (int)lab->colunas;

    if (direcao == 0) {
        l = linha - 1;
    } else if (direcao == 1) {
        c = coluna + 1;
    } else if (direcao == 2) {
        l = linha + 1;
    } else if (direcao == 3) {
        c = coluna - 1;
    } else {
        return 0;
    }

    if (modo_globo == 1 && linhas >= 3) {
        if (l < 0) {
            l = linhas - 1;
        } else if (l >= linhas) {
            l = 0;
        }
    }
    if (modo_globo == 1 && colunas >= 3) {
        if (c < 0) {
            c = colunas - 1;
        } else if (c >= colunas) {
            c = 0;
        }
    }

    if (!labirinto_eh_transitavel(lab, l, c)) {
        return 0;
    }

    *nova_linha = l;
    *nova_coluna = c;
    return 1;
}
```

**Passo 1 — andar uma casa.** Conforme a direção, soma ou subtrai 1 da linha ou da coluna. Direção fora de 0 a 3 devolve 0.

**Passo 2 — dar a volta (só no modo globo).** Se a nova linha ficou negativa, vai para a última linha; se passou da última, vai para a linha 0. O mesmo para colunas. Como o movimento é de uma casa, o valor só pode ter saído do mapa em uma unidade, e por isso um teste simples de menor que zero ou maior ou igual à dimensão basta, sem usar o operador de resto.

**Passo 3 — validar.** A função `labirinto_eh_transitavel`, da Pessoa 1, verifica ao mesmo tempo se a posição está dentro do mapa e se não é parede. No modo normal, quem saiu do mapa cai aqui e é recusado. No modo globo, quem deu a volta e caiu em uma parede também.

Complexidade: O(1) em tempo e em memória.

### 6.4 Decisões de projeto

- **A volta só vale para dimensões com 3 ou mais salas.** Com 1 sala (por exemplo, um mapa de uma linha), a volta levaria à própria sala. Com 2 salas, «para a esquerda» e «para a direita» apontariam para a mesma sala vizinha, e a busca percorreria o mesmo caminho duas vezes, duplicando as chamadas recursivas e contando o mesmo caminho duas vezes no modo `--todos`. Nesses casos, a vizinhança normal já cobre o movimento.
- **A função não verifica salas visitadas.** Isso pertence ao backtracking (que precisa desfazer a marca ao voltar), e não à geometria do mapa. Misturar os dois tornaria o módulo dependente do estado da busca.
- **Uso de `int` e não de `size_t`.** O cálculo gera valores intermediários como `-1`, que não existem em tipos sem sinal.
- **Mesma ordem de direções da busca original** (cima, direita, baixo, esquerda), para não alterar o comportamento do modo normal.
- **Reaproveitamento da função da Pessoa 1** para limites e paredes, em vez de reescrever essa verificação.

### 6.5 Exemplo

No mapa `g01_borda_horizontal` (`C A 1 1 1 X`, uma linha e seis colunas), a posição `[0,0]` tem a chave. Pedindo o vizinho à esquerda de `[0,0]`:

- sem globo: a coluna -1 está fora do mapa, e a resposta é 0;
- com globo: a coluna vira a última (5), que é o baú, e a resposta é 1 com a posição `[0,5]`.

Por isso o caminho `[0,1],[0,0],[0,5]` existe apenas no modo globo.

### 6.6 Testes do módulo

O módulo foi testado por um programa de teste em C, com 15 verificações, todas aprovadas:

| Grupo | Verificações |
|---|---|
| Vizinhança no mapa `g01` | Esquerda de `[0,1]` é `[0,0]`; sem globo, a esquerda de `[0,0]` sai do mapa; com globo, vai para `[0,5]`; direita de `[0,5]` volta para `[0,0]`; mapa de 1 linha não tem vizinho acima nem abaixo |
| Entradas inválidas | Direção inválida é recusada; labirinto nulo é recusado |
| Parede na borda (`g03`) | A volta de `[0,0]` cai na parede `[0,5]` e é recusada |
| Busca mínima sobre `g01`, `g02`, `g03` | Sem globo, nenhum tem solução; com globo, `g01` e `g02` têm exatamente 1 caminho e `g03` continua sem solução |

Além disso, o programa completo foi executado com `--globo` sobre os mapas da seção 3.2, e as saídas foram conferidas pelo verificador na modalidade `globo`.

## 7. Resultados e análise das medidas

### 7.1 Tabela de resultados (modo análise)

Todas as saídas abaixo foram aprovadas na conferência da seção 4. A coluna «Salas» é o tamanho do caminho impresso.

| Mapa | Esperado | Chamadas recursivas | Nível máximo | Salas |
|---|---|---|---|---|
| 01_uma_chave | com solução | 5 | 5 | 5 |
| 02_varias_chaves | com solução | 52 | 15 | 15 |
| 03_becos_sem_saida | com solução | 6 | 5 | 5 |
| 04_bau_inacessivel | sem solução | 3 | 2 | — |
| 05_chave_inacessivel | sem solução | 4 | 4 | — |
| 06_exige_repeticao | sem solução | 5 | 3 | — |
| 07_apenas_diagonal | sem solução | 1 | 1 | — |
| 08_corredor_longo | com solução | 20 | 20 | 20 |
| 09_enunciado_10x10 | com solução | 90 | 52 | 47 |
| 10_grade_aberta_sem_solucao | sem solução | 11894 | 21 | — |
| 11_grade_aberta_com_solucao | com solução | 70 | 25 | 25 |
| 12_minimo_sem_chaves | com solução | 2 | 2 | 2 |
| 13_serpente | com solução | 31 | 31 | 31 |
| 14_grade_6x6_sem_solucao | sem solução | 4951201 | 33 | — |

Modo globo (`--globo`):

| Mapa | Esperado | Chamadas recursivas | Nível máximo | Salas |
|---|---|---|---|---|
| g01_borda_horizontal | com solução | 3 | 3 | 3 |
| g02_borda_vertical | com solução | 3 | 3 | 3 |
| g03_parede_na_borda | sem solução | 2 | 2 | — |

### 7.2 O que as medidas mostram

- **Mapas sem ramificação** (01, 08, 12 e 13): chamadas recursivas e nível máximo são iguais ao número de salas do caminho (5, 20, 2 e 31). Não há becos, então a busca nunca volta.
- **Mapas com becos e ramificações** (02, 03 e 09): há mais chamadas do que salas no caminho, porque a busca entra em ramos que não levam à solução e volta (backtracking). No mapa 03, por exemplo, são 5 salas no caminho e 6 chamadas: a sala extra é um beco visitado e abandonado.
- **Nível máximo maior que o caminho final** (mapa 09: nível 52 para um caminho de 47 salas): o nível máximo mede a maior profundidade atingida em **qualquer** tentativa, e não só no caminho que acabou sendo a resposta. A busca chegou mais fundo por um ramo que depois se mostrou sem saída.
- **Casos sem solução pequenos** (04 a 07): poucas chamadas (de 1 a 5), porque o espaço de busca é pequeno. No mapa 07, a sala inicial não tem nenhum vizinho livre em quatro direções, e a busca termina na primeira chamada.
- **Crescimento exponencial** (mapas 10 e 14): são a mesma situação (grade aberta, uma chave, baú isolado por paredes) em dois tamanhos. O mapa 10 tem 22 salas livres e o 14 tem 34, ou seja, cerca de 1,5 vez mais salas, mas as chamadas passam de 11894 para 4951201, cerca de **416 vezes mais**. Em ambos o nível máximo (21 e 33) é igual ao número de salas alcançáveis (todas, menos o baú isolado): a busca chegou a montar um caminho passando por todas elas. Como o baú nunca é alcançado, a busca precisa esgotar **todos** os caminhos possíveis antes de concluir que não há solução. Esse comportamento é compatível com a discussão de complexidade feita pela Pessoa 2: o custo cresce de forma exponencial com o número de salas.
- **Muitas soluções, poucas chamadas** (mapa 11): a grade tem 3757 caminhos válidos, mas a busca para no primeiro e usa só 70 chamadas. O custo de achar **uma** solução depende da ordem das direções e da sorte; o custo de **provar que não há** solução é sempre o de esgotar o espaço de busca.

### 7.3 Efeito do modo globo

As bordas opostas passam a ser vizinhas, então há muito mais ligações entre as salas:

| Mapa | Caminhos válidos (normal) | Caminhos válidos (globo) | Chamadas até a 1ª solução (normal) | Chamadas até a 1ª solução (globo) |
|---|---|---|---|---|
| 02_varias_chaves | 25 | 1294 | 52 | 15 |
| 09_enunciado_10x10 | 6 | 122919 | 90 | 24 |
| 10_grade_aberta_sem_solucao | 0 | 12590 | 11894 (sem solução) | 21 |
| 11_grade_aberta_com_solucao | 3757 | 443811 | 70 | 21 |
| 12_minimo_sem_chaves | 1 | 1 | 2 | 2 |

O mapa 10 é o caso mais claro: sem solução no modo normal (o baú está cercado por paredes por cima e pela esquerda), ele passa a ter solução no globo, porque o baú fica ao lado da borda e pode ser alcançado pelo outro lado do mapa. O mapa 12 não muda: com 2 colunas a volta não se aplica (seção 6.4).

## 8. Entradas e saídas de cada teste

Para cada teste, a saída mostrada é a do modo análise; o caminho é o primeiro encontrado pela busca.

### 8.1 Programa básico

**Mapa 01_uma_chave**

```
1 5
1
A 0 C 0 X
```

Saída: `[0,0],[0,1],[0,2],[0,3],[0,4]` — 5 chamadas, nível máximo 5.

**Mapa 02_varias_chaves**

```
3 5
3
A 0 C 0 0
0 0 0 0 0
C 0 C 0 X
```

Saída: `[0,0],[0,1],[0,2],[0,3],[0,4],[1,4],[1,3],[1,2],[1,1],[1,0],[2,0],[2,1],[2,2],[2,3],[2,4]` — 52 chamadas, nível máximo 15.

**Mapa 03_becos_sem_saida**

```
3 5
1
1 1 0 1 1
A 0 0 C X
1 1 0 1 1
```

Saída: `[1,0],[1,1],[1,2],[1,3],[1,4]` — 6 chamadas, nível máximo 5.

**Mapa 04_bau_inacessivel**

```
3 3
1
A C 1
0 1 1
1 1 X
```

Saída: Não há um caminho possível para a entrada de dados fornecida — 3 chamadas, nível máximo 2.

**Mapa 05_chave_inacessivel**

```
3 4
1
A 0 0 X
1 1 1 1
C 0 0 0
```

Saída: Não há um caminho possível para a entrada de dados fornecida — 4 chamadas, nível máximo 4.

**Mapa 06_exige_repeticao**

```
3 3
2
1 C 1
A 0 X
1 C 1
```

Saída: Não há um caminho possível para a entrada de dados fornecida — 5 chamadas, nível máximo 3.

**Mapa 07_apenas_diagonal**

```
3 3
1
A 1 1
1 C 0
1 0 X
```

Saída: Não há um caminho possível para a entrada de dados fornecida — 1 chamada, nível máximo 1.

**Mapa 08_corredor_longo**

```
1 20
2
A 0 0 0 0 C 0 0 0 0 0 0 0 0 C 0 0 0 0 X
```

Saída: `[0,0],[0,1],[0,2],...,[0,18],[0,19]` (as 20 salas do corredor, em ordem) — 20 chamadas, nível máximo 20.

**Mapa 09_enunciado_10x10**

```
10 10
2
A 0 0 0 0 0 0 0 0 C
0 1 1 1 1 0 1 1 1 0
0 0 0 0 0 0 0 0 1 0
0 1 0 1 0 1 0 0 1 0
0 1 0 1 0 1 0 1 1 0
0 1 0 1 0 1 0 0 0 0
1 1 0 1 0 1 0 0 1 0
0 0 0 1 0 1 0 0 1 0
C 1 1 1 0 1 1 1 1 1
0 0 0 0 0 0 0 0 0 X
```

Saída (47 salas):

```
[0,0],[0,1],[0,2],[0,3],[0,4],[0,5],[0,6],[0,7],[0,8],[0,9],[1,9],[2,9],[3,9],[4,9],[5,9],[5,8],[5,7],[6,7],[7,7],[7,6],[6,6],[5,6],[4,6],[3,6],[2,6],[2,5],[2,4],[2,3],[2,2],[3,2],[4,2],[5,2],[6,2],[7,2],[7,1],[7,0],[8,0],[9,0],[9,1],[9,2],[9,3],[9,4],[9,5],[9,6],[9,7],[9,8],[9,9]
```

90 chamadas, nível máximo 52. O caminho pega as duas chaves (`[0,9]` e `[8,0]`) e chega ao baú sem repetir salas.

**Mapa 10_grade_aberta_sem_solucao**

```
5 5
1
A 0 0 0 0
0 0 0 0 0
0 0 C 0 1
0 0 0 0 1
0 0 0 1 X
```

Saída: Não há um caminho possível para a entrada de dados fornecida — 11894 chamadas, nível máximo 21.

**Mapa 11_grade_aberta_com_solucao**

```
5 5
3
A 0 0 0 0
0 C 0 0 0
0 0 0 C 0
0 0 0 0 0
0 C 0 0 X
```

Saída (25 salas): `[0,0],[0,1],[0,2],[0,3],[0,4],[1,4],[2,4],[3,4],[3,3],[2,3],[1,3],[1,2],[2,2],[3,2],[3,1],[2,1],[1,1],[1,0],[2,0],[3,0],[4,0],[4,1],[4,2],[4,3],[4,4]` — 70 chamadas, nível máximo 25.

**Mapa 12_minimo_sem_chaves**

```
1 2
0
A X
```

Saída: `[0,0],[0,1]` — 2 chamadas, nível máximo 2.

**Mapa 13_serpente**

```
7 7
3
A 0 0 0 0 0 C
1 1 1 1 1 1 0
C 0 0 0 0 0 0
0 1 1 1 1 1 1
0 0 0 0 0 0 C
1 1 1 1 1 1 0
X 0 0 0 0 0 0
```

Saída (31 salas): `[0,0],[0,1],[0,2],[0,3],[0,4],[0,5],[0,6],[1,6],[2,6],[2,5],[2,4],[2,3],[2,2],[2,1],[2,0],[3,0],[4,0],[4,1],[4,2],[4,3],[4,4],[4,5],[4,6],[5,6],[6,6],[6,5],[6,4],[6,3],[6,2],[6,1],[6,0]` — 31 chamadas, nível máximo 31.

**Mapa 14_grade_6x6_sem_solucao**

```
6 6
1
A 0 0 0 0 0
0 0 0 0 0 0
0 0 C 0 0 0
0 0 0 0 0 0
0 0 0 0 0 1
0 0 0 0 1 X
```

Saída: Não há um caminho possível para a entrada de dados fornecida — 4951201 chamadas, nível máximo 33.

### 8.2 Modo globo (executados com `--globo`)

**Mapa g01_borda_horizontal**

```
1 6
1
C A 1 1 1 X
```

Saída: `[0,1],[0,0],[0,5]` — 3 chamadas, nível máximo 3. Sem `--globo`, o resultado é a mensagem de que não há caminho.

**Mapa g02_borda_vertical**

```
5 1
1
C
A
1
1
X
```

Saída: `[1,0],[0,0],[4,0]` — 3 chamadas, nível máximo 3.

**Mapa g03_parede_na_borda**

```
1 6
1
C A 1 1 X 1
```

Saída: Não há um caminho possível para a entrada de dados fornecida — 2 chamadas, nível máximo 2. Mesmo no modo globo, a volta cairia na parede da última coluna.

## 9. Casos de teste da leitura

A leitura é da Pessoa 1. Os arquivos em `entradas/leitura/` foram executados no programa completo, e o resultado observado foi o seguinte:

| Arquivo | Esperado | Resultado observado |
|---|---|---|
| exemplo_enunciado | Mapa válido | Caminho encontrado |
| gerado_com_solucao | Mapa válido | Caminho encontrado |
| sem_chaves | Mapa válido | Caminho `[0,0],[0,1],[1,1]` |
| gerado_aleatorio | Mapa válido | Mensagem de falta de solução |
| sem_solucao | Mapa válido | Mensagem de falta de solução |
| valido_espacos | Mapa válido (formatação com espaços variados) | Mensagem de falta de solução |
| colunas_erradas | Erro | Linha 0 tem mais de 3 colunas (código 2) |
| dimensao_invalida | Erro | Primeira linha inválida: esperadas duas dimensões positivas (código 2) |
| duas_entradas | Erro | Mapa inválido: A=2 (esperado 1) (código 2) |
| falta_bau | Erro | Mapa inválido: X=0 (esperado 1) (código 2) |
| falta_chave | Erro | Mapa inválido: C=0 (esperado 1) (código 2) |
| linha_extra | Erro | Conteúdo extra após as linhas do mapa (código 2) |
| mapa_incompleto | Erro | Mapa incompleto: faltou a linha 2 (código 2) |
| simbolo_invalido | Erro | Símbolo inválido Z na posição [0,2] (código 2) |

Os mapas inválidos são recusados com mensagem clara e código de saída 2, sem a busca chegar a ser executada.

## 10. Organização da entrega

### 10.1 Estrutura do projeto entregue

```
TP1-PAA/
├── Makefile
├── README.md
├── relatorio.pdf
├── src/
│   ├── main.c
│   ├── labirinto.c / labirinto.h
│   ├── busca.c / busca.h
│   ├── globo.c / globo.h
│   ├── saida.c / saida.h
│   └── gerador_mapas.c   (extra, programa independente)
└── entradas/
    ├── basicos/   (mapas 01 a 14)
    ├── globo/     (mapas g01 a g03)
    └── leitura/   (casos de teste da leitura)
```

A estrutura contém apenas o que o enunciado exige: o código-fonte em C, o Makefile, o PDF com a documentação e os arquivos de entrada usados nos testes.

### 10.2 O que ficou fora da entrega e por quê

- **Programa verificador e programa de teste do modo globo:** são ferramentas de apoio aos testes, não exigidas pelo enunciado. Os resultados que elas produziram estão registrados neste documento.
- **Contagem independente de caminhos:** também uma ferramenta de apoio.
- **Scripts de teste, rascunhos de documentação e executáveis compilados (pasta `bin`):** são gerados ou de uso interno.

### 10.3 Conferências da entrega

Já conferido:

- o projeto, em uma pasta limpa (sem a pasta `bin`), compila com um único `make` e sem avisos;
- o executável roda nos modos normal e análise e com os extras.

A conferir antes do envio:

- o PDF único `relatorio.pdf`, na raiz do projeto, com os nomes e as matrículas dos quatro integrantes na capa ou no cabeçalho, e a confirmação de que ele abre corretamente;
- as regras do documento Diretrizes para relatórios de documentação, a planilha de grupos e a data de entrega no PVANet;
- o arquivo ZIP com uma única pasta na raiz;
- que apenas um integrante do grupo faz o envio.
