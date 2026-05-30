#ifndef ESTADO_H
#define ESTADO_H

#include "readFile.h"

void gravarPilha(FILE *f, PILHA *p);
void gravarJogo(ESTADO *j, const char *nome_paciencia);
void carregarLinhaPilha(PILHA *p, char *linha);
void processarLinhaSave(ESTADO *j, int i, FILE *f);
int carregarJogo(ESTADO *j);
BARALHO criarBaralho(int n_baralhos);
void baralharBaralho(BARALHO b, int n_baralhos);
int contarInits(RegrasInit ri);
RegrasInit ultimoInit(RegrasInit ri);
void inicializarPilhas(ESTADO *j, int n);
void preencherPilha(PILHA *p, const char *tipo, BARALHO deck, int *idx, int n_cartas);
void aplicarInitAoEstado(ESTADO *j, RegrasInit ri, BARALHO deck);
void guardarSnapshot(ESTADO *j);
void libertarSnapshot(SNAPSHOT *s);
int restaurarSnapshot(ESTADO *j);
void limparEstado(ESTADO *j);

#endif
