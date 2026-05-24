#include "carta.h"

int temFlag(RegrasMovAuto r, char f);
int tipoTemFlag1(RegrasTipo rt, const char *tipo);
RegrasMovAuto encontrarRegra(RegrasMovAuto rma, const char *ori, const char *des);

int validarFlagMais(int n, RegrasMovAuto r);
int validarFlagDescrescente(PILHA *ori, int n, RegrasMovAuto r);
int validarFlagCrescente(PILHA *ori, int n, RegrasMovAuto r);
int validarFlagMesmoNaipe(PILHA *ori, int n, RegrasMovAuto r);
int validarFlagNaipesAlternados(PILHA *ori, int n, RegrasMovAuto r);
int validarFlagMesmaCor(PILHA *ori, int n, RegrasMovAuto r);
int validarFlagCoresAlternadas(PILHA *ori, int n, RegrasMovAuto r);
int validarFlagMenor(PILHA *ori, PILHA *des, int n, RegrasMovAuto r);
int validarFlagMaior(PILHA *ori, PILHA *des, int n, RegrasMovAuto r);
int validarFlagTil(PILHA *ori, PILHA *des, int n, RegrasMovAuto r);
int validarFlagM(PILHA *ori, PILHA *des, int n, RegrasMovAuto r);
int validarFlagX(PILHA *ori, PILHA *des, int n, RegrasMovAuto r);
int validarFlagC(PILHA *ori, PILHA *des, int n, RegrasMovAuto r);
int validarFlagD(PILHA *ori, PILHA *des, int n, RegrasMovAuto r);
int validarFlagV(PILHA *des, RegrasMovAuto r);

int validarSequencia(PILHA *ori, int n, RegrasMovAuto regra);
int validarDestino(PILHA *o, PILHA *d, int n, RegrasMovAuto regra);
int validarMovimento(ESTADO *j, int io, int id, int n, RegrasMovAuto rma, RegrasTipo rt);

void executarMov(ESTADO *j, int io, int id, int n);
int tentarMover(ESTADO *j, int io, int id, int n, RegrasMovAuto rma, RegrasTipo rt);

int tentarAutoEntrePilhas(ESTADO *j, RegrasMovAuto r, int io, int id, RegrasTipo rt);
int tentarAutoUmaVez(ESTADO *j, RegrasMovAuto rma, RegrasTipo rt);
void processarAuto(ESTADO *j, RegrasMovAuto rma, RegrasTipo rt);

int contarCartasTipo(ESTADO *j, const char *tipo);
int condicaoWinSatisfeita(ESTADO *j, RegrasWin rw);
int verificarVitoria(ESTADO *j, RegrasWin rw);
