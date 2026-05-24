/**
 * @file logica.c
 * @brief Implementação da validação e execução de movimentos, AUTO e vitória.
 */

#include <stdio.h>
#include <string.h>
#include "logica.h"
#include "display.h"

/* ============================================================
 *  FLAGS
 * ============================================================ */

/**
 * @brief Verifica se a flag @p f está activa na regra @p r.
 * @param r  Regra MOV/AUTO (pode ser NULL).
 * @param f  Caractere da flag.
 * @return   1 se activa, 0 caso contrário.
 */
int temFlag(RegrasMovAuto r, char f) {
    if (!r) return 0;
    for (int i = 0; r->flags[0][i] != '\0'; i++)
        if (r->flags[0][i] == f && r->flags[1][i] == '1') return 1;
    return 0;
}

/**
 * @brief Verifica se o tipo de pilha @p tipo tem a flag '1' (máx. 1 carta) activa.
 * @param rt    Lista de regras TIPO.
 * @param tipo  Nome do tipo a verificar.
 * @return      1 se activa, 0 caso contrário.
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

/* ============================================================
 *  PESQUISA DE REGRAS
 * ============================================================ */

/**
 * @brief Procura a primeira regra MOV com origem e destino coincidentes.
 * @param rma  Lista de regras MOV/AUTO.
 * @param ori  Tipo da pilha de origem.
 * @param des  Tipo da pilha de destino.
 * @return     Ponteiro para a regra, ou NULL se não existir.
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
 *  AUXILIARES LOCAIS — TOPO E FUNDO DA SEQUÊNCIA
 * ============================================================ */

/**
 * @brief Devolve a carta do topo da sequência a mover (contacta o destino).
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @return     Carta na posição tamanho-n.
 */
static CARTA topoSequencia(PILHA *ori, int n) {
    return ori->cartas[ori->tamanho - n];
}

/**
 * @brief Devolve a carta do fundo da sequência (a que estava mais fundo na origem).
 * @param ori  Pilha de origem.
 * @return     Carta na posição tamanho-1.
 */
static CARTA fundoSequencia(PILHA *ori) {
    return ori->cartas[ori->tamanho - 1];
}

/* ============================================================
 *  VALIDAÇÃO DA SEQUÊNCIA
 * ============================================================ */

/**
 * @brief Valida flag '+': sem ela apenas se pode mover 1 carta.
 * @param n  Número de cartas a mover.
 * @param r  Regra aplicável.
 * @return   1 se válido, 0 se n > 1 e '+' não estiver activo.
 */
int validarFlagMais(int n, RegrasMovAuto r) {
    if (n > 1 && !temFlag(r, '+')) return 0;
    return 1;
}

/**
 * @brief Valida flag '[': sequência decrescente por valores consecutivos.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se a ordenação falhar.
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
 * @brief Valida flag ']': sequência crescente por valores consecutivos.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se a ordenação falhar.
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
 * @brief Valida flag 'm': todas as cartas a mover têm o mesmo naipe.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se os naipes diferirem.
 */
int validarFlagMesmoNaipe(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'm')) return 1;
    int base  = ori->tamanho - n;
    int naipe = getNaipe(ori->cartas[base]);
    for (int i = 1; i < n; i++)
        if (getNaipe(ori->cartas[base + i]) != naipe) return 0;
    return 1;
}

/**
 * @brief Valida flag 'x': naipes alternados em pares consecutivos da sequência.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se dois consecutivos tiverem o mesmo naipe.
 */
int validarFlagNaipesAlternados(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'x')) return 1;
    int base = ori->tamanho - n;
    for (int i = 0; i < n - 1; i++)
        if (getNaipe(ori->cartas[base + i]) == getNaipe(ori->cartas[base + i + 1])) return 0;
    return 1;
}

/**
 * @brief Valida flag 'c': todas as cartas a mover têm a mesma cor.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se as cores diferirem.
 */
int validarFlagMesmaCor(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'c')) return 1;
    int base = ori->tamanho - n;
    int cor  = getCor(ori->cartas[base]);
    for (int i = 1; i < n; i++)
        if (getCor(ori->cartas[base + i]) != cor) return 0;
    return 1;
}

/**
 * @brief Valida flag 'd': cores alternadas em pares consecutivos da sequência.
 * @param ori  Pilha de origem.
 * @param n    Número de cartas a mover.
 * @param r    Regra aplicável.
 * @return     1 se válido ou flag inactiva, 0 se dois consecutivos tiverem a mesma cor.
 */
int validarFlagCoresAlternadas(PILHA *ori, int n, RegrasMovAuto r) {
    if (!temFlag(r, 'd')) return 1;
    int base = ori->tamanho - n;
    for (int i = 0; i < n - 1; i++)
        if (getCor(ori->cartas[base + i]) == getCor(ori->cartas[base + i + 1])) return 0;
    return 1;
}

/**
 * @brief Agrega todas as validações internas à sequência.
 * @param ori    Pilha de origem.
 * @param n      Número de cartas a mover.
 * @param regra  Regra MOV aplicável.
 * @return       1 se válida, 0 caso contrário.
 */
int validarSequencia(PILHA *ori, int n, RegrasMovAuto regra) {
    if (!validarFlagMais(n, regra))                  return 0;
    if (!validarFlagDescrescente(ori, n, regra))     return 0;
    if (!validarFlagCrescente(ori, n, regra))        return 0;
    if (!validarFlagMesmoNaipe(ori, n, regra))       return 0;
    if (!validarFlagNaipesAlternados(ori, n, regra)) return 0;
    if (!validarFlagMesmaCor(ori, n, regra))         return 0;
    if (!validarFlagCoresAlternadas(ori, n, regra))  return 0;
    return 1;
}

/* ============================================================
 *  VALIDAÇÃO RELATIVA AO DESTINO — helpers internos
 * ============================================================ */

/** @brief flags '<', '>', '~', 'V' */
static int validarFlagsBasicas(PILHA *o, PILHA *d, int n, RegrasMovAuto r) {
    if (temFlag(r, '<') && (d->tamanho == 0 ||
        getValor(topoSequencia(o, n)) != getValor(topoCarta(d)) - 1)) return 0;
    if (temFlag(r, '>') && (d->tamanho == 0 ||
        getValor(topoSequencia(o, n)) != getValor(topoCarta(d)) + 1)) return 0;
    if (temFlag(r, '~') && (d->tamanho == 0 || (
        abs(getValor(topoSequencia(o, n)) - getValor(topoCarta(d))) != 1))) return 0;
    if (temFlag(r, 'V') && d->tamanho != 0) return 0;
    return 1;
}

/** @brief flags 'M', 'X', 'C', 'D' */
static int validarFlagsAtributos(PILHA *o, PILHA *d, int n, RegrasMovAuto r) {
    if (temFlag(r, 'M') && (d->tamanho == 0 ||
        getNaipe(topoSequencia(o, n)) != getNaipe(topoCarta(d)))) return 0;
    if (temFlag(r, 'X') && d->tamanho > 0 &&
        getNaipe(topoSequencia(o, n)) == getNaipe(topoCarta(d))) return 0;
    if (temFlag(r, 'C') && (d->tamanho == 0 ||
        getCor(topoSequencia(o, n)) != getCor(topoCarta(d)))) return 0;
    if (temFlag(r, 'D') && d->tamanho > 0 &&
        getCor(topoSequencia(o, n)) == getCor(topoCarta(d))) return 0;
    return 1;
}

/** @brief flags 'a', 'A', 'k', 'K' */
static int validarFlagsRanks(PILHA *o, int n, RegrasMovAuto r) {
    if (temFlag(r, 'a') && getValor(fundoSequencia(o)) != 1)  return 0;
    if (temFlag(r, 'A') && getValor(topoSequencia(o, n)) != 1) return 0;
    if (temFlag(r, 'k') && getValor(fundoSequencia(o)) != 13) return 0;
    if (temFlag(r, 'K') && getValor(topoSequencia(o, n)) != 13) return 0;
    return 1;
}

/**
 * @brief Agrega todas as validações relativas ao destino.
 *        Flag '*' aceita incondicionalmente.
 * @param ori    Pilha de origem.
 * @param des    Pilha de destino.
 * @param n      Número de cartas a mover.
 * @param regra  Regra MOV aplicável.
 * @return       1 se válido, 0 caso contrário.
 */
int validarDestino(PILHA *ori, PILHA *des, int n, RegrasMovAuto regra) {
    if (temFlag(regra, '*')) return 1;
    return validarFlagsBasicas(ori, des, n, regra) &&
           validarFlagsAtributos(ori, des, n, regra) &&
           validarFlagsRanks(ori, n, regra);
}

/* ============================================================
 *  VALIDAÇÃO COMPLETA E EXECUÇÃO
 * ============================================================ */

/** @brief Verifica se os índices io, id e n são admissíveis. */
static int indicesValidos(ESTADO *j, int io, int id, int n) {
    if (io < 0 || io >= j->num_pilhas || id < 0 || id >= j->num_pilhas) return 0;
    return (io != id && n > 0);
}

/** @brief Verifica se a regra @p r é MOV e se a sequência e destino são válidos. */
static int regraValida(PILHA *o, PILHA *d, int n, RegrasMovAuto r) {
    if (strcmp(r->comando, "MOV") || strcmp(r->origem, o->tipo) ||
        strcmp(r->destino, d->tipo)) return 0;
    return (o->tamanho >= n && validarSequencia(o, n, r) && validarDestino(o, d, n, r));
}

/**
 * @brief Valida completamente um movimento.
 *        Itera por todas as regras MOV — basta uma ser satisfeita.
 * @param j    Estado do jogo.
 * @param io   Índice da pilha de origem (base 0).
 * @param id   Índice da pilha de destino (base 0).
 * @param n    Número de cartas a mover.
 * @param rma  Lista de regras MOV/AUTO.
 * @param rt   Lista de regras TIPO.
 * @return     1 se o movimento é permitido, 0 caso contrário.
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
 * @brief Move n cartas do topo da origem para o topo do destino sem validação.
 * @param j   Estado do jogo.
 * @param io  Índice da pilha de origem (base 0).
 * @param id  Índice da pilha de destino (base 0).
 * @param n   Número de cartas a mover.
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
 * @brief Valida e, se permitido, executa o movimento de n cartas.
 * @param j    Estado do jogo.
 * @param io   Índice da pilha de origem (base 0).
 * @param id   Índice da pilha de destino (base 0).
 * @param n    Número de cartas a mover.
 * @param rma  Lista de regras MOV/AUTO.
 * @param rt   Lista de regras TIPO.
 * @return     1 se executado, 0 se inválido.
 */
int tentarMover(ESTADO *j, int io, int id, int n, RegrasMovAuto rma, RegrasTipo rt) {
    if (!validarMovimento(j, io, id, n, rma, rt)) return 0;
    executarMov(j, io, id, n);
    return 1;
}

/* ============================================================
 *  AUTO
 * ============================================================ */

/** @brief Valida e executa AUTO para n cartas entre io e id, respeitando flag '1'. */
static int executarAutoSeValido(ESTADO *j, RegrasMovAuto r, int io, int id,
                                 int n, RegrasTipo rt) {
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
 * @brief Tenta aplicar a regra AUTO @p r entre as pilhas io e id.
 *        Se '+' activo, experimenta do maior n até 1.
 * @param j   Estado do jogo.
 * @param r   Regra AUTO a testar.
 * @param io  Índice da pilha de origem (base 0).
 * @param id  Índice da pilha de destino (base 0).
 * @param rt  Lista de regras TIPO.
 * @return    1 se um movimento foi executado, 0 caso contrário.
 */
int tentarAutoEntrePilhas(ESTADO *j, RegrasMovAuto r, int io, int id, RegrasTipo rt) {
    if (strcmp(j->pilhas[io].tipo, r->origem)  != 0 ||
        strcmp(j->pilhas[id].tipo, r->destino) != 0) return 0;
    int n = temFlag(r, '+') ? j->pilhas[io].tamanho : 1;
    while (n >= 1) {
        if (executarAutoSeValido(j, r, io, id, n, rt)) return 1;
        n--;
    }
    return 0;
}

/**
 * @brief Percorre todas as regras AUTO e tenta uma única aplicação.
 * @param j    Estado do jogo.
 * @param rma  Lista de regras MOV/AUTO.
 * @param rt   Lista de regras TIPO.
 * @return     1 se algum movimento foi executado, 0 caso contrário.
 */
int tentarAutoUmaVez(ESTADO *j, RegrasMovAuto rma, RegrasTipo rt) {
    RegrasMovAuto aux = rma;
    while (aux != NULL) {
        if (strcmp(aux->comando, "AUTO") == 0)
            for (int io = 0; io < j->num_pilhas; io++)
                for (int id = 0; id < j->num_pilhas; id++)
                    if (tentarAutoEntrePilhas(j, aux, io, id, rt)) return 1;
        aux = aux->prox;
    }
    return 0;
}

/**
 * @brief Executa movimentos automáticos em cadeia até não existirem mais.
 *        Mostra o estado após cada movimento.
 * @param j    Estado do jogo.
 * @param rma  Lista de regras MOV/AUTO.
 * @param rt   Lista de regras TIPO.
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
 * @brief Conta o total de cartas em todas as pilhas do tipo @p tipo.
 * @param j     Estado do jogo.
 * @param tipo  Nome do tipo de pilha.
 * @return      Total de cartas.
 */
int contarCartasTipo(ESTADO *j, const char *tipo) {
    int total = 0;
    for (int i = 0; i < j->num_pilhas; i++)
        if (strcmp(j->pilhas[i].tipo, tipo) == 0)
            total += j->pilhas[i].tamanho;
    return total;
}

/**
 * @brief Verifica se a condição WIN @p rw está satisfeita.
 * @param j   Estado do jogo.
 * @param rw  Regra WIN a verificar.
 * @return    1 se satisfeita, 0 caso contrário.
 */
int condicaoWinSatisfeita(ESTADO *j, RegrasWin rw) {
    int total   = contarCartasTipo(j, rw->tipoDePilha);
    int npilhas = 0;
    for (int i = 0; i < j->num_pilhas; i++)
        if (strcmp(j->pilhas[i].tipo, rw->tipoDePilha) == 0) npilhas++;
    if (npilhas == 0) return 1;
    return total == rw->condicaoWin * npilhas;
}

/**
 * @brief Verifica se TODAS as condições WIN estão satisfeitas simultaneamente.
 * @param j   Estado do jogo.
 * @param rw  Lista de regras WIN.
 * @return    1 se o jogo está ganho, 0 caso contrário.
 */
int verificarVitoria(ESTADO *j, RegrasWin rw) {
    RegrasWin aux = rw;
    while (aux != NULL) {
        if (!condicaoWinSatisfeita(j, aux)) return 0;
        aux = aux->prox;
    }
    return 1;
}
