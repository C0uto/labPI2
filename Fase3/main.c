#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include "executeFile.h"

/**
 * Funcao principal (Entry Point) que inicializa o contexto e arranca o jogo
 *
 * * @return res -> 0 se o programa terminou normalmente
 */
int main() {
    CONTEXTO ctx = inicializarContexto();
    char nome[100];
    int carregar_save = 0;
    if (carregarNome(nome, &carregar_save)) return 1;
    MENSAGENS a = abreFicheiro(nome, &ctx.ma, &ctx.rj, &ctx.rb, &ctx.rt, &ctx.ri, &ctx.rw);
    if (a == OK) execute(ctx.ma, ctx.rj, ctx.rb, ctx.rt, ctx.ri, ctx.rw, carregar_save);
    else printf("Erro ao processar o ficheiro de regras (Codigo: %d).\n", a);
    return 0;
}