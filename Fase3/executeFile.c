#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readFile.h>
#include <time.h>

// Simulação da inclusão das definições da Fase 1/2
// #include <card.h> 

// Caso o header não esteja disponível no path, definimos o necessário para compilação:
// A estrutura ESTADO real deve ser definida em card.h e incluída.

typedef char CARTA;
typedef CARTA *BARALHO;
typedef struct { 
    CARTA **paciencia;           // Matriz dinâmica para o tabuleiro (tableau)
    int num_pilhas;              // Quantidade de colunas/pilhas
    int max_cartas_por_pilha;    // Capacidade máxima de cada pilha
    CARTA fundacao; 
    int cartas_no_baralho; 
    int posicao_topo_do_baralho; 
    BARALHO B; 
} ESTADO;

// ========== FUNÇÕES AUXILIARES ==========

void imprimirTipos(RegrasTipo rt) {
    while (rt != NULL) {
        printf("   - TIPO %s %s\n", rt->tipoDePilha, rt->flags[0]);
        rt = rt->prox;
    }
}

void imprimirInits(RegrasInit ri) {
    while (ri != NULL) {
        printf("   - INIT %s %d\n", ri->tipoDePilha, ri->numeroDeCartas);
        ri = ri->prox;
    }
}

void imprimirMovs(RegrasMovAuto rma) {
    while (rma != NULL) {
        printf("   - %s %s %s %s\n", rma->comando, rma->origem, rma->destino, rma->flags[0]);
        rma = rma->prox;
    }
}

void imprimirWins(RegrasWin rw) {
    while (rw != NULL) {
        printf("   - WIN %s %d\n", rw->tipoDePilha, rw->condicaoWin);
        rw = rw->prox;
    }
}

// ========== FUNÇÕES DE EXECUÇÃO DAS REGRAS ==========

BARALHO criar_baralhos(RegrasBaralhos rb) {
    int n = (rb != NULL) ? rb->numeroDeBaralhos : 1;
    int total_cartas = 52 * n;
    CARTA *b = malloc(sizeof(CARTA) * total_cartas);
    
    for (int i = 0; i < total_cartas; i++) {
        b[i] = (i % 52) + 1;
    }
    return b;
}

/**
 * Replicado de Fase1/2_Lógica.c (baralhar)
 */
void baralhar_generico(BARALHO b, int n_baralhos) {
    int total = 52 * n_baralhos;
    srand(time(NULL));
    for (int i = 0; i < total * 2; i++) {
        int idx1 = rand() % total;
        int idx2 = rand() % total;
        CARTA temp = b[idx1];
        b[idx1] = b[idx2];
        b[idx2] = temp;
    }
}

BARALHO aplicarBaralhos(RegrasBaralhos rb) {
    if (rb == NULL || rb->comando == NULL) {
        printf("ERRO: Regra BARALHOS não definida!\n");
        return NULL;
    }
    printf("2. [BARALHOS] Criando %d baralho(s)\n", rb->numeroDeBaralhos);
    BARALHO b = criar_baralhos(rb);
    baralhar_generico(b, rb->numeroDeBaralhos);
    return b;
}

void aplicarJogo(RegrasJogo rj) {
    if (rj == NULL || rj->comando == NULL) {
        printf("ERRO: Regra JOGO não definida!\n");
        return;
    }
    printf("1. [JOGO] %s\n", rj->jogoNome);
}

void aplicarTipo(RegrasTipo rt) {
    if (rt == NULL) {
        printf("   [TIPO] Nenhum tipo definido\n");
        return;
    }
    printf("3. [TIPO] Tipos de pilhas:\n");
    imprimirTipos(rt);
}

/**
 * Versão genérica da golf_init que recebe as regras lidas
 */
void aplicarInit(RegrasInit ri, ESTADO *jogo, BARALHO deck) {
    if (ri == NULL) {
        printf("   [INIT] Nenhuma inicialização definida\n");
        return;
    }
    printf("4. [INIT] Inicializando pilhas:\n");

    int total_cartas_tab = 0;
    int max_cartas_inferred = 5; // Valor base para jogos tipo Golf/Simon (pode ser ajustado)
    int num_pilhas_inferred = 0;

    // 1ª Passagem: Determinar dimensões necessárias para o tabuleiro (TAB)
    RegrasInit aux_dim = ri;
    while (aux_dim != NULL) {
        if (strcmp(aux_dim->tipoDePilha, "TAB") == 0) {
            total_cartas_tab = aux_dim->numeroDeCartas;
            num_pilhas_inferred = (total_cartas_tab + max_cartas_inferred - 1) / max_cartas_inferred;
            break; 
        }
        aux_dim = aux_dim->prox;
    }

    // Alocação dinâmica do tabuleiro (paciencia)
    if (total_cartas_tab > 0 && num_pilhas_inferred > 0) {
        jogo->num_pilhas = num_pilhas_inferred;
        jogo->max_cartas_por_pilha = max_cartas_inferred + 10; // Margem para movimentos

        jogo->paciencia = (CARTA **)malloc(jogo->num_pilhas * sizeof(CARTA *));
        if (jogo->paciencia == NULL) {
            perror("Erro ao alocar memória para as pilhas do jogo");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < jogo->num_pilhas; i++) {
            jogo->paciencia[i] = (CARTA *)calloc(jogo->max_cartas_por_pilha, sizeof(CARTA));
            if (jogo->paciencia[i] == NULL) {
                perror("Erro ao alocar memória para cartas da pilha");
                for (int j = 0; j < i; j++) {
                    free(jogo->paciencia[j]);
                }
                free(jogo->paciencia);
                exit(EXIT_FAILURE);
            }
        }
    } else {
        jogo->num_pilhas = 0;
        jogo->max_cartas_por_pilha = 0;
        jogo->paciencia = NULL;
    }

    // 2ª Passagem: Distribuir cartas de acordo com as regras lidas
    int carta_atual = 0;
    RegrasInit aux = ri;
    while (aux != NULL) {
        if (strcmp(aux->tipoDePilha, "TAB") == 0) {
            for (int i = 0; i < aux->numeroDeCartas; i++) {
                int col = i / max_cartas_inferred;
                int lin = i % max_cartas_inferred;
                
                if (col < jogo->num_pilhas && lin < jogo->max_cartas_por_pilha) {
                    jogo->paciencia[col][lin] = deck[carta_atual++];
                } else {
                    carta_atual++; 
                }
            }
        } 
        // Distribuição para a Fundação/Descarte (DESCARTE)
        else if (strcmp(aux->tipoDePilha, "DESCARTE") == 0) {
            if (aux->numeroDeCartas > 0) {
                jogo->fundacao = deck[carta_atual++];
            }
        }
        // O resto das cartas fica no Stock
        else if (strcmp(aux->tipoDePilha, "STOCK") == 0) {
            jogo->posicao_topo_do_baralho = carta_atual;
            jogo->cartas_no_baralho = aux->numeroDeCartas;
        }
        aux = aux->prox;
    }
    imprimirInits(ri);
}

void aplicarMovAuto(RegrasMovAuto rma) {
    if (rma == NULL) {
        printf("   [MOV/AUTO] Nenhum movimento definido\n");
        return;
    }
    printf("5. [MOV/AUTO] Movimentos automáticos:\n");
    imprimirMovs(rma);
}

void aplicarWin(RegrasWin rw) {
    if (rw == NULL) {
        printf("   [WIN] Nenhuma condição de vitória definida\n");
        return;
    }
    printf("6. [WIN] Condições de vitória:\n");
    imprimirWins(rw);
}

// ========== FUNÇÃO PRINCIPAL DE EXECUÇÃO ==========

void execute(RegrasMovAuto rma, RegrasJogo rj, RegrasBaralhos rb, 
             RegrasTipo rt, RegrasInit ri, RegrasWin rw) {

    ESTADO jogo;
    memset(&jogo, 0, sizeof(ESTADO)); // Limpa o estado

    printf("\n========== APLICANDO REGRAS DO JOGO ==========\n\n");
    aplicarJogo(rj);
    printf("\n");
    
    BARALHO b = aplicarBaralhos(rb);
    jogo.B = b;

    printf("\n");
    aplicarTipo(rt);
    printf("\n");
    aplicarInit(ri, &jogo, b);
    
    printf("\n");
    aplicarMovAuto(rma);
    printf("\n");
    aplicarWin(rw);
    printf("\n");
    printf("========== REGRAS APLICADAS ==========\n\n");

    // Limpeza da memória dinâmica do jogo
    if (jogo.paciencia != NULL) {
        for (int i = 0; i < jogo.num_pilhas; i++) {
            free(jogo.paciencia[i]);
        }
        free(jogo.paciencia);
    }
}
