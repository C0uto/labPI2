#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readFile.h>
#include <time.h>

// Simulação da inclusão das definições da Fase 1/2
// #include <card.h> 

// Caso o header não esteja disponível no path, definimos o necessário para compilação:
// A estrutura ESTADO real deve ser definida em card.h e incluída.

typedef char CARTA;
typedef CARTA *BARALHO;
typedef struct { 
    CARTA **paciencia;           // Matriz dinâmica para o tabuleiro (tableau)
    int num_pilhas;              // Quantidade de colunas/pilhas
    int max_cartas_por_pilha;    // Capacidade máxima de cada pilha
    CARTA fundacao; 
    int cartas_no_baralho; 
    int posicao_topo_do_baralho; 
    BARALHO B; 
} ESTADO;

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

BARALHO criar_baralhos(RegrasBaralhos rb) {
    int n = (rb != NULL) ? rb->numeroDeBaralhos : 1;
    int total_cartas = 52 * n;
    CARTA *b = malloc(sizeof(CARTA) * total_cartas);
    
    for (int i = 0; i < total_cartas; i++) {
        b[i] = (i % 52) + 1;
    }
    return b;
}

/**
 * Replicado de Fase1/2_Lógica.c (baralhar)
 */
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
    while (ri != NULL) {
        processarNoInit(ri, jogo, deck, &carta_idx);
        ri = ri->prox;
    }
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
    aplicarMovAuto(rma);
    aplicarWin(rw);
    printf("========== REGRAS APLICADAS ==========\n\n");
    limparEstado(&jogo);
}
