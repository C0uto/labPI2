/**
 * @file carta.h
 * @brief Representação e propriedades de cartas, e operações sobre pilhas.
 *
 * Define o tipo CARTA, a struct PILHA, e todas as funções de baixo nível
 * que manipulam cartas individualmente ou pilhas de cartas.
 */

#ifndef CARTA_H
#define CARTA_H

#include "readFile.h"

/* ============================================================
 *  CONVERSÃO CARTA <-> STRING
 * ============================================================ */

/**
 * @brief Converte uma CARTA para string de 2 caracteres (ex: "AS", "TC", "KH").
 * @param card  Carta a converter (1–52×n). Valor 0 ou negativo gera string vazia.
 * @param buf   Buffer de destino (mínimo 4 bytes).
 */
void  card2str(CARTA card, char *buf);

/**
 * @brief Converte string "<valor><naipe>" para CARTA.
 *        Trata o caso especial "10x" onde o valor ocupa dois caracteres.
 * @param s  String a converter (ex: "AS", "10H", "KD").
 * @return   Valor CARTA correspondente, ou 0 se a string for inválida.
 */
CARTA str2card(const char *s);

/* ============================================================
 *  PROPRIEDADES DE CARTA
 * ============================================================ */

/**
 * @brief Devolve o valor numérico da carta (Ás=1, ..., Rei=13).
 * @param c  Carta.
 * @return   Valor entre 1 e 13, ou 0 se inválida.
 */
int getValor(CARTA c);

/**
 * @brief Devolve o índice do naipe (0=Espadas, 1=Copas, 2=Ouros, 3=Paus).
 * @param c  Carta.
 * @return   Índice 0–3, ou -1 se inválida.
 */
int getNaipe(CARTA c);

/**
 * @brief Devolve a cor da carta (0=preto, 1=vermelho).
 *        Espadas e Paus são pretas; Copas e Ouros são vermelhas.
 * @param c  Carta.
 * @return   0 ou 1.
 */
int getCor(CARTA c);

/* ============================================================
 *  OPERAÇÕES SOBRE PILHAS
 * ============================================================ */

/**
 * @brief Devolve o número de cartas actualmente na pilha.
 * @param p  Ponteiro para a pilha.
 * @return   Número de cartas (≥ 0).
 */
int getPilhaTamanho(PILHA *p);

/**
 * @brief Insere uma carta no topo da pilha, realocando o array se necessário.
 * @param p  Ponteiro para a pilha.
 * @param c  Carta a inserir.
 */
void pushCarta(PILHA *p, CARTA c);

/**
 * @brief Remove e devolve a carta do topo da pilha.
 * @param p  Ponteiro para a pilha.
 * @return   Carta removida, ou 0 se a pilha estiver vazia.
 */
CARTA popCarta(PILHA *p);

/**
 * @brief Devolve a carta do topo sem a remover.
 * @param p  Ponteiro para a pilha.
 * @return   Carta do topo, ou 0 se a pilha estiver vazia.
 */
CARTA topoCarta(PILHA *p);

#endif /* CARTA_H */
