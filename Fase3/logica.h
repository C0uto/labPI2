/**
 * @file logica.h
 * @brief Validação e execução de movimentos, processamento AUTO e verificação de vitória.
 *
 * Toda a lógica de regras DSL (flags MOV/AUTO) está isolada aqui.
 * Cada flag tem a sua função de validação própria para manter
 * a complexidade ciclomática abaixo do limite de 10.
 */

#ifndef LOGICA_H
#define LOGICA_H

#include "readFile.h"
#include "carta.h"

/* ============================================================
 *  FLAGS
 * ============================================================ */

/**
 * @brief Verifica se a flag @p f está activa numa regra MOV/AUTO.
 * @param r  Regra a consultar (pode ser NULL).
 * @param f  Caractere da flag (ex: '<', '+', 'm').
 * @return   1 se activa, 0 caso contrário ou se @p r for NULL.
 */
int temFlag(RegrasMovAuto r, char f);

/**
 * @brief Verifica se o tipo de pilha @p tipo tem a flag '1' (máx. 1 carta) activa.
 * @param rt    Lista de regras TIPO.
 * @param tipo  Nome do tipo a verificar (ex: "CELL").
 * @return      1 se a flag '1' estiver activa, 0 caso contrário.
 */
int tipoTemFlag1(RegrasTipo rt, const char *tipo);

/* ============================================================
 *  PESQUISA DE REGRAS
 * ============================================================ */

/**
 * @brief Procura a primeira regra MOV cujos tipos origem e destino coincidem.
 * @param rma  Lista de regras MOV/AUTO.
 * @param ori  Tipo da pilha de origem (ex: "TAB").
 * @param des  Tipo da pilha de destino (ex: "FUND").
 * @return     Ponteiro para a regra encontrada, ou NULL se não existir.
 */
RegrasMovAuto encontrarRegra(RegrasMovAuto rma, const char *ori, const char *des);

/* ============================================================
 *  VALIDAÇÃO DA SEQUÊNCIA (flags internas à sequência)
 * ============================================================ */

/**
 * @brief Valida flag '+': sem ela apenas se pode mover 1 carta.
 * @param n  Número de cartas a mover.
 * @param r  Regra aplicável.
 * @return   1 se válido, 0 se n > 1 e '+' não estiver activo.
 */
int validarFlagMais(int n, RegrasMovAuto r);

/**
 * @brief Valida flag '[': sequência em ordem decrescente por valores consecutivos.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se a ordenação falhar.
 */
int validarFlagDescrescente(PILHA *ori, int n, RegrasMovAuto r);

/**
 * @brief Valida flag ']': sequência em ordem crescente por valores consecutivos.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se a ordenação falhar.
 */
int validarFlagCrescente(PILHA *ori, int n, RegrasMovAuto r);

/**
 * @brief Valida flag 'm': todas as cartas a mover têm o mesmo naipe.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se os naipes diferirem.
 */
int validarFlagMesmoNaipe(PILHA *ori, int n, RegrasMovAuto r);

/**
 * @brief Valida flag 'x': naipes alternados em pares consecutivos da sequência.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se dois consecutivos tiverem o mesmo naipe.
 */
int validarFlagNaipesAlternados(PILHA *ori, int n, RegrasMovAuto r);

/**
 * @brief Valida flag 'c': todas as cartas a mover têm a mesma cor.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se as cores diferirem.
 */
int validarFlagMesmaCor(PILHA *ori, int n, RegrasMovAuto r);

/**
 * @brief Valida flag 'd': cores alternadas em pares consecutivos da sequência.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se dois consecutivos tiverem a mesma cor.
 */
int validarFlagCoresAlternadas(PILHA *ori, int n, RegrasMovAuto r);

/**
 * @brief Agrega todas as validações internas à sequência de cartas a mover.
 * @param ori    Pilha de origem.
 * @param n      Número de cartas a mover.
 * @param regra  Regra MOV aplicável.
 * @return       1 se a sequência é válida, 0 caso contrário.
 */
int validarSequencia(PILHA *ori, int n, RegrasMovAuto regra);

/* ============================================================
 *  VALIDAÇÃO RELATIVA AO DESTINO
 * ============================================================ */

/**
 * @brief Agrega todas as validações relativas ao destino.
 *        Se flag '*' estiver activa, aceita incondicionalmente.
 * @param ori    Pilha de origem.
 * @param des    Pilha de destino.
 * @param n      Número de cartas a mover.
 * @param regra  Regra MOV aplicável.
 * @return       1 se o movimento para o destino é válido, 0 caso contrário.
 */
int validarDestino(PILHA *ori, PILHA *des, int n, RegrasMovAuto regra);

/* ============================================================
 *  VALIDAÇÃO E EXECUÇÃO COMPLETA
 * ============================================================ */

/**
 * @brief Valida completamente um movimento: índices, flag '1', regras MOV, sequência e destino.
 *        Itera por todas as regras MOV — basta uma ser satisfeita.
 * @param j    Estado do jogo.
 * @param io   Índice da pilha de origem (base 0).
 * @param id   Índice da pilha de destino (base 0).
 * @param n    Número de cartas a mover.
 * @param rma  Lista de regras MOV/AUTO.
 * @param rt   Lista de regras TIPO (para verificar flag '1').
 * @return     1 se o movimento é permitido, 0 caso contrário.
 */
int validarMovimento(ESTADO *j, int io, int id, int n, RegrasMovAuto rma, RegrasTipo rt);

/**
 * @brief Move n cartas do topo da pilha de origem para o topo da pilha de destino,
 *        sem qualquer validação. As cartas mantêm a sua ordem relativa.
 * @param j   Estado do jogo.
 * @param io  Índice da pilha de origem (base 0).
 * @param id  Índice da pilha de destino (base 0).
 * @param n   Número de cartas a mover.
 */
void executarMov(ESTADO *j, int io, int id, int n);

/**
 * @brief Valida e, se permitido, executa o movimento de n cartas.
 * @param j    Estado do jogo.
 * @param io   Índice da pilha de origem (base 0).
 * @param id   Índice da pilha de destino (base 0).
 * @param n    Número de cartas a mover.
 * @param rma  Lista de regras MOV/AUTO.
 * @param rt   Lista de regras TIPO.
 * @return     1 se o movimento foi realizado, 0 se inválido.
 */
int tentarMover(ESTADO *j, int io, int id, int n, RegrasMovAuto rma, RegrasTipo rt);

/* ============================================================
 *  AUTO
 * ============================================================ */

/**
 * @brief Tenta aplicar uma regra AUTO entre dois índices de pilha específicos.
 *        Se a flag '+' estiver activa, experimenta do maior n até 1.
 * @param j    Estado do jogo.
 * @param r    Regra AUTO a testar.
 * @param io   Índice da pilha de origem (base 0).
 * @param id   Índice da pilha de destino (base 0).
 * @param rt   Lista de regras TIPO (para verificar flag '1').
 * @return     1 se um movimento automático foi executado, 0 caso contrário.
 */
int tentarAutoEntrePilhas(ESTADO *j, RegrasMovAuto r, int io, int id, RegrasTipo rt);

/**
 * @brief Percorre todas as regras AUTO e tenta executar uma única aplicação.
 * @param j    Estado do jogo.
 * @param rma  Lista de regras MOV/AUTO.
 * @param rt   Lista de regras TIPO.
 * @return     1 se algum movimento automático foi executado, 0 caso contrário.
 */
int tentarAutoUmaVez(ESTADO *j, RegrasMovAuto rma, RegrasTipo rt);

/**
 * @brief Executa movimentos automáticos em cadeia até não existirem mais condições satisfeitas.
 *        Após cada movimento automático, mostra o estado no ecrã.
 * @param j    Estado do jogo.
 * @param rma  Lista de regras MOV/AUTO.
 * @param rt   Lista de regras TIPO.
 */
void processarAuto(ESTADO *j, RegrasMovAuto rma, RegrasTipo rt);

/* ============================================================
 *  VITÓRIA
 * ============================================================ */

/**
 * @brief Conta o total de cartas em todas as pilhas cujo tipo coincide com @p tipo.
 * @param j     Estado do jogo.
 * @param tipo  Nome do tipo de pilha (ex: "TAB").
 * @return      Total de cartas nesse tipo.
 */
int contarCartasTipo(ESTADO *j, const char *tipo);

/**
 * @brief Verifica se uma condição WIN individual está satisfeita.
 *        Compara total de cartas no tipo com condicaoWin × número de pilhas desse tipo.
 * @param j   Estado do jogo.
 * @param rw  Regra WIN a verificar.
 * @return    1 se satisfeita, 0 caso contrário.
 */
int condicaoWinSatisfeita(ESTADO *j, RegrasWin rw);

/**
 * @brief Verifica se TODAS as condições WIN estão simultaneamente satisfeitas.
 * @param j   Estado do jogo.
 * @param rw  Lista de regras WIN (conjunção).
 * @return    1 se o jogo está ganho, 0 caso contrário.
 */
int verificarVitoria(ESTADO *j, RegrasWin rw);

#endif /* LOGICA_H */
