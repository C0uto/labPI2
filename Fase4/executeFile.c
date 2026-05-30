#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "carta.h"
#include "logica.h"
#include "estado.h"
#include "display.h"
#include "executeFile.h"


/**
 * Funcao para imprimir no terminal a lista de comandos disponiveis
 *
 */
void comandoH(void) {
    printf("\nComandos:\n");
    printf("  p <origem> <destino> [n] - Move n cartas (default 1) da pilha oirgem para a pilha destino\n");
    printf("  u             - Desfaz a ultima jogada (undo)\n");
    printf("  e             - Mostra o estado actual\n");
    printf("  s             - Grava o jogo\n");
    printf("  r             - Reinicia o jogo\n");
    printf("  h             - Mostra esta ajuda\n");
    printf("  q             - Sair do jogo\n");
}

/**
 * Funcao que processa o comando de movimento 'p', validando e guardando snapshot
 *
 * * @param buf -> String lida do input
 * * @param j -> Estado do jogo
 * * @param rma -> Regras de movimento
 * * @param rw -> Regras de vitoria
 */
void comandoP(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasWin rw) {
    int o, d, n = 1;
    if (sscanf(buf, " p %d %d %d", &o, &d, &n) < 2) {
        printf("Uso: p <origem> <destino> [n]\n");
        return;
    }
    guardarSnapshot(j);
    if (!tentarMover(j, o - 1, d - 1, n, rma, j->rt)) {
        /* movimento inválido: descartar o snapshot que acabámos de guardar */
        SNAPSHOT *s = j->historico;
        j->historico = s->prox;
        libertarSnapshot(s);
        printf("Movimento invalido!\n");
    } else {
        printf("\n[MOV] Movimento executado: %d -> %d (%d cartas)\n", o, d, n);
        mostrarEstado(j);
        processarAuto(j, rma, j->rt);
        if (verificarWin(j, rw)) mostrar_mensagem(WIN);
    }
}


/**
 * Funcao para tratar comandos de sistema (save, reset)
 *
 * * @param c -> Carater do comando
 * * @param j -> Estado
 * * @param ri -> Regras Init
 * * @param n -> Nome da paciencia
 * * @return res -> 1 para continuar o programa
 */
int comandoSR(char c, ESTADO *j, RegrasInit ri, const char *n) {
    if (c == 's') gravarJogo(j, n);
    else if (c == 'r') { aplicarInitAoEstado(j, ri, j->B); mostrarEstado(j); }
    return 1;
}

/**
 * Funcao wrapper para o comando de undo
 *
 * * @param j -> Estado do jogo
 */
void comandoU(ESTADO *j) {
    if (restaurarSnapshot(j)) mostrarEstado(j);
}

/**
 * Funcao que faz o parsing e encaminha os comandos do utilizador para as funcoes certas
 *
 * * @param buf -> Linha de comando
 * * @param j -> Estado
 * * @param rma -> Regras Mov
 * * @param ri -> Regras Init
 * * @param rb -> Regras Baralho
 * * @param rw -> Regras Win
 * * @param nome -> Nome paciencia
 * * @return res -> 1 se o loop deve continuar, 0 se deve sair
 */
int executarComando(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasInit ri,
                    RegrasBaralhos rb, RegrasWin rw, const char *nome) {
    char c = buf[0];
    if (c == 'q') return 0;
    if (c == 'h') comandoH();
    else if (c == 'e') mostrarEstado(j);
    else if (c == 'p') comandoP(buf, j, rma, rw);
    else if (c == 'u') comandoU(j);
    else if (strchr("sr", c)) comandoSR(c, j, ri, nome);
    else printf("Comando desconhecido.\n");
    return 1;
}

/**
 * Funcao principal de ciclo de vida da interface de linha de comando
 *
 * * @param j -> Estado
 * * @param rma -> Regras Mov
 * * @param ri -> Regras Init
 * * @param rb -> Regras Baralho
 * * @param rw -> Regras Win
 * * @param nome -> Nome paciencia
 */
void loopComandos(ESTADO *j, RegrasMovAuto rma, RegrasInit ri,
                  RegrasBaralhos rb, RegrasWin rw, const char *nome) {
    char buf[256];
    int continuar = 1;
    int input_ok = 1;
    while (continuar && input_ok) {
        const char *display_nome = (nome) ? nome : "Paciencia";
        printf("%s> ", display_nome);
        if (fgets(buf, 256, stdin) == NULL) {
            input_ok = 0; // Sinaliza que não houve mais entrada
        } else {
            buf[strcspn(buf, "\n")] = '\0';
            if (buf[0] != '\0')
                continuar = executarComando(buf, j, rma, ri, rb, rw, nome);
        }
    }
}


/**
 * Funcao que configura os elementos iniciais do jogo antes de comecar a interacao
 *
 * * @param j -> Estado
 * * @param rb -> Regras Baralho
 * * @param rma -> Regras Mov
 * * @param rw -> Regras Win
 */
void prepararAmbiente(ESTADO *j, RegrasBaralhos rb, RegrasMovAuto rma, RegrasWin rw) {
    int n_baralhos = (rb) ? rb->numeroDeBaralhos : 1;
    j->B = criarBaralho(n_baralhos);
    j->total_cartas_baralho = 52 * n_baralhos;
    baralharBaralho(j->B, n_baralhos);
    printf("Movimentos validos:\n"); imprimirMovs(rma);
    printf("Condicoes de vitoria:\n"); imprimirWins(rw);
}

/**
 * Funcao principal que orquestra a execucao do motor de jogo
 *
 * * @param rma -> Regras Mov
 * * @param rj -> Regras Jogo
 * * @param rb -> Regras Baralho
 * * @param rt -> Regras Tipo
 * * @param ri -> Regras Init
 * * @param rw -> Regras Win
 * * @param carregar_save -> Flag para saber se deve carregar de ficheiro
 */
void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb,
             RegrasTipo rt, RegrasInit ri, RegrasWin rw, int carregar_save) {
    ESTADO jogo;
    memset(&jogo, 0, sizeof(ESTADO));
    jogo.rt = rt;
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
