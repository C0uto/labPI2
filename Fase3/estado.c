#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "estado.h"

/**
 * Funcao para criar e preencher um baralho sequencialmente
 *
 * * @param n_baralhos -> Numero de baralhos de 52 cartas
 * * @return res -> Array de cartas alocado
 */
BARALHO criarBaralho(int n_baralhos) {
    int total = 52 * n_baralhos;
    BARALHO b = malloc(sizeof(CARTA) * total);
    for (int i = 0; i < total; i++) b[i] = (i % 52) + 1;
    return b;
}

/**
 * Funcao para baralhar aleatoriamente as cartas do deck
 *
 * * @param b -> O baralho
 * * @param n_baralhos -> Quantidade de baralhos usados
 */
void baralharBaralho(BARALHO b, int n_baralhos) {
    int total = 52 * n_baralhos;
    srand((unsigned)time(NULL));
    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        CARTA t = b[i]; b[i] = b[j]; b[j] = t;
    }
}

/**
 * Funcao para contar o numero de blocos INIT definidos no ficheiro
 *
 * * @param ri -> Lista de regras INIT
 * * @return res -> Numero de pilhas a criar inicialmente
 */
int contarInits(RegrasInit ri) {
    int n = 0;
    while (ri) { n++; ri = ri->prox; }
    return n;
}

/**
 * Funcao para navegar ate ao final da lista duplamente ligada de INIT
 *
 * * @param ri -> Lista
 * * @return res -> Ponteiro para o ultimo elemento
 */
RegrasInit ultimoInit(RegrasInit ri) {
    if (!ri) return NULL;
    while (ri->prox) ri = ri->prox;
    return ri;
}

/**
 * Funcao para alocar a memoria necessaria para as pilhas do jogo
 *
 * * @param j -> Estado do jogo
 * * @param n -> Numero de pilhas
 */
void inicializarPilhas(ESTADO *j, int n) {
    j->pilhas = calloc(n, sizeof(PILHA));
    j->num_pilhas = n;
}

/**
 * Funcao para preencher uma pilha individual com cartas do baralho baralhado
 *
 * * @param p -> Ponteiro para a pilha
 * * @param tipo -> String com o tipo da pilha
 * * @param deck -> O baralho
 * * @param idx -> Ponteiro para o indice atual do baralho
 * * @param n_cartas -> Numero de cartas a distribuir nesta pilha
 */
void preencherPilha(PILHA *p, const char *tipo, BARALHO deck, int *idx, int n_cartas) {
    p->tipo = strdup(tipo);
    for (int i = 0; i < n_cartas; i++) pushCarta(p, deck[(*idx)++]);
}

/**
 * Funcao que aplica todas as regras de inicializacao ao estado do jogo
 *
 * * @param j -> Estado do jogo
 * * @param ri -> Lista de regras INIT
 * * @param deck -> Baralho baralhado
 */
void aplicarInitAoEstado(ESTADO *j, RegrasInit ri, BARALHO deck) {
    int n = contarInits(ri);
    if (n == 0 || deck == NULL) return;
    inicializarPilhas(j, n);
    RegrasInit cur = ultimoInit(ri);
    int idx = 0, col = 0;
    while (cur && col < n) {
        if (cur->tipoDePilha) preencherPilha(&j->pilhas[col++], cur->tipoDePilha, deck, &idx, cur->numeroDeCartas);
        cur = cur->ant;
    }
}

/**
 * Funcao para guardar uma "fotografia" do estado atual das cartas para permitir undo
 *
 * * @param j -> Estado do jogo
 */
void guardarSnapshot(ESTADO *j) {
    SNAPSHOT *s = malloc(sizeof(SNAPSHOT));
    s->num_pilhas = j->num_pilhas;
    s->cartas = malloc(j->num_pilhas * sizeof(CARTA *));
    s->tamanhos = malloc(j->num_pilhas * sizeof(int));
    for (int i = 0; i < j->num_pilhas; i++) {
        s->tamanhos[i] = j->pilhas[i].tamanho;
        s->cartas[i] = malloc(j->pilhas[i].tamanho * sizeof(CARTA));
        memcpy(s->cartas[i], j->pilhas[i].cartas, j->pilhas[i].tamanho * sizeof(CARTA));
    }
    s->prox = j->historico; j->historico = s;
}

/**
 * Funcao para libertar a memoria de um snapshot individual
 *
 * * @param s -> O snapshot a apagar
 */
void libertarSnapshot(SNAPSHOT *s) {
    for (int i = 0; i < s->num_pilhas; i++) free(s->cartas[i]);
    free(s->cartas); free(s->tamanhos); free(s);
}

/**
 * Funcao para reverter o estado do jogo para o ultimo snapshot guardado
 *
 * * @param j -> Estado do jogo
 * * @return res -> 1 se reverteu, 0 se nao havia historico
 */
int restaurarSnapshot(ESTADO *j) {
    if (!j->historico) return 0;
    SNAPSHOT *s = j->historico; j->historico = s->prox;
    for (int i = 0; i < j->num_pilhas; i++) {
        j->pilhas[i].tamanho = s->tamanhos[i];
        memcpy(j->pilhas[i].cartas, s->cartas[i], s->tamanhos[i] * sizeof(CARTA));
    }
    libertarSnapshot(s); return 1;
}

/**
 * Funcao para guardar o estado atual do jogo num ficheiro chamado "save.txt"
 *
 * * @param j -> Estado do jogo
 * * @param nome_paciencia -> Nome da paciencia atual para guardar no topo do ficheiro
 */
void gravarJogo(ESTADO *j, const char *nome_paciencia) {
    FILE *f = fopen("save.txt", "w");
    if (!f) return;
    fprintf(f, "%s.txt\n", nome_paciencia);
    for (int i = 0; i < j->num_pilhas; i++) {
        for (int k = 0; k < j->pilhas[i].tamanho; k++) {
            char buf[4]; card2str(j->pilhas[i].cartas[k], buf);
            fprintf(f, "%s ", buf);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

/**
 * Funcao principal para carregar o jogo a partir de "save.txt"
 *
 * * @param j -> Estado do jogo
 * * @return res -> 1 se carregou com sucesso, 0 caso contrario
 */
int carregarJogo(ESTADO *j) {
    FILE *f = fopen("save.txt", "r");
    if (!f) return 0;
    char b[512], token[8];
    fgets(b, 512, f);
    for (int i = 0; i < j->num_pilhas; i++) {
        j->pilhas[i].tamanho = 0;
        if (fgets(b, 512, f)) {
            int pos = 0, delta = 0;
            while (sscanf(b + pos, "%7s%n", token, &delta) == 1) {
                pos += delta; CARTA c = str2card(token);
                if (c > 0) pushCarta(&j->pilhas[i], c);
            }
        }
    }
    fclose(f); return 1;
}

/**
 * Funcao que liberta toda a memoria alocada para o estado do jogo e historico
 *
 * * @param j -> Estado do jogo
 */
void limparEstado(ESTADO *j) {
    for (int i = 0; i < j->num_pilhas; i++) { free(j->pilhas[i].cartas); free(j->pilhas[i].tipo); }
    free(j->pilhas); free(j->B);
    while (j->historico) { SNAPSHOT *s = j->historico; j->historico = s->prox; libertarSnapshot(s); }
}