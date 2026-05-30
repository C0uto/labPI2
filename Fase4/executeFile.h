#ifndef EXECUTEFILE_H
#define EXECUTEFILE_H

#include "readFile.h"

void tratarAjuda(void);
void tratarMover(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasWin rw);
int comandoSR(char *buf, ESTADO *j, RegrasInit ri, const char *n, const char *save_name);
void tratarUndo(ESTADO *j);
int executarComando(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasInit ri, RegrasBaralhos rb, RegrasWin rw, const char *nome, const char *save_name);
void loopComandos(ESTADO *j, RegrasMovAuto rma, RegrasInit ri, RegrasBaralhos rb, RegrasWin rw, const char *nome, const char *save_name);
void prepararAmbiente(ESTADO *j, RegrasBaralhos rb, RegrasMovAuto rma, RegrasWin rw);
void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb, RegrasTipo rt, RegrasInit ri, RegrasWin rw, int carregar_save, const char *save_name);

#endif
