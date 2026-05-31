#include "types.h"

void      mostrar_mensagem(MENSAGENS cod);
CONTEXTO  inicializarContexto(void);
void      libertarContexto(CONTEXTO *c);
MENSAGENS abreFicheiro(char *nome, RegrasMovAuto *listaMA, RegrasJogo *listaJ,
                        RegrasBaralhos *listaB, RegrasTipo *listaT,
                        RegrasInit *listaI, RegrasWin *listaW);
int       abrirPastaImprime(char *nome, int *carregar_save);
int       lerNomePacienciaDoSave(char *nome_paciencia);
int       carregarNome(char *nome, int *carregar_save);
