#include <stdio.h>
#include <string.h>
#include "interface.h"

/**
 * Funcao para identificar se um tipo de pilha pertence ao grupo superior do ecra
 *
 * * @param t -> Nome do tipo de pilha
 * * @return res -> 1 se for superior (FUND, DESCARTE, CELL), 0 caso contrario
 */
static int ehGrupoSuperior(const char *t) {
    return (!strcmp(t, "FUND") || !strcmp(t, "DESCARTE") || !strcmp(t, "CELL"));
}

/**
 * Funcao para descobrir qual o carater de visibilidade associado a um tipo de pilha
 *
 * * @param rt -> Lista de regras de tipo
 * * @param tipo -> Nome do tipo de pilha
 * * @return res -> Carater '=', '_' ou '^'
 */
char visibilidadeTipo(RegrasTipo rt, const char *tipo) {
    for (RegrasTipo aux = rt; aux; aux = aux->prox)
        if (!strcmp(aux->tipoDePilha, tipo))
            for (int i = 0; aux->flags[0][i]; i++)
                if ((aux->flags[0][i] == '=' || aux->flags[0][i] == '_' || aux->flags[0][i] == '^') && aux->flags[1][i] == '1')
                    return aux->flags[0][i];
    return '=';
}

/**
 * Funcao para imprimir uma carta especifica (ou o seu verso) de uma pilha numa linha
 *
 * * @param j -> Estado do jogo
 * * @param i -> Indice da pilha
 * * @param lin -> Indice da carta (linha)
 */
void printCelula(ESTADO *j, int i, int lin) {
    PILHA *p = &j->pilhas[i];
    if (lin >= p->tamanho) { printf("               "); return; }
    char vis = visibilidadeTipo(j->rt, p->tipo);
    if ((vis == '=') || (vis == '^' && lin == p->tamanho - 1)) {
        char buf[4]; card2str(p->cartas[lin], buf);
        printf("      %-2s       ", buf);
    } else printf("      **       ");
}

/**
 * Funcao para calcular a maior quantidade de cartas entre pilhas de um certo grupo
 */
int maxAltura(ESTADO *j, int sup) {
    int m = 0;
    for (int i = 0; i < j->num_pilhas; i++)
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup && j->pilhas[i].tamanho > m)
            m = j->pilhas[i].tamanho;
    return m;
}

/**
 * Funcao para imprimir os cabecalhos (nomes e indices) de um grupo de pilhas
 */
int printCabecalho(ESTADO *j, int sup) {
    int c = 0;
    for (int i = 0; i < j->num_pilhas; i++)
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup) {
            printf(" [%2d:%-8s] ", i + 1, j->pilhas[i].tipo);
            c++;
        }
    if (c > 0) printf("\n");
    return c;
}

void printLinha(ESTADO *j, int lin, int sup) {
    for (int i = 0; i < j->num_pilhas; i++)
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup) printCelula(j, i, lin);
    printf("\n");
}

/**
 * Funcao para imprimir um bloco completo de pilhas (ex: todas as TAB)
 */
static void printGrupo(ESTADO *j, int sup) {
    int m = maxAltura(j, sup);
    if (printCabecalho(j, sup) > 0) {
        for (int l = 0; l < m; l++) printLinha(j, l, sup);
    }
}

/**
 * Funcao principal de desenho do tabuleiro no terminal
 *
 * * @param j -> Estado do jogo
 */
void imprimirTabuleiro(ESTADO *j) {
    if (j->num_pilhas == 0) return;
    printGrupo(j, 1); printf("\n"); printGrupo(j, 0);
}

/**
 * Funcao que limpa o ecra visualmente com o novo estado
 *
 * * @param j -> Estado do jogo
 */
void mostrarEstado(ESTADO *j) {
    printf("\n============= ESTADO DO JOGO =============\n");
    imprimirTabuleiro(j);
    printf("==========================================\n");
}

/**
 * Funcao de diagnostico para listar todas as regras de movimento carregadas
 *
 * * @param rma -> Lista de regras
 */
void imprimirMovs(RegrasMovAuto rma) {
    for (RegrasMovAuto aux = rma; aux; aux = aux->prox) {
        printf("   %s %s %s [", aux->comando, aux->origem, aux->destino);
        for (int i = 0; aux->flags[0][i]; i++) if (aux->flags[1][i] == '1') printf("%c", aux->flags[0][i]);
        printf("]\n");
    }
}

/**
 * Funcao de diagnostico para listar as condicoes de vitoria carregadas
 *
 * * @param rw -> Lista de regras de vitoria
 */
void imprimirWins(RegrasWin rw) {
    for (RegrasWin aux = rw; aux; aux = aux->prox) printf("   WIN %s %d\n", aux->tipoDePilha, aux->condicaoWin);
}