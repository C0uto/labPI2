#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "carta.h"


/**
 * Funcao para transformar o valor numerico da carta numa string
 *
 * * @param card -> O valor numerico da carta
 * * @param buf -> Buffer onde sera guardada a string resultante
 *
 */
void card2str(CARTA card, char *buf) {
    const char *valor = "A23456789TJQK";
    const char *naipe = "SHDC";
    if (card == 0 || !buf) { if (buf) buf[0] = '\0'; return; }
    int v = (card - 1) % 13;
    int n = (card - 1) / 13;
    buf[0] = valor[v];
    buf[1] = naipe[n];
    buf[2] = '\0';
}

/**
 * Funcao para transformar uma string (ex: "AH") num valor numerico de carta
 *
 * * @param s -> String que representa a carta
 * * @return res -> O valor numerico da carta (ou 0 se invalida)
 */
CARTA str2card(const char *s) {
    if (!s || !s[0]) return 0;
    int v, off;
    if (s[0] == '1' && s[1] == '0') { v = 9; off = 2; }
    else {
        const char *v_p = strchr("A23456789TJQK", s[0]);
        v = v_p ? (int)(v_p - "A23456789TJQK") : -1;
        off = 1;
    }
    const char *s_p = strchr("SHDC", s[off]);
    int n = s_p ? (int)(s_p - "SHDC") : -1;
    return (v >= 0 && n >= 0) ? n * 13 + v + 1 : 0;
}


/**
 * Funcao para obter o valor (rank) de uma carta (1 a 13)
 *
 * * @param c -> O valor numerico da carta
 * * @return res -> O rank da carta
 */
int getValor(CARTA c) { return (c <= 0) ? 0 : ((c - 1) % 13) + 1; }

/**
 * Funcao para obter o naipe de uma carta (0 a 3)
 *
 * * @param c -> O valor numerico da carta
 * * @return res -> O indice do naipe (S=0, H=1, D=2, C=3)
 */
int getNaipe(CARTA c) { return (c <= 0) ? -1 : (c - 1) / 13; }

/**
 * Funcao para obter a cor de uma carta (preto ou vermelho)
 *
 * * @param c -> O valor numerico da carta
 * * @return res -> 1 se for vermelha (H, D), 0 se for preta (S, C)
 */
int getCor(CARTA c)   {
    int n = getNaipe(c);
    /* S=0(preto), H=1(vermelho), D=2(vermelho), C=3(preto) */
    return (n == 1 || n == 2) ? 1 : 0;
}


/**
 * Funcao para obter o numero de cartas numa pilha
 *
 * * @param p -> Ponteiro para a pilha
 * * @return res -> Quantidade de cartas na pilha
 */
int getPilhaTamanho(PILHA *p) { return p->tamanho; }

/**
 * Funcao para adicionar uma carta ao topo de uma pilha
 *
 * * @param p -> Ponteiro para a pilha
 * * @param c -> A carta a ser adicionada
 */
void pushCarta(PILHA *p, CARTA c) {
    if (p->tamanho >= p->capacidade) {
        p->capacidade += 16;
        p->cartas = realloc(p->cartas, p->capacidade * sizeof(CARTA));
    }
    p->cartas[p->tamanho++] = c;
}

/**
 * Funcao para remover e retornar a carta do topo de uma pilha
 *
 * * @param p -> Ponteiro para a pilha
 * * @return res -> A carta removida (ou 0 se vazia)
 */
CARTA popCarta(PILHA *p) {
    if (p->tamanho <= 0) return 0;
    return p->cartas[--p->tamanho];
}

/**
 * Funcao para consultar a carta do topo de uma pilha sem a remover
 *
 * * @param p -> Ponteiro para a pilha
 * * @return res -> A carta no topo
 */
CARTA topoCarta(PILHA *p) {
    if (p->tamanho <= 0) return 0;
    return p->cartas[p->tamanho - 1];
}

