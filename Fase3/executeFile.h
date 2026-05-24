#include "interface.h"
// executeFile.h não precisa incluir types.h diretamente, pois interface.h já o faz.
void tratarAjuda(void);
void tratarMover(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasWin rw);
void tratarUndo(ESTADO *j);
int acaoSistema(char c, ESTADO *j, RegrasInit ri, const char *n);
int executarComando(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasInit ri, RegrasBaralhos rb, RegrasWin rw, const char *nome);
void loopComandos(ESTADO *j, RegrasMovAuto rma, RegrasInit ri, RegrasBaralhos rb, RegrasWin rw, const char *nome);
void prepararAmbiente(ESTADO *j, RegrasBaralhos rb, RegrasMovAuto rma, RegrasWin rw);
void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb,
             RegrasTipo rt, RegrasInit ri, RegrasWin rw, int carregar_save);
