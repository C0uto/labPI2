void tratarAjuda(void); // Necessita de ESTADO, RegrasMovAuto, RegrasWin, RegrasInit, RegrasBaralhos, RegrasTipo
void tratarMover(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasWin rw); // Necessita de ESTADO, RegrasMovAuto, RegrasWin, RegrasTipo
void tratarUndo(ESTADO *j); // Necessita de ESTADO
int acaoSistema(char c, ESTADO *j, RegrasInit ri, const char *n); // Necessita de ESTADO, RegrasInit
int executarComando(char *buf, ESTADO *j, RegrasMovAuto rma, RegrasInit ri, RegrasBaralhos rb, RegrasWin rw, const char *nome); // Necessita de ESTADO, RegrasMovAuto, RegrasInit, RegrasBaralhos, RegrasWin
void loopComandos(ESTADO *j, RegrasMovAuto rma, RegrasInit ri, RegrasBaralhos rb, RegrasWin rw, const char *nome); // Necessita de ESTADO, RegrasMovAuto, RegrasInit, RegrasBaralhos, RegrasWin
void prepararAmbiente(ESTADO *j, RegrasBaralhos rb, RegrasMovAuto rma, RegrasWin rw); // Necessita de ESTADO, RegrasBaralhos, RegrasMovAuto, RegrasWin
void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb, RegrasTipo rt, RegrasInit ri, RegrasWin rw, int carregar_save); // Necessita de todos os tipos de regras e ESTADO
