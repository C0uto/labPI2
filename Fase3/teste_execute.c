#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Estruturas
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

// ========== CRIAÇÃO DE DADOS DUMMY ==========

int main() {
    printf("====== TESTE COM DADOS DUMMY - JOGO GOLF ======\n");
    
    // 1. JOGO
    RegrasJogo listaJ = malloc(sizeof(struct regra2));
    listaJ->comando = malloc(5);
    listaJ->jogoNome = malloc(5);
    strcpy(listaJ->comando, "JOGO");
    strcpy(listaJ->jogoNome, "Golf");
    
    // 2. BARALHOS
    RegrasBaralhos listaB = malloc(sizeof(struct regra3));
    listaB->comando = malloc(9);
    strcpy(listaB->comando, "BARALHOS");
    listaB->numeroDeBaralhos = 1;
    
    // 3. TIPOS DE PILHAS
    RegrasTipo listaT = NULL;
    
    // TIPO DESCARTE =
    RegrasTipo t3 = malloc(sizeof(struct regra4));
    strcpy(t3->flags[0], "=_^1");
    strcpy(t3->flags[1], "1000");
    t3->comando = malloc(5);
    t3->tipoDePilha = malloc(9);
    strcpy(t3->comando, "TIPO");
    strcpy(t3->tipoDePilha, "DESCARTE");
    t3->ant = NULL;
    t3->prox = listaT;
    if (listaT) listaT->ant = t3;
    listaT = t3;
    
    // TIPO STOCK _
    RegrasTipo t2 = malloc(sizeof(struct regra4));
    strcpy(t2->flags[0], "=_^1");
    strcpy(t2->flags[1], "0100");
    t2->comando = malloc(5);
    t2->tipoDePilha = malloc(6);
    strcpy(t2->comando, "TIPO");
    strcpy(t2->tipoDePilha, "STOCK");
    t2->ant = NULL;
    t2->prox = listaT;
    if (listaT) listaT->ant = t2;
    listaT = t2;
    
    // TIPO TAB =
    RegrasTipo t1 = malloc(sizeof(struct regra4));
    strcpy(t1->flags[0], "=_^1");
    strcpy(t1->flags[1], "1000");
    t1->comando = malloc(5);
    t1->tipoDePilha = malloc(4);
    strcpy(t1->comando, "TIPO");
    strcpy(t1->tipoDePilha, "TAB");
    t1->ant = NULL;
    t1->prox = listaT;
    if (listaT) listaT->ant = t1;
    listaT = t1;
    
    // 4. INITS
    RegrasInit listaI = NULL;
    
    // INIT STOCK 16
    RegrasInit i2 = malloc(sizeof(struct regra5));
    i2->comando = malloc(5);
    i2->tipoDePilha = malloc(6);
    strcpy(i2->comando, "INIT");
    strcpy(i2->tipoDePilha, "STOCK");
    i2->numeroDeCartas = 16;
    i2->ant = NULL;
    i2->prox = listaI;
    if (listaI) listaI->ant = i2;
    listaI = i2;
    
    // INIT DESCARTE 1
    RegrasInit i1 = malloc(sizeof(struct regra5));
    i1->comando = malloc(5);
    i1->tipoDePilha = malloc(9);
    strcpy(i1->comando, "INIT");
    strcpy(i1->tipoDePilha, "DESCARTE");
    i1->numeroDeCartas = 1;
    i1->ant = NULL;
    i1->prox = listaI;
    if (listaI) listaI->ant = i1;
    listaI = i1;
    
    // 5. MOVIMENTOS
    RegrasMovAuto listaMA = NULL;
    
    // MOV TAB DESCARTE ~
    RegrasMovAuto m2 = malloc(sizeof(struct regra));
    strcpy(m2->flags[0], "*+[]<>~mMxXcCdDVaAkK");
    strcpy(m2->flags[1], "00000001000000000000");
    m2->comando = malloc(4);
    m2->origem = malloc(4);
    m2->destino = malloc(9);
    strcpy(m2->comando, "MOV");
    strcpy(m2->origem, "TAB");
    strcpy(m2->destino, "DESCARTE");
    m2->ant = NULL;
    m2->prox = listaMA;
    if (listaMA) listaMA->ant = m2;
    listaMA = m2;
    
    // MOV STOCK DESCARTE *
    RegrasMovAuto m1 = malloc(sizeof(struct regra));
    strcpy(m1->flags[0], "*+[]<>~mMxXcCdDVaAkK");
    strcpy(m1->flags[1], "10000000000000000000");
    m1->comando = malloc(4);
    m1->origem = malloc(6);
    m1->destino = malloc(9);
    strcpy(m1->comando, "MOV");
    strcpy(m1->origem, "STOCK");
    strcpy(m1->destino, "DESCARTE");
    m1->ant = NULL;
    m1->prox = listaMA;
    if (listaMA) listaMA->ant = m1;
    listaMA = m1;
    
    // 6. WIN CONDITIONS
    RegrasWin listaW = NULL;
    
    // WIN TAB 0
    RegrasWin w1 = malloc(sizeof(struct regra6));
    w1->comando = malloc(4);
    w1->tipoDePilha = malloc(4);
    strcpy(w1->comando, "WIN");
    strcpy(w1->tipoDePilha, "TAB");
    w1->condicaoWin = 0;
    w1->ant = NULL;
    w1->prox = listaW;
    if (listaW) listaW->ant = w1;
    listaW = w1;
    
    // ========== EXECUTAR ==========
    execute(listaMA, listaJ, listaB, listaT, listaI, listaW);
    
    return 0;
}
