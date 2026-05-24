#ifndef READFILE_H
#define READFILE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ========== TIPOS BASE ========== */
typedef int CARTA;
typedef CARTA *BARALHO;

typedef enum {
    OK = 0,
    ERRO_PILHA_INVALIDA,
    ERRO_PILHA_VAZIA,
    ERRO_JOGADA_INVALIDA,
    ERRO_BARALHO_VAZIO,
    WIN,
    GAME_OVER,
    Comando_INVALIDO,
    Flag_INVALIDA,
    ERRO_CAMINHO_INVALIDO
} MENSAGENS;

typedef struct {
    MENSAGENS mensagem;
    const char *feedback;
} FEEDBACK;

/* ========== ESTRUTURAS DE REGRAS ========== */

/* MOV / AUTO */
typedef struct regra {
    char *comando;   /* "MOV" ou "AUTO" */
    char *origem;
    char *destino;
    char flags[2][25]; /* flags[0]=chars possiveis, flags[1]=activas */
    struct regra *prox;
    struct regra *ant;
} *RegrasMovAuto;

/* JOGO */
typedef struct regra2 {
    char *comando;
    char *jogoNome;
} *RegrasJogo;

/* BARALHOS */
typedef struct regra3 {
    char *comando;
    int   numeroDeBaralhos;
} *RegrasBaralhos;

/* TIPO */
typedef struct regra4 {
    char *comando;
    char *tipoDePilha;
    char flags[2][6]; /* flags[0]=chars possiveis, flags[1]=activas */
    struct regra4 *prox;
    struct regra4 *ant;
} *RegrasTipo;

/* INIT */
typedef struct regra5 {
    char *comando;
    char *tipoDePilha;
    int   numeroDeCartas;
    struct regra5 *prox;
    struct regra5 *ant;
} *RegrasInit;

/* WIN */
typedef struct regra6 {
    char *comando;
    char *tipoDePilha;
    int   condicaoWin;
    struct regra6 *prox;
    struct regra6 *ant;
} *RegrasWin;

/* ========== ESTADO DO JOGO ========== */

typedef struct {
    char  *tipo;          /* "TAB", "FUND", "CELL", "STOCK", "DESCARTE" */
    CARTA *cartas;
    int    tamanho;       /* numero de cartas actual */
    int    capacidade;    /* espaco alocado */
} PILHA;

/* Snapshot de um estado para undo */
typedef struct snapshot {
    CARTA **cartas;
    int    *tamanhos;
    int     num_pilhas;
    struct snapshot *prox;
} SNAPSHOT;

typedef struct {
    PILHA *pilhas;
    int    num_pilhas;
    BARALHO B;
    int    total_cartas_baralho;
    RegrasTipo rt;      /* tipos de pilha – para visibilidade e flag 1 */
    SNAPSHOT  *historico; /* pilha de undo */
} ESTADO;

void     mostrar_mensagem(MENSAGENS cod);
MENSAGENS abreFicheiro(char *nome, RegrasMovAuto *listaMA, RegrasJogo *listaJ,
                        RegrasBaralhos *listaB, RegrasTipo *listaT,
                        RegrasInit *listaI, RegrasWin *listaW);
int      abrirPastaImprime(char *nome);

#endif /* READFILE_H */