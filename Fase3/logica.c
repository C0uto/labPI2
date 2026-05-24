#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "carta.h"
#include "logica.h"
#include "display.h"


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
int verificarWin(ESTADO *j, RegrasWin rw) {
    RegrasWin aux = rw;
    while (aux != NULL) {
        if (!condicaoWinSatisfeita(j, aux)) return 0;
        aux = aux->prox;
    }
    return 1;
}

