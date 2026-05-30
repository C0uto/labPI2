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
        {ERRO_CARREGAR_SAVE, "Erro ao carregar o ficheiro de save."},
        {0, NULL}
};

/**
 * Funcao para imprimir mensagens de feedback com base num codigo de erro ou estado
 *
 * * @param cod -> O codigo da mensagem definido no enum MENSAGENS
 *
 */
void mostrar_mensagem(MENSAGENS cod) {
        for(int i = 0; tabela_mensagens[i].feedback != NULL; i++) {
                if(tabela_mensagens[i].mensagem == cod) {
                        printf("\n%s\n", tabela_mensagens[i].feedback);
                        return;
                }
        }
}

/**
 * Funcao para inicializar as strings de flags de uma regra de movimento/auto
 *
 * * @param r -> Ponteiro para a estrutura da regra
 *
 */
void inicializarMA (RegrasMovAuto r) {
    strcpy(r->flags[0], "*+[]<>~mMxXcCdDVaAkK");
    strcpy(r->flags[1], "00000000000000000000");
    r->ant = NULL;
    r->prox = NULL;
}

/**
 * Funcao para inicializar as strings de flags de uma regra de tipo de pilha
 *
 * * @param rt -> Ponteiro para a estrutura de tipo
 *
 */
void inicializarT (RegrasTipo rt) {
    strcpy(rt->flags[0], "=_^1");
    strcpy(rt->flags[1], "0000");
    rt->ant = NULL;
    rt->prox = NULL;
}

/**
 * Funcao para inicializar uma regra de INIT
 *
 * * @param r -> Ponteiro para a estrutura
 *
 */
void inicializarInit(RegrasInit r) {
    r->ant = NULL;
    r->prox = NULL;
}

/**
 * Funcao para inicializar uma regra de vitoria
 *
 * * @param r -> Ponteiro para a estrutura
 *
 */
void inicializarWin(RegrasWin r) {
    r->ant = NULL;
    r->prox = NULL;
}

/**
 * Funcao para ativar os bits de flags numa regra MOV/AUTO a partir de uma string
 *
 * * @param r -> A regra
 * * @param flags_str -> String contendo as flags (ex: "+[m")
 * * @return res -> OK ou Flag_INVALIDA
 *
 */
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

/**
 * Funcao para ativar os bits de flags numa regra de TIPO a partir de uma string
 *
 * * @param rt -> A regra de tipo
 * * @param flags_str -> String contendo as flags (ex: "=")
 * * @return res -> OK ou Flag_INVALIDA
 *
 */
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

/**
 * Funcao auxiliar para alocar e copiar strings basicas de uma regra de movimento
 *
 * * @param novo -> Ponteiro para a nova regra
 * * @param temp1 -> String do comando
 * * @param temp2 -> String da origem
 * * @param temp3 -> String do destino
 *
 */
void novaStructMovAuto (RegrasMovAuto novo, char *temp1, char *temp2, char *temp3 ) {
    novo->comando = malloc(strlen(temp1) + 1);
    novo->origem = malloc(strlen(temp2) + 1);
    novo->destino = malloc(strlen(temp3) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->origem,temp2);
    strcpy(novo->destino,temp3);
}

/**
 * Funcao para processar uma linha do tipo MOV ou AUTO do ficheiro
 *
 * * @param rma -> Ponteiro para a lista de regras
 * * @param temp1 -> Comando
 * * @param temp2 -> Origem
 * * @param temp3 -> Destino
 * * @param flags_str -> String de flags
 * * @param linha -> A linha completa lida do ficheiro
 * * @return res -> OK ou erro de comando/flag
 *
 */
MENSAGENS leLinhaMovAuto (RegrasMovAuto *rma, char *temp1, char *temp2, char *temp3,char *flags_str,char *linha) {
    RegrasMovAuto novo = malloc(sizeof(struct regra));
    inicializarMA(novo);
    if(sscanf(linha,"%s %s %s %s",temp1,temp2,temp3,flags_str) != 4) return Comando_INVALIDO;
    novaStructMovAuto (novo,temp1,temp2,temp3);
    MENSAGENS x = guardaflagsMA(novo,flags_str);
    if(x==Flag_INVALIDA) return x;
    novo->prox = *rma;
    if (*rma) (*rma)->ant = novo;
    *rma = novo;
    return OK;
}

/**
 * Funcao para processar uma linha do tipo INIT do ficheiro
 *
 * * @param ri -> Ponteiro para a lista de regras de inicializacao
 * * @param temp1 -> Comando
 * * @param temp2 -> Tipo da pilha
 * * @param linha -> A linha lida
 * * @return res -> OK ou Comando_INVALIDO
 *
 */
MENSAGENS leLinhaInit (RegrasInit *ri, char *temp1, char *temp2, char *linha) {
    RegrasInit novo = malloc(sizeof(struct regra5));
    inicializarInit(novo);
    if(sscanf(linha,"%s %s %d",temp1,temp2,&novo->numeroDeCartas) != 3 || novo->numeroDeCartas < 0) return Comando_INVALIDO;
    novo->comando = malloc(strlen(temp1) + 1);
    novo->tipoDePilha = malloc(strlen(temp2) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->tipoDePilha,temp2);
    novo->prox = *ri;
    if (*ri) (*ri)->ant = novo;
    *ri = novo;
    return OK;
}

/**
 * Funcao auxiliar para alocar memoria para as strings de uma regra de TIPO
 *
 * * @param novo -> Ponteiro para a regra
 * * @param temp1 -> Comando
 * * @param temp2 -> Tipo da pilha
 *
 */
void novaStructTipo (RegrasTipo novo, char *temp1, char *temp2) {
    novo->comando = malloc(strlen(temp1) + 1);
    novo->tipoDePilha = malloc(strlen(temp2) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->tipoDePilha,temp2);
}

/**
 * Funcao para processar uma linha do tipo TIPO do ficheiro
 *
 * * @param rt -> Ponteiro para a lista de regras de tipo
 * * @param temp1 -> Comando
 * * @param temp2 -> Nome do tipo
 * * @param flags_str -> String com as flags
 * * @param linha -> Linha lida
 * * @return res -> OK ou Flag_INVALIDA
 *
 */
MENSAGENS leLinhaTipo (RegrasTipo *rt, char *temp1, char *temp2,char *flags_str,char *linha) {
    RegrasTipo novo = malloc(sizeof(struct regra4));
    inicializarT(novo);
    if(sscanf(linha,"%s %s %s",temp1,temp2,flags_str) != 3) return Comando_INVALIDO;
    novaStructTipo (novo,temp1,temp2);
    MENSAGENS x = guardaflagsT(novo,flags_str);
    if(x==Flag_INVALIDA) return x;
    novo->prox = *rt;
    if (*rt) (*rt)->ant = novo;
    *rt = novo;
    return OK;
}

/**
 * Funcao para processar uma linha do tipo WIN do ficheiro
 *
 * * @param rw -> Ponteiro para a lista de condicoes de vitoria
 * * @param temp1 -> Comando
 * * @param temp2 -> Tipo de pilha
 * * @param linha -> Linha lida
 * * @return res -> OK ou Comando_INVALIDO
 *
 */
MENSAGENS leLinhaWin (RegrasWin *rw, char *temp1, char *temp2, char *linha) {
    RegrasWin novo = malloc(sizeof(struct regra6));
    inicializarWin(novo);
    if(sscanf(linha,"%s %s %d",temp1,temp2,&novo->condicaoWin) != 3 || novo->condicaoWin < 0) return Comando_INVALIDO;
    novo->comando = malloc(strlen(temp1) + 1);
    novo->tipoDePilha = malloc(strlen(temp2) + 1);
    strcpy(novo->comando,temp1);
    strcpy(novo->tipoDePilha,temp2);
    novo->prox = *rw;
    if (*rw) (*rw)->ant = novo;
    *rw = novo;
    return OK;
}

/**
 * Funcao para processar a linha JOGO (nome da paciencia)
 *
 * * @param rj -> Estrutura do jogo
 * * @param temp1 -> Comando
 * * @param temp2 -> Nome do jogo
 * * @param linha -> Linha lida
 * * @return res -> OK ou Comando_INVALIDO
 *
 */
MENSAGENS leLinhaJogo (RegrasJogo *rj, char *temp1, char *temp2,char *linha) {
    if((*rj)->comando != NULL) return Comando_INVALIDO;
    if(sscanf(linha,"%s %s",temp1,temp2) != 2) return Comando_INVALIDO;
    (*rj)->comando = malloc(strlen(temp1) + 1);
    (*rj)->jogoNome = malloc(strlen(temp2) + 1);
    strcpy((*rj)->comando,temp1);
    strcpy((*rj)->jogoNome,temp2);
    return OK;
}

/**
 * Funcao para processar a linha BARALHOS
 *
 * * @param rb -> Estrutura da regra de baralhos
 * * @param temp1 -> Comando
 * * @param linha -> Linha lida
 * * @return res -> OK ou Comando_INVALIDO
 *
 */
MENSAGENS leLinhaBaralho (RegrasBaralhos *rb, char *temp1, char *linha) {
    if((*rb)->comando != NULL) return Comando_INVALIDO;
    if(sscanf(linha,"%s %d",temp1,&(*rb)->numeroDeBaralhos) != 2 || (*rb)->numeroDeBaralhos < 1) return Comando_INVALIDO;
    (*rb)->comando = malloc(strlen(temp1) + 1);
    strcpy((*rb)->comando,temp1);
    return OK;
}

/**
 * Funcao que identifica o comando no inicio da linha e chama o parser adequado
 *
 * * @param x -> Mensagem de retorno
 * * @param f -> Ficheiro aberto
 * * @param rma -> Regras Mov
 * * @param rj -> Regras Jogo
 * * @param rb -> Regras Baralhos
 * * @param rt -> Regras Tipo
 * * @param ri -> Regras Init
 * * @param rw -> Regras Win
 * * @param temp1 -> Buffer de string 1
 * * @param temp2 -> Buffer de string 2
 * * @param temp3 -> Buffer de string 3
 * * @param flags_str -> Buffer para flags
 * * @param linha -> A linha lida
 * * @return res -> OK ou erro
 *
 */
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

/**
 * Funcao que le o ficheiro linha a linha, remove comentarios e chama o processador
 *
 * * @param f -> Ficheiro aberto
 * * @param rma, rj, rb, rt, ri, rw -> Listas de regras
 * * @return res -> OK ou Erro
 *
 */
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

/**
 * Funcao que abre o ficheiro de regras na pasta "paciencias" e inicia a leitura
 *
 * * @param nome -> Nome do ficheiro (ex: "golf.txt")
 * * @param listaMA, listaJ, listaB, listaT, listaI, listaW -> Listas de regras de destino
 * * @return res -> OK ou ERRO_CAMINHO_INVALIDO
 *
 */
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

/**
 * Funcao para inicializar o contexto global de regras com valores default
 *
 * * @return res -> Estrutura CONTEXTO inicializada
 *
 */
CONTEXTO inicializarContexto(void) {
    CONTEXTO c;
    c.ma = NULL;
    c.rj = malloc(sizeof(struct regra2));
    c.rb = malloc(sizeof(struct regra3));
    c.rt = NULL;
    c.ri = NULL;
    c.rw = NULL;
    return c;
}

/**
 * Funcao para ler a primeira linha do ficheiro de save para saber que regras carregar
 *
 * * @param nome_save -> Nome do ficheiro .save a abrir
 * * @param nome_regras -> Buffer onde sera guardado o nome lido (ex: Golf.txt)
 * * @return res -> 1 se leu com sucesso, 0 caso contrario
 *
 */
int lerNomeDoJogoDoSave(char *nome_save, char *nome_regras) {
    char caminho[200];
    sprintf(caminho, "saves/%s", nome_save);
    FILE *f = fopen(caminho, "r");
    if (!f) return 0;
    char linha[200];
    if (!fgets(linha, 200, f)) { fclose(f); return 0; }
    fclose(f);
    linha[strcspn(linha, "\n")] = '\0';
    strcpy(nome_regras, linha);
    return 1;
}

/**
 * Funcao auxiliar para listar todos os ficheiros .txt dentro da pasta "paciencias"
 *
 * * @param d -> Ponteiro para o diretorio aberto
 *
 */
static void listarFicheirosPaciencia(DIR *d) {
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        int len = strlen(dir->d_name);
        if (len >= 4 && strcmp(dir->d_name + len - 4, ".txt") == 0)
            printf("%s\n", dir->d_name);
    }
}

/**
 * Funcao auxiliar para listar todos os ficheiros .save dentro da pasta "saves"
 *
 * * @param d -> Ponteiro para o diretorio aberto
 *
 */
static void listarFicheirosSave(DIR *d) {
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        int len = strlen(dir->d_name);
        if (len >= 4 && strcmp(dir->d_name + len - 4, ".save") == 0)
            printf("%s\n", dir->d_name);
    }
}

/**
 * Funcao que gere a interface inicial de escolha de ficheiro de paciencia e save
 *
 * * @param nome -> Buffer para guardar o nome escolhido
 * * @param carregar_save -> Ponteiro para flag de carregamento de save
 * * @return res -> 0 se sucesso, 1 se erro de pasta
 *
 */
int abrirPastaImprime(char *nome, int *carregar_save) {
    DIR *d = opendir("paciencias");
    if (!d) {
        printf("Erro: pasta 'paciencias' nao encontrada.\n");
        return 1;
    }
    listarFicheirosPaciencia(d);
    closedir(d);
    DIR *d_save = opendir("saves");
    if (!d_save) {
        printf("Erro: pasta 'saves' nao encontrada.\n");
        return 1;
    }
    listarFicheirosSave(d_save);
    closedir(d_save);
    printf("Escolha uma paciencia ou um save para carregar o jogo gravado: ");
    scanf("%s", nome);
    while (getchar() != '\n');
    size_t len = strlen(nome);
    *carregar_save = (len >= 5 && strcmp(nome + len - 5, ".save") == 0);
    return 0;
}

/**
 * Funcao que orquestra a definicao do nome do ficheiro de jogo (novo ou save)
 *
 * * @param nome -> Buffer de destino
 * * @param carregar_save -> Ponteiro para a flag de save
 * * @return res -> 0 se sucesso, 1 se falhou
 *
 */
int carregarNome(char *nome, int *carregar_save) {
    if (abrirPastaImprime(nome, carregar_save)) return 1;
    if (*carregar_save && !lerNomeDoJogoDoSave(nome)) {
        mostrarmensagem(ERRO_CARREGAR_SAVE);
        return 1;
    }
    return 0;
}