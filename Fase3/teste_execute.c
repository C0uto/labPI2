#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef char CARTA;
typedef CARTA *BARALHO;

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

typedef struct { 
    CARTA **paciencia;           // Matriz dinâmica para o tabuleiro
    int num_pilhas;              // Quantidade de colunas
    int max_cartas_por_pilha;    // Capacidade de cada pilha
    CARTA fundacao; 
    int cartas_no_baralho; 
    int posicao_topo_do_baralho; 
    BARALHO B; 
} ESTADO;

// ========== FUNÇÕES AUXILIARES ==========

const char *card2str(CARTA card) {
    char *valor = "A23456789TJQK", *naipe = "SHDC";
    static char res[3];
    if (card <= 0) return "  ";
    res[0] = valor[(card - 1) % 13];
    res[1] = naipe[(card - 1) / 13];
    res[2] = '\0';
    return res;
}

void imprimirLinha(ESTADO *j, int lin) {
    for (int col = 0; col < j->num_pilhas; col++) {
        printf(" %s\t", card2str(j->paciencia[col][lin]));
    }
    printf("\n");
}

void imprimirTabuleiro(ESTADO *j) {
    if (!j->paciencia) return;
    for (int lin = 0; lin < j->max_cartas_por_pilha; lin++) {
        int tem_carta = 0;
        for (int col = 0; col < j->num_pilhas; col++) {
            if (j->paciencia[col][lin] > 0) tem_carta = 1;
        }
        if (!tem_carta) break;
        imprimirLinha(j, lin);
    }
}

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

BARALHO criar_baralhos(RegrasBaralhos rb) {
    int n = (rb != NULL) ? rb->numeroDeBaralhos : 1;
    int total_cartas = 52 * n;
    CARTA *b = malloc(sizeof(CARTA) * total_cartas);
    for (int i = 0; i < total_cartas; i++) {
        b[i] = (i % 52) + 1;
    }
    return b;
}

void baralhar_generico(BARALHO b, int n_baralhos) {
    int total = 52 * n_baralhos;
    srand(time(NULL));
    for (int i = 0; i < total * 2; i++) {
        int idx1 = rand() % total;
        int idx2 = rand() % total;
        CARTA temp = b[idx1];
        b[idx1] = b[idx2];
        b[idx2] = temp;
    }
}

BARALHO aplicarBaralhos(RegrasBaralhos rb) {
    if (rb == NULL || rb->comando == NULL) {
        printf("ERRO: Regra BARALHOS não definida!\n");
        return NULL;
    }
    printf("2. [BARALHOS] Criando %d baralho(s)\n", rb->numeroDeBaralhos);
    BARALHO b = criar_baralhos(rb);
    baralhar_generico(b, rb->numeroDeBaralhos);
    return b;
}

int encontrarTotalTab(RegrasInit ri) {
    while (ri != NULL) {
        if (strcmp(ri->tipoDePilha, "TAB") == 0) return ri->numeroDeCartas;
        ri = ri->prox;
    }
    return 0;
}

void alocarTabuleiro(ESTADO *jogo, int total) {
    if (total <= 0) return;
    int max_inf = 5;
    jogo->num_pilhas = (total + max_inf - 1) / max_inf;
    jogo->max_cartas_por_pilha = max_inf + 10;
    jogo->paciencia = malloc(jogo->num_pilhas * sizeof(CARTA *));
    for (int i = 0; i < jogo->num_pilhas; i++) {
        jogo->paciencia[i] = calloc(jogo->max_cartas_por_pilha, sizeof(CARTA));
        if (!jogo->paciencia[i]) exit(EXIT_FAILURE);
    }
}

void preencherTAB(RegrasInit ri, ESTADO *j, BARALHO d, int *idx) {
    int max_inf = 5;
    for (int i = 0; i < ri->numeroDeCartas; i++) {
        int col = i / max_inf, lin = i % max_inf;
        if (col < j->num_pilhas && lin < j->max_cartas_por_pilha) {
            j->paciencia[col][lin] = d[(*idx)++];
        } else (*idx)++;
    }
}

void processarNoInit(RegrasInit ri, ESTADO *j, BARALHO d, int *idx) {
    if (strcmp(ri->tipoDePilha, "TAB") == 0) {
        preencherTAB(ri, j, d, idx);
    } else if (strcmp(ri->tipoDePilha, "DESCARTE") == 0) {
        if (ri->numeroDeCartas > 0) j->fundacao = d[(*idx)++];
    } else if (strcmp(ri->tipoDePilha, "STOCK") == 0) {
        j->posicao_topo_do_baralho = *idx;
        j->cartas_no_baralho = ri->numeroDeCartas;
    }
}

void aplicarInit(RegrasInit ri, ESTADO *jogo, BARALHO deck) {
    if (ri == NULL) return;
    printf("4. [INIT] Inicializando pilhas:\n");
    alocarTabuleiro(jogo, encontrarTotalTab(ri));
    int carta_idx = 0;
    RegrasInit aux = ri;
    while (aux != NULL) {
        processarNoInit(aux, jogo, deck, &carta_idx);
        aux = aux->prox;
    }
    imprimirInits(ri);
}

void mostrarEstado(ESTADO *jogo) {
    printf("\n================ ESTADO DO JOGO ===============\n");
    if (jogo->fundacao > 0)
        printf("Fundação atual: %s\n", card2str(jogo->fundacao));
    if (jogo->cartas_no_baralho > 0)
        printf("Cartas no Stock: %d\n", jogo->cartas_no_baralho);
    if (jogo->paciencia != NULL) {
        printf("-----------------------------------------------\n");
        imprimirTabuleiro(jogo);
    }
    printf("===============================================\n");
}

void aplicarJogo(RegrasJogo rj) {
    if (rj && rj->comando) printf("1. [JOGO] %s\n", rj->jogoNome);
}

void aplicarTipo(RegrasTipo rt) {
    if (rt) {
        printf("3. [TIPO] Tipos de pilhas:\n");
        imprimirTipos(rt);
    }
}

void aplicarMovAuto(RegrasMovAuto rma) {
    if (rma) {
        printf("5. [MOV/AUTO] Movimentos automáticos:\n");
        imprimirMovs(rma);
    }
}

void aplicarWin(RegrasWin rw) {
    if (rw) {
        printf("6. [WIN] Condições de vitória:\n");
        imprimirWins(rw);
    }
}

void limparEstado(ESTADO *jogo) {
    if (jogo->paciencia) {
        for (int i = 0; i < jogo->num_pilhas; i++) free(jogo->paciencia[i]);
        free(jogo->paciencia);
    }
    if (jogo->B) free(jogo->B);
}

// ========== FUNÇÃO PRINCIPAL DE EXECUÇÃO ==========

void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb, 
             RegrasTipo rt, RegrasInit ri, RegrasWin rw) {

    ESTADO jogo;
    memset(&jogo, 0, sizeof(ESTADO));

    printf("\n========== APLICANDO REGRAS DO JOGO ==========\n\n");
    aplicarJogo(rj);
    jogo.B = aplicarBaralhos(rb);
    aplicarTipo(rt);
    aplicarInit(ri, &jogo, jogo.B);
    mostrarEstado(&jogo);
    aplicarMovAuto(rma);
    aplicarWin(rw);
    printf("========== REGRAS APLICADAS ==========\n\n");
    limparEstado(&jogo);
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
    listaB->numeroDeBaralhos = 3;
    
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
    
    // INIT TAB 35 (Adicionado para teste real de alocação)
    RegrasInit i3 = malloc(sizeof(struct regra5));
    i3->comando = malloc(5);
    i3->tipoDePilha = malloc(4);
    strcpy(i3->comando, "INIT");
    strcpy(i3->tipoDePilha, "TAB");
    i3->numeroDeCartas = 35;
    i3->prox = listaI;
    listaI = i3;
    
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
