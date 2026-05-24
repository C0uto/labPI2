/**
 * @file carta.c
 * @brief Implementação da representação de cartas e operações sobre pilhas.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "carta.h"

/* ============================================================
 *  CONVERSÃO CARTA <-> STRING
 * ============================================================ */

/**
 * @brief Converte uma CARTA para string de 2 caracteres (ex: "AS", "TC", "KH").
 * @param card  Carta a converter. Valor 0 ou negativo escreve string vazia em @p buf.
 * @param buf   Buffer de destino com espaço para pelo menos 4 bytes.
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
 * @brief Converte string "<valor><naipe>" para CARTA.
 *        Trata o caso especial "10x" onde o valor ocupa dois caracteres.
 * @param s  String a converter (ex: "AS", "10H", "KD").
 * @return   Valor CARTA correspondente, ou 0 se a string for inválida.
 */
CARTA str2card(const char *s) {
    if (!s || !s[0]) return 0;
    int v, off;
    if (s[0] == '1' && s[1] == '0') { v = 9; off = 2; }
    else {
        const char *v_p = strchr("A23456789TJQK", s[0]);
        v   = v_p ? (int)(v_p - "A23456789TJQK") : -1;
        off = 1;
    }
    const char *s_p = strchr("SHDC", s[off]);
    int n = s_p ? (int)(s_p - "SHDC") : -1;
    return (v >= 0 && n >= 0) ? n * 13 + v + 1 : 0;
}

/* ============================================================
 *  PROPRIEDADES DE CARTA
 * ============================================================ */

/**
 * @brief Devolve o valor numérico da carta (Ás=1, ..., Rei=13).
 * @param c  Carta.
 * @return   Valor entre 1 e 13, ou 0 se inválida.
 */
int getValor(CARTA c) { return (c <= 0) ? 0 : ((c - 1) % 13) + 1; }

/**
 * @brief Devolve o índice do naipe (0=Espadas, 1=Copas, 2=Ouros, 3=Paus).
 * @param c  Carta.
 * @return   Índice 0–3, ou -1 se inválida.
 */
int getNaipe(CARTA c) { return (c <= 0) ? -1 : (c - 1) / 13; }

/**
 * @brief Devolve a cor da carta: 0=preto (Espadas/Paus), 1=vermelho (Copas/Ouros).
 * @param c  Carta.
 * @return   0 ou 1.
 */
int getCor(CARTA c) {
    int n = getNaipe(c);
    return (n == 1 || n == 2) ? 1 : 0;
}

/* ============================================================
 *  OPERAÇÕES SOBRE PILHAS
 * ============================================================ */

/**
 * @brief Devolve o número de cartas na pilha.
 * @param p  Ponteiro para a pilha.
 * @return   Número de cartas (≥ 0).
 */
int getPilhaTamanho(PILHA *p) { return p->tamanho; }

/**
 * @brief Insere uma carta no topo da pilha, realocando o array se necessário.
 *        A capacidade cresce em blocos de 16.
 * @param p  Ponteiro para a pilha.
 * @param c  Carta a inserir.
 */
void pushCarta(PILHA *p, CARTA c) {
    if (p->tamanho >= p->capacidade) {
        p->capacidade += 16;
        p->cartas = realloc(p->cartas, p->capacidade * sizeof(CARTA));
    }
    p->cartas[p->tamanho++] = c;
}

/**
 * @brief Remove e devolve a carta do topo da pilha.
 * @param p  Ponteiro para a pilha.
 * @return   Carta removida, ou 0 se a pilha estiver vazia.
 */
CARTA popCarta(PILHA *p) {
    if (p->tamanho <= 0) return 0;
    return p->cartas[--p->tamanho];
}

/**
 * @brief Devolve a carta do topo sem a remover.
 * @param p  Ponteiro para a pilha.
 * @return   Carta do topo, ou 0 se a pilha estiver vazia.
 */
CARTA topoCarta(PILHA *p) {
    if (p->tamanho <= 0) return 0;
    return p->cartas[p->tamanho - 1];
}
