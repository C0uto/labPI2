#include "readFile.h"

/* ========== PROTÓTIPOS executeFile ========== */

/* Conversão carta <-> string */
void        card2str(CARTA card, char *buf);
CARTA       str2card(const char *s);

/* Acesso a propriedades de cartas */
int getValor(CARTA c);
int getNaipe(CARTA c);
int getCor(CARTA c);

/* Operações sobre pilhas */
int   getPilhaTamanho(PILHA *p);
void  pushCarta(PILHA *p, CARTA c);
CARTA popCarta(PILHA *p);
CARTA topoCarta(PILHA *p);

/* Validação de movimentos */
int temFlag(RegrasMovAuto r, char f);
RegrasMovAuto encontrarRegra(RegrasMovAuto rma, const char *ori, const char *des);
int tipoTemFlag1(RegrasTipo rt, const char *tipo);
int validarSequencia(PILHA *ori, int n, RegrasMovAuto regra);
int validarDestino(PILHA *ori, PILHA *des, int n, RegrasMovAuto regra);
int validarMovimento(ESTADO *j, int io, int id, int n, RegrasMovAuto rma, RegrasTipo rt);

/* Movimentos */
void executarMov(ESTADO *j, int io, int id, int n);
int  tentarMover(ESTADO *j, int io, int id, int n, RegrasMovAuto rma, RegrasTipo rt);

/* UNDO */
void guardarSnapshot(ESTADO *j);
void libertarSnapshot(SNAPSHOT *s);
int  restaurarSnapshot(ESTADO *j);

/* AUTO */
int  tentarAutoUmaVez(ESTADO *j, RegrasMovAuto rma, RegrasTipo rt);
void processarAuto(ESTADO *j, RegrasMovAuto rma, RegrasTipo rt);

/* Vitória */
int contarCartasTipo(ESTADO *j, const char *tipo);
int condicaoWinSatisfeita(ESTADO *j, RegrasWin rw);
int verificarVitoria(ESTADO *j, RegrasWin rw);

/* Save / Load */
void gravarPilha(FILE *f, PILHA *p);
void gravarJogo(ESTADO *j, const char *nome_paciencia);
int  carregarJogo(ESTADO *j);

/* Display */
char visibilidadeTipo(RegrasTipo rt, const char *tipo);
void printCelula(ESTADO *j, int i, int lin);
void imprimirTabuleiro(ESTADO *j);
void mostrarEstado(ESTADO *j);

/* Loop principal */
void tratarUndo(ESTADO *j);
void prepararAmbiente(ESTADO *j, RegrasBaralhos rb, RegrasMovAuto rma, RegrasWin rw);

/* Loop principal */
void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb,
             RegrasTipo rt, RegrasInit ri, RegrasWin rw, int carregar_save);