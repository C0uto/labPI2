#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "executeFile.h"

/* ============================================================
 *  CONVERSÃO CARTA <-> STRING
 * ============================================================ */

/**
 * Funcao para transformar o valor numerico da carta numa string
 *
 * * @param card -> O valor numerico da carta
 * * @param buf -> Buffer onde sera guardada a string resultante
 *
 */
void card2str(CARTA card, char *buf) {
    const char *valor = "A23456789TJQK";
    const char *naipe = "SHDC";
    if (card == 0 || !buf) { if (buf) buf[0] = '\0'; return; }
    int v = (card - 1) % 13;
    int n = (card - 1) / 13;
    buf[0] = valor[v];
    buf[1] = naipe[n];
    buf[2] = '\0';
}

/**
 * Funcao para transformar uma string (ex: "AH") num valor numerico de carta
 *
 * * @param s -> String que representa a carta
 * * @return res -> O valor numerico da carta (ou 0 se invalida)
 */
CARTA str2card(const char *s) {
    if (!s || !s[0]) return 0;
    int v, off;
    if (s[0] == '1' && s[1] == '0') { v = 9; off = 2; }
    else {
        const char *v_p = strchr("A23456789TJQK", s[0]);
        v = v_p ? (int)(v_p - "A23456789TJQK") : -1;
        off = 1;
    }
    const char *s_p = strchr("SHDC", s[off]);
    int n = s_p ? (int)(s_p - "SHDC") : -1;
    return (v >= 0 && n >= 0) ? n * 13 + v + 1 : 0;
}

/* ============================================================
 *  PROPRIEDADES DE CARTA
 * ============================================================ */

/**
 * Funcao para obter o valor (rank) de uma carta (1 a 13)
 *
 * * @param c -> O valor numerico da carta
 * * @return res -> O rank da carta
 */
int getValor(CARTA c) { return (c <= 0) ? 0 : ((c - 1) % 13) + 1; }

/**
 * Funcao para obter o naipe de uma carta (0 a 3)
 *
 * * @param c -> O valor numerico da carta
 * * @return res -> O indice do naipe (S=0, H=1, D=2, C=3)
 */
int getNaipe(CARTA c) { return (c <= 0) ? -1 : (c - 1) / 13; }

/**
 * Funcao para obter a cor de uma carta (preto ou vermelho)
 *
 * * @param c -> O valor numerico da carta
 * * @return res -> 1 se for vermelha (H, D), 0 se for preta (S, C)
 */
int getCor(CARTA c)   {
    int n = getNaipe(c);
    /* S=0(preto), H=1(vermelho), D=2(vermelho), C=3(preto) */
    return (n == 1 || n == 2) ? 1 : 0;
}

/* ============================================================
 *  OPERAÇÕES SOBRE PILHAS
 * ============================================================ */

/**
 * Funcao para obter o numero de cartas numa pilha
 *
 * * @param p -> Ponteiro para a pilha
 * * @return res -> Quantidade de cartas na pilha
 */
int getPilhaTamanho(PILHA *p) { return p->tamanho; }

/**
 * Funcao para adicionar uma carta ao topo de uma pilha
 *
 * * @param p -> Ponteiro para a pilha
 * * @param c -> A carta a ser adicionada
 */
void pushCarta(PILHA *p, CARTA c) {
    if (p->tamanho >= p->capacidade) {
        p->capacidade += 16;
        p->cartas = realloc(p->cartas, p->capacidade * sizeof(CARTA));
    }
    p->cartas[p->tamanho++] = c;
}

/**
 * Funcao para remover e retornar a carta do topo de uma pilha
 *
 * * @param p -> Ponteiro para a pilha
 * * @return res -> A carta removida (ou 0 se vazia)
 */
CARTA popCarta(PILHA *p) {
    if (p->tamanho <= 0) return 0;
    return p->cartas[--p->tamanho];
}

/**
 * Funcao para consultar a carta do topo de uma pilha sem a remover
 *
 * * @param p -> Ponteiro para a pilha
 * * @return res -> A carta no topo
 */
CARTA topoCarta(PILHA *p) {
    if (p->tamanho <= 0) return 0;
    return p->cartas[p->tamanho - 1];
}

/* ============================================================
 *  ACESSO A FLAGS
 * ============================================================ */

/**
 * Funcao para verificar se uma determinada flag esta ativa numa regra
 *
 * * @param r -> A estrutura da regra
 * * @param f -> O carater da flag a procurar
 * * @return res -> 1 se a flag estiver ativa, 0 caso contrario
 */
int temFlag(RegrasMovAuto r, char f) {
    if (!r) return 0;
    for (int i = 0; r->flags[0][i] != '\0'; i++)
        if (r->flags[0][i] == f && r->flags[1][i] == '1') return 1;
    return 0;
}

/* ============================================================
 *  ENCONTRAR REGRA MOV/AUTO
 * ============================================================ */

/**
 * Funcao para encontrar uma regra de movimento especifica para um par de tipos de pilha
 *
 * * @param rma -> Lista de regras de movimento
 * * @param ori -> Tipo da pilha de origem
 * * @param des -> Tipo da pilha de destino
 * * @return res -> A regra encontrada ou NULL
 */
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

/**
 * Funcao para validar a flag '+': permite mover mais do que uma carta
 *
 * * @param n -> Numero de cartas a mover
 * * @param r -> Regra de movimento
 * * @return res -> 1 se valido, 0 se invalido
 */
int validarFlagMais(int n, RegrasMovAuto r) {
    if (n > 1 && !temFlag(r, '+')) return 0;
    return 1;
}

/**
 * Funcao para validar a flag '[': sequencia deve ser decrescente consecutiva
 *
 * * @param ori -> Pilha de origem
 * * @param n -> Numero de cartas na sequencia
 * * @param r -> Regra de movimento
 * * @return res -> 1 se a sequencia for decrescente, 0 caso contrario
 */
int validarFlagDescrescente(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, '[')) return 1;
    int base = ori->tamanho - n;
    for (int i = 0; i < n - 1; i++) {
        if (getValor(ori->cartas[base + i]) != getValor(ori->cartas[base + i + 1]) + 1)
            return 0;
    }
    return 1;
}

/**
 * Funcao para validar a flag ']': sequencia deve ser crescente consecutiva
 *
 * * @param ori -> Pilha de origem
 * * @param n -> Numero de cartas na sequencia
 * * @param r -> Regra de movimento
 * * @return res -> 1 se a sequencia for crescente, 0 caso contrario
 */
int validarFlagCrescente(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, ']')) return 1;
    int base = ori->tamanho - n;
    for (int i = 0; i < n - 1; i++) {
        if (getValor(ori->cartas[base + i]) != getValor(ori->cartas[base + i + 1]) - 1)
            return 0;
    }
    return 1;
}

/**
 * Funcao para validar a flag 'm': todas as cartas da sequencia devem ter o mesmo naipe
 *
 * * @param ori -> Pilha de origem
 * * @param n -> Numero de cartas na sequencia
 * * @param r -> Regra de movimento
 * * @return res -> 1 se todas tiverem o mesmo naipe, 0 caso contrario
 */
int validarFlagMesmoNaipe(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'm')) return 1;
    int base = ori->tamanho - n;
    int naipe = getNaipe(ori->cartas[base]);
    for (int i = 1; i < n; i++)
        if (getNaipe(ori->cartas[base + i]) != naipe) return 0;
    return 1;
}

/**
 * Funcao para validar a flag 'x': naipes devem alternar na sequencia
 *
 * * @param ori -> Pilha de origem
 * * @param n -> Numero de cartas na sequencia
 * * @param r -> Regra de movimento
 * * @return res -> 1 se os naipes alternarem, 0 caso contrario
 */
int validarFlagNaipesAlternados(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'x')) return 1;
    int base = ori->tamanho - n;
    for (int i = 0; i < n - 1; i++)
        if (getNaipe(ori->cartas[base + i]) == getNaipe(ori->cartas[base + i + 1])) return 0;
    return 1;
}

/**
 * Funcao para validar a flag 'c': todas as cartas da sequencia devem ter a mesma cor
 *
 * * @param ori -> Pilha de origem
 * * @param n -> Numero de cartas na sequencia
 * * @param r -> Regra de movimento
 * * @return res -> 1 se todas tiverem a mesma cor, 0 caso contrario
 */
int validarFlagMesmaCor(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'c')) return 1;
    int base = ori->tamanho - n;
    int cor = getCor(ori->cartas[base]);
    for (int i = 1; i < n; i++)
        if (getCor(ori->cartas[base + i]) != cor) return 0;
    return 1;
}

/**
 * Funcao para validar a flag 'd': cores devem alternar na sequencia
 *
 * * @param ori -> Pilha de origem
 * * @param n -> Numero de cartas na sequencia
 * * @param r -> Regra de movimento
 * * @return res -> 1 se as cores alternarem, 0 caso contrario
 */
int validarFlagCoresAlternadas(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'd')) return 1;
    int base = ori->tamanho - n;
    for (int i = 0; i < n - 1; i++)
        if (getCor(ori->cartas[base + i]) == getCor(ori->cartas[base + i + 1])) return 0;
    return 1;
}

/**
 * Funcao que agrupa todas as validacoes de flags relativas a sequencia de cartas a mover
 *
 * * @param ori -> Pilha de origem
 * * @param n -> Numero de cartas
 * * @param r -> Regra de movimento
 * * @return res -> 1 se a sequencia for valida segundo todas as flags, 0 caso contrario
 */
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

/**
 * Funcao interna para obter a carta que ficara no topo da sequencia apos o movimento
 *
 * * @param ori -> Pilha de origem
 * * @param n -> Numero de cartas a mover
 * * @return res -> A carta que serve de topo da sequencia a mover
 */
static CARTA topoSequencia(PILHA *ori, int n) {
    return ori->cartas[ori->tamanho - n];
}

/**
 * Funcao interna para obter a carta que ficara no fundo da sequencia movida
 *
 * * @param ori -> Pilha de origem
 * * @return res -> A carta no fundo da sequencia
 */
static CARTA fundoSequencia(PILHA *ori) {
    return ori->cartas[ori->tamanho - 1];
}

/**
 * Funcao para validar a flag '<': topo da sequencia deve ser menor que topo do destino
 *
 * * @param ori -> Pilha origem
 * * @param des -> Pilha destino
 * * @param n -> Numero de cartas
 * * @param r -> Regra
 * * @return res -> 1 se valido, 0 caso contrario
 */
int validarFlagMenor(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, '<')) return 1;
    if (des->tamanho == 0) return 0;
    return getValor(topoSequencia(ori, n)) == getValor(topoCarta(des)) - 1;
}

/**
 * Funcao para validar a flag '>': topo da sequencia deve ser maior que topo do destino
 *
 * * @param ori -> Pilha origem
 * * @param des -> Pilha destino
 * * @param n -> Numero de cartas
 * * @param r -> Regra
 * * @return res -> 1 se valido, 0 caso contrario
 */
int validarFlagMaior(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, '>')) return 1;
    if (des->tamanho == 0) return 0;
    return getValor(topoSequencia(ori, n)) == getValor(topoCarta(des)) + 1;
}

/**
 * Funcao para validar a flag '~': diferenca de valor absoluta deve ser 1 entre topos
 *
 * * @param ori -> Pilha origem
 * * @param des -> Pilha destino
 * * @param n -> Numero de cartas
 * * @param r -> Regra
 * * @return res -> 1 se valido, 0 caso contrario
 */
int validarFlagTil(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, '~')) return 1;
    if (des->tamanho == 0) return 0;
    int diff = getValor(topoSequencia(ori, n)) - getValor(topoCarta(des));
    return (diff == 1 || diff == -1);
}

/**
 * Funcao para validar a flag 'M': mesmo naipe entre topos de origem e destino
 *
 * * @param ori -> Pilha origem
 * * @param des -> Pilha destino
 * * @param n -> Numero de cartas
 * * @param r -> Regra
 * * @return res -> 1 se valido, 0 caso contrario
 */
int validarFlagM(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'M')) return 1;
    if (des->tamanho == 0) return 0;
    return getNaipe(topoSequencia(ori, n)) == getNaipe(topoCarta(des));
}

/**
 * Funcao para validar a flag 'X': naipes diferentes entre topos
 *
 * * @param ori -> Pilha origem
 * * @param des -> Pilha destino
 * * @param n -> Numero de cartas
 * * @param r -> Regra
 * * @return res -> 1 se valido, 0 caso contrario
 */
int validarFlagX(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'X')) return 1;
    if (des->tamanho == 0) return 1;
    return getNaipe(topoSequencia(ori, n)) != getNaipe(topoCarta(des));
}

/**
 * Funcao para validar a flag 'C': mesma cor entre topos
 *
 * * @param ori -> Pilha origem
 * * @param des -> Pilha destino
 * * @param n -> Numero de cartas
 * * @param r -> Regra
 * * @return res -> 1 se valido, 0 caso contrario
 */
int validarFlagC(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'C')) return 1;
    if (des->tamanho == 0) return 0;
    return getCor(topoSequencia(ori, n)) == getCor(topoCarta(des));
}

/**
 * Funcao para validar a flag 'D': cores diferentes entre topos
 *
 * * @param ori -> Pilha origem
 * * @param des -> Pilha destino
 * * @param n -> Numero de cartas
 * * @param r -> Regra
 * * @return res -> 1 se valido, 0 caso contrario
 */
int validarFlagD(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'D')) return 1;
    if (des->tamanho == 0) return 1;
    return getCor(topoSequencia(ori, n)) != getCor(topoCarta(des));
}

/**
 * Funcao para validar a flag 'V': destino deve estar vazio
 *
 * * @param des -> Pilha destino
 * * @param r -> Regra
 * * @return res -> 1 se valido, 0 caso contrario
 */
int validarFlagV(PILHA *des, RegrasMovAuto r) {
    if (!temFlag(r, 'V')) return 1;
    return des->tamanho == 0;
}

/* --- HELPERS PARA DESTINO --- */

/**
 * Funcao auxiliar que valida as flags basicas de relacao de valor entre origem e destino
 *
 * * @param ori -> Pilha origem
 * * @param des -> Pilha destino
 * * @param n -> Numero de cartas
 * * @param r -> Regra
 * * @return res -> 1 se todas as flags basicas passarem
 */
int validarFlagsBasicas(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    return validarFlagMenor(ori, des, n, r) && validarFlagMaior(ori, des, n, r) &&
           validarFlagTil(ori, des, n, r) && validarFlagV(des, r);
}

/**
 * Funcao auxiliar que valida flags de atributos (cor/naipe) entre origem e destino
 *
 * * @param ori -> Pilha origem
 * * @param des -> Pilha destino
 * * @param n -> Numero de cartas
 * * @param r -> Regra
 * * @return res -> 1 se todas passarem
 */
int validarFlagsAtributos(PILHA *ori, PILHA *des, int n, RegrasMovAuto r) {
    return validarFlagM(ori, des, n, r) && validarFlagX(ori, des, n, r) &&
           validarFlagC(ori, des, n, r) && validarFlagD(ori, des, n, r);
}

/**
 * Funcao para validar flags de Ranks especificos (a, A, k, K) na sequencia movida
 *
 * * @param o -> Pilha de origem
 * * @param n -> Numero de cartas
 * * @param r -> Regra
 * * @return res -> 1 se as restricoes de rank forem cumpridas
 */
int validarFlagsRanks(PILHA *o, int n, RegrasMovAuto r) {
    int a = (temFlag(r, 'a') ? getValor(fundoSequencia(o)) == 1 : 1);
    int A = (temFlag(r, 'A') ? getValor(topoSequencia(o, n)) == 1 : 1);
    int k = (temFlag(r, 'k') ? getValor(fundoSequencia(o)) == 13 : 1);
    int K = (temFlag(r, 'K') ? getValor(topoSequencia(o, n)) == 13 : 1);
    return a && A && k && K;
}

/**
 * Funcao que realiza a validacao completa das condicoes de destino de uma regra
 *
 * * @param o -> Pilha origem
 * * @param d -> Pilha destino
 * * @param n -> Numero de cartas
 * * @param r -> Regra
 * * @return res -> 1 se o destino for valido para o movimento
 */
int validarDestino(PILHA *o, PILHA *d, int n, RegrasMovAuto r) {
    if (temFlag(r, '*')) return 1;
    return validarFlagsBasicas(o, d, n, r) && 
           validarFlagsAtributos(o, d, n, r) && 
           validarFlagsRanks(o, n, r);
}

/* ============================================================
 *  VALIDAÇÃO COMPLETA DE UM MOVIMENTO
 * ============================================================ */

/**
 * Funcao para verificar se um tipo de pilha tem a restricao de conter no maximo 1 carta
 *
 * * @param rt -> Lista de regras de tipo
 * * @param tipo -> Nome do tipo de pilha (ex: "FUND")
 * * @return res -> 1 se tiver a flag '1', 0 caso contrario
 */
int tipoTemFlag1(RegrasTipo rt, const char *tipo) {
    RegrasTipo aux = rt;
    while (aux) {
        if (strcmp(aux->tipoDePilha, tipo) == 0) {
            for (int i = 0; aux->flags[0][i] != '\0'; i++)
                if (aux->flags[0][i] == '1' && aux->flags[1][i] == '1') return 1;
        }
        aux = aux->prox;
    }
    return 0;
}

/**
 * Funcao interna para validar se os indices das pilhas e a quantidade de cartas sao plausiveis
 *
 * * @param j -> Estado do jogo
 * * @param io -> Indice da pilha de origem
 * * @param id -> Indice da pilha de destino
 * * @param n -> Numero de cartas a mover
 * * @return res -> 1 se indices sao validos
 */
static int indicesValidos(ESTADO *j, int io, int id, int n) {
    if (io < 0 || io >= j->num_pilhas || id < 0 || id >= j->num_pilhas) return 0;
    return (io != id && n > 0);
}

/**
 * Funcao interna que verifica se uma regra especifica valida o movimento entre duas pilhas
 *
 * * @param o -> Ponteiro para pilha de origem
 * * @param d -> Ponteiro para pilha de destino
 * * @param n -> Numero de cartas
 * * @param r -> Regra a testar
 * * @return res -> 1 se a regra valida o movimento
 */
static int regraValida(PILHA *o, PILHA *d, int n, RegrasMovAuto r) {
    if (strcmp(r->comando, "MOV") || strcmp(r->origem, o->tipo) || strcmp(r->destino, d->tipo)) return 0;
    return (o->tamanho >= n && validarSequencia(o, n, r) && validarDestino(o, d, n, r));
}

/**
 * Funcao principal para validar um movimento solicitado pelo utilizador
 *
 * * @param j -> Estado do jogo
 * * @param io, id, n -> Origem, destino e quantidade
 * * @param rma, rt -> Listas de regras
 * * @return res -> 1 se o movimento for permitido, 0 caso contrario
 */
int validarMovimento(ESTADO *j, int io, int id, int n, RegrasMovAuto rma, RegrasTipo rt) {
    if (!indicesValidos(j, io, id, n)) return 0;
    PILHA *d = &j->pilhas[id];
    if (tipoTemFlag1(rt, d->tipo) && (d->tamanho + n) > 1) return 0;
    for (RegrasMovAuto r = rma; r; r = r->prox)
        if (regraValida(&j->pilhas[io], d, n, r)) return 1;
    return 0;
}

/* ============================================================
 *  EXECUTAR MOVIMENTO (sem validação)
 * ============================================================ */

/**
 * Funcao que realiza a transferencia física das cartas entre pilhas
 *
 * * @param j -> Estado do jogo
 * * @param io, id -> Indices de origem e destino
 * * @param n -> Numero de cartas a transferir
 */
void executarMov(ESTADO *j, int io, int id, int n) {
    PILHA *ori = &j->pilhas[io];
    PILHA *des = &j->pilhas[id];
    int base = ori->tamanho - n;
    for (int i = 0; i < n; i++)
        pushCarta(des, ori->cartas[base + i]);
    ori->tamanho -= n;
}

/**
 * Funcao que tenta validar e, em caso de sucesso, executa o movimento
 *
 * * @param j -> Estado do jogo
 * * @param io, id, n -> Parametros do movimento
 * * @param rma, rt -> Regras
 * * @return res -> 1 se moveu com sucesso, 0 se falhou a validacao
 */
int tentarMover(ESTADO *j, int io, int id, int n, RegrasMovAuto rma, RegrasTipo rt) {
    if (!validarMovimento(j, io, id, n, rma, rt)) return 0;
    executarMov(j, io, id, n);
    return 1;
}

/* ============================================================
 *  AUTO
 * ============================================================ */

/**
 * Funcao auxiliar para validar e executar um movimento automatico especifico
 *
 * * @param j -> Estado do jogo
 * * @param r -> Regra de AUTO
 * * @param io, id, n -> Indices e quantidade
 * * @param rt -> Regras de tipo
 * * @return res -> 1 se executou, 0 caso contrario
 */
int executarAutoSeValido(ESTADO *j, RegrasMovAuto r, int io, int id, int n, RegrasTipo rt) {
    PILHA *d = &j->pilhas[id];
    if (tipoTemFlag1(rt, d->tipo) && (d->tamanho + n) > 1) return 0;
    if (validarSequencia(&j->pilhas[io], n, r) && 
        validarDestino(&j->pilhas[io], d, n, r)) {
        executarMov(j, io, id, n);
        return 1;
    }
    return 0;
}

/**
 * Funcao que tenta aplicar uma regra de AUTO entre duas pilhas especificas
 *
 * * @param j -> Estado do jogo
 * * @param r -> Regra de AUTO
 * * @param io, id -> Indices das pilhas
 * * @param rt -> Regras de tipo
 * * @return res -> 1 se conseguiu fazer o movimento
 */
int tentarAutoEntrePilhas(ESTADO *j, RegrasMovAuto r, int io, int id, RegrasTipo rt) {
    if (strcmp(j->pilhas[io].tipo, r->origem) != 0 || 
        strcmp(j->pilhas[id].tipo, r->destino) != 0) return 0;
    int n = temFlag(r, '+') ? j->pilhas[io].tamanho : 1;
    while (n >= 1) {
        if (executarAutoSeValido(j, r, io, id, n, rt)) return 1;
        n--;
    }
    return 0;
}

/**
 * Funcao que percorre todas as regras de AUTO e tenta realizar um movimento
 *
 * * @param j -> Estado do jogo
 * * @param rma, rt -> Listas de regras
 * * @return res -> 1 se encontrou e realizou um movimento, 0 caso contrario
 */
int tentarAutoUmaVez(ESTADO *j, RegrasMovAuto rma, RegrasTipo rt) {
    RegrasMovAuto aux = rma;
    while (aux != NULL) {
        if (strcmp(aux->comando, "AUTO") == 0) {
            for (int io = 0; io < j->num_pilhas; io++)
                for (int id = 0; id < j->num_pilhas; id++)
                    if (tentarAutoEntrePilhas(j, aux, io, id, rt)) return 1;
        }
        aux = aux->prox;
    }
    return 0;
}

/**
 * Funcao que executa movimentos automaticos em loop ate que nenhum mais seja possivel
 *
 * * @param j -> Estado do jogo
 * * @param rma, rt -> Regras
 */
void processarAuto(ESTADO *j, RegrasMovAuto rma, RegrasTipo rt) {
    int encontrou;
    do {
        encontrou = tentarAutoUmaVez(j, rma, rt);
        if (encontrou) {
            printf("\n[AUTO] Movimento automatico executado.\n");
            mostrarEstado(j);
        }
    } while (encontrou);
}

/* ============================================================
 *  VITÓRIA
 * ============================================================ */

/**
 * Funcao para contar o total de cartas presentes em todas as pilhas de um certo tipo
 *
 * * @param j -> Estado do jogo
 * * @param tipo -> Nome do tipo de pilha (ex: "TAB")
 * * @return res -> Numero total de cartas
 */
int contarCartasTipo(ESTADO *j, const char *tipo) {
    int total = 0;
    for (int i = 0; i < j->num_pilhas; i++)
        if (strcmp(j->pilhas[i].tipo, tipo) == 0)
            total += j->pilhas[i].tamanho;
    return total;
}

/**
 * Funcao que verifica se uma condicao de vitoria especifica foi atingida
 *
 * * @param j -> Estado do jogo
 * * @param rw -> Estrutura da regra de vitoria
 * * @return res -> 1 se satisfeita, 0 caso contrario
 */
int condicaoWinSatisfeita(ESTADO *j, RegrasWin rw) {
    int total = contarCartasTipo(j, rw->tipoDePilha);
    /* condicaoWin é cartas por pilha; conta pilhas do tipo */
    int npilhas = 0;
    for (int i = 0; i < j->num_pilhas; i++)
        if (strcmp(j->pilhas[i].tipo, rw->tipoDePilha) == 0) npilhas++;
    if (npilhas == 0) return 1;
    return total == rw->condicaoWin * npilhas;
}

/**
 * Funcao que verifica se todas as condicoes de vitoria do ficheiro foram atingidas
 *
 * * @param j -> Estado do jogo
 * * @param rw -> Lista de regras de vitoria
 * * @return res -> 1 se o jogador venceu
 */
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

/**
 * Funcao para escrever o conteúdo de uma pilha num ficheiro de save
 *
 * * @param f -> Ponteiro do ficheiro aberto
 * * @param p -> Ponteiro da pilha
 */
void gravarPilha(FILE *f, PILHA *p) {
    for (int k = 0; k < p->tamanho; k++) {
        char buf[4];
        card2str(p->cartas[k], buf);
        fprintf(f, "%s ", buf);
    }
    fprintf(f, "\n");
}

/**
 * Funcao para guardar o estado atual do jogo num ficheiro chamado "save.txt"
 *
 * * @param j -> Estado do jogo
 * * @param nome_paciencia -> Nome da paciencia atual para guardar no topo do ficheiro
 */
void gravarJogo(ESTADO *j, const char *nome_paciencia) {
    FILE *f = fopen("save.txt", "w");
    if (!f) { printf("Erro ao gravar.\n"); return; }
    fprintf(f, "%s.txt\n", nome_paciencia);
    for (int i = 0; i < j->num_pilhas; i++)
        gravarPilha(f, &j->pilhas[i]);
    fclose(f);
    printf("Jogo gravado em save.txt\n");
}

/**
 * Funcao para carregar as cartas de uma linha de texto para uma estrutura de pilha
 *
 * * @param p -> Ponteiro da pilha de destino
 * * @param linha -> String com as cartas (ex: "AH 2S 3D")
 */
void carregarLinhaPilha(PILHA *p, char *linha) {
    char token[8];
    int pos = 0, delta = 0;
    while (sscanf(linha + pos, "%7s%n", token, &delta) == 1) {
        pos += delta;
        CARTA c = str2card(token);
        if (c > 0) pushCarta(p, c);
    }
}

/**
 * Funcao para processar a leitura de uma pilha a partir do ficheiro de save
 *
 * * @param j -> Estado do jogo
 * * @param i -> Indice da pilha
 * * @param f -> Ponteiro do ficheiro
 */
void processarLinhaSave(ESTADO *j, int i, FILE *f) {
    char linha[512];
    j->pilhas[i].tamanho = 0;
    if (fgets(linha, 512, f)) carregarLinhaPilha(&j->pilhas[i], linha);
}

/**
 * Funcao principal para carregar o jogo a partir de "save.txt"
 *
 * * @param j -> Estado do jogo
 * * @return res -> 1 se carregou com sucesso, 0 caso contrario
 */
int carregarJogo(ESTADO *j) {
    FILE *f = fopen("save.txt", "r");
    if (!f) { printf("Sem ficheiro de save.\n"); return 0; }
    char b[512];
    fgets(b, 512, f); /* skip name line */
    for (int i = 0; i < j->num_pilhas; i++) processarLinhaSave(j, i, f);
    fclose(f);
    return 1;
}

/* ============================================================
 *  DISPLAY
 * ============================================================ */

/**
 * Funcao para identificar se um tipo de pilha pertence ao grupo superior do ecra
 *
 * * @param t -> Nome do tipo de pilha
 * * @return res -> 1 se for superior (FUND, DESCARTE, CELL), 0 caso contrario
 */
int ehGrupoSuperior(const char *t) {
    if (strcmp(t, "FUND") == 0 || strcmp(t, "DESCARTE") == 0) return 1;
    return (strcmp(t, "CELL") == 0);
}

/**
 * Funcao para calcular a maior quantidade de cartas entre pilhas de um certo grupo
 *
 * * @param j -> Estado do jogo
 * * @param sup -> 1 para grupo superior, 0 para grupo inferior (TAB)
 * * @return res -> Altura máxima encontrada
 */
int maxAltura(ESTADO *j, int sup) {
    int m = 0;
    for (int i = 0; i < j->num_pilhas; i++) {
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup && j->pilhas[i].tamanho > m)
            m = j->pilhas[i].tamanho;
    }
    return m;
}

/**
 * Funcao para imprimir os cabecalhos (nomes e indices) de um grupo de pilhas
 *
 * * @param j -> Estado do jogo
 * * @param sup -> 1 para superior, 0 para inferior
 * * @return res -> Numero de pilhas impressas
 */
int printCabecalho(ESTADO *j, int sup) {
    int c = 0;
    for (int i = 0; i < j->num_pilhas; i++) {
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup) {
            printf(" [%2d:%-8s] ", i + 1, j->pilhas[i].tipo);
            c++;
        }
    }
    if (c > 0) printf("\n");
    return c;
}

/**
 * Funcao para descobrir qual o carater de visibilidade associado a um tipo de pilha
 *
 * * @param rt -> Lista de regras de tipo
 * * @param tipo -> Nome do tipo de pilha
 * * @return res -> Carater '=', '_' ou '^'
 */
char visibilidadeTipo(RegrasTipo rt, const char *tipo) {
    RegrasTipo aux = rt;
    while (aux) {
        if (strcmp(aux->tipoDePilha, tipo) == 0) {
            for (int i = 0; aux->flags[0][i] != '\0'; i++) {
                char f = aux->flags[0][i];
                if ((f == '=' || f == '_' || f == '^') && aux->flags[1][i] == '1')
                    return f;
            }
        }
        aux = aux->prox;
    }
    return '='; /* default: todas visíveis */
}

/**
 * Funcao para imprimir uma carta especifica (ou o seu verso) de uma pilha numa linha
 *
 * * @param j -> Estado do jogo
 * * @param i -> Indice da pilha
 * * @param lin -> Indice da carta (linha)
 */
void printCelula(ESTADO *j, int i, int lin) {
    PILHA *p = &j->pilhas[i];
    if (lin >= p->tamanho) { printf("               "); return; }
    char vis = visibilidadeTipo(j->rt, p->tipo);
    int mostrar = (vis == '=') || (vis == '^' && lin == p->tamanho - 1);
    if (mostrar) {
        char buf[4];
        card2str(p->cartas[lin], buf);
        printf("      %-2s       ", buf);
    } else {
        printf("      **       ");
    }
}

/**
 * Funcao para imprimir uma linha completa horizontal do tabuleiro
 *
 * * @param j -> Estado do jogo
 * * @param lin -> Indice da linha
 * * @param sup -> Grupo (1 superior, 0 inferior)
 */
void printLinha(ESTADO *j, int lin, int sup) {
    for (int i = 0; i < j->num_pilhas; i++) {
        if (ehGrupoSuperior(j->pilhas[i].tipo) == sup)
            printCelula(j, i, lin);
    }
    printf("\n");
}

/**
 * Funcao para imprimir um bloco completo de pilhas (ex: todas as TAB)
 *
 * * @param j -> Estado do jogo
 * * @param sup -> 1 para superior, 0 para inferior
 */
void printGrupo(ESTADO *j, int sup) {
    int m = maxAltura(j, sup);
    if (printCabecalho(j, sup) > 0) {
        for (int l = 0; l < m; l++)
            printLinha(j, l, sup);
    }
}

/**
 * Funcao principal de desenho do tabuleiro no terminal
 *
 * * @param j -> Estado do jogo
 */
void imprimirTabuleiro(ESTADO *j) {
    if (j->num_pilhas == 0) return;
    printGrupo(j, 1); // Grupo superior (FUND, DESCARTE, CELL)
    printf("\n");
    printGrupo(j, 0); // Grupo TAB
}

/**
 * Funcao que limpa o ecra visualmente com o novo estado
 *
 * * @param j -> Estado do jogo
 */
void mostrarEstado(ESTADO *j) {
    printf("\n============= ESTADO DO JOGO =============\n");
    imprimirTabuleiro(j);
    printf("==========================================\n");
}

/* ============================================================
 *  INICIALIZAÇÃO DO ESTADO (BARALHO)
 * ============================================================ */

/**
 * Funcao para criar e preencher um baralho sequencialmente
 *
 * * @param n_baralhos -> Numero de baralhos de 52 cartas
 * * @return res -> Array de cartas alocado
 */
BARALHO criarBaralho(int n_baralhos) {
    int total = 52 * n_baralhos;
    BARALHO b = malloc(sizeof(CARTA) * total);
    for (int i = 0; i < total; i++) b[i] = (i % 52) + 1;
    return b;
}

/**
 * Funcao para baralhar aleatoriamente as cartas do deck
 *
 * * @param b -> O baralho
 * * @param n_baralhos -> Quantidade de baralhos usados
 */
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

/**
 * Funcao para contar o numero de blocos INIT definidos no ficheiro
 *
 * * @param ri -> Lista de regras INIT
 * * @return res -> Numero de pilhas a criar inicialmente
 */
int contarInits(RegrasInit ri) {
    int n = 0;
    RegrasInit aux = ri;
    while (aux) { n++; aux = aux->prox; }
    return n;
}

/**
 * Funcao para navegar ate ao final da lista duplamente ligada de INIT
 *
 * * @param ri -> Lista
 * * @return res -> Ponteiro para o ultimo elemento
 */
RegrasInit ultimoInit(RegrasInit ri) {
    if (!ri) return NULL;
    while (ri->prox) ri = ri->prox;
    return ri;
}

/**
 * Funcao para alocar a memoria necessaria para as pilhas do jogo
 *
 * * @param j -> Estado do jogo
 * * @param n -> Numero de pilhas
 */
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

/**
 * Funcao para preencher uma pilha individual com cartas do baralho baralhado
 *
 * * @param p -> Ponteiro para a pilha
 * * @param tipo -> String com o tipo da pilha
 * * @param deck -> O baralho
 * * @param idx -> Ponteiro para o indice atual do baralho
 * * @param n_cartas -> Numero de cartas a distribuir nesta pilha
 */
void preencherPilha(PILHA *p, const char *tipo, BARALHO deck,
                    int *idx, int n_cartas) {
    p->tipo = malloc(strlen(tipo) + 1);
    strcpy(p->tipo, tipo);
    for (int i = 0; i < n_cartas; i++)
        pushCarta(p, deck[(*idx)++]);
}

/**
 * Funcao que aplica todas as regras de inicializacao ao estado do jogo
 *
 * * @param j -> Estado do jogo
 * * @param ri -> Lista de regras INIT
 * * @param deck -> Baralho baralhado
 */
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
 *  UNDO – HISTÓRICO DE ESTADOS
 * ============================================================ */

/**
 * Funcao para guardar uma "fotografia" do estado atual das cartas para permitir undo
 *
 * * @param j -> Estado do jogo
 */
void guardarSnapshot(ESTADO *j) {
    SNAPSHOT *s = malloc(sizeof(SNAPSHOT));
    s->num_pilhas = j->num_pilhas;
    s->cartas   = malloc(j->num_pilhas * sizeof(CARTA *));
    s->tamanhos = malloc(j->num_pilhas * sizeof(int));
    for (int i = 0; i < j->num_pilhas; i++) {
        s->tamanhos[i] = j->pilhas[i].tamanho;
        s->cartas[i]   = malloc(j->pilhas[i].tamanho * sizeof(CARTA));
        memcpy(s->cartas[i], j->pilhas[i].cartas, j->pilhas[i].tamanho * sizeof(CARTA));
    }
    s->prox = j->historico;
    j->historico = s;
}

/**
 * Funcao para libertar a memoria de um snapshot individual
 *
 * * @param s -> O snapshot a apagar
 */
void libertarSnapshot(SNAPSHOT *s) {
    for (int i = 0; i < s->num_pilhas; i++) free(s->cartas[i]);
    free(s->cartas);
    free(s->tamanhos);
    free(s);
}

/**
 * Funcao para reverter o estado do jogo para o ultimo snapshot guardado
 *
 * * @param j -> Estado do jogo
 * * @return res -> 1 se reverteu, 0 se nao havia historico
 */
int restaurarSnapshot(ESTADO *j) {
    if (!j->historico) { printf("Nao ha jogadas para desfazer.\n"); return 0; }
    SNAPSHOT *s = j->historico;
    j->historico = s->prox;
    for (int i = 0; i < j->num_pilhas; i++) {
        j->pilhas[i].tamanho = s->tamanhos[i];
        memcpy(j->pilhas[i].cartas, s->cartas[i], s->tamanhos[i] * sizeof(CARTA));
    }
    libertarSnapshot(s);
    return 1;
}

/* ============================================================
 *  PROCESSAMENTO DE COMANDOS DO UTILIZADOR
 * ============================================================ */

/**
 * Funcao para imprimir no terminal a lista de comandos disponiveis
 *
 */
void tratarAjuda(void) {
    printf("\nComandos:\n");
    printf("  p <origem> <destino> [n] - Move n cartas (default 1) da pilha oirgem para a pilha destino\n");
    printf("  u             - Desfaz a ultima jogada (undo)\n");
    printf("  e             - Mostra o estado actual\n");
    printf("  s             - Grava o jogo\n");
    printf("  r             - Reinicia o jogo\n");
    printf("  h             - Mostra esta ajuda\n");
    printf("  q             - Sair do jogo\n");
}

/**
 * Funcao que processa o comando de movimento 'p', validando e guardando snapshot
 *
 * * @param buf -> String lida do input
 * * @param j -> Estado do jogo
 * * @param rma -> Regras de movimento
 * * @param rw -> Regras de vitoria
 */
void tratarMover(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasWin rw) {
    int o, d, n = 1;
    if (sscanf(buf, " p %d %d %d", &o, &d, &n) < 2) {
        printf("Uso: p <origem> <destino> [n]\n");
        return;
    }
    guardarSnapshot(j);
    if (!tentarMover(j, o - 1, d - 1, n, rma, j->rt)) {
        /* movimento inválido: descartar o snapshot que acabámos de guardar */
        SNAPSHOT *s = j->historico;
        j->historico = s->prox;
        libertarSnapshot(s);
        printf("Movimento invalido!\n");
    } else {
        printf("\n[MOV] Movimento executado: %d -> %d (%d cartas)\n", o, d, n);
        mostrarEstado(j);
        processarAuto(j, rma, j->rt);
        if (verificarVitoria(j, rw)) mostrar_mensagem(WIN);
    }
}

/* ============================================================
 *  LOOP PRINCIPAL
 * ============================================================ */

/**
 * Funcao para tratar comandos de sistema (save, load, reset)
 *
 * * @param c -> Carater do comando
 * * @param j -> Estado
 * * @param ri -> Regras Init
 * * @param n -> Nome da paciencia
 * * @return res -> 1 para continuar o programa
 */
int acaoSistema(char c, ESTADO *j, RegrasInit ri, const char *n) {
    if (c == 's') gravarJogo(j, n);
    else if (c == 'r') { aplicarInitAoEstado(j, ri, j->B); mostrarEstado(j); }
    return 1;
}

/**
 * Funcao wrapper para o comando de undo
 *
 * * @param j -> Estado do jogo
 */
void tratarUndo(ESTADO *j) {
    if (restaurarSnapshot(j)) mostrarEstado(j);
}

/**
 * Funcao que faz o parsing e encaminha os comandos do utilizador para as funcoes certas
 *
 * * @param buf -> Linha de comando
 * * @param j -> Estado
 * * @param rma -> Regras Mov
 * * @param ri -> Regras Init
 * * @param rb -> Regras Baralho
 * * @param rw -> Regras Win
 * * @param nome -> Nome paciencia
 * * @return res -> 1 se o loop deve continuar, 0 se deve sair
 */
int executarComando(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasInit ri,
                    RegrasBaralhos rb, RegrasWin rw, const char *nome) {
    char c = buf[0];
    if (c == 'q') return 0;
    if (c == 'h') tratarAjuda();
    else if (c == 'e') mostrarEstado(j);
    else if (c == 'p') tratarMover(buf, j, rma, rw);
    else if (c == 'u') tratarUndo(j);
    else if (strchr("sr", c)) acaoSistema(c, j, ri, nome);
    else printf("Comando desconhecido.\n");
    return 1;
}

/**
 * Funcao principal de ciclo de vida da interface de linha de comando
 *
 * * @param j -> Estado
 * * @param rma -> Regras Mov
 * * @param ri -> Regras Init
 * * @param rb -> Regras Baralho
 * * @param rw -> Regras Win
 * * @param nome -> Nome paciencia
 */
void loopComandos(ESTADO *j, RegrasMovAuto rma, RegrasInit ri,
                  RegrasBaralhos rb, RegrasWin rw, const char *nome) {
    char buf[256];
    int continuar = 1;
    int input_ok = 1;
    while (continuar && input_ok) {
        const char *display_nome = (nome) ? nome : "Paciencia";
        printf("%s> ", display_nome);
        if (fgets(buf, 256, stdin) == NULL) {
            input_ok = 0; // Sinaliza que não houve mais entrada
        } else {
            buf[strcspn(buf, "\n")] = '\0';
            if (buf[0] != '\0')
                continuar = executarComando(buf, j, rma, ri, rb, rw, nome);
        }
    }
}

/* ============================================================
 *  IMPRIMIR REGRAS (diagnóstico)
 * ============================================================ */

/**
 * Funcao de diagnostico para listar todas as regras de movimento carregadas
 *
 * * @param rma -> Lista de regras
 */
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

/**
 * Funcao de diagnostico para listar as condicoes de vitoria carregadas
 *
 * * @param rw -> Lista de regras de vitoria
 */
void imprimirWins(RegrasWin rw) {
    while (rw) {
        printf("   WIN %s %d\n", rw->tipoDePilha, rw->condicaoWin);
        rw = rw->prox;
    }
}

/* ============================================================
 *  LIMPAR ESTADO
 * ============================================================ */

/**
 * Funcao que liberta toda a memoria alocada para o estado do jogo e historico
 *
 * * @param j -> Estado do jogo
 */
void limparEstado(ESTADO *j) {
    for (int i = 0; i < j->num_pilhas; i++) {
        free(j->pilhas[i].cartas);
        free(j->pilhas[i].tipo);
    }
    free(j->pilhas);
    free(j->B);
    while (j->historico) {
        SNAPSHOT *s = j->historico;
        j->historico = s->prox;
        libertarSnapshot(s);
    }
}

/* ============================================================
 *  ENTRY POINT
 * ============================================================ */

/**
 * Funcao que configura os elementos iniciais do jogo antes de comecar a interacao
 *
 * * @param j -> Estado
 * * @param rb -> Regras Baralho
 * * @param rma -> Regras Mov
 * * @param rw -> Regras Win
 */
void prepararAmbiente(ESTADO *j, RegrasBaralhos rb, RegrasMovAuto rma, RegrasWin rw) {
    int n_baralhos = (rb) ? rb->numeroDeBaralhos : 1;
    j->B = criarBaralho(n_baralhos);
    j->total_cartas_baralho = 52 * n_baralhos;
    baralharBaralho(j->B, n_baralhos);
    printf("Movimentos validos:\n"); imprimirMovs(rma);
    printf("Condicoes de vitoria:\n"); imprimirWins(rw);
}


/**
 * Funcao principal que orquestra a execucao do motor de jogo
 *
 * * @param rma -> Regras Mov
 * * @param rj -> Regras Jogo
 * * @param rb -> Regras Baralho
 * * @param rt -> Regras Tipo
 * * @param ri -> Regras Init
 * * @param rw -> Regras Win
 * * @param carregar_save -> Flag para saber se deve carregar de ficheiro
 */
void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb,
             RegrasTipo rt, RegrasInit ri, RegrasWin rw, int carregar_save) {
    ESTADO jogo;
    memset(&jogo, 0, sizeof(ESTADO));
    jogo.rt = rt;
    jogo.historico = NULL;

    const char *nome = (rj && rj->jogoNome) ? rj->jogoNome : "Jogo Desconhecido";
    printf("\n=== %s ===\n", nome);
    prepararAmbiente(&jogo, rb, rma, rw);
    aplicarInitAoEstado(&jogo, ri, jogo.B);
    if (carregar_save) carregarJogo(&jogo);
    mostrarEstado(&jogo);
    loopComandos(&jogo, rma, ri, rb, rw, nome);
    limparEstado(&jogo);
}