#ifndef EXECUTEFILE_H
#define EXECUTEFILE_H

#include "readFile.h"

void tratarAjuda(void);
void tratarMover(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasWin rw);
int acaoSistema(char c, ESTADO *j, RegrasInit ri, const char *n);
void tratarUndo(ESTADO *j);
int executarComando(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasInit ri, RegrasBaralhos rb, RegrasWin rw, const char *nome);
void loopComandos(ESTADO *j, RegrasMovAuto rma, RegrasInit ri, RegrasBaralhos rb, RegrasWin rw, const char *nome);
void prepararAmbiente(ESTADO *j, RegrasBaralhos rb, RegrasMovAuto rma, RegrasWin rw);
void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb, RegrasTipo rt, RegrasInit ri, RegrasWin rw, int carregar_save);

#endif
