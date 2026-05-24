#include "logica.h"
#include "estado.h"

char visibilidadeTipo(RegrasTipo rt, const char *tipo);
void printCelula(ESTADO *j, int i, int lin);
void imprimirTabuleiro(ESTADO *j);
void mostrarEstado(ESTADO *j);
void imprimirMovs(RegrasMovAuto rma);
void imprimirWins(RegrasWin rw);