/**
 * @file executeFile.h
 * @brief Loop principal, processamento de comandos do utilizador e ponto de entrada do jogo.
 *
 * Este módulo coordena os restantes (carta, logica, estado, display) e
 * implementa o ciclo interactivo: leitura de comandos, execução e feedback.
 */

#ifndef EXECUTEFILE_H
#define EXECUTEFILE_H

#include "readFile.h"
#include "carta.h"
#include "logica.h"
#include "estado.h"
#include "display.h"

/* ============================================================
 *  COMANDOS DO UTILIZADOR
 * ============================================================ */

/**
 * @brief Apresenta no ecrã a lista de todos os comandos disponíveis.
 */
void tratarAjuda(void);

/**
 * @brief Interpreta e executa o comando 'p' (mover cartas).
 *        Guarda snapshot antes do movimento para permitir UNDO.
 *        Após movimento válido, processa AUTO e verifica vitória.
 * @param buf  Buffer com a linha de comando completa (ex: "p 3 7 2").
 * @param j    Estado do jogo.
 * @param rma  Lista de regras MOV/AUTO.
 * @param rw   Lista de regras WIN.
 */
void tratarMover(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasWin rw);

/**
 * @brief Executa o comando 'u' (undo): restaura o estado anterior e mostra-o.
 * @param j  Estado do jogo.
 */
void tratarUndo(ESTADO *j);

/**
 * @brief Executa acções de sistema ('s' save, 'l' load, 'r' reiniciar).
 * @param c   Caractere do comando.
 * @param j   Estado do jogo.
 * @param ri  Lista de regras INIT (para reiniciar).
 * @param n   Nome da paciência (usado no save).
 * @return    Sempre 1 (loop continua).
 */
int acaoSistema(char c, ESTADO *j, RegrasInit ri, const char *n);

/* ============================================================
 *  LOOP PRINCIPAL
 * ============================================================ */

/**
 * @brief Executa um único comando lido do stdin.
 * @param buf   Linha de comando (sem newline).
 * @param j     Estado do jogo.
 * @param rma   Lista de regras MOV/AUTO.
 * @param ri    Lista de regras INIT.
 * @param rb    Regras de baralho (passadas a acaoSistema).
 * @param rw    Lista de regras WIN.
 * @param nome  Nome da paciência.
 * @return      1 para continuar o loop, 0 para terminar ('q').
 */
int executarComando(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasInit ri,
                    RegrasBaralhos rb, RegrasWin rw, const char *nome);

/**
 * @brief Loop de leitura e execução de comandos até o utilizador sair com 'q' ou EOF.
 * @param j     Estado do jogo.
 * @param rma   Lista de regras MOV/AUTO.
 * @param ri    Lista de regras INIT.
 * @param rb    Regras de baralho.
 * @param rw    Lista de regras WIN.
 * @param nome  Nome da paciência (usado no prompt e no save).
 */
void loopComandos(ESTADO *j, RegrasMovAuto rma, RegrasInit ri,
                  RegrasBaralhos rb, RegrasWin rw, const char *nome);

/* ============================================================
 *  SETUP E ENTRY POINT
 * ============================================================ */

/**
 * @brief Cria e baralha o baralho e imprime as regras de diagnóstico.
 * @param j    Estado do jogo (j->B será preenchido).
 * @param rb   Regras de baralho (número de baralhos).
 * @param rma  Lista de regras MOV/AUTO (para diagnóstico).
 * @param rw   Lista de regras WIN (para diagnóstico).
 */
void prepararAmbiente(ESTADO *j, RegrasBaralhos rb, RegrasMovAuto rma, RegrasWin rw);

/**
 * @brief Ponto de entrada do motor de jogo.
 *        Inicializa o estado, aplica os INITs, carrega save se pedido,
 *        mostra o estado inicial e entra no loop de comandos.
 *        Liberta toda a memória antes de terminar.
 * @param rma           Lista de regras MOV/AUTO.
 * @param rj            Regra JOGO (nome da paciência).
 * @param rb            Regra BARALHOS (número de baralhos).
 * @param rt            Lista de regras TIPO.
 * @param ri            Lista de regras INIT.
 * @param rw            Lista de regras WIN.
 * @param carregar_save Se 1, tenta carregar "save.txt" antes de começar.
 */
void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb,
             RegrasTipo rt, RegrasInit ri, RegrasWin rw, int carregar_save);

#endif /* EXECUTEFILE_H */
