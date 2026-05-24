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
    listaJ->comando = NULL;
    listaJ->jogoNome = NULL;
    listaB->comando = NULL;
    listaB->numeroDeBaralhos = 0;
    DIR *d = opendir("paciencias");
    if (!d) {
        printf("Erro: pasta 'paciencias' não encontrada.\n");
        return 1;
    }
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        int len = strlen(dir->d_name);
        if (len > 10 && strcmp(dir->d_name + len - 10, ".paciencia") == 0)
        printf("%s\n", dir->d_name);
    }
    closedir(d);
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