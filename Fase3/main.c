#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include "executeFile.h"

void inicializarListas(RegrasMovAuto *ma, RegrasJogo *rj, RegrasBaralhos *rb,
                       RegrasTipo *rt, RegrasInit *ri, RegrasWin *rw) {
    *ma = NULL;
    *rj = malloc(sizeof(struct regra2));
    *rb = malloc(sizeof(struct regra3));
    *rt = NULL;
    *ri = NULL;
    *rw = NULL;
}

int main () {
    RegrasMovAuto listaMA;
    RegrasJogo listaJ;
    RegrasBaralhos listaB;
    RegrasTipo listaT;
    RegrasInit listaI;
    RegrasWin listaW;
    inicializarListas(&listaMA, &listaJ, &listaB, &listaT, &listaI, &listaW);
    char nome[100];
    int carregar_save = 0;
    if (abrirPastaImprime(nome, &carregar_save)) return 1;
    if (carregar_save && !lerNomePacienciaDoSave(nome)) {
        printf("Erro: nao foi possivel ler o nome da paciencia do save.txt\n");
        return 1;
    }
    MENSAGENS a = abreFicheiro(nome, &listaMA, &listaJ, &listaB, &listaT, &listaI, &listaW);
    if (a == OK) execute(listaMA, listaJ, listaB, listaT, listaI, listaW, carregar_save);
    else printf("Erro ao processar o ficheiro de regras (Codigo: %d).\n", a);
    return 0;
}