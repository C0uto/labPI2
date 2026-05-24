#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "executeFile.h"

// ========== FUNÇÕES AUXILIARES ==========

void mostrarEstado(ESTADO *jogo) {
    printf("\n================================= ESTADO DO JOGO ===================================\n\n");
    if (jogo->fundacao > 0)
        printf("Fundação atual: %s\n", card2str(jogo->fundacao));
    if (jogo->cartas_no_baralho > 0)
        printf("Cartas no Stock: %d\n", jogo->cartas_no_baralho);
    if (jogo->paciencia != NULL) {
        imprimirTabuleiro(jogo);
    }
    printf("\n====================================================================================\n");
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

void imprimirCabecalho(int n) {
    for (int col = 0; col < n; col++) {
        printf(" [%d]\t", col + 1);
    }
    printf("\n");
}

void imprimirTabuleiro(ESTADO *j) {
    if (!j->paciencia) return;
    imprimirCabecalho(j->num_pilhas);
    for (int lin = 0; lin < j->max_cartas_por_pilha; lin++) {
        int tem = 0;
        for (int col = 0; col < j->num_pilhas; col++) {
            if (j->paciencia[col][lin] > 0) tem = 1;
        }
        if (!tem) break;
        imprimirLinha(j, lin);
    }
}

int getPilhaTamanho(ESTADO *j, int col) {
    int i = 0;
    while (i < j->max_cartas_por_pilha && j->paciencia[col][i] != 0) i++;
    return i;
}

int existeRegraMov(RegrasMovAuto rma, const char *origem, const char *destino) {
    RegrasMovAuto aux = rma;
    while (aux != NULL) {
        if (aux->origem && aux->destino &&
            strcmp(aux->origem, origem) == 0 &&
            strcmp(aux->destino, destino) == 0) {
            return 1;
        }
        aux = aux->prox;
    }
    return 0;
}

void moverCartas(ESTADO *j, int o, int d, int n, RegrasMovAuto rma) {
    (void)rma; // Por agora apenas move, a validação de flags virá depois
    int to = getPilhaTamanho(j, o);
    int td = getPilhaTamanho(j, d);
    for (int i = 0; i < n; i++) {
        j->paciencia[d][td + i] = j->paciencia[o][to - n + i];
        j->paciencia[o][to - n + i] = 0;
    }
    mostrarEstado(j);
}

void tratarPilha(char *input, ESTADO *j, RegrasMovAuto rma) {
    int o_idx, d_idx, n = 1; 
    char dummy;

    if (!existeRegraMov(rma, "TAB", "TAB")) {
        printf("Jogada inválida! Não é permitido mover cartas entre pilhas do tabuleiro neste jogo.\n");
        return;
    }

    int res = sscanf(input, " %c %d %d %d", &dummy, &o_idx, &d_idx, &n);
    if (res < 3) {
        printf("Comando incompleto! Tenta o formato: p <origem> <destino> [<cartas>]\n");
        return;
    }
    int o = o_idx - 1, d = d_idx - 1;
    int to = getPilhaTamanho(j, o);
    if (o < 0 || o >= j->num_pilhas || d < 0 || d >= j->num_pilhas || to < n || n <= 0) {
        printf("Jogada inválida!\n");
        return;
    }
    moverCartas(j, o, d, n, rma);
}

void tratarBaralho(ESTADO *j, RegrasMovAuto rma) {
    // O comando 'b' retira do Stock para a Fundação/Descarte
    if (!existeRegraMov(rma, "STOCK", "DESCARTE") && !existeRegraMov(rma, "STOCK", "FUND")) {
        printf("\nErro: Não é permitido retirar cartas do baralho neste jogo.\n");
        return;
    }

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
    printf(" p Origem Destino [Numero de Cartas] - Move o numero de cartas especificado da pilha Origem para a pilha Destino\n");
    printf(" b - Retira uma carta do baralho para a fundação, caso exista baralho\n");
    printf(" e - Mostra o estado atual do jogo\n");
    printf(" h - Mostra os comandos permitidos\n");
    printf(" q - Sai do jogo\n");
}


void imprimirTipos(RegrasTipo rt) {
    while (rt != NULL) {
        printf("   - TIPO %s ", rt->tipoDePilha);
        for (int i = 0; rt->flags[0][i] != '\0'; i++) {
            if (rt->flags[1][i] == '1') {
                printf("%c", rt->flags[0][i]);
            }
        }
        printf("\n");
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
        printf("   - %s %s %s ", rma->comando, rma->origem, rma->destino);
        for (int i = 0; rma->flags[0][i] != '\0'; i++) {
            if (rma->flags[1][i] == '1') {
                printf("%c", rma->flags[0][i]);
            }
        }
        printf("\n");
        rma = rma->prox;
    }
}

void imprimirWins(RegrasWin rw) {
    while (rw != NULL) {
        printf("   - WIN %s %d\n", rw->tipoDePilha, rw->condicaoWin);
        rw = rw->prox;
    }
}


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

int contarColunasTab(RegrasInit ri) {
    int colunas = 0;
    while (ri != NULL) {
        if (strcmp(ri->tipoDePilha, "TAB") == 0) colunas++;
        ri = ri->prox;
    }
    return colunas;
}

int encontrarMaxCartasInit(RegrasInit ri) {
    int max = 0;
    while (ri != NULL) {
        if (ri->numeroDeCartas > max) max = ri->numeroDeCartas;
        ri = ri->prox;
    }
    return max;
}

void limparTabuleiro(ESTADO *j) {
    if (j->paciencia == NULL) return;
    for (int i = 0; i < j->num_pilhas; i++) free(j->paciencia[i]);
    free(j->paciencia);
    j->paciencia = NULL;
}

void alocarTabuleiro(ESTADO *j, int n_pilhas, int max_cartas) {
    if (n_pilhas <= 0) return;
    limparTabuleiro(j);
    j->num_pilhas = n_pilhas;
    j->max_cartas_por_pilha = max_cartas + 15;
    j->paciencia = malloc(n_pilhas * sizeof(CARTA *));
    for (int i = 0; i < n_pilhas; i++) {
        j->paciencia[i] = calloc(j->max_cartas_por_pilha, sizeof(CARTA));
        if (!j->paciencia[i]) exit(EXIT_FAILURE);
    }
}

void preencherTAB(RegrasInit ri, ESTADO *j, int col, BARALHO d, int *idx) {
    if (col >= j->num_pilhas) return;
    for (int i = 0; i < ri->numeroDeCartas; i++) {
        if (i < j->max_cartas_por_pilha) {
            j->paciencia[col][i] = d[(*idx)++];
        }
    }
}

void processarNoInit(RegrasInit ri, ESTADO *j, int *col_tab, BARALHO d, int *idx) {
    if (strcmp(ri->tipoDePilha, "TAB") == 0) {
        preencherTAB(ri, j, *col_tab, d, idx);
        (*col_tab)++;
    } else if (strcmp(ri->tipoDePilha, "DESCARTE") == 0 || strcmp(ri->tipoDePilha, "FUND") == 0) {
        // Se houver várias cartas para a fundação, idx deve avançar para todas
        for (int i = 0; i < ri->numeroDeCartas; i++) {
            j->fundacao = d[(*idx)++];
        }
    } else if (strcmp(ri->tipoDePilha, "STOCK") == 0) {
        j->posicao_topo_do_baralho = *idx;
        j->cartas_no_baralho = ri->numeroDeCartas;
        // Avançar o índice do baralho para não sobrepor com outras pilhas
        (*idx) += ri->numeroDeCartas;
    }
}

void aplicarInit(RegrasInit ri, ESTADO *jogo, BARALHO deck) {
    if (ri == NULL) return;
    printf("4. [INIT] Pilhas:\n");
    
    int n_pilhas = contarColunasTab(ri);
    int max_altura = encontrarMaxCartasInit(ri);
    alocarTabuleiro(jogo, n_pilhas, max_altura);

    int carta_idx = 0;
    int col_atual = 0;
    RegrasInit aux = ri;
    while (aux != NULL) {
        processarNoInit(aux, jogo, &col_atual, deck, &carta_idx);
        aux = aux->prox;
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
        printf("5. [MOV/AUTO] Movimentos:\n");
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



typedef struct {
    char tecla;
    void (*funcao)(char *, ESTADO *, RegrasMovAuto);
} COMANDO_INFO;

// Wrappers para uniformizar as assinaturas das funções de comando
void cmd_baralho(char *s, ESTADO *j, RegrasMovAuto rma) { (void)s; tratarBaralho(j, rma); }
void cmd_estado(char *s, ESTADO *j, RegrasMovAuto rma) { (void)s; (void)rma; mostrarEstado(j); }
void cmd_ajuda(char *s, ESTADO *j, RegrasMovAuto rma) { (void)s; (void)j; (void)rma; tratarAjuda(); }

void processarComando(char *buf, ESTADO *j, RegrasMovAuto rma) {
    COMANDO_INFO comandos[] = {
        {'p', tratarPilha},
        {'b', cmd_baralho},
        {'e', cmd_estado},
        {'h', cmd_ajuda},
        {0,   NULL} // Sentinela
    };

    int i = 0;
    int encontrado = 0;

    while (comandos[i].tecla != 0 && !encontrado) {
        if (buf[0] == comandos[i].tecla) {
            comandos[i].funcao(buf, j, rma);
            encontrado = 1;
        }
        i++;
    }

    if (!encontrado) {
        printf("Comando desconhecido. Use 'h' para ajuda.\n");
    }
}

int executarEntrada(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasInit ri, BARALHO deck) {
    if (buf[0] == 'q') return 0;
    if (buf[0] == 'r') {
        aplicarInit(ri, j, deck);
        mostrarEstado(j);
    } else {
        processarComando(buf, j, rma);
    }
    return 1;
}

void loopComandos(ESTADO *j, RegrasMovAuto rma, RegrasInit ri, BARALHO originalDeck, const char *nome_jogo) {
    char buf[256];
    int continuar = 1;
    while (continuar) {
        printf("%s> ", nome_jogo);
        if (fgets(buf, 256, stdin) == NULL) {
            continuar = 0;
        } else {
            buf[strcspn(buf, "\n")] = 0;
            if (buf[0] != '\0') {
                continuar = executarEntrada(buf, j, rma, ri, originalDeck);
            }
        }
    }
}

void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb, 
             RegrasTipo rt, RegrasInit ri, RegrasWin rw) {

    ESTADO jogo;
    memset(&jogo, 0, sizeof(ESTADO));

    printf("\n========== APLICAR REGRAS DO JOGO ==========\n\n");
    aplicarJogo(rj);
    jogo.B = aplicarBaralhos(rb);
    aplicarTipo(rt);
    aplicarInit(ri, &jogo, jogo.B);
    aplicarMovAuto(rma);
    aplicarWin(rw);
    printf("============= REGRAS APLICADAS ==============\n\n");
    mostrarEstado(&jogo);
    loopComandos(&jogo, rma, ri, jogo.B, rj->jogoNome);
    limparEstado(&jogo);
}
