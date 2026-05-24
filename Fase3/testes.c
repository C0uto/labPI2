#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "readFile.h"
#include "carta.h"
#include "logica.h"

/* ============================================================
 * AUXILIARES PARA POUPAR LINHAS (<= 15 Instruções)
 * ============================================================ */

void prepEstado(ESTADO *j) {
    memset(j, 0, sizeof(ESTADO));
    j->num_pilhas = 2;
    j->pilhas = malloc(2 * sizeof(PILHA));
    for (int i = 0; i < 2; i++) {
        j->pilhas[i].tamanho = 0;
        j->pilhas[i].capacidade = 10;
        j->pilhas[i].cartas = malloc(10 * sizeof(CARTA));
        j->pilhas[i].tipo = (i == 0) ? "TAB" : "FUND";
    }
}

void limparEstado(ESTADO *j) {
    free(j->pilhas[0].cartas);
    free(j->pilhas[1].cartas);
    free(j->pilhas);
}

void prepRegra(struct regra *r, char *f0, char *f1) {
    memset(r, 0, sizeof(struct regra));
    strcpy(r->flags[0], f0);
    strcpy(r->flags[1], f1);
    r->comando = "MOV";
    r->origem = "TAB";
    r->destino = "FUND";
}

/* ============================================================
 * TESTES GRUPO 1: Flags e Regras Base
 * ============================================================ */

void test_flags_basicas(void) {
    struct regra r; prepRegra(&r, "+[]", "100");
    
    CU_ASSERT_TRUE(temFlag(&r, '+'));
    CU_ASSERT_FALSE(temFlag(&r, '['));
    
    CU_ASSERT_PTR_EQUAL(encontrarRegra(&r, "TAB", "FUND"), &r);
    CU_ASSERT_PTR_NULL(encontrarRegra(&r, "TAB", "CELL"));
    
    CU_ASSERT_TRUE(validarFlagMais(3, &r));
    prepRegra(&r, "+[]", "000");
    CU_ASSERT_FALSE(validarFlagMais(3, &r));
}

void test_seq_desc_cresc(void) {
    ESTADO j; prepEstado(&j);
    struct regra r; prepRegra(&r, "[]", "11");
    
    pushCarta(&j.pilhas[0], 2); /* 2S */
    pushCarta(&j.pilhas[0], 1); /* AS */
    CU_ASSERT_TRUE(validarFlagDescrescente(&j.pilhas[0], 2, &r));
    CU_ASSERT_FALSE(validarFlagCrescente(&j.pilhas[0], 2, &r));
    
    j.pilhas[0].tamanho = 0; /* Reset */
    pushCarta(&j.pilhas[0], 1); /* AS */
    pushCarta(&j.pilhas[0], 2); /* 2S */
    CU_ASSERT_FALSE(validarFlagDescrescente(&j.pilhas[0], 2, &r));
    CU_ASSERT_TRUE(validarFlagCrescente(&j.pilhas[0], 2, &r));
    limparEstado(&j);
}

void test_seq_atributos(void) {
    ESTADO j; prepEstado(&j);
    struct regra r; prepRegra(&r, "mxcd", "1111");
    
    pushCarta(&j.pilhas[0], 1);  /* AS (Preto S) */
    pushCarta(&j.pilhas[0], 15); /* 2H (Vermelho H) */
    
    CU_ASSERT_FALSE(validarFlagMesmoNaipe(&j.pilhas[0], 2, &r));
    CU_ASSERT_TRUE(validarFlagNaipesAlternados(&j.pilhas[0], 2, &r));
    CU_ASSERT_FALSE(validarFlagMesmaCor(&j.pilhas[0], 2, &r));
    CU_ASSERT_TRUE(validarFlagCoresAlternadas(&j.pilhas[0], 2, &r));
    CU_ASSERT_FALSE(validarSequencia(&j.pilhas[0], 2, &r));
    
    limparEstado(&j);
}

/* ============================================================
 * TESTES GRUPO 2: Destinos e Ranks
 * ============================================================ */

void test_dest_valores(void) {
    ESTADO j; prepEstado(&j);
    struct regra r; prepRegra(&r, "<>~V", "1111");
    
    CU_ASSERT_TRUE(validarFlagV(&j.pilhas[1], &r));
    pushCarta(&j.pilhas[0], 2); /* 2S (Mover) */
    pushCarta(&j.pilhas[1], 3); /* 3S (Destino) */
    
    CU_ASSERT_FALSE(validarFlagV(&j.pilhas[1], &r));
    CU_ASSERT_TRUE(validarFlagMenor(&j.pilhas[0], &j.pilhas[1], 1, &r));
    CU_ASSERT_FALSE(validarFlagMaior(&j.pilhas[0], &j.pilhas[1], 1, &r));
    CU_ASSERT_TRUE(validarFlagTil(&j.pilhas[0], &j.pilhas[1], 1, &r));
    CU_ASSERT_FALSE(validarFlagsBasicas(&j.pilhas[0], &j.pilhas[1], 1, &r));
    limparEstado(&j);
}

void test_dest_atributos(void) {
    ESTADO j; prepEstado(&j);
    struct regra r; prepRegra(&r, "MXCD", "1111");
    
    pushCarta(&j.pilhas[0], 1);  /* AS (Preto S) */
    pushCarta(&j.pilhas[1], 14); /* AH (Vermelho H) */
    
    CU_ASSERT_FALSE(validarFlagM(&j.pilhas[0], &j.pilhas[1], 1, &r));
    CU_ASSERT_TRUE(validarFlagX(&j.pilhas[0], &j.pilhas[1], 1, &r));
    CU_ASSERT_FALSE(validarFlagC(&j.pilhas[0], &j.pilhas[1], 1, &r));
    CU_ASSERT_TRUE(validarFlagD(&j.pilhas[0], &j.pilhas[1], 1, &r));
    CU_ASSERT_FALSE(validarFlagsAtributos(&j.pilhas[0], &j.pilhas[1], 1, &r));
    limparEstado(&j);
}

void test_dest_ranks(void) {
    ESTADO j; prepEstado(&j);
    struct regra r; prepRegra(&r, "aAkK*", "11110");
    
    pushCarta(&j.pilhas[0], 1); /* AS */
    CU_ASSERT_FALSE(validarFlagsRanks(&j.pilhas[0], 1, &r));
    
    j.pilhas[0].tamanho = 0;
    pushCarta(&j.pilhas[0], 13); /* KS */
    CU_ASSERT_FALSE(validarFlagsRanks(&j.pilhas[0], 1, &r));
    
    prepRegra(&r, "*", "1");
    CU_ASSERT_TRUE(validarDestino(&j.pilhas[0], &j.pilhas[1], 1, &r));
    limparEstado(&j);
}

/* ============================================================
 * TESTES GRUPO 3: Movimentação, Auto e Tipos
 * ============================================================ */

void test_movimentos_e_tipos(void) {
    ESTADO j; prepEstado(&j);
    struct regra r; prepRegra(&r, "V", "1");
    struct regra4 rt;
    rt.tipoDePilha = "FUND"; strcpy(rt.flags[0], "1"); strcpy(rt.flags[1], "1");
    rt.prox = NULL;
    
    pushCarta(&j.pilhas[0], 1);
    CU_ASSERT_TRUE(tipoTemFlag1(&rt, "FUND"));
    CU_ASSERT_TRUE(validarMovimento(&j, 0, 1, 1, &r, NULL));
    
    executarMov(&j, 0, 1, 1);
    CU_ASSERT_EQUAL(j.pilhas[0].tamanho, 0);
    CU_ASSERT_EQUAL(j.pilhas[1].tamanho, 1);
    CU_ASSERT_FALSE(tentarMover(&j, 0, 1, 1, &r, NULL)); /* Já está vazia */
    limparEstado(&j);
}

void test_processar_auto(void) {
    ESTADO j; prepEstado(&j);
    struct regra r; prepRegra(&r, "V", "1");
    r.comando = "AUTO"; r.prox = NULL;
    
    pushCarta(&j.pilhas[0], 1);
    CU_ASSERT_TRUE(executarAutoSeValido(&j, &r, 0, 1, 1, NULL));
    CU_ASSERT_EQUAL(j.pilhas[1].tamanho, 1);
    
    pushCarta(&j.pilhas[0], 2);
    CU_ASSERT_TRUE(tentarAutoEntrePilhas(&j, &r, 0, 1, NULL)); /* Já n é validado V, mas força exec */
    processarAuto(&j, &r, NULL);
    CU_ASSERT_EQUAL(j.pilhas[0].tamanho, 1); /* Loop parou pq falhou regra V */
    limparEstado(&j);
}

void test_vitoria(void) {
    ESTADO j; prepEstado(&j);
    pushCarta(&j.pilhas[0], 1);
    pushCarta(&j.pilhas[1], 2);
    
    CU_ASSERT_EQUAL(contarCartasTipo(&j, "TAB"), 1);
    struct regra6 rw; rw.tipoDePilha = "FUND"; rw.condicaoWin = 1; rw.prox = NULL;
    
    CU_ASSERT_TRUE(condicaoWinSatisfeita(&j, &rw));
    CU_ASSERT_TRUE(verificarWin(&j, &rw));
    
    rw.condicaoWin = 5; /* Fictício: precisa de 5, só tem 1 */
    CU_ASSERT_FALSE(verificarWin(&j, &rw));
    limparEstado(&j);
}

/* ============================================================
 * REGISTO CUNIT (Dividido em A e B para <15 instruções)
 * ============================================================ */

void registar_grupo_a(CU_pSuite pS) {
    CU_add_test(pS, "Flags Basicas", test_flags_basicas);
    CU_add_test(pS, "Sequencias (Valores)", test_seq_desc_cresc);
    CU_add_test(pS, "Sequencias (Atributos)", test_seq_atributos);
    CU_add_test(pS, "Destino (Valores)", test_dest_valores);
}

void registar_grupo_b(CU_pSuite pS) {
    CU_add_test(pS, "Destino (Atributos)", test_dest_atributos);
    CU_add_test(pS, "Destino (Ranks)", test_dest_ranks);
    CU_add_test(pS, "Movimentos", test_movimentos_e_tipos);
    CU_add_test(pS, "Movimentos Auto", test_processar_auto);
    CU_add_test(pS, "Vitoria", test_vitoria);
}

int main(void) {
    if (CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();
    CU_pSuite pS = CU_add_suite("Logica", NULL, NULL);
    
    if (pS != NULL) {
        registar_grupo_a(pS);
        registar_grupo_b(pS);
    }
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}