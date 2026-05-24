#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "carta.h"
#include "estado.h"


/**
 * Funcao para escrever o conteúdo de uma pilha num ficheiro de save
 *
 * * @param f -> Ponteiro do ficheiro aberto
 * * @param p -> Ponteiro da pilha
 */
void gravarPilha(FILE *f, PILHA *p) {
    for (int k = 0; k < p->tamanho; k++) {
        char buf[4];
        card2str(p->cartas[k], buf);
        fprintf(f, "%s ", buf);
    }
    fprintf(f, "\n");
}

/**
 * Funcao para guardar o estado atual do jogo num ficheiro chamado "save.txt"
 *
 * * @param j -> Estado do jogo
 * * @param nome_paciencia -> Nome da paciencia atual para guardar no topo do ficheiro
 */
void gravarJogo(ESTADO *j, const char *nome_paciencia) {
    FILE *f = fopen("save.txt", "w");
    if (!f) { printf("Erro ao gravar.\n"); return; }
    fprintf(f, "%s.txt\n", nome_paciencia);
    for (int i = 0; i < j->num_pilhas; i++)
        gravarPilha(f, &j->pilhas[i]);
    fclose(f);
    printf("Jogo gravado em save.txt\n");
}

/**
 * Funcao para carregar as cartas de uma linha de texto para uma estrutura de pilha
 *
 * * @param p -> Ponteiro da pilha de destino
 * * @param linha -> String com as cartas (ex: "AH 2S 3D")
 */
void carregarLinhaPilha(PILHA *p, char *linha) {
    char token[8];
    int pos = 0, delta = 0;
    while (sscanf(linha + pos, "%7s%n", token, &delta) == 1) {
        pos += delta;
        CARTA c = str2card(token);
        if (c > 0) pushCarta(p, c);
    }
}

/**
 * Funcao para processar a leitura de uma pilha a partir do ficheiro de save
 *
 * * @param j -> Estado do jogo
 * * @param i -> Indice da pilha
 * * @param f -> Ponteiro do ficheiro
 */
void processarLinhaSave(ESTADO *j, int i, FILE *f) {
    char linha[512];
    j->pilhas[i].tamanho = 0;
    if (fgets(linha, 512, f)) carregarLinhaPilha(&j->pilhas[i], linha);
}

/**
 * Funcao principal para carregar o jogo a partir de "save.txt"
 *
 * * @param j -> Estado do jogo
 * * @return res -> 1 se carregou com sucesso, 0 caso contrario
 */
int carregarJogo(ESTADO *j) {
    FILE *f = fopen("save.txt", "r");
    if (!f) { printf("Sem ficheiro de save.\n"); return 0; }
    char b[512];
    fgets(b, 512, f); /* skip name line */
    for (int i = 0; i < j->num_pilhas; i++) processarLinhaSave(j, i, f);
    fclose(f);
    return 1;
}


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
    RegrasInit aux = ri;
    while (aux) { n++; aux = aux->prox; }
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
    j->pilhas     = malloc(n * sizeof(PILHA));
    j->num_pilhas = n;
    for (int i = 0; i < n; i++) {
        j->pilhas[i].cartas    = NULL;
        j->pilhas[i].tamanho   = 0;
        j->pilhas[i].capacidade = 0;
        j->pilhas[i].tipo      = NULL;
    }
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
void preencherPilha(PILHA *p, const char *tipo, BARALHO deck,
                    int *idx, int n_cartas) {
    p->tipo = malloc(strlen(tipo) + 1);
    strcpy(p->tipo, tipo);
    for (int i = 0; i < n_cartas; i++)
        pushCarta(p, deck[(*idx)++]);
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
    /* percorre do último para o primeiro (lista foi construída por prepend) */
    RegrasInit cur = ultimoInit(ri);
    int idx = 0, col = 0;
    while (cur != NULL && col < n) {
        if (cur->tipoDePilha)
            preencherPilha(&j->pilhas[col++], cur->tipoDePilha, deck, &idx, cur->numeroDeCartas);
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
    s->cartas   = malloc(j->num_pilhas * sizeof(CARTA *));
    s->tamanhos = malloc(j->num_pilhas * sizeof(int));
    for (int i = 0; i < j->num_pilhas; i++) {
        s->tamanhos[i] = j->pilhas[i].tamanho;
        s->cartas[i]   = malloc(j->pilhas[i].tamanho * sizeof(CARTA));
        memcpy(s->cartas[i], j->pilhas[i].cartas, j->pilhas[i].tamanho * sizeof(CARTA));
    }
    s->prox = j->historico;
    j->historico = s;
}

/**
 * Funcao para libertar a memoria de um snapshot individual
 *
 * * @param s -> O snapshot a apagar
 */
void libertarSnapshot(SNAPSHOT *s) {
    for (int i = 0; i < s->num_pilhas; i++) free(s->cartas[i]);
    free(s->cartas);
    free(s->tamanhos);
    free(s);
}

/**
 * Funcao para reverter o estado do jogo para o ultimo snapshot guardado
 *
 * * @param j -> Estado do jogo
 * * @return res -> 1 se reverteu, 0 se nao havia historico
 */
int restaurarSnapshot(ESTADO *j) {
    if (!j->historico) { printf("Nao ha jogadas para desfazer.\n"); return 0; }
    SNAPSHOT *s = j->historico;
    j->historico = s->prox;
    for (int i = 0; i < j->num_pilhas; i++) {
        j->pilhas[i].tamanho = s->tamanhos[i];
        memcpy(j->pilhas[i].cartas, s->cartas[i], s->tamanhos[i] * sizeof(CARTA));
    }
    libertarSnapshot(s);
    return 1;
}


/**
 * Funcao que liberta toda a memoria alocada para o estado do jogo e historico
 *
 * * @param j -> Estado do jogo
 */
void limparEstado(ESTADO *j) {
    for (int i = 0; i < j->num_pilhas; i++) {
        free(j->pilhas[i].cartas);
        free(j->pilhas[i].tipo);
    }
    free(j->pilhas);
    free(j->B);
    while (j->historico) {
        SNAPSHOT *s = j->historico;
        j->historico = s->prox;
        libertarSnapshot(s);
    }
}

