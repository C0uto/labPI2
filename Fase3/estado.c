/**
 * @file estado.c
 * @brief Implementação do baralho, inicialização do estado, UNDO e save/load.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "estado.h"

/* ============================================================
 *  BARALHO
 * ============================================================ */

/**
 * @brief Aloca e preenche um baralho com n_baralhos × 52 cartas ordenadas.
 * @param n_baralhos  Número de baralhos (≥ 1).
 * @return            Ponteiro para o array alocado.
 */
BARALHO criarBaralho(int n_baralhos) {
    int total = 52 * n_baralhos;
    BARALHO b = malloc(sizeof(CARTA) * total);
    for (int i = 0; i < total; i++) b[i] = (i % 52) + 1;
    return b;
}

/**
 * @brief Baralha o array com o algoritmo Fisher-Yates.
 * @param b           Array a baralhar.
 * @param n_baralhos  Número de baralhos (define o total de cartas).
 */
void baralharBaralho(BARALHO b, int n_baralhos) {
    int total = 52 * n_baralhos;
    srand((unsigned)time(NULL));
    for (int i = total - 1; i > 0; i--) {
        int j    = rand() % (i + 1);
        CARTA t  = b[i]; b[i] = b[j]; b[j] = t;
    }
}

/* ============================================================
 *  INICIALIZAÇÃO DO ESTADO
 * ============================================================ */

/**
 * @brief Conta o número de nós na lista INIT.
 * @param ri  Cabeça da lista.
 * @return    Número de INITs.
 */
int contarInits(RegrasInit ri) {
    int n = 0;
    RegrasInit aux = ri;
    while (aux) { n++; aux = aux->prox; }
    return n;
}

/**
 * @brief Devolve o último nó da lista INIT (primeiro INIT do ficheiro).
 * @param ri  Cabeça da lista (construída por prepend).
 * @return    Ponteiro para o último nó, ou NULL se vazia.
 */
RegrasInit ultimoInit(RegrasInit ri) {
    if (!ri) return NULL;
    while (ri->prox) ri = ri->prox;
    return ri;
}

/**
 * @brief Aloca o array de pilhas e inicializa cada uma a vazio.
 * @param j  Estado do jogo.
 * @param n  Número de pilhas.
 */
void inicializarPilhas(ESTADO *j, int n) {
    j->pilhas     = malloc(n * sizeof(PILHA));
    j->num_pilhas = n;
    for (int i = 0; i < n; i++) {
        j->pilhas[i].cartas     = NULL;
        j->pilhas[i].tamanho    = 0;
        j->pilhas[i].capacidade = 0;
        j->pilhas[i].tipo       = NULL;
    }
}

/**
 * @brief Define o tipo e distribui cartas para uma pilha a partir do deck.
 * @param p        Pilha a preencher.
 * @param tipo     Nome do tipo (copiado para memória própria).
 * @param deck     Array de cartas.
 * @param idx      Índice actual no deck (incrementado pela função).
 * @param n_cartas Número de cartas a distribuir.
 */
void preencherPilha(PILHA *p, const char *tipo, BARALHO deck,
                    int *idx, int n_cartas) {
    p->tipo = malloc(strlen(tipo) + 1);
    strcpy(p->tipo, tipo);
    for (int i = 0; i < n_cartas; i++)
        pushCarta(p, deck[(*idx)++]);
}

/**
 * @brief Aplica os INITs ao estado, percorrendo a lista do último para o primeiro.
 * @param j     Estado do jogo a preencher.
 * @param ri    Lista de regras INIT.
 * @param deck  Baralho já baralhado.
 */
void aplicarInitAoEstado(ESTADO *j, RegrasInit ri, BARALHO deck) {
    int n = contarInits(ri);
    if (n == 0 || deck == NULL) return;
    inicializarPilhas(j, n);
    RegrasInit cur = ultimoInit(ri);
    int idx = 0, col = 0;
    while (cur != NULL && col < n) {
        if (cur->tipoDePilha)
            preencherPilha(&j->pilhas[col++], cur->tipoDePilha, deck,
                           &idx, cur->numeroDeCartas);
        cur = cur->ant;
    }
}

/* ============================================================
 *  UNDO
 * ============================================================ */

/**
 * @brief Grava um snapshot do estado actual no topo do histórico.
 *        Copia os arrays de cartas de todas as pilhas.
 * @param j  Estado do jogo.
 */
void guardarSnapshot(ESTADO *j) {
    SNAPSHOT *s   = malloc(sizeof(SNAPSHOT));
    s->num_pilhas = j->num_pilhas;
    s->cartas     = malloc(j->num_pilhas * sizeof(CARTA *));
    s->tamanhos   = malloc(j->num_pilhas * sizeof(int));
    for (int i = 0; i < j->num_pilhas; i++) {
        s->tamanhos[i] = j->pilhas[i].tamanho;
        s->cartas[i]   = malloc(j->pilhas[i].tamanho * sizeof(CARTA));
        memcpy(s->cartas[i], j->pilhas[i].cartas,
               j->pilhas[i].tamanho * sizeof(CARTA));
    }
    s->prox      = j->historico;
    j->historico = s;
}

/**
 * @brief Liberta toda a memória associada a um snapshot.
 * @param s  Snapshot a libertar.
 */
void libertarSnapshot(SNAPSHOT *s) {
    for (int i = 0; i < s->num_pilhas; i++) free(s->cartas[i]);
    free(s->cartas);
    free(s->tamanhos);
    free(s);
}

/**
 * @brief Restaura o estado a partir do snapshot mais recente e remove-o do histórico.
 * @param j  Estado do jogo.
 * @return   1 se restaurado, 0 se o histórico estiver vazio.
 */
int restaurarSnapshot(ESTADO *j) {
    if (!j->historico) { printf("Nao ha jogadas para desfazer.\n"); return 0; }
    SNAPSHOT *s  = j->historico;
    j->historico = s->prox;
    for (int i = 0; i < j->num_pilhas; i++) {
        j->pilhas[i].tamanho = s->tamanhos[i];
        memcpy(j->pilhas[i].cartas, s->cartas[i],
               s->tamanhos[i] * sizeof(CARTA));
    }
    libertarSnapshot(s);
    return 1;
}

/* ============================================================
 *  SAVE / LOAD
 * ============================================================ */

/**
 * @brief Escreve o conteúdo de uma pilha numa linha do ficheiro de save.
 * @param f  Ficheiro aberto em escrita.
 * @param p  Pilha a gravar.
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
 * @brief Grava o estado actual em "save.txt".
 * @param j               Estado do jogo.
 * @param nome_paciencia  Nome da paciência (1ª linha do ficheiro).
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

/** @brief Lê uma linha do save e preenche a pilha com as cartas encontradas. */
static void carregarLinhaPilha(PILHA *p, char *linha) {
    char token[8];
    int pos = 0, delta = 0;
    while (sscanf(linha + pos, "%7s%n", token, &delta) == 1) {
        pos += delta;
        CARTA c = str2card(token);
        if (c > 0) pushCarta(p, c);
    }
}

/** @brief Esvazia a pilha i e preenche-a com os dados de uma linha do save. */
static void processarLinhaSave(ESTADO *j, int i, FILE *f) {
    char linha[512];
    j->pilhas[i].tamanho = 0;
    if (fgets(linha, 512, f)) carregarLinhaPilha(&j->pilhas[i], linha);
}

/**
 * @brief Carrega o estado de jogo a partir de "save.txt".
 * @param j  Estado do jogo a preencher.
 * @return   1 se bem-sucedido, 0 se o ficheiro não existir.
 */
int carregarJogo(ESTADO *j) {
    FILE *f = fopen("save.txt", "r");
    if (!f) { printf("Sem ficheiro de save.\n"); return 0; }
    char b[512];
    fgets(b, 512, f); /* ignora linha com o nome da paciência */
    for (int i = 0; i < j->num_pilhas; i++) processarLinhaSave(j, i, f);
    fclose(f);
    return 1;
}

/* ============================================================
 *  LIMPAR ESTADO
 * ============================================================ */

/**
 * @brief Liberta toda a memória do estado: pilhas, baralho e histórico.
 * @param j  Estado do jogo.
 */
void limparEstado(ESTADO *j) {
    for (int i = 0; i < j->num_pilhas; i++) {
        free(j->pilhas[i].cartas);
        free(j->pilhas[i].tipo);
    }
    free(j->pilhas);
    free(j->B);
    while (j->historico) {
        SNAPSHOT *s  = j->historico;
        j->historico = s->prox;
        libertarSnapshot(s);
    }
}
