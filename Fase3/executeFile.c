#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "executeFile.h"

// ========== FUNÇÕES AUXILIARES ==========

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

int getPilhaTamanho(ESTADO *j, int col) {
    int i = 0;
    while (i < j->max_cartas_por_pilha && j->paciencia[col][i] != 0) i++;
    return i;
}

void tratarPilha(char *input, ESTADO *j) {
    int o, d;
    if (sscanf(input, "p %d %d", &o, &d) != 2) return;
    o--; d--; 
    if (o < 0 || o >= j->num_pilhas || d < 0 || d >= j->num_pilhas) return;
    int to = getPilhaTamanho(j, o), td = getPilhaTamanho(j, d);
    if (to > 0 && td < j->max_cartas_por_pilha) {
        j->paciencia[d][td] = j->paciencia[o][to - 1];
        j->paciencia[o][to - 1] = 0;
        mostrarEstado(j);
    }
}

void tratarBaralho(ESTADO *j) {
    if (j->B == NULL || j->cartas_no_baralho <= 0) {
        printf("\nErro: Operação inválida. O baralho não existe ou está vazio.\n");
        return;
    }
    j->fundacao = j->B[j->posicao_topo_do_baralho];
    j->posicao_topo_do_baralho++;
    j->cartas_no_baralho--;
    mostrarEstado(j);
}

void tratarAjuda() {
    printf("\nComandos permitidos:\n");
    printf(" p O D - Move a carta do topo da pilha O para a pilha D\n");
    printf(" b     - Retira uma carta do baralho para a fundação\n");
    printf(" e     - Mostra o estado atual do jogo\n");
    printf(" h     - Mostra esta ajuda\n");
    printf(" q     - Sai do jogo\n");
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
    // Libertar memória se já existir tabuleiro (evita leak no reset)
    if (jogo->paciencia != NULL) {
        for (int i = 0; i < jogo->num_pilhas; i++) {
            free(jogo->paciencia[i]);
        }
        free(jogo->paciencia);
    }
    
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

void processarComando(char *buf, ESTADO *j) {
    if (buf[0] == 'p') tratarPilha(buf, j);
    else if (buf[0] == 'b') tratarBaralho(j);
    else if (buf[0] == 'e') mostrarEstado(j);
    else if (buf[0] == 'h') tratarAjuda();
    else printf("Comando desconhecido. Use 'h' para ajuda.\n");
}

void loopComandos(ESTADO *j, RegrasInit ri, BARALHO originalDeck) {
    char buf[256];
    while (1) {
        printf("JOGO> ");
        if (!fgets(buf, 256, stdin)) break;
        buf[strcspn(buf, "\n")] = 0;
        if (buf[0] == 'q') break;
        if (buf[0] == 'r') aplicarInit(ri, j, originalDeck);
        else processarComando(buf, j);
    }
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

    printf("\nUse 'h' para ver os comandos disponíveis.\n");
    loopComandos(&jogo, ri, jogo.B);

    aplicarMovAuto(rma);
    aplicarWin(rw);
    printf("========== REGRAS APLICADAS ==========\n\n");
    limparEstado(&jogo);
}
