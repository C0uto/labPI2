//Card.h

#include <string.h>
typedef char CARTA;
typedef CARTA *BARALHO;
typedef struct {
        CARTA simon[10][21]; //pilha iniciais
        CARTA simonFoundation[4][14]; //pilhas por naipe
}ESTADO;

// enumeracao das feedback messages
typedef enum {
        OK,
        ERRO_PILHA_INVALIDA,
        ERRO_PILHA_VAZIA,
        ERRO_JOGADA_INVALIDA,
        ERRO_BARALHO_VAZIO,
        WIN,
        GAME_OVER
} MENSAGENS;

// estrutura para associar a numeracao com uma mensagem
typedef struct {
        MENSAGENS mensagem;
        const char *feedback;
} FEEDBACK;

#define tam_pilha strlen


BARALHO create_deck();
void baralhar(BARALHO B);
void print_pile(CARTA [7][6]);
const char *card2str(CARTA card);
void golf_init(BARALHO B, CARTA pilha[7][6]);
int pile_length(CARTA colunm[6]);
MENSAGENS update_jogo(ESTADO *jogo, int column);
MENSAGENS update_fundacao_com_baralho(ESTADO *jogo);
int validar_jogada(CARTA fundacao, CARTA pilha);
void iniciar_jogo(ESTADO *estado);
MENSAGENS jogoNaoAcabou (ESTADO *jogo);
int pilhasVazias (CARTA golf[7][6]);
int naoHaJogadasPossiveis (CARTA golf[7][6],int estoque,CARTA fundacaoTopo);

//Card.c

#include <stdlib.h>
#include <stdio.h>
#include "card.h"

#define FOR(var, limite) for(int var = 0; var < limite; var++)

//funcao para assignar valores iniciais as variaveis da estrutura ESTADO
void iniciar_jogo(ESTADO *estado) {

        estado->B = create_deck();
        baralhar(estado->B);

        golf_init(estado->B, estado->golf);

        estado->fundacao = estado->B[35];
        estado->posicao_topo_do_baralho = 36;
        estado->cartas_no_baralho = 16;
}

int mesmoNaipe (Carta a, Carta b) {
        int z = 0;
        if((a<=13 && b<=13) || 
           (a<=26 && b<=26 && a>=14 && b>=14) || 
           (a<=39 && b<=39 && a>=27 && b>=27) || 
           (a<=52 && b<=52 && a>=40 && b>=40)) z=1;
        return z;
}

void moverSequenciaFeita (ESTADO *jogo) {
        int k = jogo -> proxFoundation;
        for(int i = 0; i<10; i++) {
                if(verificaPilha(jogo, i, 13)) {
                        int length_da_pilha_i = pile_length(jogo->simon[i]);
                        for(int z = 0, p=13; p >= 1 ; z++,p--) {
                                jogo -> simonFoundation[k][z] = jogo -> simon[i][length_da_pilha_i - p];
                                jogo -> simon[i][length_da_pilha_i - p] = 0;
                        }
                        k++;
                }
        }
        jogo -> proxFoundation = k;
}

int verificaPilha (ESTADO *jogo, int column, int numCartaColumn) {
        int length_da_pilha = pile_length(jogo->golf[column]);
        if(length_da_pilha >= numCartaColumn) {
                int x = jogo->golf[column][length_da_pilha-1] % 13;
                int y = jogo->golf[column][length_da_pilha-1];
                if(numCartaColumn > 1) {
                   for(int i = 2; i<=numCartaColumn; i++) {
                        if(x + 1 == jogo->golf[column][length_da_pilha-i] % 13 && mesmoNaipe(y, jogo->golf[column][length_da_pilha-i])) x = jogo->golf[column][length_da_pilha-i] % 13;
                        else return 0;
                   }
                }
        }
        else return 0;
        return 1;
}

//funcao que atualiza o estado do jogo apos uma jogada
MENSAGENS update_jogo(ESTADO *jogo,int column, int column2, int numCartaColumn) {
        column--;
        if(column < 0 || column > 9 || column2 < 0 || column2 > 9){
                return ERRO_PILHA_INVALIDA;
        }
        int length_da_pilha = pile_length(jogo->golf[column]);
        int length_da_pilha2 = pile_length(jogo->golf[column2]);
        if (length_da_pilha == 0 || length_da_pilha2 == 0){
                return ERRO_PILHA_VAZIA;
        }else{
                if(validar_jogada(jogo->golf[column][length_da_pilha - 1], jogo->golf[column2][length_da_pilha - 1]) && verificaPilha (jogo, column, numCartaColumn)){
                        jogo->golf[column2][length_da_pilha] = jogo->golf[column][length_da_pilha - 1];
                        jogo->golf[column][length_da_pilha - 1] = 0;
                        return OK;
                }else{
                        return ERRO_JOGADA_INVALIDA;
                }
        }
}

//funcao para atualizar a fundacao com a carta do topo do baralho
MENSAGENS update_fundacao_com_baralho(ESTADO *jogo){
        if(jogo->cartas_no_baralho == 0){
                return ERRO_BARALHO_VAZIO;
        }else{
                jogo->fundacao = jogo->B[jogo->posicao_topo_do_baralho];
                jogo->cartas_no_baralho--;
                jogo->posicao_topo_do_baralho++;
                return OK;
        }
}

//funcao para validar se uma jogada e valida
int validar_jogada(CARTA fundacao, CARTA pilha){
        int valor_pilha = (pilha - 1) % 13;
        int valor_fundacao = (fundacao - 1) % 13;

        if((valor_pilha - valor_fundacao) == 1){
                return 1;
        }else{
                return 0;
        }
}

//funcao para calcular a quantidade de cartas numa certa pilha
int pile_length(CARTA column[6]) {
        int counter = 0;
        while(counter < 6 && column[counter] != 0){
                counter++ ;
        }
        return counter;
}

//funcao para imprimir na linha de comandos as pilhas de cima para baixo
void print_pile(CARTA golf[10][9]){
        for(int line = 0; line < 9; line++){
                for(int pile = 0; pile < 10; pile++){
                        if(golf[pile][line] > 0){
                                printf(" %s\t", card2str(golf[pile][line]));
                        }else{
                                printf(" \t");
                        }
                }
                printf("\n");
        }
}


int pilhasVazias (CARTA golf[10][9]) {
    int z = 0;
    for(int i = 0; i < 10; i++) {
        if (golf[i][0] == 0) z += 0;
        else z += 1;
    }
    return (z==0);
}

int naoHaJogadasPossiveis (CARTA golf[10][9], int estoque, CARTA fundacaoTopo) {
    int y = 0;
    for(int i = 0; i < 10; i++) {
                if (!validar_jogada(fundacaoTopo,golf[i][pile_length(golf[i])-1]) && estoque==0) y+=0;
                else y+=1;
    }
    return (y==0);
}

MENSAGENS jogoNaoAcabou (ESTADO *jogo) {
    if (pilhasVazias (jogo->golf)) {
        return WIN;
    }
    if (naoHaJogadasPossiveis (jogo->golf, jogo->cartas_no_baralho, jogo->fundacao)) {
        return GAME_OVER;
    }
    else return OK;

}

void golf_init(BARALHO B, CARTA pilha[10][9]) {
        FOR(P, 10)
                FOR(I, 9)
                        pilha[P][I] = 0;

        FOR(I, 35) {
                int p = I / 5;
                int i = I % 5;
                pilha[p][i] = B[I];
        }
}

//funcao para randomizar as cartas no baralho
void baralhar(BARALHO B) {
        for(int N = 0; N < 100; N++) {
                int I = random() % 52;
                int J = random() % 52;

                CARTA T = B[I];
                B[I] = B[J];
                B[J] = T;
        }
}

BARALHO create_deck() {
        static CARTA B[52];
        for(int I = 0; I < 52; I++)
                B[I] = I + 1;
        return B;
}

const char *card2str(CARTA card) {
        char *valor = "A23456789TJQK";
        char *naipe = "SHDC";

        if(card == 0)
                return NULL;

        int v = (card - 1 ) % 13;
        int n = (card - 1) / 13;
        static char res[3] = {0};
        res[0] = valor[v];
        res[1] = naipe[n];
        return res;
}

/**
 * Verifica se existe pelo menos um movimento valido possivel
 * entre qualquer par de colunas, para qualquer tamanho de sequencia
 */
int naoHaJogadasPossiveis(CARTA simon[10][21]) {
    for (int orig = 0; orig < 10; orig++) {
        int tam_orig = pile_length(simon[orig]);
        if (tam_orig == 0) continue;
        for (int dest = 0; dest < 10; dest++) {
            if (orig == dest) continue;
            int tam_dest = pile_length(simon[dest]);
            if (tam_dest == 0) return 0;
            for (int m = 1; m <= tam_orig; m++) {
                int naipe_a = (simon[orig][tam_orig - m]     - 1) / 13;
                int naipe_b = (simon[orig][tam_orig - m + 1] - 1) / 13;
                int val_fundo = (simon[orig][tam_orig - m]   - 1) % 13;
                int val_topo  = (simon[dest][tam_dest - 1]   - 1) % 13;
                if (m > 1 && naipe_a != naipe_b) continue;
                if (val_fundo == val_topo - 1)   return 0;
            }
        }
    }
    return 1;
}