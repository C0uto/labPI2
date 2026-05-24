/**
 * @file executeFile.c
 * @brief Implementação do loop principal e dos comandos do utilizador.
 *
 * Coordena carta, logica, estado e display.
 * Cada comando do utilizador é tratado por uma função dedicada,
 * mantendo a complexidade ciclomática abaixo de 10.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "executeFile.h"

/* ============================================================
 *  COMANDOS DO UTILIZADOR
 * ============================================================ */

/**
 * @brief Apresenta a lista de comandos disponíveis.
 */
void tratarAjuda(void) {
    printf("\nComandos:\n");
    printf("  p <o> <d> [n] - Move n cartas (default 1) da pilha o para d\n");
    printf("  u             - Desfaz a ultima jogada (undo)\n");
    printf("  e             - Mostra o estado actual\n");
    printf("  s             - Grava o jogo\n");
    printf("  l             - Carrega jogo gravado\n");
    printf("  r             - Reinicia\n");
    printf("  h             - Esta ajuda\n");
    printf("  q             - Sair\n");
}

/**
 * @brief Descarta o snapshot do topo do histórico (movimento inválido).
 * @param j  Estado do jogo.
 */
static void descartarSnapshot(ESTADO *j) {
    SNAPSHOT *s  = j->historico;
    j->historico = s->prox;
    libertarSnapshot(s);
}

/**
 * @brief Interpreta e executa o comando 'p'.
 *        Guarda snapshot antes; descarta-o se o movimento for inválido.
 * @param buf  Linha de comando (ex: "p 3 7 2").
 * @param j    Estado do jogo.
 * @param rma  Lista de regras MOV/AUTO.
 * @param rw   Lista de regras WIN.
 */
void tratarMover(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasWin rw) {
    int o, d, n = 1;
    if (sscanf(buf, " p %d %d %d", &o, &d, &n) < 2) {
        printf("Uso: p <origem> <destino> [n]\n");
        return;
    }
    guardarSnapshot(j);
    if (!tentarMover(j, o - 1, d - 1, n, rma, j->rt)) {
        descartarSnapshot(j);
        printf("Movimento invalido!\n");
    } else {
        printf("\n[MOV] Movimento executado: %d -> %d (%d cartas)\n", o, d, n);
        mostrarEstado(j);
        processarAuto(j, rma, j->rt);
        if (verificarVitoria(j, rw)) mostrar_mensagem(WIN);
    }
}

/**
 * @brief Executa o comando 'u': restaura o estado anterior e mostra-o.
 * @param j  Estado do jogo.
 */
void tratarUndo(ESTADO *j) {
    if (restaurarSnapshot(j)) mostrarEstado(j);
}

/**
 * @brief Executa acções de sistema: 's' (save), 'l' (load), 'r' (reiniciar).
 * @param c   Caractere do comando.
 * @param j   Estado do jogo.
 * @param ri  Lista de regras INIT (para reiniciar).
 * @param n   Nome da paciência (para o save).
 * @return    Sempre 1 (loop continua).
 */
int acaoSistema(char c, ESTADO *j, RegrasInit ri, const char *n) {
    if (c == 's') gravarJogo(j, n);
    else if (c == 'l') { carregarJogo(j); mostrarEstado(j); }
    else if (c == 'r') { aplicarInitAoEstado(j, ri, j->B); mostrarEstado(j); }
    return 1;
}

/* ============================================================
 *  LOOP PRINCIPAL
 * ============================================================ */

/**
 * @brief Executa um único comando.
 * @param buf   Linha de comando (sem newline).
 * @param j     Estado do jogo.
 * @param rma   Lista de regras MOV/AUTO.
 * @param ri    Lista de regras INIT.
 * @param rb    Regras de baralho (não usado directamente aqui).
 * @param rw    Lista de regras WIN.
 * @param nome  Nome da paciência.
 * @return      1 para continuar, 0 para terminar ('q').
 */
int executarComando(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasInit ri,
                    RegrasBaralhos rb, RegrasWin rw, const char *nome) {
    char c = buf[0];
    if (c == 'q') return 0;
    if (c == 'h') tratarAjuda();
    else if (c == 'e') mostrarEstado(j);
    else if (c == 'p') tratarMover(buf, j, rma, rw);
    else if (c == 'u') tratarUndo(j);
    else if (strchr("slr", c)) acaoSistema(c, j, ri, nome);
    else printf("Comando desconhecido. Use 'h' para ajuda.\n");
    return 1;
    (void)rb;
}

/**
 * @brief Loop de leitura e execução de comandos até 'q' ou EOF.
 * @param j     Estado do jogo.
 * @param rma   Lista de regras MOV/AUTO.
 * @param ri    Lista de regras INIT.
 * @param rb    Regras de baralho.
 * @param rw    Lista de regras WIN.
 * @param nome  Nome da paciência (prompt e save).
 */
void loopComandos(ESTADO *j, RegrasMovAuto rma, RegrasInit ri,
                  RegrasBaralhos rb, RegrasWin rw, const char *nome) {
    char buf[256];
    int continuar = 1;
    const char *display = (nome) ? nome : "Paciencia";
    while (continuar) {
        printf("%s> ", display);
        if (fgets(buf, 256, stdin) == NULL) break;
        buf[strcspn(buf, "\n")] = '\0';
        if (buf[0] != '\0')
            continuar = executarComando(buf, j, rma, ri, rb, rw, nome);
    }
}

/* ============================================================
 *  SETUP E ENTRY POINT
 * ============================================================ */

/**
 * @brief Cria e baralha o baralho; imprime regras de diagnóstico.
 * @param j    Estado do jogo (j->B será preenchido).
 * @param rb   Regras de baralho.
 * @param rma  Lista de regras MOV/AUTO.
 * @param rw   Lista de regras WIN.
 */
void prepararAmbiente(ESTADO *j, RegrasBaralhos rb, RegrasMovAuto rma, RegrasWin rw) {
    int n = (rb) ? rb->numeroDeBaralhos : 1;
    j->B = criarBaralho(n);
    j->total_cartas_baralho = 52 * n;
    baralharBaralho(j->B, n);
    printf("Movimentos validos:\n");   imprimirMovs(rma);
    printf("Condicoes de vitoria:\n"); imprimirWins(rw);
}

/**
 * @brief Ponto de entrada do motor de jogo.
 * @param rma           Lista de regras MOV/AUTO.
 * @param rj            Regra JOGO.
 * @param rb            Regra BARALHOS.
 * @param rt            Lista de regras TIPO.
 * @param ri            Lista de regras INIT.
 * @param rw            Lista de regras WIN.
 * @param carregar_save Se 1, carrega "save.txt" antes de começar.
 */
void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb,
             RegrasTipo rt, RegrasInit ri, RegrasWin rw, int carregar_save) {
    ESTADO jogo;
    memset(&jogo, 0, sizeof(ESTADO));
    jogo.rt       = rt;
    jogo.historico = NULL;

    const char *nome = (rj && rj->jogoNome) ? rj->jogoNome : "Jogo Desconhecido";
    printf("\n=== %s ===\n", nome);
    prepararAmbiente(&jogo, rb, rma, rw);
    aplicarInitAoEstado(&jogo, ri, jogo.B);
    if (carregar_save) carregarJogo(&jogo);
    mostrarEstado(&jogo);
    loopComandos(&jogo, rma, ri, rb, rw, nome);
    limparEstado(&jogo);
}
