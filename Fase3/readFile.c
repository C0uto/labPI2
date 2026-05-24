/**
 * @file readFile.c
 * @brief Implementação do parser do ficheiro DSL das paciências.
 *
 * Lê linha a linha, remove comentários, e distribui cada comando
 * para a função de leitura correspondente, populando as listas de regras.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "readFile.h"

/* ========== TABELA DE MENSAGENS ========== */

/** @brief Tabela estática que associa cada código MENSAGENS ao seu texto. */
const FEEDBACK tabela_mensagens[] = {
    {OK,                   "\n"},
    {ERRO_PILHA_INVALIDA,  "Pilha invalida, tenta novamente."},
    {ERRO_PILHA_VAZIA,     "A pilha selecionada esta vazia."},
    {ERRO_JOGADA_INVALIDA, "Jogada invalida!"},
    {ERRO_BARALHO_VAZIO,   "Nao ha mais cartas no baralho"},
    {WIN,                  ">>>>>>> PARABENS! Venceste o jogo! Para sair usa 'q', para reiniciar 'r'."},
    {GAME_OVER,            ">>>>> GAME OVER! Para sair usa 'q', para reiniciar 'r'."},
    {Comando_INVALIDO,     "Comando invalido!"},
    {Flag_INVALIDA,        "Flag invalida!"},
    {ERRO_CAMINHO_INVALIDO,"Caminho invalido!"},
    {0,                    NULL}
};

/**
 * @brief Apresenta no ecrã a mensagem associada ao código dado.
 * @param cod Código MENSAGENS a apresentar.
 */
void mostrar_mensagem(MENSAGENS cod) {
    for (int i = 0; tabela_mensagens[i].feedback != NULL; i++) {
        if (tabela_mensagens[i].mensagem == cod) {
            printf("\n%s\n", tabela_mensagens[i].feedback);
            return;
        }
    }
}

/* ========== INICIALIZADORES ========== */

/**
 * @brief Inicializa um nó de regra MOV/AUTO com todas as flags a '0' e ponteiros a NULL.
 * @param r Nó a inicializar.
 */
void inicializarMA(RegrasMovAuto r) {
    strcpy(r->flags[0], "*+[]<>~mMxXcCdDVaAkK");
    strcpy(r->flags[1], "00000000000000000000");
    r->ant  = NULL;
    r->prox = NULL;
}

/**
 * @brief Inicializa um nó de regra TIPO com todas as flags a '0' e ponteiros a NULL.
 * @param rt Nó a inicializar.
 */
void inicializarT(RegrasTipo rt) {
    strcpy(rt->flags[0], "=_^1");
    strcpy(rt->flags[1], "0000");
    rt->ant  = NULL;
    rt->prox = NULL;
}

/**
 * @brief Inicializa um nó de regra INIT com ponteiros a NULL.
 * @param r Nó a inicializar.
 */
void inicializarInit(RegrasInit r) {
    r->ant  = NULL;
    r->prox = NULL;
}

/**
 * @brief Inicializa um nó de regra WIN com ponteiros a NULL.
 * @param r Nó a inicializar.
 */
void inicializarWin(RegrasWin r) {
    r->ant  = NULL;
    r->prox = NULL;
}

/* ========== GUARDAR FLAGS ========== */

/**
 * @brief Activa nos flags[1] as flags presentes na string @p flags_str.
 * @param r         Nó MOV/AUTO onde as flags são escritas.
 * @param flags_str String de flags lida do ficheiro (ex: "+[m<").
 * @return OK se todas as flags forem reconhecidas, Flag_INVALIDA caso contrário.
 */
MENSAGENS guardaflagsMA(RegrasMovAuto r, char *flags_str) {
    for (int i = 0; flags_str[i] != '\0'; i++) {
        int encontrado = 0;
        for (int z = 0; r->flags[0][z] != '\0'; z++) {
            if (flags_str[i] == r->flags[0][z]) {
                r->flags[1][z] = '1';
                encontrado = 1;
            }
        }
        if (!encontrado) return Flag_INVALIDA;
    }
    return OK;
}

/**
 * @brief Activa nos flags[1] as flags presentes na string @p flags_str para um nó TIPO.
 * @param rt        Nó TIPO onde as flags são escritas.
 * @param flags_str String de flags lida do ficheiro (ex: "=").
 * @return OK se todas as flags forem reconhecidas, Flag_INVALIDA caso contrário.
 */
MENSAGENS guardaflagsT(RegrasTipo rt, char *flags_str) {
    for (int i = 0; flags_str[i] != '\0'; i++) {
        int encontrado = 0;
        for (int z = 0; rt->flags[0][z] != '\0'; z++) {
            if (flags_str[i] == rt->flags[0][z]) {
                rt->flags[1][z] = '1';
                encontrado = 1;
            }
        }
        if (!encontrado) return Flag_INVALIDA;
    }
    return OK;
}

/* ========== CONSTRUTORES DE STRUCT ========== */

/**
 * @brief Aloca e copia comando, origem e destino para um nó MOV/AUTO.
 * @param novo Nó já alocado a preencher.
 * @param t1   String do comando ("MOV" ou "AUTO").
 * @param t2   String da origem.
 * @param t3   String do destino.
 */
void novaStructMovAuto(RegrasMovAuto novo, char *t1, char *t2, char *t3) {
    novo->comando = malloc(strlen(t1) + 1);
    novo->origem  = malloc(strlen(t2) + 1);
    novo->destino = malloc(strlen(t3) + 1);
    strcpy(novo->comando, t1);
    strcpy(novo->origem,  t2);
    strcpy(novo->destino, t3);
}

/**
 * @brief Aloca e copia comando e tipo de pilha para um nó TIPO.
 * @param novo Nó já alocado a preencher.
 * @param t1   String do comando ("TIPO").
 * @param t2   String do tipo de pilha.
 */
void novaStructTipo(RegrasTipo novo, char *t1, char *t2) {
    novo->comando     = malloc(strlen(t1) + 1);
    novo->tipoDePilha = malloc(strlen(t2) + 1);
    strcpy(novo->comando,     t1);
    strcpy(novo->tipoDePilha, t2);
}

/* ========== LEITURA DE LINHAS ========== */

/**
 * @brief Lê uma linha MOV/AUTO, cria o nó e insere-o na lista por prepend.
 * @param rma       Ponteiro para a cabeça da lista de regras MOV/AUTO.
 * @param t1        Buffer temporário para o token 1 (comando).
 * @param t2        Buffer temporário para o token 2 (origem).
 * @param t3        Buffer temporário para o token 3 (destino).
 * @param flags_str Buffer temporário para as flags.
 * @param linha     Linha completa lida do ficheiro.
 * @return OK em sucesso, Comando_INVALIDO ou Flag_INVALIDA em erro.
 */
MENSAGENS leLinhaMovAuto(RegrasMovAuto *rma, char *t1, char *t2,
                          char *t3, char *flags_str, char *linha) {
    RegrasMovAuto novo = malloc(sizeof(struct regra));
    inicializarMA(novo);
    int lidos = sscanf(linha, "%s %s %s %s", t1, t2, t3, flags_str);
    if (lidos != 4) { free(novo); return Comando_INVALIDO; }
    novaStructMovAuto(novo, t1, t2, t3);
    MENSAGENS x = guardaflagsMA(novo, flags_str);
    if (x == Flag_INVALIDA) { free(novo); return x; }
    novo->prox = *rma;
    if (*rma) (*rma)->ant = novo;
    *rma = novo;
    return OK;
}

/**
 * @brief Lê uma linha INIT, cria o nó e insere-o na lista por prepend.
 * @param ri    Ponteiro para a cabeça da lista de regras INIT.
 * @param t1    Buffer temporário para o token 1 (comando).
 * @param t2    Buffer temporário para o token 2 (tipo de pilha).
 * @param linha Linha completa lida do ficheiro.
 * @return OK em sucesso, Comando_INVALIDO em erro.
 */
MENSAGENS leLinhaInit(RegrasInit *ri, char *t1, char *t2, char *linha) {
    RegrasInit novo = malloc(sizeof(struct regra5));
    inicializarInit(novo);
    int lidos = sscanf(linha, "%s %s %d", t1, t2, &novo->numeroDeCartas);
    if (lidos != 3 || novo->numeroDeCartas < 0) { free(novo); return Comando_INVALIDO; }
    novo->comando     = malloc(strlen(t1) + 1);
    novo->tipoDePilha = malloc(strlen(t2) + 1);
    strcpy(novo->comando,     t1);
    strcpy(novo->tipoDePilha, t2);
    novo->prox = *ri;
    if (*ri) (*ri)->ant = novo;
    *ri = novo;
    return OK;
}

/**
 * @brief Lê uma linha TIPO, cria o nó e insere-o na lista por prepend.
 * @param rt        Ponteiro para a cabeça da lista de regras TIPO.
 * @param t1        Buffer temporário para o token 1.
 * @param t2        Buffer temporário para o token 2 (tipo de pilha).
 * @param flags_str Buffer temporário para as flags.
 * @param linha     Linha completa lida do ficheiro.
 * @return OK em sucesso, Comando_INVALIDO ou Flag_INVALIDA em erro.
 */
MENSAGENS leLinhaTipo(RegrasTipo *rt, char *t1, char *t2,
                       char *flags_str, char *linha) {
    RegrasTipo novo = malloc(sizeof(struct regra4));
    inicializarT(novo);
    int lidos = sscanf(linha, "%s %s %s", t1, t2, flags_str);
    if (lidos != 3) { free(novo); return Comando_INVALIDO; }
    novaStructTipo(novo, t1, t2);
    MENSAGENS x = guardaflagsT(novo, flags_str);
    if (x == Flag_INVALIDA) { free(novo); return x; }
    novo->prox = *rt;
    if (*rt) (*rt)->ant = novo;
    *rt = novo;
    return OK;
}

/**
 * @brief Lê uma linha WIN, cria o nó e insere-o na lista por prepend.
 * @param rw    Ponteiro para a cabeça da lista de regras WIN.
 * @param t1    Buffer temporário para o token 1.
 * @param t2    Buffer temporário para o token 2 (tipo de pilha).
 * @param linha Linha completa lida do ficheiro.
 * @return OK em sucesso, Comando_INVALIDO em erro.
 */
MENSAGENS leLinhaWin(RegrasWin *rw, char *t1, char *t2, char *linha) {
    RegrasWin novo = malloc(sizeof(struct regra6));
    inicializarWin(novo);
    int lidos = sscanf(linha, "%s %s %d", t1, t2, &novo->condicaoWin);
    if (lidos != 3 || novo->condicaoWin < 0) { free(novo); return Comando_INVALIDO; }
    novo->comando     = malloc(strlen(t1) + 1);
    novo->tipoDePilha = malloc(strlen(t2) + 1);
    strcpy(novo->comando,     t1);
    strcpy(novo->tipoDePilha, t2);
    novo->prox = *rw;
    if (*rw) (*rw)->ant = novo;
    *rw = novo;
    return OK;
}

/**
 * @brief Lê uma linha JOGO e preenche a estrutura RegrasJogo.
 *        Devolve erro se já havia um JOGO definido (duplicado).
 * @param rj    Ponteiro para a estrutura RegrasJogo a preencher.
 * @param t1    Buffer temporário para o token 1.
 * @param t2    Buffer temporário para o token 2 (nome do jogo).
 * @param linha Linha completa lida do ficheiro.
 * @return OK em sucesso, Comando_INVALIDO em erro.
 */
MENSAGENS leLinhaJogo(RegrasJogo *rj, char *t1, char *t2, char *linha) {
    if ((*rj)->comando != NULL) return Comando_INVALIDO;
    int lidos = sscanf(linha, "%s %s", t1, t2);
    if (lidos != 2) return Comando_INVALIDO;
    (*rj)->comando  = malloc(strlen(t1) + 1);
    (*rj)->jogoNome = malloc(strlen(t2) + 1);
    strcpy((*rj)->comando,  t1);
    strcpy((*rj)->jogoNome, t2);
    return OK;
}

/**
 * @brief Lê uma linha BARALHOS e preenche a estrutura RegrasBaralhos.
 *        Devolve erro se já havia um BARALHOS definido (duplicado).
 * @param rb    Ponteiro para a estrutura RegrasBaralhos a preencher.
 * @param t1    Buffer temporário para o token 1.
 * @param linha Linha completa lida do ficheiro.
 * @return OK em sucesso, Comando_INVALIDO em erro.
 */
MENSAGENS leLinhaBaralho(RegrasBaralhos *rb, char *t1, char *linha) {
    if ((*rb)->comando != NULL) return Comando_INVALIDO;
    int lidos = sscanf(linha, "%s %d", t1, &(*rb)->numeroDeBaralhos);
    if (lidos != 2 || (*rb)->numeroDeBaralhos < 1) return Comando_INVALIDO;
    (*rb)->comando = malloc(strlen(t1) + 1);
    strcpy((*rb)->comando, t1);
    return OK;
}

/* ========== DISPATCH DE LINHA ========== */

/**
 * @brief Identifica o tipo de comando pelo primeiro token e chama a função de leitura adequada.
 * @param t1        Primeiro token da linha (nome do comando).
 * @param t2        Buffer para o segundo token.
 * @param t3        Buffer para o terceiro token.
 * @param flags_str Buffer para as flags.
 * @param linha     Linha completa (passada às funções de leitura).
 * @param rma       Lista MOV/AUTO a actualizar.
 * @param rj        Estrutura JOGO a actualizar.
 * @param rb        Estrutura BARALHOS a actualizar.
 * @param rt        Lista TIPO a actualizar.
 * @param ri        Lista INIT a actualizar.
 * @param rw        Lista WIN a actualizar.
 * @return OK se o comando foi reconhecido e lido com sucesso, erro caso contrário.
 */
MENSAGENS processarToken(char *t1, char *t2, char *t3, char *flags_str,
                          char *linha, RegrasMovAuto *rma, RegrasJogo *rj,
                          RegrasBaralhos *rb, RegrasTipo *rt,
                          RegrasInit *ri, RegrasWin *rw) {
    if (strcmp(t1, "MOV")      == 0 ||
        strcmp(t1, "AUTO")     == 0)  return leLinhaMovAuto(rma, t1, t2, t3, flags_str, linha);
    if (strcmp(t1, "INIT")     == 0)  return leLinhaInit(ri, t1, t2, linha);
    if (strcmp(t1, "TIPO")     == 0)  return leLinhaTipo(rt, t1, t2, flags_str, linha);
    if (strcmp(t1, "WIN")      == 0)  return leLinhaWin(rw, t1, t2, linha);
    if (strcmp(t1, "JOGO")     == 0)  return leLinhaJogo(rj, t1, t2, linha);
    if (strcmp(t1, "BARALHOS") == 0)  return leLinhaBaralho(rb, t1, linha);
    return Comando_INVALIDO;
}

/* ========== LEITURA DO FICHEIRO ========== */

/**
 * @brief Lê todas as linhas do ficheiro DSL, ignorando comentários e linhas em branco.
 * @param f   Ficheiro aberto em modo leitura.
 * @param rma Lista MOV/AUTO a preencher.
 * @param rj  Estrutura JOGO a preencher.
 * @param rb  Estrutura BARALHOS a preencher.
 * @param rt  Lista TIPO a preencher.
 * @param ri  Lista INIT a preencher.
 * @param rw  Lista WIN a preencher.
 * @return OK em caso de sucesso, ou o primeiro código de erro encontrado.
 */
MENSAGENS leFicheiro(FILE *f, RegrasMovAuto *rma, RegrasJogo *rj,
                      RegrasBaralhos *rb, RegrasTipo *rt,
                      RegrasInit *ri, RegrasWin *rw) {
    char t1[200], t2[200], t3[200], flags_str[50], linha[256];
    MENSAGENS x = OK;
    while (fgets(linha, 256, f) != NULL) {
        char *coment = strchr(linha, '#');
        if (coment) *coment = '\0';
        if (sscanf(linha, "%s", t1) != 1) continue;
        x = processarToken(t1, t2, t3, flags_str, linha, rma, rj, rb, rt, ri, rw);
        if (x != OK) { mostrar_mensagem(x); return x; }
    }
    mostrar_mensagem(OK);
    return OK;
}

/* ========== ABRIR FICHEIRO ========== */

/**
 * @brief Inicializa as estruturas de regras, constrói o caminho completo e lê o ficheiro DSL.
 * @param nome    Nome do ficheiro (sem o prefixo "paciencias/").
 * @param listaMA Lista MOV/AUTO a preencher.
 * @param listaJ  Estrutura JOGO a preencher.
 * @param listaB  Estrutura BARALHOS a preencher.
 * @param listaT  Lista TIPO a preencher.
 * @param listaI  Lista INIT a preencher.
 * @param listaW  Lista WIN a preencher.
 * @return OK se a leitura foi bem-sucedida, ERRO_CAMINHO_INVALIDO se o ficheiro não existe.
 */
MENSAGENS abreFicheiro(char *nome, RegrasMovAuto *listaMA, RegrasJogo *listaJ,
                        RegrasBaralhos *listaB, RegrasTipo *listaT,
                        RegrasInit *listaI, RegrasWin *listaW) {
    (*listaJ)->comando         = NULL;
    (*listaJ)->jogoNome        = NULL;
    (*listaB)->comando         = NULL;
    (*listaB)->numeroDeBaralhos = 0;
    char caminho[200];
    sprintf(caminho, "paciencias/%s", nome);
    FILE *f = fopen(caminho, "r");
    if (!f) return ERRO_CAMINHO_INVALIDO;
    MENSAGENS a = leFicheiro(f, listaMA, listaJ, listaB, listaT, listaI, listaW);
    fclose(f);
    return a;
}

/* ========== ABRIR PASTA ========== */

/**
 * @brief Lista os ficheiros .txt da pasta "paciencias/" e solicita ao utilizador a sua escolha.
 * @param nome Buffer onde o nome do ficheiro escolhido é escrito (mínimo 100 bytes).
 * @return 0 em caso de sucesso, 1 se a pasta não for encontrada.
 */
int abrirPastaImprime(char *nome) {
    DIR *d = opendir("paciencias");
    if (!d) { printf("Erro: pasta 'paciencias' nao encontrada.\n"); return 1; }
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        int len = strlen(dir->d_name);
        if (len > 4 && strcmp(dir->d_name + len - 4, ".txt") == 0)
            printf("%s\n", dir->d_name);
    }
    closedir(d);
    printf("Escolha uma paciencia: ");
    scanf("%99s", nome);
    while (getchar() != '\n');
    return 0;
}
