#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

typedef struct regra {
    char flags[2][21];
    char *comando;
    char *origem;
    char *destino;
    struct regra *ant;
    struct regra *prox;
} *RegrasMovAuto;

typedef struct regra2 {
    char *comando;
    char *jogoNome;
} *RegrasJogo;

typedef struct regra3 {
    char *comando;
    int numeroDeBaralhos;
} *RegrasBaralhos;

typedef struct regra4 {
    char flags[2][5];
    char *comando;
    char *tipoDePilha;
    struct regra4 *ant;
    struct regra4 *prox;
} *RegrasTipo;

typedef struct regra5 {
    char *comando;
    int numeroDeCartas;
    char *tipoDePilha;
    struct regra5 *ant;
    struct regra5 *prox;
} *RegrasInit;

typedef struct regra6 {
    char *comando;
    char *tipoDePilha;
    int condicaoWin;
    struct regra6 *ant;
    struct regra6 *prox;
} *RegrasWin;

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

Mensagens leLinhaMovAuto (RegrasMovAuto *rma, char *temp1, char *temp2, char *temp3,char *flags_str,char *linha) {
    int lidos = 0;
    RegrasMovAuto novo = malloc(sizeof(struct regra));
    inicializarMA(novo);
    lidos = sscanf(linha,"%s %s %s %s",temp1,temp2,temp3,flags_str);
    if(lidos != 4) return Comando_Invalido;
    novo->comando = malloc(strlen(temp1) + 1);
    novo->origem = malloc(strlen(temp2) + 1);
    novo->destino = malloc(strlen(temp3) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->origem,temp2);
    strcpy(novo->destino,temp3);
    Mensagens x = guardaflagsMA(novo,flags_str);
    if(x==Flag_Invalida) return Flag_Invalida;
    novo->prox = *rma;
    if (*rma) (*rma)->ant = novo;
    *rma = novo;
    return OK;
}

Mensagens leLinhaInit (RegrasInit *ri, char *temp1, char *temp2,char *linha) {
    int lidos = 0;
    RegrasInit novo = malloc(sizeof(struct regra5));
    inicializarInit(novo);
    lidos = sscanf(linha,"%s %s %d",temp1,temp2,&novo->numeroDeCartas);
    if(lidos != 3 || novo->numeroDeCartas < 0) return Comando_Invalido;
    novo->comando = malloc(strlen(temp1) + 1);
    novo->tipoDePilha = malloc(strlen(temp2) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->tipoDePilha,temp2);
    novo->prox = *ri;
    if (*ri) (*ri)->ant = novo;
    *ri = novo;
    return OK;
}

Mensagens leLinhaTipo (RegrasTipo *rt, char *temp1, char *temp2,char *flags_str,char *linha) {
    int lidos = 0;
    RegrasTipo novo = malloc(sizeof(struct regra4));
    inicializarT(novo);
    lidos = sscanf(linha,"%s %s %s",temp1,temp2,flags_str);
    if(lidos != 3) return Comando_Invalido;
    novo->comando = malloc(strlen(temp1) + 1);
    novo->tipoDePilha = malloc(strlen(temp2) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->tipoDePilha,temp2);
    Mensagens x = guardaflagsT(novo,flags_str);
    if(x==Flag_Invalida) return Flag_Invalida;
    novo->prox = *rt;
    if (*rt) (*rt)->ant = novo;
    *rt = novo;
    return OK;
}

Mensagens leLinhaWin (RegrasWin *rw, char *temp1, char *temp2,char *linha) {
    int lidos = 0;
    RegrasWin novo = malloc(sizeof(struct regra6));
    inicializarWin(novo);
    lidos = sscanf(linha,"%s %s %d",temp1,temp2,&novo->condicaoWin);
    if(lidos != 3 || novo->condicaoWin < 0) return Comando_Invalido;
    novo->comando = malloc(strlen(temp1) + 1);
    novo->tipoDePilha = malloc(strlen(temp2) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->tipoDePilha,temp2);
    novo->prox = *rw;
    if (*rw) (*rw)->ant = novo;
    *rw = novo;
    return OK;
}

Mensagens leLinhaJogo (RegrasJogo *rj, char *temp1, char *temp2,char *linha) {
    int lidos = 0;
    if((*rj)->comando != NULL) return Comando_Invalido;
    lidos = sscanf(linha,"%s %s",temp1,temp2);
    if(lidos != 2) return Comando_Invalido;
    (*rj)->comando = malloc(strlen(temp1) + 1);
    (*rj)->jogoNome = malloc(strlen(temp2) + 1);
    strcpy((*rj)->comando,temp1);
    strcpy((*rj)->jogoNome,temp2);
    return OK;
}

Mensagens leLinhaBaralho (RegrasBaralhos *rb, char *temp1,char *linha) {
    int lidos = 0;
    if((*rb)->comando != NULL) return Comando_Invalido;
    lidos = sscanf(linha,"%s %d",temp1,&(*rb)->numeroDeBaralhos);
    if(lidos != 2 || (*rb)->numeroDeBaralhos < 1) return Comando_Invalido;
    (*rb)->comando = malloc(strlen(temp1) + 1);
    strcpy((*rb)->comando,temp1);
    return OK;
}

Mensagens leFicheiro (FILE *f,RegrasMovAuto *rma,RegrasJogo *rj,RegrasBaralhos *rb,RegrasTipo *rt,RegrasInit *ri,RegrasWin *rw) {
    char temp1[200];
    char temp2[200];
    char temp3[200];
    char linha[256];
    char flags_str[50];
    int lida = 0;
  while(fgets(linha,256,f)!=NULL) {
   char *coment = strchr(linha, '#');//o strchr procura a primeira ocorrência do caractere # dentro da string. Se encontrar, devolve um ponteiro para essa posição dentro da própria string.
   if (coment != NULL) *coment = '\0';//vejo se o ponteiro não é null, ou seja se existe '#' e se existir coloco '\0' para apagar os comentários
   lida = sscanf(linha,"%s",temp1);
   if(lida==1) {
    if(strcmp(temp1,"MOV") == 0 || strcmp(temp1,"AUTO") == 0) {
        Mensagens x = leLinhaMovAuto (rma,temp1,temp2,temp3,flags_str,linha);
        if(x==Flag_Invalida) return Flag_Invalida;
        if(x == Comando_Invalido) return Comando_Invalido;
    }
    else if(strcmp(temp1,"INIT") == 0) {
        Mensagens x = leLinhaInit (ri,temp1,temp2,linha);
        if(x == Comando_Invalido) return Comando_Invalido;
    }
    else if(strcmp(temp1,"TIPO") == 0) {
        Mensagens x = leLinhaTipo (rt, temp1,temp2,flags_str,linha);
        if(x==Flag_Invalida) return Flag_Invalida;
        if(x == Comando_Invalido) return Comando_Invalido;
    }
    else if(strcmp(temp1,"WIN") == 0) {
        Mensagens x = leLinhaWin (rw,temp1,temp2,linha);
        if(x == Comando_Invalido) return Comando_Invalido;
    }
    else if(strcmp(temp1,"JOGO") == 0) {
        Mensagens x = leLinhaJogo (rj,temp1,temp2,linha);
        if(x == Comando_Invalido) return Comando_Invalido;
    }
    else if(strcmp(temp1,"BARALHOS") == 0) {
        Mensagens x = leLinhaBaralho (rb,temp1,linha);
        if(x == Comando_Invalido) return Comando_Invalido;
    }
    else return Comando_Invalido;
   }
  }
  return OK;
}

Mensagens guardaflagsMA (RegrasMovAuto r, char *flags_str) {
    for(int i = 0; flags_str[i]!='\0';i++) {
        int encontrado = 0;
        for(int z = 0; z<21; z++) {
            if(flags_str[i]==r->flags[0][z]){
                r->flags[1][z] = '1';
                encontrado++;
            }
        }
        if(!encontrado) return Flag_Invalida;
    }
    return OK;
}

Mensagens guardaflagsT (RegrasTipo rt, char *flags_str) {
    for(int i = 0; flags_str[i]!='\0';i++) {
        int encontrado = 0;
        for(int z = 0; z<5; z++) {
            if(flags_str[i]==rt->flags[0][z]){
                rt->flags[1][z] = '1';
                encontrado++;
            }
        }
        if(!encontrado) return Flag_Invalida;
    }
    return OK;
}

void efetuaRegrasTipo (RegrasTipo *rt) {
    //vou considerar que estou na primeira ou seja ant = NULL
    while((*rt)->prox !=NULL) {
        
    }
    }

int main () {
    RegrasMovAuto listaMA = NULL;
    RegrasJogo listaJ = malloc(sizeof(struct regra2));;
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
    printf("Escolha uma paciência: ");
    scanf("%s", nome);
    char caminho[200];
    sprintf(caminho, "paciencias/%s", nome);
    FILE *f = fopen(caminho, "r");
    if (!f) {
        printf("Erro ao abrir %s\n", caminho);
        return 1;
    }
    Mensagens a = leFicheiro(f,&listaMA,&listaJ,&listaB,&listaT,&listaI,&listaW);
    fclose(f);
}

