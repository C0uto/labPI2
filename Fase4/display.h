#ifndef DISPLAY_H
#define DISPLAY_H

#include "readFile.h"

int ehGrupoSuperior(const char *t);
int maxAltura(ESTADO *j, int sup);
int printCabecalho(ESTADO *j, int sup);
char visibilidadeTipo(RegrasTipo rt, const char *tipo);
void printCelula(ESTADO *j, int i, int lin);
void printLinha(ESTADO *j, int lin, int sup);
void printGrupo(ESTADO *j, int sup);
void imprimirTabuleiro(ESTADO *j);
void mostrarEstado(ESTADO *j);
void imprimirMovs(RegrasMovAuto rma);
void imprimirWins(RegrasWin rw);

#endif
