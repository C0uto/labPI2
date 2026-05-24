#ifndef CARTA_H
#define CARTA_H

#include "readFile.h"

void card2str(CARTA card, char *buf);
CARTA str2card(const char *s);
int getValor(CARTA c);
int getNaipe(CARTA c);
int getCor(CARTA c);
int getPilhaTamanho(PILHA *p);
void pushCarta(PILHA *p, CARTA c);
CARTA popCarta(PILHA *p);
CARTA topoCarta(PILHA *p);

#endif
