#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "readFile.h"

const FEEDBACK tabela_mensagens[] = {
        {OK, "\n"},
        {ERRO_PILHA_INVALIDA, "Pilha invalida, tenta novamente."},
        {ERRO_PILHA_VAZIA, "A pilha seleciona esta vazia."},
        {ERRO_JOGADA_INVALIDA, "Jogada invalida!"},
        {ERRO_BARALHO_VAZIO, "Nao ha mais cartas no baralho"},
        {WIN, ">>>>>>>>>>>>PARABENS! venceu o jogo, para sair use 'q' para reiniciar use 'r'."},
        {GAME_OVER, ">>>>> GAME OVER! para sair use 'q', para reiniciar use 'r'."},
        {Comando_INVALIDO, "Comando invalido!"},
        {Flag_INVALIDA, "Flag invalida!"},
        {ERRO_CAMINHO_INVALIDO, "Caminho invalido!"},
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

void inicializarMA (RegrasMovAuto r) {
    strcpy(r->flags[0], "*+[]<>~mMxXcCdDVaAkK");
    strcpy(r->flags[1], "00000000000000000000");
    r->ant = NULL;
    r->prox = NULL;
}

void inicializarT (RegrasTipo rt) {
    strcpy(rt->flags[0], "=_^1");
    strcpy(rt->flags[1], "0000");
    rt->ant = NULL;
    rt->prox = NULL;
}

void inicializarInit(RegrasInit r) {
    r->ant = NULL;
    r->prox = NULL;
}

void inicializarWin(RegrasWin r) {
    r->ant = NULL;
    r->prox = NULL;
}

MENSAGENS guardaflagsMA (RegrasMovAuto r, char *flags_str) {
    for(int i = 0; flags_str[i]!='\0';i++) {
        int encontrado = 0;
        for(int z = 0; z<21; z++) {
            if(flags_str[i]==r->flags[0][z]){
                r->flags[1][z] = '1';
                encontrado++;
            }
        }
        if(!encontrado) return Flag_INVALIDA;
    }
    return OK;
}

MENSAGENS guardaflagsT (RegrasTipo rt, char *flags_str) {
    for(int i = 0; flags_str[i]!='\0';i++) {
        int encontrado = 0;
        for(int z = 0; z<5; z++) {
            if(flags_str[i]==rt->flags[0][z]){
                rt->flags[1][z] = '1';
                encontrado++;
            }
        }
        if(!encontrado) return Flag_INVALIDA;
    }
    return OK;
}

void novaStructMovAuto (RegrasMovAuto novo, char *temp1, char *temp2, char *temp3 ) {
    novo->comando = malloc(strlen(temp1) + 1);
    novo->origem = malloc(strlen(temp2) + 1);
    novo->destino = malloc(strlen(temp3) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->origem,temp2);
    strcpy(novo->destino,temp3);
}

MENSAGENS leLinhaMovAuto (RegrasMovAuto *rma, char *temp1, char *temp2, char *temp3,char *flags_str,char *linha,int lidos) {
    RegrasMovAuto novo = malloc(sizeof(struct regra));
    inicializarMA(novo);
    lidos = sscanf(linha,"%s %s %s %s",temp1,temp2,temp3,flags_str);
    if(lidos != 4) return Comando_INVALIDO;
    novaStructMovAuto (novo,temp1,temp2,temp3);
    MENSAGENS x = guardaflagsMA(novo,flags_str);
    if(x==Flag_INVALIDA) return x;
    novo->prox = *rma;
    if (*rma) (*rma)->ant = novo;
    *rma = novo;
    return OK;
}

MENSAGENS leLinhaInit (RegrasInit *ri, char *temp1, char *temp2,char *linha,int lidos) {
    RegrasInit novo = malloc(sizeof(struct regra5));
    inicializarInit(novo);
    lidos = sscanf(linha,"%s %s %d",temp1,temp2,&novo->numeroDeCartas);
    if(lidos != 3 || novo->numeroDeCartas < 0) return Comando_INVALIDO;
    novo->comando = malloc(strlen(temp1) + 1);
    novo->tipoDePilha = malloc(strlen(temp2) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->tipoDePilha,temp2);
    novo->prox = *ri;
    if (*ri) (*ri)->ant = novo;
    *ri = novo;
    return OK;
}

void novaStructTipo (RegrasTipo novo, char *temp1, char *temp2) {
    novo->comando = malloc(strlen(temp1) + 1);
    novo->tipoDePilha = malloc(strlen(temp2) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->tipoDePilha,temp2);
}

MENSAGENS leLinhaTipo (RegrasTipo *rt, char *temp1, char *temp2,char *flags_str,char *linha,int lidos) {
    RegrasTipo novo = malloc(sizeof(struct regra4));
    inicializarT(novo);
    lidos = sscanf(linha,"%s %s %s",temp1,temp2,flags_str);
    if(lidos != 3) return Comando_INVALIDO;
    novaStructTipo (novo,temp1,temp2);
    MENSAGENS x = guardaflagsT(novo,flags_str);
    if(x==Flag_INVALIDA) return x;
    novo->prox = *rt;
    if (*rt) (*rt)->ant = novo;
    *rt = novo;
    return OK;
}

MENSAGENS leLinhaWin (RegrasWin *rw, char *temp1, char *temp2,char *linha,int lidos) {
    RegrasWin novo = malloc(sizeof(struct regra6));
    inicializarWin(novo);
    lidos = sscanf(linha,"%s %s %d",temp1,temp2,&novo->condicaoWin);
    if(lidos != 3 || novo->condicaoWin < 0) return Comando_INVALIDO;
    novo->comando = malloc(strlen(temp1) + 1);
    novo->tipoDePilha = malloc(strlen(temp2) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->tipoDePilha,temp2);
    novo->prox = *rw;
    if (*rw) (*rw)->ant = novo;
    *rw = novo;
    return OK;
}

MENSAGENS leLinhaJogo (RegrasJogo *rj, char *temp1, char *temp2,char *linha,int lidos) {
    if((*rj)->comando != NULL) return Comando_INVALIDO;
    lidos = sscanf(linha,"%s %s",temp1,temp2);
    if(lidos != 2) return Comando_INVALIDO;
    (*rj)->comando = malloc(strlen(temp1) + 1);
    (*rj)->jogoNome = malloc(strlen(temp2) + 1);
    strcpy((*rj)->comando,temp1);
    strcpy((*rj)->jogoNome,temp2);
    return OK;
}

MENSAGENS leLinhaBaralho (RegrasBaralhos *rb, char *temp1,char *linha,int lidos) {
    if((*rb)->comando != NULL) return Comando_INVALIDO;
    lidos = sscanf(linha,"%s %d",temp1,&(*rb)->numeroDeBaralhos);
    if(lidos != 2 || (*rb)->numeroDeBaralhos < 1) return Comando_INVALIDO;
    (*rb)->comando = malloc(strlen(temp1) + 1);
    strcpy((*rb)->comando,temp1);
    return OK;
}

MENSAGENS verificaLinhaBranco (MENSAGENS x,FILE *f,RegrasMovAuto *rma,RegrasJogo *rj,RegrasBaralhos *rb,RegrasTipo *rt,RegrasInit *ri,RegrasWin *rw, char *temp1, char *temp2, char *temp3,char *flags_str,char *linha,int lida){
    if(lida==1) {
       if(strcmp(temp1,"MOV") == 0 || strcmp(temp1,"AUTO") == 0) x = leLinhaMovAuto (rma,temp1,temp2,temp3,flags_str,linha,lida);
       else if(strcmp(temp1,"INIT") == 0) x = leLinhaInit (ri,temp1,temp2,linha,lida);
       else if(strcmp(temp1,"TIPO") == 0) x = leLinhaTipo (rt, temp1,temp2,flags_str,linha,lida);
       else if(strcmp(temp1,"WIN") == 0) x = leLinhaWin (rw,temp1,temp2,linha,lida);
       else if(strcmp(temp1,"JOGO") == 0) x = leLinhaJogo (rj,temp1,temp2,linha,lida);
       else if(strcmp(temp1,"BARALHOS") == 0) x = leLinhaBaralho (rb,temp1,linha,lida);
       else return Comando_INVALIDO;
   }
   return x;
}

MENSAGENS leFicheiro (FILE *f,RegrasMovAuto *rma,RegrasJogo *rj,RegrasBaralhos *rb,RegrasTipo *rt,RegrasInit *ri,RegrasWin *rw) {
    char temp1[200];
    char temp2[200];
    char temp3[200];
    char linha[256];
    char flags_str[50];
    MENSAGENS x = OK;
    while(fgets(linha,256,f)!=NULL) {
      char *coment = strchr(linha, '#');//o strchr procura a primeira ocorrência do caractere # dentro da string. Se encontrar, devolve um ponteiro para essa posição dentro da própria string.
      if (coment != NULL) *coment = '\0';//vejo se o ponteiro não é null, ou seja se existe '#' e se existir coloco '\0' para apagar os comentários
      int lida = sscanf(linha,"%s",temp1);
      x = verificaLinhaBranco (x,f,rma,rj,rb,rt,ri,rw,temp1,temp2,temp3,flags_str,linha,lida);
      if(x!=OK) return mostrar_mensagem(x), x;
    }
    return mostrar_mensagem(OK), OK;
}

MENSAGENS abreFicheiro(char *nome,RegrasMovAuto *listaMA,RegrasJogo *listaJ,RegrasBaralhos *listaB,RegrasTipo *listaT,RegrasInit *listaI,RegrasWin *listaW) {
    (*listaJ)->comando = NULL;
    (*listaJ)->jogoNome = NULL;
    (*listaB)->comando = NULL;
    (*listaB)->numeroDeBaralhos = 0;
    char caminho[200];
    sprintf(caminho, "paciencias/%s", nome);
    FILE *f = fopen(caminho, "r");
    if (!f) {
        return ERRO_CAMINHO_INVALIDO;
    }
    MENSAGENS a = leFicheiro(f,listaMA,listaJ,listaB,listaT,listaI,listaW);
    fclose(f);
    return a;
}

int abrirPastaImprime (char *nome) {
    DIR *d = opendir("paciencias");
    if (!d) {
        printf("Erro: pasta 'paciencias' não encontrada.\n");
        return 1;
    }
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        int len = strlen(dir->d_name);
        if (len > 4 && strcmp(dir->d_name + len - 4, ".txt") == 0)
        printf("%s\n", dir->d_name);
    }
    closedir(d);
    printf("Escolha uma paciência: ");
    scanf("%s", nome);
    while (getchar() != '\n'); // Limpa o buffer de entrada (consome o \n deixado pelo scanf)
    return 0;
}