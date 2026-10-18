# Parte da Pessoa 3 — Integração, execução e saída

Trabalho Prático 1 — Projeto e Análise de Algoritmos · UFV, Campus Florestal · Responsável: Pessoa 3 (nome e matrícula: preencher)

## 1. Visão geral

A Pessoa 3 é responsável por transformar os módulos individuais (leitura, busca e vizinhança) em um programa completo e utilizável. Isso inclui o programa principal, a escolha entre modo normal e modo análise, o Makefile, a impressão do resultado, a disponibilização dos extras como opções e os testes de integração.

| Item | Arquivo | Nível de detalhe nesta documentação |
|---|---|---|
| Programa principal e leitura das opções | `src/main.c` | Detalhado |
| Impressão do caminho, do mapa e do modo análise | `src/saida.c`, `src/saida.h` | Detalhado |
| Modo análise (definição e contagem das medidas) | `src/main.c`, `src/saida.c` | Detalhado |
| Integração do modo globo na busca | `src/busca.c`, `src/busca.h` | Detalhado |
| Compilação | `Makefile` | Detalhado |
| Execução normal, extras como opções | `src/main.c` | Resumido |
| Testes de integração | — | Resumido |

O funcionamento básico exigido no enunciado não foi alterado por nenhum extra: sem opções, o programa lê o mapa, executa o backtracking e imprime o caminho ou a mensagem de que não há solução.

## 2. Modularização

### 2.1 Módulos e responsabilidades

| Módulo | Arquivos | Responsabilidade | Autor |
|---|---|---|---|
| Leitura e estruturas | `labirinto.h`, `labirinto.c` | Ler o arquivo, validar o mapa, guardar o labirinto e liberar a memória | Pessoa 1 |
| Busca | `busca.h`, `busca.c` | Backtracking, contadores de recursão e extra de todos os caminhos | Pessoa 2 |
| Vizinhança | `globo.h`, `globo.c` | Dizer qual é a sala vizinha em uma direção, inclusive no modo globo | Pessoa 4 |
| Saída | `saida.h`, `saida.c` | Imprimir o caminho, a mensagem, o mapa desenhado e o modo análise | Pessoa 3 |
| Principal | `main.c` | Ler as opções e ligar os módulos na ordem certa | Pessoa 3 |

### 2.2 Dependências entre os módulos

```
main.c ──► labirinto.h / labirinto.c
  │   └──► busca.h / busca.c ──► globo.h / globo.c ──► labirinto.h
  └──────► saida.h / saida.c ──► busca.h (tipos Caminho e ResultadoBusca), labirinto.h
```

A seta indica «usa». Nenhum módulo usa o `main.c`, e a busca não depende do módulo de saída.

### 2.3 Por que a divisão é assim

- **Cada módulo tem uma única responsabilidade.** A busca não imprime nada; ela só devolve um resultado. Assim ela pode ser testada isoladamente e o formato da saída pode mudar sem tocar no algoritmo.
- **O arquivo `.h` é a interface.** Quem usa um módulo só precisa conhecer o que está no cabeçalho (tipos e protótipos), não a implementação.
- **O `main.c` só liga as peças.** Ele não contém lógica de busca nem de formatação.
- **Programas com `main` próprio ficam fora do executável principal.** O gerador de mapas é um programa independente; por isso o Makefile lista os arquivos do executável principal um a um, em vez de compilar todos os `.c` de uma pasta (o que causaria duas funções `main`).

## 3. Programa principal (`main.c`)

### 3.1 A estrutura `Opcoes`

Todas as escolhas do usuário na linha de comando ficam reunidas em uma única estrutura, que é preenchida uma vez e depois só consultada:

| Campo | Opção correspondente | Significado |
|---|---|---|
| `arquivo` | primeiro argumento sem `--` | Caminho do arquivo do mapa |
| `analise` | `--analise` | Liga o modo análise |
| `todos` | `--todos` | Extra: todos os caminhos |
| `globo` | `--globo` | Extra: modo globo |
| `mapa` | `--mapa` | Extra: desenhar o mapa |
| `limite` | `--limite N` | Quantos caminhos imprimir com `--todos` (padrão 10) |

Reunir tudo em uma estrutura evita variáveis soltas e permite passar as escolhas a outras funções por um único ponteiro.

### 3.2 A função `ler_limite` (conversão segura de texto em número)

O valor de `--limite` chega como texto (`argv`). Foi usada a função `strtol`, e não `atoi`, porque `strtol` permite detectar entradas inválidas. A função aplica quatro verificações:

1. **Texto nulo ou vazio** é rejeitado.
2. **Estouro:** antes da chamada, `errno` é zerado; se depois dela `errno` for diferente de zero, o número não cabe em um `long`.
3. **Texto sobrando:** `strtol` devolve em um ponteiro (`fim`) o ponto onde parou a leitura. Se `*fim` não for o fim da string, havia caracteres inválidos. Assim, `12abc` é rejeitado, e `abc` também (nenhum dígito é lido, então `fim` continua apontando para o início).
4. **Valor negativo** é rejeitado, já que o limite é uma quantidade.

Só depois dessas verificações o valor é convertido para `size_t`. A função devolve 0 em sucesso e -1 em erro.

### 3.3 A função `ler_opcoes`

Percorre os argumentos da linha de comando, do segundo em diante, e devolve um de três valores:

| Retorno | Situação | O que o `main` faz |
|---|---|---|
| 0 | Opções válidas | Continua a execução |
| 1 | O usuário pediu `--ajuda` | Mostra o uso na saída padrão e termina com código 0 |
| -1 | Erro de uso | Mostra a mensagem de erro e o uso na saída de erro e termina com código 2 |

Regras aplicadas a cada argumento:

- `--ajuda` encerra imediatamente a leitura (retorno 1), mesmo que haja outras opções.
- `--analise`, `--todos`, `--globo` e `--mapa` apenas ligam o campo correspondente.
- `--limite` consome também o argumento seguinte (o contador `i` é avançado), que é convertido por `ler_limite`. Se faltar o valor ou ele for inválido, é erro.
- Qualquer outro argumento que comece com `-` é uma opção desconhecida (erro).
- O primeiro argumento que não começa com `-` é o arquivo do mapa; um segundo é erro.
- Ao final, se nenhum arquivo foi informado, é erro.

A estrutura é zerada com `memset` antes da leitura, e `limite` recebe o valor padrão (`LIMITE_PADRAO`, igual a 10).

### 3.4 A função `mostrar_uso`

Recebe o destino da mensagem (`FILE *`). Com `--ajuda` o texto vai para a saída padrão (o usuário pediu a informação); em caso de erro vai para a saída de erro, para não se misturar com o resultado do programa.

### 3.5 O fluxo da função `main`, passo a passo

1. **Ler as opções** (`ler_opcoes`). Se for ajuda, mostra e termina com 0; se for erro, termina com 2.
2. **Carregar o mapa** (`labirinto_carregar`). Em falha, imprime `Erro ao ler o mapa:` seguido da mensagem da Pessoa 1 na saída de erro e termina com 2.
3. **Executar a busca** (`busca_resolver_opcoes`), cronometrada por `clock()` antes e depois.
4. **Verificar falha da busca.** Retorno negativo significa argumento inválido ou falta de memória; o programa libera o labirinto, avisa e termina com 2.
5. **Imprimir o resultado**, em três casos: sem solução (mensagem exigida); com `--todos` (total e caminhos); caso básico (uma linha de coordenadas, e o mapa desenhado se houver `--mapa`).
6. **Imprimir o modo análise**, se foi pedido. Isso vale também quando não há solução.
7. **Liberar a memória** do resultado e do labirinto.

Códigos de saída:

| Código | Significado |
|---|---|
| 0 | O programa executou. Não existir caminho **não** é erro do programa, é uma resposta válida. |
| 2 | Erro de uso, de leitura do arquivo ou de memória. |

O resultado vai para a **saída padrão** e as mensagens de erro para a **saída de erro**. Assim é possível redirecionar apenas a resposta (`> saida.txt`) sem misturar mensagens de erro nela.

### 3.6 Medição do tempo

O tempo é medido com `clock()`, que devolve o tempo de processador em «ticks»; a diferença entre o fim e o início, dividida por `CLOCKS_PER_SEC`, dá segundos. A medição cobre **apenas a busca** (não inclui leitura do arquivo nem impressão). Como depende da máquina e da carga do sistema, o valor muda entre execuções, e por isso ele não é usado como resultado fixo nos testes. Ele serve para comparar a ordem de grandeza entre mapas.

## 4. Módulo de saída (`saida.c`)

### 4.1 Formato do caminho e mensagem de falta de solução (parte simples)

`saida_imprimir_caminho` percorre as posições do caminho imprimindo `[linha,coluna]`, com vírgula entre elas e uma quebra de linha ao final, exatamente como no exemplo do enunciado. `saida_imprimir_sem_solucao` imprime a mensagem exigida: Não há um caminho possível para a entrada de dados fornecida.

### 4.2 Impressão criativa do mapa (`saida_imprimir_mapa`)

Desenha o labirinto e marca o caminho com setas. Cada seta aponta o **próximo** movimento a partir daquela sala. A função tem três etapas.

**Etapa 1: copiar o mapa para uma grade de caracteres.** A grade é um vetor único de `linhas × colunas` caracteres, alocado com `malloc`; a posição `(i, j)` fica no índice `i * colunas + j`. Ao copiar, `0` vira `.` (sala livre) e `1` vira `#` (parede); `A`, `C` e `X` são mantidos. Se a alocação falhar, a função avisa e retorna, sem derrubar o programa.

**Etapa 2: marcar o caminho.** Para cada posição do caminho, exceto a última (o baú), a função calcula a seta em direção à posição seguinte e a grava na grade, **somente se a célula for `.`**. Assim o início, as chaves e o baú continuam visíveis.

**Etapa 3: imprimir** a grade com um espaço entre as colunas (para o desenho ficar mais quadrado) e a legenda.

**O cálculo da seta (função `seta`).** Recebe duas posições consecutivas, `a` e `b`. As coordenadas são `size_t` (sem sinal); subtrair duas delas pode «dar a volta» e gerar um número enorme. Por isso as diferenças são calculadas depois de converter as coordenadas para `long`:

- `dl = b.linha - a.linha` e `dc = b.coluna - a.coluna`.
- Se `dl` é diferente de zero, o movimento é vertical: `v` quando `dl == 1` ou `dl < -1`; caso contrário, `^`.
- Se `dl` é zero, o movimento é horizontal: `>` quando `dc == 1` ou `dc < -1`; caso contrário, `<`.

A condição `dl < -1` trata a passagem pela borda do modo globo. Ao sair pela última linha e chegar à linha 0, a diferença é negativa e grande (`-(linhas-1)`), e o movimento foi para baixo. Ao sair da linha 0 e chegar à última linha, a diferença é positiva e grande, e o movimento foi para cima. O mesmo raciocínio vale para as colunas. Não há ambiguidade com movimentos comuns, porque a volta só ocorre em dimensões com 3 ou mais salas (ver o módulo de vizinhança), e nesse caso a diferença tem módulo maior ou igual a 2.

Complexidade: O(L + R·C) em tempo (L é o tamanho do caminho; R e C são as dimensões do mapa) e O(R·C) de memória adicional.

Exemplo de saída, para o mapa 10x10 do enunciado:

```
A > > > > > > > > C
. # # # # . # # # v
. . v < < < < . # v
. # v # . # ^ . # v
. # v # . # ^ # # v
. # v # . # ^ v < <
# # v # . # ^ v # .
v < < # . # ^ < # .
C # # # . # # # # #
> > > > > > > > > X
Legenda: A inicio | C chave | X bau | # parede | . sala livre | ^ > v < caminho
```

### 4.3 Todos os caminhos (`saida_imprimir_todos`)

Imprime `Total de caminhos validos: N` e, em seguida, os primeiros caminhos, cada um com um cabeçalho `Caminho k (T salas):`. A quantidade impressa é o menor valor entre o limite e o total; com limite 0, imprime todos. Se houve corte, uma linha final informa quantos foram mostrados e como mostrar todos. Com `--mapa`, cada caminho também é desenhado. **O limite controla apenas a impressão**: a busca sempre encontra e guarda todos os caminhos.

### 4.4 Modo análise (`saida_imprimir_analise`)

Imprime o bloco descrito na seção 5. É chamada depois do resultado e só quando `--analise` foi pedido.

## 5. Modo análise

### 5.1 Como é ativado

O modo análise é uma **opção de execução** (`--analise`), e não uma compilação diferente. O mesmo executável serve para os dois modos, o que evita manter duas versões do programa e garante que a medida seja feita sobre o mesmo código que roda no modo normal.

### 5.2 O que é impresso

```
=== MODO ANALISE ===
Mapa: 10x10, 2 chave(s)
Modo globo: nao | Todos os caminhos: nao
Chamadas recursivas: 90
Nivel maximo de recursao: 52
Caminhos encontrados: 1
Tempo de busca: 0.000036 s
```

| Medida | Definição |
|---|---|
| Chamadas recursivas | Quantas vezes a função recursiva `explorar` foi chamada. Cada chamada corresponde a **entrar em uma sala**, incluindo a sala inicial. Paredes e salas já visitadas no caminho atual são descartadas antes da chamada, então não contam. |
| Nível máximo de recursão | A chamada inicial está no nível 1, e cada movimento para uma sala nova aumenta o nível em 1. O máximo é a maior profundidade atingida durante toda a busca. |
| Caminhos encontrados | 1 no modo básico (a busca para na primeira solução); no modo `--todos`, o total de caminhos válidos. |
| Tempo de busca | Tempo de processador gasto na busca (seção 3.6). |

Os contadores são mantidos pela busca da Pessoa 2; o modo análise apenas os apresenta.

### 5.3 Exemplo de contagem, passo a passo

Mapa `03_becos_sem_saida`:

```
1 1 0 1 1
A 0 0 C X
1 1 0 1 1
```

A busca tenta as direções na ordem cima, direita, baixo, esquerda:

1. Entra em `A` [1,0] (chamada 1, nível 1).
2. Vai para a direita, [1,1] (chamada 2, nível 2) e depois [1,2] (chamada 3, nível 3).
3. Em [1,2], a primeira direção é cima: entra em [0,2] (chamada 4, nível 4). Ali há uma parede à direita, parede à esquerda e nada acima: é um beco sem saída, e a busca **volta** (backtracking).
4. De volta a [1,2], tenta a direita: [1,3], que tem a chave (chamada 5, nível 4), e depois [1,4], o baú (chamada 6, nível 5), com a chave coletada. Solução encontrada, e a busca para.

Resultado: 6 chamadas e nível máximo 5. As 5 salas do caminho final mais a sala do beco explicam as 6 chamadas. A sala de baixo, [2,2], nem chegou a ser explorada, porque a busca parou antes.

## 6. Integração do modo globo na busca (`busca.c` e `busca.h`)

O modo globo precisava funcionar usando a busca da Pessoa 2, sem duplicar a recursão. Para isso foi feita uma alteração pequena e controlada.

### 6.1 O que mudou

Antes, a função recursiva calculava os vizinhos com um vetor de deslocamentos e verificava os limites do mapa à mão. Agora, ela pergunta ao módulo de vizinhança:

```c
for (int direcao = 0;
     direcao < GLOBO_NUM_DIRECOES && !estado->parar && !estado->erro;
     ++direcao) {
    int nova_linha, nova_coluna;

    if (!globo_vizinho(lab, (int)linha, (int)coluna, direcao,
                       estado->modo_globo, &nova_linha, &nova_coluna)) {
        continue;
    }
    if (estado->visitadas[indice(lab, (size_t)nova_linha, (size_t)nova_coluna)]) {
        continue;
    }
    explorar(estado, (size_t)nova_linha, (size_t)nova_coluna, chaves,
             tamanho, nivel + 1);
}
```

### 6.2 Divisão de responsabilidades

- `globo_vizinho` cuida da **geometria**: limites do mapa, paredes e (no modo globo) passagem entre bordas.
- A busca cuida do **backtracking**: salas já visitadas no caminho atual, chaves, contadores e desfazer escolhas ao retornar.

### 6.3 Por que há conversões entre `size_t` e `int`

As posições do labirinto são `size_t` (sem sinal). Já `globo_vizinho` trabalha com `int`, porque calcula posições intermediárias que podem ser `-1` (ao sair pela borda superior ou esquerda). As conversões são feitas na fronteira entre os dois módulos: de `size_t` para `int` ao chamar a função e de volta para `size_t` ao indexar a matriz.

### 6.4 A interface antiga foi preservada

Foi criada a função `busca_resolver_opcoes(labirinto, todos_caminhos, modo_globo, resultado)`. A função original `busca_resolver(labirinto, todos_caminhos, resultado)` continua existindo com a mesma assinatura e apenas chama a nova com o globo desligado. O novo campo `modo_globo` da estrutura interna `EstadoBusca` é validado (só 0 ou 1).

### 6.5 O comportamento básico não mudou

A ordem das direções (cima, direita, baixo, esquerda) é a mesma de antes. Por isso, no modo normal, a busca visita as salas na mesma ordem e os contadores permanecem idênticos: o mapa 09 resultou em 90 chamadas e nível máximo 52 antes e depois da alteração.

## 7. Makefile

```make
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -Isrc

FONTES = src/main.c src/saida.c src/busca.c src/globo.c src/labirinto.c
CABECALHOS = $(wildcard src/*.h)

all: bin/labirinto

bin/labirinto: $(FONTES) $(CABECALHOS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(FONTES) -o $@

# Extra: gerador de mapas (programa independente)
gerador: bin/gerador_mapas

bin/gerador_mapas: src/gerador_mapas.c
	@mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf bin

.PHONY: all gerador clean
```

| Linha | Explicação |
|---|---|
| `CC = gcc` | Compilador usado. |
| `-std=c11` | Compila na versão C11 da linguagem. |
| `-Wall -Wextra -Wpedantic` | Ativam avisos do compilador para detectar erros comuns e usos fora do padrão. |
| `-O2` | Otimização moderada. |
| `-Isrc` | Indica que os arquivos `.h` estão em `src/`. |
| `FONTES` | Lista explícita dos `.c` do executável principal. É explícita de propósito, para não incluir arquivos com `main` próprio. |
| `CABECALHOS = $(wildcard src/*.h)` | Reúne todos os `.h`. São dependências: se um cabeçalho mudar, o programa é recompilado. |
| `all` | Alvo padrão, executado ao digitar apenas `make`. |
| `bin/labirinto: $(FONTES) $(CABECALHOS)` | O executável depende dos fontes e dos cabeçalhos; só é refeito se algum deles for alterado. |
| `@mkdir -p bin` | Cria a pasta `bin` se não existir. O `@` evita mostrar o comando. |
| `$(CC) $(CFLAGS) $(FONTES) -o $@` | Compila tudo de uma vez; `$@` é o nome do alvo (`bin/labirinto`). |
| `gerador` | Compila o programa independente `gerador_mapas.c` (extra da Pessoa 1) em `bin/gerador_mapas`. |
| `clean` | Apaga a pasta `bin`. |
| `.PHONY` | Declara que `all`, `gerador` e `clean` são comandos, não arquivos. |

O Makefile foi pensado para que o projeto entregue compile com um único `make`, sem bibliotecas externas.

## 8. Como compilar e executar

### 8.1 Compilação

```bash
make            # gera bin/labirinto
make gerador    # extra: gera bin/gerador_mapas
make clean      # apaga a pasta bin/
```

Sem o `make`, o executável principal pode ser gerado diretamente:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Isrc \
    src/main.c src/saida.c src/busca.c src/globo.c src/labirinto.c -o bin/labirinto
```

### 8.2 Execução

```bash
./bin/labirinto ARQUIVO_DO_MAPA                  # modo normal
./bin/labirinto ARQUIVO_DO_MAPA --analise        # modo análise
```

| Opção | Efeito |
|---|---|
| `--analise` | Modo análise: chamadas recursivas, nível máximo, caminhos encontrados e tempo |
| `--todos` | Extra: busca e relata todos os caminhos válidos |
| `--globo` | Extra: as bordas opostas do mapa são vizinhas |
| `--mapa` | Extra: desenha o labirinto com o caminho destacado |
| `--limite N` | Com `--todos`: quantos caminhos imprimir (padrão 10; 0 imprime todos) |
| `--ajuda` | Mostra as opções |

As opções podem ser combinadas em qualquer ordem. Exemplo de saída no modo normal (mapa 01, uma linha com cinco salas):

```
[0,0],[0,1],[0,2],[0,3],[0,4]
```

E no modo análise, a mesma linha seguida do bloco de medidas da seção 5.2.

## 9. Extras sob responsabilidade da Pessoa 3

Todos são opções de linha de comando e, sem elas, o programa se comporta como o enunciado exige.

- **`--todos`** (relatar todos os caminhos): usa a opção `todos_caminhos` da busca da Pessoa 2; a Pessoa 3 implementou a apresentação (seção 4.3).
- **`--mapa`** (impressão criativa): seção 4.2.
- **`--limite`**: controla quantos caminhos aparecem na saída do `--todos`.
- **`--globo`**: a vizinhança é da Pessoa 4; a integração na busca está na seção 6.

Nenhum extra usou outra linguagem de programação: todo o projeto foi feito em C, e portanto não há o que justificar neste ponto.

## 10. Testes de integração realizados

- **Compilação em pasta limpa:** o projeto foi copiado para uma pasta sem a pasta `bin`, e `make` compilou sem avisos nem erros.
- **Mapas de teste:** os 14 mapas do programa básico e os 3 do modo globo foram executados no modo análise e as saídas foram conferidas (documentação da Pessoa 4).
- **Extras:** o total de caminhos do `--todos` foi comparado com uma contagem independente (25, 6 e 3757 nos mapas 02, 09 e 11), e o do `--globo` também.
- **Memória:** o programa foi compilado com `-fsanitize=address,undefined` e executado sobre os mapas, com e sem opções, sem nenhum erro de memória ou comportamento indefinido.
- **Erros de uso:** sem arquivo, arquivo inexistente, opção desconhecida, `--limite` sem valor ou com valor inválido e dois arquivos de mapa. Em todos o programa mostra uma mensagem e termina com o código 2.
- **Gerador:** um mapa criado pelo gerador de mapas foi lido e resolvido normalmente pelo programa principal.

## 11. Limitações e observações

- No modo `--todos`, todos os caminhos ficam guardados em memória; em mapas grandes e abertos o número de caminhos cresce muito, e a memória pode ser insuficiente. O `--limite` não reduz esse consumo, pois afeta só a impressão.
- A recursão pode atingir profundidade igual ao número de salas. Em mapas muito grandes, isso pode esgotar a pilha do sistema.
- O programa não impõe limite de tamanho ao mapa; a validação do formato é feita pela leitura (Pessoa 1).
- O tempo medido varia entre execuções e máquinas; as demais medidas (chamadas e nível) são determinísticas.
