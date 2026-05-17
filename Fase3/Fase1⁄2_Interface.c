//commands.h

#include "card.h"
#define MAX_BUF 10240

void executar(char *line, ESTADO *jogo);
void estado(ESTADO *jogo);

//commands.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

typedef struct {
        char *cmd;
        void (*fun)(ESTADO *jogo);
} COMANDO;

const FEEDBACK tabela_mensagens[] = {
        {OK, "\n"},
        {ERRO_PILHA_INVALIDA, "Pilha invalida, tenta novamente."},
        {ERRO_PILHA_VAZIA, "A pilha seleciona esta vazia."},
        {ERRO_JOGADA_INVALIDA, "Jogada invalida!"},
        {ERRO_BARALHO_VAZIO, "Nao ha mais cartas no baralho"},
        {WIN, ">>>>>>>>>>>>PARABENS! venceu o jogo, para sair use 'q' para reiniciar use 'r'."},
        {GAME_OVER, ">>>>> GAME OVER! para sair use 'q', para reiniciar use 'r'."},
        {0, NULL}
};

void mostrar_mensagem(MENSAGENS cod) {
        for(int i = 0; tabela_mensagens[i].feedback != NULL; i++) {
                if(tabela_mensagens[i].mensagem == cod) {
                        printf("\n%s\n", tabela_mensagens[i].feedback);
                        return;
                }
        }
}

//funcao executada apos o comando 'estado' para mostrar o estado do jogo atual
void estado(ESTADO *jogo) {
     printf("\n===================================================\n");
     printf("Fundacao atual: %s\n", card2str(jogo->fundacao));
     printf("Cartas no baralho: %d\n\n", jogo->cartas_no_baralho);
     printf("---------------------------------------------------\n");
     printf("Pilhas:\n");
     print_pile(jogo->golf);
     printf("===================================================\n");
}

int eNumero (char a) {
        return(a>=48 && a<=57);
}
//funcao executada apos o comando 'pilha' para selecionar uma pilha donde queremos retirar uma carta
void pilha(ESTADO *jogo) {
        char input[MAX_BUF];

        if(fgets(input, MAX_BUF, stdin) == NULL) return;
        input[strlen(input) - 1] = 0; // limpar o enter

        if(input[0] == 'p' && eNumero(input[2]) && eNumero(input[4]) && eNumero(input[6]))
        MENSAGENS mensagem = update_jogo(jogo, atoi(input[2]), atoi(input[4]), atoi(input[6])); // exemplo: o utilizador escreve "p 1 2 5" o imput[2]='1' (pilha de origem) e o imput[4]='2' (pilha de destino) imput[6]='5' (5 cartas da pilha de origem a ser movidas)

        if(mensagem != OK){
                mostrar_mensagem(mensagem);
        }else{
                estado(jogo);
        }

        MENSAGENS mensagem_final = jogoNaoAcabou(jogo);

        if(mensagem_final != OK){
                mostrar_mensagem(mensagem_final);
        }
}

// funcao executada apos o comando 'baralho' para retirar uma carta do baralho e colocar a mesma na fundacao
void baralho(ESTADO *jogo) {
        MENSAGENS mensagem = update_fundacao_com_baralho(jogo);

        if(mensagem != OK){
                mostrar_mensagem(mensagem);
        }else{
                estado(jogo);
        }

        MENSAGENS mensagem_final = jogoNaoAcabou(jogo);

        if(mensagem_final != OK){
                mostrar_mensagem(mensagem_final);
        }
}

// funcao executada apos o comando 'quit' para sair do jogo
void quit(ESTADO *jogo) {
        printf("Obrigado por jogar!\n");
        printf("Feito por Nuno Couto a113253, Tobias Rouquette a115972 e a115351\n");
        exit(0);
}

//funcao para reiniciar o jogo
void reset(ESTADO *jogo) {
        iniciar_jogo(jogo);

        printf("\n=================JOGO REINICIADO!==================\n");
        estado(jogo);
}

//funcao executada apos o comando 'help' para mostrar os comandos permitidos neste jogo
void help(ESTADO *jogo){
        printf("Comandos permitidos: \n");
        printf("p ------- Comando que permite selecionar uma pilha de onde queremos retirar uma carta.\n");
        printf("b ----- Comando que permite ir buscar a carta do topo do baralho e colocar na fundacao.\n");
        printf("e ------ Comando que permite obter o estado atual do jogo, mostrando a carta que esta na fundacao as pilhas e o numero de cartas que restam no baralho.\n");
        printf("r ------- Comando que permite o utilizador reiniciar o jogo.\n");
        printf("h -------- Comando para obter informacao sobre os comandos permitidos\n");
        printf("q -------- Comando que permite o utilizador sair do jogo.\n");
}

// funcao que executa os comandos introduzidos pelo utilizador
void executar(char *command, ESTADO *jogo) {
        COMANDO cmd[] = {
                {"p", pilha},
                {"b", baralho},
                {"q", quit},
                {"e", estado},
                {"r",reset},
                {"h", help},
                {NULL, NULL},
        };

        for(int I = 0; cmd[I].cmd != NULL; I++) {
                if(strcmp(command, cmd[I].cmd) == 0) {
                        cmd[I].fun(jogo);
                        return;
                }
        }
        printf("Comando desconhecido!\n");
}