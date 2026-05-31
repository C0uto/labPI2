#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "readFile.h" // Inclui types.h indiretamente

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

void libertarRegrasMovAuto(RegrasMovAuto r) {
    while (r) {
        RegrasMovAuto next = r->prox;
        free(r->comando);
        free(r->origem);
        free(r->destino);
        free(r);
        r = next;
    }
}

void libertarRegrasTipo(RegrasTipo rt) {
    while (rt) {
        RegrasTipo next = rt->prox;
        free(rt->comando);
        free(rt->tipoDePilha);
        free(rt);
        rt = next;
    }
}

void libertarRegrasInit(RegrasInit ri) {
    while (ri) {
        RegrasInit next = ri->prox;
        free(ri->comando);
        free(ri->tipoDePilha);
        free(ri);
        ri = next;
    }
}

void libertarRegrasWin(RegrasWin rw) {
    while (rw) {
        RegrasWin next = rw->prox;
        free(rw->comando);
        free(rw->tipoDePilha);
        free(rw);
        rw = next;
    }
}

void libertarRegrasJogo(RegrasJogo rj) {
    if (!rj) return;
    free(rj->comando);
    free(rj->jogoNome);
    free(rj);
}

void libertarRegrasBaralhos(RegrasBaralhos rb) {
    if (!rb) return;
    free(rb->comando);
    free(rb);
}

void libertarContexto(CONTEXTO *c) {
    if (!c) return;
    libertarRegrasMovAuto(c->ma);
    libertarRegrasTipo(c->rt);
    libertarRegrasInit(c->ri);
    libertarRegrasWin(c->rw);
    libertarRegrasJogo(c->rj);
    libertarRegrasBaralhos(c->rb);
    c->ma = NULL;
    c->rj = NULL;
    c->rb = NULL;
    c->rt = NULL;
    c->ri = NULL;
    c->rw = NULL;
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

void limpaNovaStructMovAuto (RegrasMovAuto novo) {
    free(novo->comando);
    free(novo->origem);
    free(novo->destino);
    free(novo);
}

MENSAGENS leLinhaMovAuto (RegrasMovAuto *rma, char *temp1, char *temp2, char *temp3,char *flags_str,char *linha) {
    if (sscanf(linha,"%s %s %s %s",temp1,temp2,temp3,flags_str) != 4) return Comando_INVALIDO;
    RegrasMovAuto novo = malloc(sizeof(struct regra));
    if (!novo) return Comando_INVALIDO;
    inicializarMA(novo);
    novaStructMovAuto (novo,temp1,temp2,temp3);
    MENSAGENS x = guardaflagsMA(novo,flags_str);
    if(x==Flag_INVALIDA) {
        limpaNovaStructMovAuto (novo);
        return x;
    }
    novo->prox = *rma;
    if (*rma) (*rma)->ant = novo;
    *rma = novo;
    return OK;
}

MENSAGENS leLinhaInit (RegrasInit *ri, char *temp1, char *temp2, char *linha) {
    int numeroDeCartas;
    if(sscanf(linha,"%s %s %d",temp1,temp2,&numeroDeCartas) != 3 || numeroDeCartas < 0) return Comando_INVALIDO;
    RegrasInit novo = malloc(sizeof(struct regra5));
    if (!novo) return Comando_INVALIDO;
    inicializarInit(novo);
    novo->numeroDeCartas = numeroDeCartas;
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

void limpaNovaStructTipo (RegrasTipo novo) {
        free(novo->comando);
        free(novo->tipoDePilha);
        free(novo);
}

MENSAGENS leLinhaTipo (RegrasTipo *rt, char *temp1, char *temp2,char *flags_str,char *linha) {
    if (sscanf(linha,"%s %s %s",temp1,temp2,flags_str) != 3) return Comando_INVALIDO;
    RegrasTipo novo = malloc(sizeof(struct regra4));
    if (!novo) return Comando_INVALIDO;
    inicializarT(novo);
    novaStructTipo (novo,temp1,temp2);
    MENSAGENS x = guardaflagsT(novo,flags_str);
    if(x==Flag_INVALIDA) {
        limpaNovaStructTipo (novo);
        return x;
    }
    novo->prox = *rt;
    if (*rt) (*rt)->ant = novo;
    *rt = novo;
    return OK;
}

MENSAGENS leLinhaWin (RegrasWin *rw, char *temp1, char *temp2, char *linha) {
    int condicaoWin;
    if (sscanf(linha,"%s %s %d",temp1,temp2,&condicaoWin) != 3 || condicaoWin < 0) return Comando_INVALIDO;
    RegrasWin novo = malloc(sizeof(struct regra6));
    if (!novo) return Comando_INVALIDO;
    inicializarWin(novo);
    novo->condicaoWin = condicaoWin;
    novo->comando = malloc(strlen(temp1) + 1);
    novo->tipoDePilha = malloc(strlen(temp2) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->tipoDePilha,temp2);
    novo->prox = *rw;
    if (*rw) (*rw)->ant = novo;
    *rw = novo;
    return OK;
}

MENSAGENS verificaLimpaLinhaJogo (RegrasJogo *rj, int *criado) {
    if (*rj == NULL) {
        *rj = malloc(sizeof(struct regra2));
        if (!*rj) return Comando_INVALIDO;
        (*rj)->comando = NULL;
        (*rj)->jogoNome = NULL;
        *criado = 1;
    }
    if((*rj)->comando != NULL) {
        if (*criado) free(*rj);
        return Comando_INVALIDO;
    }
    return OK;
}

MENSAGENS leLinhaJogo (RegrasJogo *rj, char *temp1, char *temp2,char *linha) {
    int criado = 0; 
    MENSAGENS a = verificaLimpaLinhaJogo (rj,&criado);
    if(a==Comando_INVALIDO) return a;
    if(sscanf(linha,"%s %s",temp1,temp2) != 2) {
        if (criado) {
            free(*rj);
            *rj = NULL;
        }
        return Comando_INVALIDO;
    }
    (*rj)->comando = malloc(strlen(temp1) + 1);
    (*rj)->jogoNome = malloc(strlen(temp2) + 1);
    strcpy((*rj)->comando,temp1);
    strcpy((*rj)->jogoNome,temp2);
    return OK;
}


MENSAGENS verificaLimpaLinhaBaralho (RegrasBaralhos *rb, int *criado) {
    if (*rb == NULL) {
        *rb = malloc(sizeof(struct regra3));
        if (!*rb) return Comando_INVALIDO;
        (*rb)->comando = NULL;
        (*rb)->numeroDeBaralhos = 0;
        *criado = 1;
    }
    if((*rb)->comando != NULL) {
        if (*criado) free(*rb);
        return Comando_INVALIDO;
    }
    return OK;
}

MENSAGENS leLinhaBaralho (RegrasBaralhos *rb, char *temp1, char *linha) {
    int criado = 0;
    MENSAGENS a = verificaLimpaLinhaBaralho (rb,&criado);
    if(a == Comando_INVALIDO) return a;
    if(sscanf(linha,"%s %d",temp1,&(*rb)->numeroDeBaralhos) != 2 || (*rb)->numeroDeBaralhos < 1) {
        if (criado) {
            free(*rb);
            *rb = NULL;
        }
        return Comando_INVALIDO;
    }
    (*rb)->comando = malloc(strlen(temp1) + 1);
    strcpy((*rb)->comando,temp1);
    return OK;
}

MENSAGENS verificaLinhaBranco (MENSAGENS x,FILE *f,RegrasMovAuto *rma,RegrasJogo *rj,RegrasBaralhos *rb,RegrasTipo *rt,RegrasInit *ri,RegrasWin *rw, char *temp1, char *temp2, char *temp3,char *flags_str,char *linha,int lida){
    if(lida==1) {
       if(strcmp(temp1,"MOV") == 0 || strcmp(temp1,"AUTO") == 0) x = leLinhaMovAuto (rma,temp1,temp2,temp3,flags_str,linha);
       else if(strcmp(temp1,"INIT") == 0) x = leLinhaInit (ri,temp1,temp2,linha);
       else if(strcmp(temp1,"TIPO") == 0) x = leLinhaTipo (rt, temp1,temp2,flags_str,linha);
       else if(strcmp(temp1,"WIN") == 0) x = leLinhaWin (rw,temp1,temp2,linha);
       else if(strcmp(temp1,"JOGO") == 0) x = leLinhaJogo (rj,temp1,temp2,linha);
       else if(strcmp(temp1,"BARALHOS") == 0) x = leLinhaBaralho (rb,temp1,linha);
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

CONTEXTO inicializarContexto(void) {
    CONTEXTO c;
    c.ma = NULL;
    c.rj = NULL;
    c.rb = NULL;
    c.rt = NULL;
    c.ri = NULL;
    c.rw = NULL;
    return c;
}

int lerNomePacienciaDoSave(char *nome_paciencia) {
    FILE *f = fopen("save.txt", "r");
    if (!f) return 0;
    char linha[200];
    if (!fgets(linha, 200, f)) { fclose(f); return 0; }
    fclose(f);
    linha[strcspn(linha, "\n")] = '\0';
    strcpy(nome_paciencia, linha);
    return 1;
}

/* Função auxiliar para imprimir os ficheiros .txt da pasta 'paciencias' */
static void printPacienciaFiles(DIR *d) {
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        int len = strlen(dir->d_name);
        if (len >= 4 && strcmp(dir->d_name + len - 4, ".txt") == 0)
            printf("%s\n", dir->d_name);
    }
}

/* Função auxiliar para verificar e imprimir a existência de 'save.txt' */
static void checkAndPrintSaveFile(void) {
    FILE *sv = fopen("save.txt", "r");
    if (sv) {
        printf("save.txt\n");
        fclose(sv);
    }
}

int abrirPastaImprime(char *nome, int *carregar_save) {
    DIR *d = opendir("paciencias");
    if (!d) {
        printf("Erro: pasta 'paciencias' nao encontrada.\n");
        return 1;
    }
    printPacienciaFiles(d);
    closedir(d);
    checkAndPrintSaveFile();
    printf("Escolha uma paciencia (ou save.txt para carregar jogo gravado): ");
    scanf("%s", nome);
    while (getchar() != '\n');
    *carregar_save = (strcmp(nome, "save.txt") == 0);
    return 0;
}

int carregarNome(char *nome, int *carregar_save) {
    if (abrirPastaImprime(nome, carregar_save)) return 1;
    if (*carregar_save && !lerNomePacienciaDoSave(nome)) {
        printf("Erro: nao foi possivel ler o nome da paciencia do save.txt\n");
        return 1;
    }
    return 0;
}