#include <string.h>
#include <stdio.h>
typedef struct regra {
    char flags[2][21];
    char *comando;
    char *origem;
    char *destino;
    struct regra *ant;
    struct regra *prox;
} *RegrasMovAuto;

typedef struct regra2 {
    char *comando;
    char *jogoNome;
} *RegrasJogo;

typedef struct regra3 {
    char *comando;
    int numeroDeBaralhos;
} *RegrasBaralhos;

typedef struct regra4 {
    char flags[2][5];
    char *comando;
    char *tipoDePilha;
    struct regra4 *ant;
    struct regra4 *prox;
} *RegrasTipo;

typedef struct regra5 {
    char *comando;
    int numeroDeCartas;
    char *tipoDePilha;
    struct regra5 *ant;
    struct regra5 *prox;
} *RegrasInit;

typedef struct regra6 {
    char *comando;
    char *tipoDePilha;
    int condicaoWin;
    struct regra6 *ant;
    struct regra6 *prox;
} *RegrasWin;

// enumeracao das feedback messages
typedef enum {
        OK,
        Comando_INVALIDO,
        Flag_INVALIDA,
        ERRO_PILHA_INVALIDA,
        ERRO_PILHA_VAZIA,
        ERRO_JOGADA_INVALIDA,
        WIN,
        GAME_OVER
} MENSAGENS;

// estrutura para associar a numeracao com uma mensagem
typedef struct {
        MENSAGENS mensagem;
        const char *feedback;
} FEEDBACK;

MENSAGENS leFicheiro (FILE *f, RegrasMovAuto *rma, RegrasJogo *rj, RegrasBaralhos *rb, RegrasTipo *rt, RegrasInit *ri, RegrasWin *rw);
MENSAGENS leLinhaMovAuto (RegrasMovAuto *rma, char *temp1, char *temp2, char *temp3, char *flags_str, char *linha, int lidos);
MENSAGENS leLinhaInit (RegrasInit *ri, char *temp1, char *temp2, char *linha, int lidos);
MENSAGENS leLinhaTipo (RegrasTipo *rt, char *temp1, char *temp2, char *flags_str, char *linha, int lidos);
MENSAGENS leLinhaWin (RegrasWin *rw, char *temp1,   char *temp2, char *linha, int lidos);
MENSAGENS leLinhaBaralho (RegrasBaralhos *rb, char *temp1, char *linha, int lidos);
MENSAGENS