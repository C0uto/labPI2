/**
 * @file display.c
 * @brief Implementação de todas as funções de apresentação no ecrã.
 */

#include <stdio.h>
#include <string.h>
#include "display.h"

/* ============================================================
 *  VISIBILIDADE
 * ============================================================ */

/**
 * @brief Devolve o char de visibilidade do tipo ('=', '_', '^') ou '=' por omissão.
 * @param rt    Lista de regras TIPO.
 * @param tipo  Nome do tipo de pilha.
 * @return      '=', '_' ou '^'.
 */
char visibilidadeTipo(RegrasTipo rt, const char *tipo) {
    RegrasTipo aux = rt;
    while (aux) {
        if (strcmp(aux->tipoDePilha, tipo) == 0) {
            for (int i = 0; aux->flags[0][i] != '\0'; i++) {
                char f = aux->flags[0][i];
                if ((f == '=' || f == '_' || f == '^') && aux->flags[1][i] == '1')
                    return f;
            }
        }
        aux = aux->prox;
    }
    return '=';
}

/* ============================================================
 *  AUXILIARES INTERNOS
 * ============================================================ */

/**
 * @brief Devolve 1 se o tipo pertence ao grupo superior (FUND, DESCARTE, CELL).
 * @param t  Nome do tipo.
 * @return   1 se superior, 0 se TAB.
 */
static int ehGrupoSuperior(const char *t) {
    return (strcmp(t, "FUND") == 0 || strcmp(t, "DESCARTE") == 0 ||
            strcmp(t, "CELL") == 0);
}

/**
 * @brief Calcula a altura máxima das pilhas de um grupo (superior ou TAB).
 * @param j    Estado do jogo.
 * @param sup  1 para grupo superior, 0 para TAB.
 * @return     Número de linhas a imprimir para esse grupo.
 */
static int maxAltura(ESTADO *j, int sup) {
    int m = 0;
    for (int i = 0; i < j->num_pilhas; i++) {
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup && j->pilhas[i].tamanho > m)
            m = j->pilhas[i].tamanho;
    }
    return m;
}

/**
 * @brief Imprime o cabeçalho com índice e tipo das pilhas de um grupo.
 * @param j    Estado do jogo.
 * @param sup  1 para grupo superior, 0 para TAB.
 * @return     Número de colunas impressas.
 */
static int printCabecalho(ESTADO *j, int sup) {
    int c = 0;
    for (int i = 0; i < j->num_pilhas; i++) {
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup) {
            printf(" [%2d:%-8s] ", i + 1, j->pilhas[i].tipo);
            c++;
        }
    }
    if (c > 0) printf("\n");
    return c;
}

/* ============================================================
 *  IMPRESSÃO DE CÉLULAS E LINHAS
 * ============================================================ */

/**
 * @brief Imprime a célula da pilha @p i na linha @p lin.
 *        Respeita a visibilidade: carta, "**" (escondida) ou espaço (vazio).
 * @param j    Estado do jogo.
 * @param i    Índice da pilha (base 0).
 * @param lin  Linha a imprimir (base 0).
 */
void printCelula(ESTADO *j, int i, int lin) {
    PILHA *p = &j->pilhas[i];
    if (lin >= p->tamanho) { printf("               "); return; }
    char vis    = visibilidadeTipo(j->rt, p->tipo);
    int mostrar = (vis == '=') || (vis == '^' && lin == p->tamanho - 1);
    if (mostrar) {
        char buf[4];
        card2str(p->cartas[lin], buf);
        printf("      %-2s       ", buf);
    } else {
        printf("      **       ");
    }
}

/**
 * @brief Imprime uma linha do tabuleiro para o grupo @p sup.
 * @param j    Estado do jogo.
 * @param lin  Linha a imprimir (base 0).
 * @param sup  1 para grupo superior, 0 para TAB.
 */
static void printLinha(ESTADO *j, int lin, int sup) {
    for (int i = 0; i < j->num_pilhas; i++)
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup)
            printCelula(j, i, lin);
    printf("\n");
}

/**
 * @brief Imprime o cabeçalho e todas as linhas de cartas do grupo @p sup.
 * @param j    Estado do jogo.
 * @param sup  1 para grupo superior, 0 para TAB.
 */
static void printGrupo(ESTADO *j, int sup) {
    int m = maxAltura(j, sup);
    if (printCabecalho(j, sup) > 0)
        for (int l = 0; l < m; l++)
            printLinha(j, l, sup);
}

/* ============================================================
 *  IMPRESSÃO DO TABULEIRO
 * ============================================================ */

/**
 * @brief Imprime o tabuleiro completo: grupo superior (FUND/DESCARTE/CELL) e TAB.
 * @param j  Estado do jogo.
 */
void imprimirTabuleiro(ESTADO *j) {
    if (j->num_pilhas == 0) return;
    printGrupo(j, 1);
    printf("\n");
    printGrupo(j, 0);
}

/**
 * @brief Imprime o estado do jogo com moldura decorativa.
 * @param j  Estado do jogo.
 */
void mostrarEstado(ESTADO *j) {
    printf("\n============= ESTADO DO JOGO =============\n");
    imprimirTabuleiro(j);
    printf("==========================================\n");
}

/* ============================================================
 *  DIAGNÓSTICO
 * ============================================================ */

/**
 * @brief Imprime todas as regras MOV/AUTO com as flags activas.
 * @param rma  Lista de regras MOV/AUTO.
 */
void imprimirMovs(RegrasMovAuto rma) {
    RegrasMovAuto aux = rma;
    while (aux) {
        printf("   %s %s %s [", aux->comando, aux->origem, aux->destino);
        for (int i = 0; aux->flags[0][i] != '\0'; i++)
            if (aux->flags[1][i] == '1') printf("%c", aux->flags[0][i]);
        printf("]\n");
        aux = aux->prox;
    }
}

/**
 * @brief Imprime todas as regras WIN.
 * @param rw  Lista de regras WIN.
 */
void imprimirWins(RegrasWin rw) {
    while (rw) {
        printf("   WIN %s %d\n", rw->tipoDePilha, rw->condicaoWin);
        rw = rw->prox;
    }
}
