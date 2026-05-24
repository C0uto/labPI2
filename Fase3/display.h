/**
 * @file display.h
 * @brief Funções de apresentação do estado do jogo no ecrã.
 *
 * Separa completamente a interface textual da lógica do jogo.
 * Todas as funções que escrevem para stdout estão aqui.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "readFile.h"
#include "carta.h"

/* ============================================================
 *  VISIBILIDADE
 * ============================================================ */

/**
 * @brief Devolve o caractere de visibilidade do tipo de pilha.
 *        Consulta as regras TIPO para encontrar '=', '_' ou '^'.
 * @param rt    Lista de regras TIPO.
 * @param tipo  Nome do tipo de pilha a consultar.
 * @return      '=' (todas visíveis), '_' (nenhuma), '^' (só topo), ou '=' por omissão.
 */
char visibilidadeTipo(RegrasTipo rt, const char *tipo);

/* ============================================================
 *  IMPRESSÃO DE CÉLULAS E LINHAS
 * ============================================================ */

/**
 * @brief Imprime uma célula do tabuleiro para a pilha i, na linha lin.
 *        Respeita a visibilidade do tipo de pilha (mostra carta, "**" ou espaço).
 * @param j    Estado do jogo.
 * @param i    Índice da pilha (base 0).
 * @param lin  Índice da linha a imprimir (base 0).
 */
void printCelula(ESTADO *j, int i, int lin);

/* ============================================================
 *  IMPRESSÃO DO TABULEIRO
 * ============================================================ */

/**
 * @brief Imprime o tabuleiro completo dividido em dois grupos:
 *        grupo superior (FUND, DESCARTE, CELL) e grupo TAB.
 * @param j  Estado do jogo.
 */
void imprimirTabuleiro(ESTADO *j);

/**
 * @brief Imprime o estado completo do jogo com moldura decorativa.
 * @param j  Estado do jogo.
 */
void mostrarEstado(ESTADO *j);

/* ============================================================
 *  DIAGNÓSTICO
 * ============================================================ */

/**
 * @brief Imprime todas as regras MOV/AUTO com as flags activas entre parênteses rectos.
 * @param rma  Lista de regras MOV/AUTO.
 */
void imprimirMovs(RegrasMovAuto rma);

/**
 * @brief Imprime todas as regras WIN.
 * @param rw  Lista de regras WIN.
 */
void imprimirWins(RegrasWin rw);

#endif /* DISPLAY_H */
