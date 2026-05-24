#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include "executeFile.h"

int main () {
    RegrasMovAuto listaMA = NULL;
    RegrasJogo listaJ = malloc(sizeof(struct regra2));
    RegrasBaralhos listaB = malloc(sizeof(struct regra3));
    RegrasTipo listaT = NULL;
    RegrasInit listaI = NULL;
    RegrasWin listaW = NULL;
    char nome[100];
    int m = abrirPastaImprime(nome);
    if(m==1) return 1;
    MENSAGENS a = abreFicheiro(nome,&listaMA,&listaJ,&listaB,&listaT,&listaI,&listaW);
    if (a == OK) {
        execute(listaMA, listaJ, listaB, listaT, listaI, listaW);
    } else {
        printf("Erro ao processar o ficheiro de regras (Codigo: %d).\n", a);
    }

    return 0;
}