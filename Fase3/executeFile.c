#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readFile.h>

// ========== FUNÇÕES AUXILIARES ==========

void imprimirTipos(RegrasTipo rt) {
    while (rt != NULL) {
        printf("   - TIPO %s %s\n", rt->tipoDePilha, rt->flags[0]);
        rt = rt->prox;
    }
}

void imprimirInits(RegrasInit ri) {
    while (ri != NULL) {
        printf("   - INIT %s %d\n", ri->tipoDePilha, ri->numeroDeCartas);
        ri = ri->prox;
    }
}

void imprimirMovs(RegrasMovAuto rma) {
    while (rma != NULL) {
        printf("   - %s %s %s %s\n", rma->comando, rma->origem, rma->destino, rma->flags[0]);
        rma = rma->prox;
    }
}

void imprimirWins(RegrasWin rw) {
    while (rw != NULL) {
        printf("   - WIN %s %d\n", rw->tipoDePilha, rw->condicaoWin);
        rw = rw->prox;
    }
}

// ========== FUNÇÕES DE EXECUÇÃO DAS REGRAS ==========

void aplicarJogo(RegrasJogo rj) {
    if (rj == NULL || rj->comando == NULL) {
        printf("ERRO: Regra JOGO não definida!\n");
        return;
    }
    printf("1. [JOGO] %s\n", rj->jogoNome);
}

void aplicarBaralhos(RegrasBaralhos rb) {
    if (rb == NULL || rb->comando == NULL) {
        printf("ERRO: Regra BARALHOS não definida!\n");
        return;
    }
    printf("2. [BARALHOS] Criando %d baralho(s)\n", rb->numeroDeBaralhos);
}

void aplicarTipo(RegrasTipo rt) {
    if (rt == NULL) {
        printf("   [TIPO] Nenhum tipo definido\n");
        return;
    }
    printf("3. [TIPO] Tipos de pilhas:\n");
    imprimirTipos(rt);
}

void aplicarInit(RegrasInit ri) {
    if (ri == NULL) {
        printf("   [INIT] Nenhuma inicialização definida\n");
        return;
    }
    printf("4. [INIT] Inicializando pilhas:\n");
    imprimirInits(ri);
}

void aplicarMovAuto(RegrasMovAuto rma) {
    if (rma == NULL) {
        printf("   [MOV/AUTO] Nenhum movimento definido\n");
        return;
    }
    printf("5. [MOV/AUTO] Movimentos automáticos:\n");
    imprimirMovs(rma);
}

void aplicarWin(RegrasWin rw) {
    if (rw == NULL) {
        printf("   [WIN] Nenhuma condição de vitória definida\n");
        return;
    }
    printf("6. [WIN] Condições de vitória:\n");
    imprimirWins(rw);
}

// ========== FUNÇÃO PRINCIPAL DE EXECUÇÃO ==========

void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb, 
             RegrasTipo rt, RegrasInit ri, RegrasWin rw) {
    printf("\n========== APLICANDO REGRAS DO JOGO ==========\n\n");
    aplicarJogo(rj);
    printf("\n");
    aplicarBaralhos(rb);
    printf("\n");
    aplicarTipo(rt);
    printf("\n");
    aplicarInit(ri);
    printf("\n");
    aplicarMovAuto(rma);
    printf("\n");
    aplicarWin(rw);
    printf("\n");
    printf("========== REGRAS APLICADAS ==========\n\n");
}
