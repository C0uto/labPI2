#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "carta.h"
#include "display.h"


/**
 * Funcao para identificar se um tipo de pilha pertence ao grupo superior do ecra
 *
 * * @param t -> Nome do tipo de pilha
 * * @return res -> 1 se for superior (FUND, DESCARTE, CELL), 0 caso contrario
 */
int eGrupoSuperior(const char *t) {
    if (strcmp(t, "FUND") == 0 || strcmp(t, "DESCARTE") == 0) return 1;
    return (strcmp(t, "CELL") == 0);
}

/**
 * Funcao para calcular a maior quantidade de cartas entre pilhas de um certo grupo
 *
 * * @param j -> Estado do jogo
 * * @param sup -> 1 para grupo superior, 0 para grupo inferior (TAB)
 * * @return res -> Altura máxima encontrada
 */
int maxAltura(ESTADO *j, int sup) {
    int m = 0;
    for (int i = 0; i < j->num_pilhas; i++) {
        if (eGrupoSuperior(j->pilhas[i].tipo) == sup && j->pilhas[i].tamanho > m)
            m = j->pilhas[i].tamanho;
    }
    return m;
}

/**
 * Funcao para imprimir os cabecalhos (nomes e indices) de um grupo de pilhas
 *
 * * @param j -> Estado do jogo
 * * @param sup -> 1 para superior, 0 para inferior
 * * @return res -> Numero de pilhas impressas
 */
int printCabecalho(ESTADO *j, int sup) {
    int c = 0;
    for (int i = 0; i < j->num_pilhas; i++) {
        if (eGrupoSuperior(j->pilhas[i].tipo) == sup) {
            printf(" [%2d:%-8s] ", i + 1, j->pilhas[i].tipo);
            c++;
        }
    }
    if (c > 0) printf("\n");
    return c;
}

/**
 * Funcao para descobrir qual o carater de visibilidade associado a um tipo de pilha
 *
 * * @param rt -> Lista de regras de tipo
 * * @param tipo -> Nome do tipo de pilha
 * * @return res -> Carater '=', '_' ou '^'
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
    return '='; /* default: todas visíveis */
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
    int mostrar = (vis == '=') || (vis == '^' && lin == p->tamanho - 1);
    if (mostrar) {
        char buf[4];
        card2str(p->cartas[lin], buf);
        printf("      %-2s       ", buf);
    } else {
        printf("      ** ");
    }
}

/**
 * Funcao para imprimir uma linha completa horizontal do tabuleiro
 *
 * * @param j -> Estado do jogo
 * * @param lin -> Indice da linha
 * * @param sup -> Grupo (1 superior, 0 inferior)
 */
void printLinha(ESTADO *j, int lin, int sup) {
    for (int i = 0; i < j->num_pilhas; i++) {
        if (eGrupoSuperior(j->pilhas[i].tipo) == sup)
            printCelula(j, i, lin);
    }
    printf("\n");
}

/**
 * Funcao para imprimir um bloco completo de pilhas (ex: todas as TAB)
 *
 * * @param j -> Estado do jogo
 * * @param sup -> 1 para superior, 0 para inferior
 */
void printGrupo(ESTADO *j, int sup) {
    int m = maxAltura(j, sup);
    if (printCabecalho(j, sup) > 0) {
        for (int l = 0; l < m; l++)
            printLinha(j, l, sup);
    }
}

/**
 * Funcao principal de desenho do tabuleiro no terminal
 *
 * * @param j -> Estado do jogo
 */
void imprimirTabuleiro(ESTADO *j) {
    if (j->num_pilhas == 0) return;
    printGrupo(j, 1); // Grupo superior (FUND, DESCARTE, CELL)
    printf("\n");
    printGrupo(j, 0); // Grupo TAB
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
 * Funcao de diagnostico para listar as condicoes de vitoria carregadas
 *
 * * @param rw -> Lista de regras de vitoria
 */
void imprimirWins(RegrasWin rw) {
    while (rw) {
        printf("   WIN %s %d\n", rw->tipoDePilha, rw->condicaoWin);
        rw = rw->prox;
    }
}

