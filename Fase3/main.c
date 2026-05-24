#include <stdio.h>
#include <stdlib.h>
#include "executeFile.h"

int main(void) {
    RegrasMovAuto listaMA = NULL;
    RegrasJogo    listaJ  = malloc(sizeof(struct regra2));
    RegrasBaralhos listaB = malloc(sizeof(struct regra3));
    RegrasTipo    listaT  = NULL;
    RegrasInit    listaI  = NULL;
    RegrasWin     listaW  = NULL;
    char nome[100];

    if (abrirPastaImprime(nome) != 0) return 1;

    MENSAGENS a = abreFicheiro(nome, &listaMA, &listaJ, &listaB,
                                &listaT, &listaI, &listaW);
    if (a == OK)
        execute(listaMA, listaJ, listaB, listaT, listaI, listaW, 0);
    else
        printf("Erro ao processar ficheiro (codigo: %d).\n", a);

    return 0;
}
