
#include "readFile.h"

/* ========== PROTÓTIPOS executeFile ========== */

/* Conversão carta <-> string */
const char *card2str(CARTA card);
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
int validarSequencia(PILHA *ori, int n, RegrasMovAuto regra);
int validarDestino(PILHA *ori, PILHA *des, int n, RegrasMovAuto regra);
int validarMovimento(ESTADO *j, int io, int id, int n, RegrasMovAuto rma);

/* Movimentos */
void executarMov(ESTADO *j, int io, int id, int n);
int  tentarMover(ESTADO *j, int io, int id, int n, RegrasMovAuto rma);

/* AUTO */
int  tentarAutoUmaVez(ESTADO *j, RegrasMovAuto rma);
void processarAuto(ESTADO *j, RegrasMovAuto rma);

/* Vitória */
int contarCartasTipo(ESTADO *j, const char *tipo);
int condicaoWinSatisfeita(ESTADO *j, RegrasWin rw);
int verificarVitoria(ESTADO *j, RegrasWin rw);

/* Save / Load */
void gravarJogo(ESTADO *j, const char *nome_paciencia);
int  carregarJogo(ESTADO *j, RegrasInit ri, RegrasBaralhos rb);

/* Display */
void imprimirTabuleiro(ESTADO *j);
void mostrarEstado(ESTADO *j);

/* Loop principal */
void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb,
             RegrasTipo rt, RegrasInit ri, RegrasWin rw);

