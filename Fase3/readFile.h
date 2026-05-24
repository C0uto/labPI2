/**
 * @file readFile.h
 * @brief Definição de tipos, estruturas de regras e estado do jogo,
 *        e protótipos das funções de leitura/parsing do ficheiro DSL.
 */

#ifndef READFILE_H
#define READFILE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ========== TIPOS BASE ========== */

/** @brief Representa uma carta como inteiro de 1 a 52*n_baralhos. Valor 0 = célula vazia. */
typedef int CARTA;

/** @brief Ponteiro para array de CARTAs — representa um baralho completo. */
typedef CARTA *BARALHO;

/* ========== CÓDIGOS DE MENSAGEM ========== */

/** @brief Códigos de retorno/estado usados em todo o programa. */
typedef enum {
    OK = 0,
    ERRO_PILHA_INVALIDA,
    ERRO_PILHA_VAZIA,
    ERRO_JOGADA_INVALIDA,
    ERRO_BARALHO_VAZIO,
    WIN,
    GAME_OVER,
    Comando_INVALIDO,
    Flag_INVALIDA,
    ERRO_CAMINHO_INVALIDO
} MENSAGENS;

/** @brief Par (código, texto) usado na tabela de mensagens de feedback. */
typedef struct {
    MENSAGENS  mensagem;
    const char *feedback;
} FEEDBACK;

/* ========== ESTRUTURAS DE REGRAS ========== */

/** @brief Nó de lista duplamente ligada para regras MOV e AUTO. */
typedef struct regra {
    char *comando;      /**< "MOV" ou "AUTO". */
    char *origem;       /**< Tipo de pilha de origem. */
    char *destino;      /**< Tipo de pilha de destino. */
    char flags[2][25];  /**< [0]=chars válidos; [1]=activos ('1'/'0'). */
    struct regra *prox;
    struct regra *ant;
} *RegrasMovAuto;

/** @brief Regra JOGO — nome da paciência. */
typedef struct regra2 {
    char *comando;
    char *jogoNome;
} *RegrasJogo;

/** @brief Regra BARALHOS — número de baralhos. */
typedef struct regra3 {
    char *comando;
    int   numeroDeBaralhos;
} *RegrasBaralhos;

/** @brief Nó de lista duplamente ligada para regras TIPO. */
typedef struct regra4 {
    char *comando;
    char *tipoDePilha;
    char flags[2][6];   /**< [0]="=_^1"; [1]=activos. */
    struct regra4 *prox;
    struct regra4 *ant;
} *RegrasTipo;

/** @brief Nó de lista duplamente ligada para regras INIT. */
typedef struct regra5 {
    char *comando;
    char *tipoDePilha;
    int   numeroDeCartas;
    struct regra5 *prox;
    struct regra5 *ant;
} *RegrasInit;

/** @brief Nó de lista duplamente ligada para regras WIN. */
typedef struct regra6 {
    char *comando;
    char *tipoDePilha;
    int   condicaoWin;
    struct regra6 *prox;
    struct regra6 *ant;
} *RegrasWin;

/* ========== ESTADO DO JOGO ========== */

/**
 * @brief Representa uma pilha individual (TAB, FUND, CELL, STOCK, DESCARTE).
 */
typedef struct {
    char  *tipo;       /**< Nome do tipo (ex: "TAB"). */
    CARTA *cartas;     /**< Array dinâmico, fundo → topo. */
    int    tamanho;    /**< Cartas actualmente na pilha. */
    int    capacidade; /**< Capacidade alocada. */
} PILHA;

/**
 * @brief Snapshot de um estado passado, usado para UNDO.
 */
typedef struct snapshot {
    int     num_pilhas;  /**< Número de pilhas. */
    CARTA **cartas;      /**< Cópia dos arrays de cartas. */
    int    *tamanhos;    /**< Cópia dos tamanhos. */
    struct snapshot *prox; /**< Snapshot anterior na pilha de histórico. */
} SNAPSHOT;

/**
 * @brief Estado global do jogo em curso.
 */
typedef struct {
    PILHA    *pilhas;               /**< Array de todas as pilhas. */
    int       num_pilhas;           /**< Número total de pilhas. */
    BARALHO   B;                    /**< Baralho completo (para reiniciar). */
    int       total_cartas_baralho; /**< Total de cartas no baralho. */
    RegrasTipo rt;                  /**< Regras TIPO (para visibilidade no display). */
    SNAPSHOT  *historico;           /**< Pilha de snapshots para UNDO. */
} ESTADO;

/* ========== PROTÓTIPOS readFile ========== */

/**
 * @brief Apresenta no ecrã a mensagem associada ao código dado.
 * @param cod Código MENSAGENS a apresentar.
 */
void mostrar_mensagem(MENSAGENS cod);

/**
 * @brief Abre e lê o ficheiro DSL, populando todas as listas de regras.
 * @param nome    Nome do ficheiro (sem o prefixo "paciencias/").
 * @param listaMA Lista MOV/AUTO a preencher.
 * @param listaJ  Estrutura JOGO a preencher.
 * @param listaB  Estrutura BARALHOS a preencher.
 * @param listaT  Lista TIPO a preencher.
 * @param listaI  Lista INIT a preencher.
 * @param listaW  Lista WIN a preencher.
 * @return OK em sucesso, código de erro caso contrário.
 */
MENSAGENS abreFicheiro(char *nome, RegrasMovAuto *listaMA, RegrasJogo *listaJ,
                        RegrasBaralhos *listaB, RegrasTipo *listaT,
                        RegrasInit *listaI, RegrasWin *listaW);

/**
 * @brief Lista ficheiros .txt da pasta "paciencias/" e pede ao utilizador a sua escolha.
 * @param nome Buffer de destino (mínimo 100 bytes).
 * @return 0 em sucesso, 1 se a pasta não for encontrada.
 */
int abrirPastaImprime(char *nome);

#endif /* READFILE_H */
