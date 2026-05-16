#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Estruturas importadas de 3faseGrupo.c
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

// ========== FUNÇÕES AUXILIARES ==========

void imprimirTipos(RegrasTipo rt) {
    while (rt != NULL) {
        printf("   - TIPO %s %s\n", rt->tipoDePilha, rt->flags[0]);
        rt = rt->prox;
    }
}

void imprimirInits(RegrasInit ri) {
    while (ri != NULL) {
        printf("   - INIT %s %d\n", ri->tipoDePilha, ri->numeroDeCartas);
        ri = ri->prox;
    }
}

void imprimirMovs(RegrasMovAuto rma) {
    while (rma != NULL) {
        printf("   - %s %s %s %s\n", rma->comando, rma->origem, rma->destino, rma->flags[0]);
        rma = rma->prox;
    }
}

void imprimirWins(RegrasWin rw) {
    while (rw != NULL) {
        printf("   - WIN %s %d\n", rw->tipoDePilha, rw->condicaoWin);
        rw = rw->prox;
    }
}

// ========== FUNÇÕES DE EXECUÇÃO DAS REGRAS ==========

void aplicarJogo(RegrasJogo rj) {
    if (rj == NULL || rj->comando == NULL) {
        printf("ERRO: Regra JOGO não definida!\n");
        return;
    }
    printf("1. [JOGO] %s\n", rj->jogoNome);
}

void aplicarBaralhos(RegrasBaralhos rb) {
    if (rb == NULL || rb->comando == NULL) {
        printf("ERRO: Regra BARALHOS não definida!\n");
        return;
    }
    printf("2. [BARALHOS] Criando %d baralho(s)\n", rb->numeroDeBaralhos);
}

void aplicarTipo(RegrasTipo rt) {
    if (rt == NULL) {
        printf("   [TIPO] Nenhum tipo definido\n");
        return;
    }
    printf("3. [TIPO] Tipos de pilhas:\n");
    imprimirTipos(rt);
}

void aplicarInit(RegrasInit ri) {
    if (ri == NULL) {
        printf("   [INIT] Nenhuma inicialização definida\n");
        return;
    }
    printf("4. [INIT] Inicializando pilhas:\n");
    imprimirInits(ri);
}

void aplicarMovAuto(RegrasMovAuto rma) {
    if (rma == NULL) {
        printf("   [MOV/AUTO] Nenhum movimento definido\n");
        return;
    }
    printf("5. [MOV/AUTO] Movimentos automáticos:\n");
    imprimirMovs(rma);
}

void aplicarWin(RegrasWin rw) {
    if (rw == NULL) {
        printf("   [WIN] Nenhuma condição de vitória definida\n");
        return;
    }
    printf("6. [WIN] Condições de vitória:\n");
    imprimirWins(rw);
}

// ========== FUNÇÃO PRINCIPAL DE EXECUÇÃO ==========

void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb, 
             RegrasTipo rt, RegrasInit ri, RegrasWin rw) {
    printf("\n========== APLICANDO REGRAS DO JOGO ==========\n\n");
    aplicarJogo(rj);
    printf("\n");
    aplicarBaralhos(rb);
    printf("\n");
    aplicarTipo(rt);
    printf("\n");
    aplicarInit(ri);
    printf("\n");
    aplicarMovAuto(rma);
    printf("\n");
    aplicarWin(rw);
    printf("\n");
    printf("========== REGRAS APLICADAS ==========\n\n");
}
