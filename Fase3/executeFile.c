#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "executeFile.h"

/* ============================================================
 *  CONVERSÃO CARTA <-> STRING
 * ============================================================ */

const char *card2str(CARTA card) {
    static const char *valores = "A23456789TJQK";
    static const char *naipes  = "SHDC";
    static char res[4];
    if (card <= 0) { strcpy(res, "  "); return res; }
    res[0] = valores[(card - 1) % 13];
    res[1] = naipes[(card  - 1) / 13];
    res[2] = '\0';
    return res;
}

/* Converte string "AS","10H","KD",... para CARTA */
CARTA str2card(const char *s) {
    static const char *vals = "A23456789TJQK";
    static const char *suits = "SHDC";
    if (!s || s[0] == '\0') return 0;
    int v = -1, n = -1;
    for (int i = 0; i < 13; i++)
        if (s[0] == vals[i]) { v = i; break; }
    int offset = (s[1] == '0') ? 2 : 1; /* "10x" */
    if (s[0] == '1' && s[1] == '0') {
        v = 8; /* valor 10 é índice 8 (A=0..9=8,T=9?) */
        /* na nossa codificação T=index 9 */
        v = 9; offset = 2;
    }
    char suit_char = s[offset];
    for (int i = 0; i < 4; i++)
        if (suit_char == suits[i]) { n = i; break; }
    if (v < 0 || n < 0) return 0;
    return n * 13 + v + 1;
}

/* ============================================================
 *  PROPRIEDADES DE CARTA
 * ============================================================ */

int getValor(CARTA c) { return (c <= 0) ? 0 : ((c - 1) % 13) + 1; }
int getNaipe(CARTA c) { return (c <= 0) ? -1 : (c - 1) / 13; }
int getCor(CARTA c)   {
    int n = getNaipe(c);
    /* S=0(preto), H=1(vermelho), D=2(vermelho), C=3(preto) */
    return (n == 1 || n == 2) ? 1 : 0;
}

/* ============================================================
 *  OPERAÇÕES SOBRE PILHAS
 * ============================================================ */

int getPilhaTamanho(PILHA *p) { return p->tamanho; }

void pushCarta(PILHA *p, CARTA c) {
    if (p->tamanho >= p->capacidade) {
        p->capacidade += 16;
        p->cartas = realloc(p->cartas, p->capacidade * sizeof(CARTA));
    }
    p->cartas[p->tamanho++] = c;
}

CARTA popCarta(PILHA *p) {
    if (p->tamanho <= 0) return 0;
    return p->cartas[--p->tamanho];
}

CARTA topoCarta(PILHA *p) {
    if (p->tamanho <= 0) return 0;
    return p->cartas[p->tamanho - 1];
}

/* ============================================================
 *  ACESSO A FLAGS
 * ============================================================ */

int temFlag(RegrasMovAuto r, char f) {
    if (!r) return 0;
    for (int i = 0; r->flags[0][i] != '\0'; i++)
        if (r->flags[0][i] == f && r->flags[1][i] == '1') return 1;
    return 0;
}

/* ============================================================
 *  ENCONTRAR REGRA MOV/AUTO
 * ============================================================ */

RegrasMovAuto encontrarRegra(RegrasMovAuto rma, const char *ori, const char *des) {
    RegrasMovAuto aux = rma;
    while (aux != NULL) {
        if (strcmp(aux->comando, "MOV") == 0 &&
            strcmp(aux->origem,  ori)   == 0 &&
            strcmp(aux->destino, des)   == 0)
            return aux;
        aux = aux->prox;
    }
    return NULL;
}

/* ============================================================
 *  VALIDAÇÃO DA SEQUÊNCIA A MOVER (flags sobre as cartas em si)
 * ============================================================ */

/* Valida flag '+': sem ela só se move 1 carta */
int validarFlagMais(int n, RegrasMovAuto r) {
    if (n > 1 && !temFlag(r, '+')) return 0;
    return 1;
}

/* Valida flag '[': sequência decrescente por valores consecutivos */
int validarFlagDescrescente(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, '[')) return 1;
    int base = ori->tamanho - n;
    for (int i = 0; i < n - 1; i++) {
        if (getValor(ori->cartas[base + i]) != getValor(ori->cartas[base + i + 1]) + 1)
            return 0;
    }
    return 1;
}

/* Valida flag ']': sequência crescente por valores consecutivos */
int validarFlagCrescente(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, ']')) return 1;
    int base = ori->tamanho - n;
    for (int i = 0; i < n - 1; i++) {
        if (getValor(ori->cartas[base + i]) != getValor(ori->cartas[base + i + 1]) - 1)
            return 0;
    }
    return 1;
}

/* Valida flag 'm': todas as cartas a mover têm o mesmo naipe */
int validarFlagMesmoNaipe(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'm')) return 1;
    int base = ori->tamanho - n;
    int naipe = getNaipe(ori->cartas[base]);
    for (int i = 1; i < n; i++)
        if (getNaipe(ori->cartas[base + i]) != naipe) return 0;
    return 1;
}

/* Valida flag 'x': naipes alternados na sequência */
int validarFlagNaipesAlternados(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'x')) return 1;
    int base = ori->tamanho - n;
    for (int i = 0; i < n - 1; i++)
        if (getNaipe(ori->cartas[base + i]) == getNaipe(ori->cartas[base + i + 1])) return 0;
    return 1;
}

/* Valida flag 'c': todas as cartas a mover têm a mesma cor */
int validarFlagMesmaCor(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'c')) return 1;
    int base = ori->tamanho - n;
    int cor = getCor(ori->cartas[base]);
    for (int i = 1; i < n; i++)
        if (getCor(ori->cartas[base + i]) != cor) return 0;
    return 1;
}

/* Valida flag 'd': cores alternadas na sequência */
int validarFlagCoresAlternadas(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'd')) return 1;
    int base = ori->tamanho - n;
    for (int i = 0; i < n - 1; i++)
        if (getCor(ori->cartas[base + i]) == getCor(ori->cartas[base + i + 1])) return 0;
    return 1;
}

/* Agrupa todas as validações da sequência */
int validarSequencia(PILHA *ori, int n, RegrasMovAuto r) {
    if (!validarFlagMais(n, r))                  return 0;
    if (!validarFlagDescrescente(ori, n, r))      return 0;
    if (!validarFlagCrescente(ori, n, r))         return 0;
    if (!validarFlagMesmoNaipe(ori, n, r))        return 0;
    if (!validarFlagNaipesAlternados(ori, n, r))  return 0;
    if (!validarFlagMesmaCor(ori, n, r))          return 0;
    if (!validarFlagCoresAlternadas(ori, n, r))   return 0;
    return 1;
}

/* ============================================================
 *  VALIDAÇÃO RELATIVA AO DESTINO
 * ============================================================ */

/* Carta de topo da sequência a mover */
static CARTA topoSequencia(PILHA *ori, int n) {
    return ori->cartas[ori->tamanho - n];
}

/* Carta de fundo da sequência a mover */
static CARTA fundoSequencia(PILHA *ori, int n) {
    (void)n;
    return ori->cartas[ori->tamanho - 1];
}

int validarFlagMenor(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, '<')) return 1;
    if (des->tamanho == 0) return 0;
    return getValor(topoSequencia(ori, n)) == getValor(topoCarta(des)) - 1;
}

int validarFlagMaior(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, '>')) return 1;
    if (des->tamanho == 0) return 0;
    return getValor(topoSequencia(ori, n)) == getValor(topoCarta(des)) + 1;
}

int validarFlagTil(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, '~')) return 1;
    if (des->tamanho == 0) return 0;
    int diff = getValor(topoSequencia(ori, n)) - getValor(topoCarta(des));
    return (diff == 1 || diff == -1);
}

int validarFlagM(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'M')) return 1;
    if (des->tamanho == 0) return 0;
    return getNaipe(topoSequencia(ori, n)) == getNaipe(topoCarta(des));
}

int validarFlagX(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'X')) return 1;
    if (des->tamanho == 0) return 1;
    return getNaipe(topoSequencia(ori, n)) != getNaipe(topoCarta(des));
}

int validarFlagC(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'C')) return 1;
    if (des->tamanho == 0) return 0;
    return getCor(topoSequencia(ori, n)) == getCor(topoCarta(des));
}

int validarFlagD(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'D')) return 1;
    if (des->tamanho == 0) return 1;
    return getCor(topoSequencia(ori, n)) != getCor(topoCarta(des));
}

int validarFlagV(PILHA *des, RegrasMovAuto r) {
    if (!temFlag(r, 'V')) return 1;
    return des->tamanho == 0;
}

int validarFlagPequenoA(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'a')) return 1;
    return getValor(fundoSequencia(ori, n)) == 1; /* Ace at the end of moved sequence */
}

int validarFlagGrandeA(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'A')) return 1;
    return getValor(topoSequencia(ori, n)) == 1;  /* Ace at the start of moved sequence */
}

int validarFlagPequenoK(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'k')) return 1;
    return getValor(fundoSequencia(ori, n)) == 13; /* King at the end */
}

int validarFlagGrandeK(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'K')) return 1;
    return getValor(topoSequencia(ori, n)) == 13; /* King at the start */
}

/* Agrupa validações relativas ao destino */
int validarDestino(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (temFlag(r, '*')) return 1;
    if (!validarFlagMenor(ori, des, n, r))     return 0;
    if (!validarFlagMaior(ori, des, n, r))     return 0;
    if (!validarFlagTil(ori, des, n, r))       return 0;
    if (!validarFlagM(ori, des, n, r))         return 0;
    if (!validarFlagX(ori, des, n, r))         return 0;
    if (!validarFlagC(ori, des, n, r))         return 0;
    if (!validarFlagD(ori, des, n, r))         return 0;
    if (!validarFlagV(des, r))                 return 0;
    if (!validarFlagPequenoA(ori, n, r))       return 0;
    if (!validarFlagGrandeA(ori, n, r))        return 0;
    if (!validarFlagPequenoK(ori, n, r))       return 0;
    if (!validarFlagGrandeK(ori, n, r))        return 0;
    return 1;
}

/* ============================================================
 *  VALIDAÇÃO COMPLETA DE UM MOVIMENTO
 * ============================================================ */

int validarMovimento(ESTADO *j, int io, int id, int n, RegrasMovAuto rma) {
    if (io < 0 || io >= j->num_pilhas) return 0;
    if (id < 0 || id >= j->num_pilhas) return 0;
    if (io == id) return 0;
    PILHA *ori = &j->pilhas[io];
    PILHA *des = &j->pilhas[id];
    if (ori->tamanho < n || n <= 0) return 0;
    RegrasMovAuto r = encontrarRegra(rma, ori->tipo, des->tipo);
    if (r == NULL) return 0;
    if (!validarSequencia(ori, n, r)) return 0;
    if (!validarDestino(ori, des, n, r)) return 0;
    return 1;
}

/* ============================================================
 *  EXECUTAR MOVIMENTO (sem validação)
 * ============================================================ */

void executarMov(ESTADO *j, int io, int id, int n) {
    PILHA *ori = &j->pilhas[io];
    PILHA *des = &j->pilhas[id];
    int base = ori->tamanho - n;
    for (int i = 0; i < n; i++)
        pushCarta(des, ori->cartas[base + i]);
    ori->tamanho -= n;
}

int tentarMover(ESTADO *j, int io, int id, int n, RegrasMovAuto rma) {
    if (!validarMovimento(j, io, id, n, rma)) return 0;
    executarMov(j, io, id, n);
    return 1;
}

/* ============================================================
 *  AUTO
 * ============================================================ */

/* Tenta aplicar uma regra AUTO entre dois índices de pilha */
int tentarAutoEntrePilhas(ESTADO *j, RegrasMovAuto r, int io, int id) {
    PILHA *ori = &j->pilhas[io];
    PILHA *des = &j->pilhas[id];
    if (strcmp(ori->tipo, r->origem)  != 0) return 0;
    if (strcmp(des->tipo, r->destino) != 0) return 0;
    if (ori->tamanho <= 0) return 0;
    int n = 1;
    if (temFlag(r, '+')) n = ori->tamanho;
    for (; n >= 1; n--) {
        if (!validarSequencia(ori, n, r)) continue;
        if (!validarDestino(ori, des, n, r)) continue;
        executarMov(j, io, id, n);
        return 1;
    }
    return 0;
}

int tentarAutoUmaVez(ESTADO *j, RegrasMovAuto rma) {
    RegrasMovAuto aux = rma;
    while (aux != NULL) {
        if (strcmp(aux->comando, "AUTO") == 0) {
            for (int io = 0; io < j->num_pilhas; io++)
                for (int id = 0; id < j->num_pilhas; id++)
                    if (tentarAutoEntrePilhas(j, aux, io, id)) return 1;
        }
        aux = aux->prox;
    }
    return 0;
}

void processarAuto(ESTADO *j, RegrasMovAuto rma) {
    int encontrou;
    do {
        encontrou = tentarAutoUmaVez(j, rma);
        if (encontrou) {
            printf("\n[AUTO] Movimento automatico executado.\n");
            mostrarEstado(j);
        }
    } while (encontrou);
}

/* ============================================================
 *  VITÓRIA
 * ============================================================ */

int contarCartasTipo(ESTADO *j, const char *tipo) {
    int total = 0;
    for (int i = 0; i < j->num_pilhas; i++)
        if (strcmp(j->pilhas[i].tipo, tipo) == 0)
            total += j->pilhas[i].tamanho;
    return total;
}

int condicaoWinSatisfeita(ESTADO *j, RegrasWin rw) {
    int total = contarCartasTipo(j, rw->tipoDePilha);
    /* condicaoWin é cartas por pilha; conta pilhas do tipo */
    int npilhas = 0;
    for (int i = 0; i < j->num_pilhas; i++)
        if (strcmp(j->pilhas[i].tipo, rw->tipoDePilha) == 0) npilhas++;
    if (npilhas == 0) return 1;
    return total == rw->condicaoWin * npilhas;
}

int verificarVitoria(ESTADO *j, RegrasWin rw) {
    RegrasWin aux = rw;
    while (aux != NULL) {
        if (!condicaoWinSatisfeita(j, aux)) return 0;
        aux = aux->prox;
    }
    return 1;
}

/* ============================================================
 *  SAVE / LOAD
 * ============================================================ */

void gravarJogo(ESTADO *j, const char *nome_paciencia) {
    FILE *f = fopen("save.txt", "w");
    if (!f) { printf("Erro ao gravar.\n"); return; }
    fprintf(f, "%s\n", nome_paciencia);
    for (int i = 0; i < j->num_pilhas; i++) {
        for (int k = 0; k < j->pilhas[i].tamanho; k++)
            fprintf(f, "%s ", card2str(j->pilhas[i].cartas[k]));
        fprintf(f, "\n");
    }
    fclose(f);
    printf("Jogo gravado em save.txt\n");
}

/* Lê uma linha de save e preenche a pilha */
void carregarLinhaPilha(PILHA *p, char *linha) {
    char token[8];
    int pos = 0;
    while (sscanf(linha + pos, "%7s%n", token, &pos) == 1) {
        CARTA c = str2card(token);
        if (c > 0) pushCarta(p, c);
    }
}

int carregarJogo(ESTADO *j, RegrasInit ri, RegrasBaralhos rb) {
    FILE *f = fopen("save.txt", "r");
    if (!f) { printf("Sem ficheiro de save.\n"); return 0; }
    char linha[512];
    fgets(linha, 512, f); /* nome da paciência – ignorar */
    for (int i = 0; i < j->num_pilhas; i++) {
        j->pilhas[i].tamanho = 0;
        if (fgets(linha, 512, f) == NULL) break;
        carregarLinhaPilha(&j->pilhas[i], linha);
    }
    fclose(f);
    printf("Jogo carregado de save.txt\n");
    return 1;
    (void)ri; (void)rb;
}

/* ============================================================
 *  DISPLAY
 * ============================================================ */

int ehGrupoSuperior(const char *t) {
    if (strcmp(t, "FUND") == 0 || strcmp(t, "DESCARTE") == 0) return 1;
    return (strcmp(t, "CELL") == 0);
}

int maxAltura(ESTADO *j, int sup) {
    int m = 0;
    for (int i = 0; i < j->num_pilhas; i++) {
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup && j->pilhas[i].tamanho > m)
            m = j->pilhas[i].tamanho;
    }
    return m;
}

int printCabecalho(ESTADO *j, int sup) {
    int c = 0;
    for (int i = 0; i < j->num_pilhas; i++) {
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup) {
            printf("[%d:%s] ", i + 1, j->pilhas[i].tipo);
            c++;
        }
    }
    if (c > 0) printf("\n");
    return c;
}

void printLinha(ESTADO *j, int lin, int sup) {
    for (int i = 0; i < j->num_pilhas; i++) {
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup) {
            if (lin < j->pilhas[i].tamanho)
                printf("  %s\t", card2str(j->pilhas[i].cartas[lin]));
            else
                printf(" \t");
        }
    }
    printf("\n");
}

void printGrupo(ESTADO *j, int sup) {
    int m = maxAltura(j, sup);
    if (printCabecalho(j, sup) > 0) {
        for (int l = 0; l < m; l++)
            printLinha(j, l, sup);
    }
}

void imprimirTabuleiro(ESTADO *j) {
    if (j->num_pilhas == 0) return;
    printGrupo(j, 1); // Grupo superior (FUND, DESCARTE, CELL)
    printf("\n");
    printGrupo(j, 0); // Grupo TAB
}

void mostrarEstado(ESTADO *j) {
    printf("\n============= ESTADO DO JOGO =============\n");
    imprimirTabuleiro(j);
    printf("==========================================\n");
}

/* ============================================================
 *  INICIALIZAÇÃO DO ESTADO (BARALHO)
 * ============================================================ */

BARALHO criarBaralho(int n_baralhos) {
    int total = 52 * n_baralhos;
    BARALHO b = malloc(sizeof(CARTA) * total);
    for (int i = 0; i < total; i++) b[i] = (i % 52) + 1;
    return b;
}

void baralharBaralho(BARALHO b, int n_baralhos) {
    int total = 52 * n_baralhos;
    srand((unsigned)time(NULL));
    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        CARTA t = b[i]; b[i] = b[j]; b[j] = t;
    }
}

/* ============================================================
 *  CONSTRUÇÃO DO ESTADO A PARTIR DAS REGRAS INIT
 *  (percorre a lista de trás para a frente porque a lista
 *   foi construída ao contrário via prepend)
 * ============================================================ */

/* Conta quantos INITs existem */
int contarInits(RegrasInit ri) {
    int n = 0;
    RegrasInit aux = ri;
    while (aux) { n++; aux = aux->prox; }
    return n;
}

/* Vai ao último nó da lista */
RegrasInit ultimoInit(RegrasInit ri) {
    if (!ri) return NULL;
    while (ri->prox) ri = ri->prox;
    return ri;
}

/* Inicializa array de pilhas */
void inicializarPilhas(ESTADO *j, int n) {
    j->pilhas     = malloc(n * sizeof(PILHA));
    j->num_pilhas = n;
    for (int i = 0; i < n; i++) {
        j->pilhas[i].cartas    = NULL;
        j->pilhas[i].tamanho   = 0;
        j->pilhas[i].capacidade = 0;
        j->pilhas[i].tipo      = NULL;
    }
}

/* Preenche uma pilha a partir do índice do deck */
void preencherPilha(PILHA *p, const char *tipo, BARALHO deck,
                    int *idx, int n_cartas) {
    p->tipo = malloc(strlen(tipo) + 1);
    strcpy(p->tipo, tipo);
    for (int i = 0; i < n_cartas; i++)
        pushCarta(p, deck[(*idx)++]);
}

void aplicarInitAoEstado(ESTADO *j, RegrasInit ri, BARALHO deck) {
    int n = contarInits(ri);
    if (n == 0 || deck == NULL) return;
    
    inicializarPilhas(j, n);
    /* percorre do último para o primeiro (lista foi construída por prepend) */
    RegrasInit cur = ultimoInit(ri);
    int idx = 0, col = 0;
    while (cur != NULL && col < n) {
        if (cur->tipoDePilha)
            preencherPilha(&j->pilhas[col++], cur->tipoDePilha, deck, &idx, cur->numeroDeCartas);
        cur = cur->ant;
    }
}

/* ============================================================
 *  PROCESSAMENTO DE COMANDOS DO UTILIZADOR
 * ============================================================ */

void tratarAjuda(void) {
    printf("\nComandos:\n");
    printf("  p <o> <d> [n] - Move n cartas (default 1) da pilha o para d\n");
    printf("  e             - Mostra o estado actual\n");
    printf("  s             - Grava o jogo\n");
    printf("  l             - Carrega jogo gravado\n");
    printf("  r             - Reinicia\n");
    printf("  h             - Esta ajuda\n");
    printf("  q             - Sair\n");
}

void tratarMover(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasWin rw) {
    int o, d, n = 1;
    if (sscanf(buf, " p %d %d %d", &o, &d, &n) < 2) {
        printf("Uso: p <origem> <destino> [n]\n");
        return;
    }
    if (!tentarMover(j, o - 1, d - 1, n, rma))
        printf("Movimento invalido!\n");
    else {
        printf("\n[MOV] Movimento executado: %d -> %d (%d cartas)\n", o, d, n);
        mostrarEstado(j);
        processarAuto(j, rma);
        if (verificarVitoria(j, rw)) mostrar_mensagem(WIN);
    }
}

/* ============================================================
 *  LOOP PRINCIPAL
 * ============================================================ */

int executarComando(char *buf, ESTADO *j, RegrasMovAuto rma,
                    RegrasInit ri, RegrasBaralhos rb,
                    RegrasWin rw, const char *nome) {
    if (buf[0] == 'q') return 0;
    if (buf[0] == 'h') { tratarAjuda(); return 1; }
    if (buf[0] == 'e') { mostrarEstado(j); return 1; }
    if (buf[0] == 's') { gravarJogo(j, nome); return 1; }
    if (buf[0] == 'l') { carregarJogo(j, ri, rb); mostrarEstado(j); return 1; }
    if (buf[0] == 'r') {
        aplicarInitAoEstado(j, ri, j->B);
        mostrarEstado(j);
        return 1;
    }
    if (buf[0] == 'p') { tratarMover(buf, j, rma, rw); return 1; }
    printf("Comando desconhecido. Use 'h' para ajuda.\n");
    return 1;
}

void loopComandos(ESTADO *j, RegrasMovAuto rma, RegrasInit ri,
                  RegrasBaralhos rb, RegrasWin rw, const char *nome) {
    char buf[256];
    int continuar = 1;
    while (continuar) {
        const char *display_nome = (nome) ? nome : "Paciencia";
        printf("%s> ", display_nome);
        if (fgets(buf, 256, stdin) == NULL) break;
        buf[strcspn(buf, "\n")] = '\0';
        if (buf[0] != '\0')
            continuar = executarComando(buf, j, rma, ri, rb, rw, nome);
    }
}

/* ============================================================
 *  IMPRIMIR REGRAS (diagnóstico)
 * ============================================================ */

void imprimirMovs(RegrasMovAuto rma) {
    RegrasMovAuto aux = rma;
    while (aux) {
        printf("   %s %s %s [", aux->comando, aux->origem, aux->destino);
        for (int i = 0; aux->flags[0][i] != '\0'; i++)
            if (aux->flags[1][i] == '1') printf("%c", aux->flags[0][i]);
        printf("]\n");
        aux = aux->prox;
    }
}

void imprimirWins(RegrasWin rw) {
    while (rw) {
        printf("   WIN %s %d\n", rw->tipoDePilha, rw->condicaoWin);
        rw = rw->prox;
    }
}

/* ============================================================
 *  LIMPAR ESTADO
 * ============================================================ */

void limparEstado(ESTADO *j) {
    for (int i = 0; i < j->num_pilhas; i++) {
        free(j->pilhas[i].cartas);
        free(j->pilhas[i].tipo);
    }
    free(j->pilhas);
    free(j->B);
}

/* ============================================================
 *  ENTRY POINT
 * ============================================================ */

void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb,
             RegrasTipo rt, RegrasInit ri, RegrasWin rw) {
    ESTADO jogo;
    memset(&jogo, 0, sizeof(ESTADO));

    const char *nome = (rj && rj->jogoNome) ? rj->jogoNome : "Jogo Desconhecido";
    printf("\n=== %s ===\n", nome);

    int n_baralhos = (rb) ? rb->numeroDeBaralhos : 1;
    if (n_baralhos < 1) n_baralhos = 1;
    jogo.B = criarBaralho(n_baralhos);
    jogo.total_cartas_baralho = 52 * n_baralhos;
    baralharBaralho(jogo.B, rb->numeroDeBaralhos);

    printf("Movimentos validos:\n"); imprimirMovs(rma);
    printf("Condicoes de vitoria:\n"); imprimirWins(rw);

    aplicarInitAoEstado(&jogo, ri, jogo.B);
    mostrarEstado(&jogo);
    loopComandos(&jogo, rma, ri, rb, rw, nome);
    limparEstado(&jogo);

    (void)rt; /* RegrasTipo guardada mas não usada no display aqui */
}
