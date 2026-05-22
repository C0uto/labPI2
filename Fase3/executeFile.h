#include "readFile.h"

typedef char CARTA;
typedef CARTA *BARALHO;
typedef struct { 
    CARTA **paciencia;           // Matriz dinâmica para o tabuleiro 
    int num_pilhas;              // Quantidade de pilhas
    int max_cartas_por_pilha;    // Capacidade máxima de cada pilha
    CARTA fundacao; 
    int cartas_no_baralho; 
    int posicao_topo_do_baralho; 
    BARALHO B; 
} ESTADO;

// Protótipos das funções para evitar avisos de declaração implícita
const char *card2str(CARTA card);
void mostrarEstado(ESTADO *jogo);
void imprimirTabuleiro(ESTADO *j);
void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb, 
             RegrasTipo rt, RegrasInit ri, RegrasWin rw);
:
