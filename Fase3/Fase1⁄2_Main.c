#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "commands.h"


int main() {
        srandom(time(NULL));
        ESTADO jogo;
        iniciar_jogo(&jogo);

        estado(&jogo);

        while(1) {
                char command[MAX_BUF];
                printf("GOLF> ");
                if(fgets(command, MAX_BUF, stdin) == NULL){
                        return 0;
                }
                assert(command[strlen(command) - 1] == '\n');
                command[strlen(command) - 1] = 0;
                executar(command, &jogo);
        }
        return 0;
}