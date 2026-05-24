/**
 * @file estado.h
 * @brief Baralho, inicialização do estado, UNDO e save/load.
 *
 * Agrupa todas as operações que criam, transformam ou persistem
 * o estado global do jogo: baralho, pilhas, histórico de UNDO e ficheiro de save.
 */

#ifndef ESTADO_H
#define ESTADO_H

#include <stdio.h>
#include "readFile.h"
#include "carta.h"

/* ============================================================
 *  BARALHO
 * ============================================================ */

/**
 * @brief Aloca e preenche um baralho com n_baralhos × 52 cartas ordenadas (1..52).
 * @param n_baralhos  Número de baralhos a criar (≥ 1).
 * @return            Ponteiro para o array alocado.
 */
BARALHO criarBaralho(int n_baralhos);

/**
 * @brief Baralha o array usando o algoritmo Fisher-Yates.
 * @param b           Array de cartas a baralhar.
 * @param n_baralhos  Número de baralhos (determina o total de cartas).
 */
void baralharBaralho(BARALHO b, int n_baralhos);

/* ============================================================
 *  INICIALIZAÇÃO DO ESTADO A PARTIR DAS REGRAS INIT
 * ============================================================ */

/**
 * @brief Conta o número de nós na lista INIT (= número de pilhas a criar).
 * @param ri  Cabeça da lista INIT.
 * @return    Número de INITs.
 */
int contarInits(RegrasInit ri);

/**
 * @brief Devolve o último nó da lista INIT.
 *        A lista foi construída por prepend, logo o último nó corresponde
 *        ao primeiro INIT do ficheiro DSL.
 * @param ri  Cabeça da lista INIT.
 * @return    Ponteiro para o último nó, ou NULL se vazia.
 */
RegrasInit ultimoInit(RegrasInit ri);

/**
 * @brief Aloca o array de pilhas no estado e inicializa cada uma a vazio.
 * @param j  Estado do jogo a inicializar.
 * @param n  Número de pilhas a criar.
 */
void inicializarPilhas(ESTADO *j, int n);

/**
 * @brief Define o tipo de uma pilha e distribui @p n_cartas cartas a partir do deck.
 * @param p        Pilha a preencher.
 * @param tipo     Nome do tipo (copiado para memória própria).
 * @param deck     Array de cartas de onde se retiram as cartas.
 * @param idx      Índice actual no deck (actualizado pela função).
 * @param n_cartas Número de cartas a distribuir.
 */
void preencherPilha(PILHA *p, const char *tipo, BARALHO deck, int *idx, int n_cartas);

/**
 * @brief Aplica as regras INIT ao estado: cria as pilhas e distribui as cartas.
 *        Percorre a lista do último para o primeiro (lista construída por prepend).
 * @param j     Estado do jogo a preencher.
 * @param ri    Lista de regras INIT.
 * @param deck  Baralho já baralhado.
 */
void aplicarInitAoEstado(ESTADO *j, RegrasInit ri, BARALHO deck);

/* ============================================================
 *  UNDO — HISTÓRICO DE ESTADOS
 * ============================================================ */

/**
 * @brief Grava um snapshot do estado actual no topo do histórico.
 *        Copia os arrays de cartas de todas as pilhas.
 * @param j  Estado do jogo cujo snapshot se quer guardar.
 */
void guardarSnapshot(ESTADO *j);

/**
 * @brief Liberta toda a memória associada a um snapshot.
 * @param s  Snapshot a libertar.
 */
void libertarSnapshot(SNAPSHOT *s);

/**
 * @brief Restaura o estado a partir do snapshot mais recente e remove-o do histórico.
 * @param j  Estado do jogo a restaurar.
 * @return   1 se o restauro foi bem-sucedido, 0 se o histórico estiver vazio.
 */
int restaurarSnapshot(ESTADO *j);

/* ============================================================
 *  SAVE / LOAD
 * ============================================================ */

/**
 * @brief Escreve o conteúdo de uma pilha numa linha do ficheiro de save.
 *        Formato: cartas separadas por espaços, fundo → topo, terminada por '\n'.
 * @param f  Ficheiro aberto em escrita.
 * @param p  Pilha a gravar.
 */
void gravarPilha(FILE *f, PILHA *p);

/**
 * @brief Grava o estado actual em "save.txt".
 *        1ª linha: nome do ficheiro da paciência; restantes: uma linha por pilha.
 * @param j               Estado do jogo a gravar.
 * @param nome_paciencia  Nome da paciência (escrito na 1ª linha).
 */
void gravarJogo(ESTADO *j, const char *nome_paciencia);

/**
 * @brief Carrega o estado de jogo a partir de "save.txt".
 *        As pilhas são esvaziadas antes do carregamento.
 *        A primeira linha (nome) é ignorada.
 * @param j  Estado do jogo a preencher.
 * @return   1 se bem-sucedido, 0 se o ficheiro não existir.
 */
int carregarJogo(ESTADO *j);

/* ============================================================
 *  LIMPAR ESTADO
 * ============================================================ */

/**
 * @brief Liberta toda a memória do estado do jogo: pilhas, baralho e histórico.
 * @param j  Estado do jogo a limpar.
 */
void limparEstado(ESTADO *j);

#endif /* ESTADO_H */
