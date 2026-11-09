#ifndef LOGICA_H
#define LOGICA_H

/*
 * Módulo de LÓGICA PROPOSICIONAL (puro: sem Raylib, sem estado global).
 *
 * - Valoracao: o valor-verdade (V/F) de cada proposição P, Q, R.
 * - Formula:   árvore de uma fórmula (variáveis, ~, &, |, ->, <->).
 * - avalia():  calcula o valor-verdade da fórmula para uma valoração.
 *
 * Tudo aqui é função pura: mesma entrada, mesma saída. Dá para testar
 * sem abrir janela (veja tests/test_logica.c).
 */

#include <stdbool.h>
#include <stddef.h>

#define NUM_PROPS 3     /* P, Q, R */
#define MAX_NOS   16    /* tamanho máximo de uma fórmula (nós da árvore) */

typedef enum {
    OP_VAR,   /* P, Q, R          */
    OP_NAO,   /* ~A     (negação)      */
    OP_E,     /* A & B  (conjunção)    */
    OP_OU,    /* A | B  (disjunção)    */
    OP_SE,    /* A -> B (condicional)  */
    OP_SSE    /* A <-> B (bicondicional) */
} Operador;

typedef struct {
    Operador op;
    int var;        /* índice da proposição (só em OP_VAR) */
    int esq, dir;   /* índices dos filhos em nos[]; -1 se não tem */
} No;

typedef struct {
    No  nos[MAX_NOS];
    int n;          /* quantos nós em uso */
    int raiz;       /* índice do nó raiz */
} Formula;

typedef struct {
    bool v[NUM_PROPS];   /* v[0]=P, v[1]=Q, v[2]=R */
} Valoracao;

/* ---------- construtores (compõem fórmulas, devolvem uma nova) ---------- */
Formula f_var(int v);
Formula f_nao(Formula a);
Formula f_e(Formula a, Formula b);
Formula f_ou(Formula a, Formula b);
Formula f_se(Formula a, Formula b);     /* a -> b   */
Formula f_sse(Formula a, Formula b);    /* a <-> b  */

/* ---------- avaliação e propriedades ---------- */
bool     avalia(const Formula *f, Valoracao val);
Valoracao valoracaoDeMascara(unsigned mascara);   /* bit2=P, bit1=Q, bit0=R (ordem da tabela-verdade) */
unsigned  mascaraDeValoracao(Valoracao val);
int      contaModelos(const Formula *f);          /* nº de linhas V na tabela-verdade (0..8) */
bool     ehTautologia(const Formula *f);
bool     ehContradicao(const Formula *f);
bool     saoEquivalentes(const Formula *a, const Formula *b);

/* ---------- texto ---------- */
const char *nomeProp(int i);                                   /* "P", "Q", "R" */
void formulaParaTexto(const Formula *f, char *buf, size_t tam); /* ex.: "(P & ~Q) -> R" */

/* ---------- catálogo de fórmulas dos portões ---------- */
int     catalogoTamanho(void);
Formula catalogoFormula(int id);
int     catalogoNivel(int id);    /* 0 (fácil) .. 3 (difícil) */

#endif
