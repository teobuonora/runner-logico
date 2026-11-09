#include "logica.h"
#include <stdio.h>
#include <string.h>

#define CATALOGO_TAM 21

static const char *NOMES_PROP[NUM_PROPS] = { "P", "Q", "R" };

const char *nomeProp(int i) {
    return (i >= 0 && i < NUM_PROPS) ? NOMES_PROP[i] : "?";
}

/* ======================= construtores ======================= */

Formula f_var(int v) {
    Formula f;
    memset(&f, 0, sizeof f);
    if (v < 0 || v >= NUM_PROPS) v = 0;
    f.nos[0] = (No){ OP_VAR, v, -1, -1 };
    f.n = 1;
    f.raiz = 0;
    return f;
}

Formula f_nao(Formula a) {
    if (a.n + 1 > MAX_NOS) return a;                 /* robustez: fórmula grande demais */
    a.nos[a.n] = (No){ OP_NAO, -1, a.raiz, -1 };
    a.raiz = a.n;
    a.n++;
    return a;
}

/* junta duas fórmulas sob um operador binário (copia os nós de b depois dos de a) */
static Formula une(Operador op, Formula a, Formula b) {
    if (a.n + b.n + 1 > MAX_NOS) return a;
    int desloc = a.n;
    for (int i = 0; i < b.n; i++) {
        No no = b.nos[i];
        if (no.esq >= 0) no.esq += desloc;
        if (no.dir >= 0) no.dir += desloc;
        a.nos[desloc + i] = no;
    }
    int raizB = b.raiz + desloc;
    a.n = desloc + b.n;
    a.nos[a.n] = (No){ op, -1, a.raiz, raizB };
    a.raiz = a.n;
    a.n++;
    return a;
}

Formula f_e(Formula a, Formula b)   { return une(OP_E,   a, b); }
Formula f_ou(Formula a, Formula b)  { return une(OP_OU,  a, b); }
Formula f_se(Formula a, Formula b)  { return une(OP_SE,  a, b); }
Formula f_sse(Formula a, Formula b) { return une(OP_SSE, a, b); }

/* ======================= avaliação ======================= */

static bool avaliaNo(const Formula *f, int i, Valoracao val) {
    if (i < 0 || i >= f->n) return false;
    const No *no = &f->nos[i];
    switch (no->op) {
        case OP_VAR: return val.v[no->var];
        case OP_NAO: return !avaliaNo(f, no->esq, val);
        case OP_E:   return avaliaNo(f, no->esq, val) && avaliaNo(f, no->dir, val);
        case OP_OU:  return avaliaNo(f, no->esq, val) || avaliaNo(f, no->dir, val);
        case OP_SE:  return !avaliaNo(f, no->esq, val) || avaliaNo(f, no->dir, val);
        case OP_SSE: return avaliaNo(f, no->esq, val) == avaliaNo(f, no->dir, val);
    }
    return false;
}

bool avalia(const Formula *f, Valoracao val) {
    if (f == NULL || f->n <= 0) return false;
    return avaliaNo(f, f->raiz, val);
}

Valoracao valoracaoDeMascara(unsigned m) {
    Valoracao v;
    v.v[0] = (m >> 2) & 1u;   /* P */
    v.v[1] = (m >> 1) & 1u;   /* Q */
    v.v[2] =  m       & 1u;   /* R */
    return v;
}

unsigned mascaraDeValoracao(Valoracao v) {
    return (v.v[0] ? 4u : 0u) | (v.v[1] ? 2u : 0u) | (v.v[2] ? 1u : 0u);
}

/* fold sobre as 8 linhas da tabela-verdade */
int contaModelos(const Formula *f) {
    int total = 0;
    for (unsigned m = 0; m < (1u << NUM_PROPS); m++)
        if (avalia(f, valoracaoDeMascara(m))) total++;
    return total;
}

bool ehTautologia(const Formula *f)  { return contaModelos(f) == (1 << NUM_PROPS); }
bool ehContradicao(const Formula *f) { return contaModelos(f) == 0; }

bool saoEquivalentes(const Formula *a, const Formula *b) {
    for (unsigned m = 0; m < (1u << NUM_PROPS); m++) {
        Valoracao v = valoracaoDeMascara(m);
        if (avalia(a, v) != avalia(b, v)) return false;
    }
    return true;
}

/* ======================= texto ======================= */

static void anexa(char *buf, size_t tam, const char *s) {
    size_t len = strlen(buf);
    if (len + 1 >= tam) return;
    snprintf(buf + len, tam - len, "%s", s);
}

static void escreveNo(const Formula *f, int i, bool raiz, char *buf, size_t tam) {
    if (i < 0 || i >= f->n) return;
    const No *no = &f->nos[i];
    switch (no->op) {
        case OP_VAR:
            anexa(buf, tam, nomeProp(no->var));
            break;
        case OP_NAO:
            anexa(buf, tam, "~");
            escreveNo(f, no->esq, false, buf, tam);
            break;
        default: {
            const char *simbolo = " & ";
            if (no->op == OP_OU)  simbolo = " | ";
            if (no->op == OP_SE)  simbolo = " -> ";
            if (no->op == OP_SSE) simbolo = " <-> ";
            if (!raiz) anexa(buf, tam, "(");
            escreveNo(f, no->esq, false, buf, tam);
            anexa(buf, tam, simbolo);
            escreveNo(f, no->dir, false, buf, tam);
            if (!raiz) anexa(buf, tam, ")");
        }
    }
}

void formulaParaTexto(const Formula *f, char *buf, size_t tam) {
    if (buf == NULL || tam == 0) return;
    buf[0] = '\0';
    if (f == NULL || f->n <= 0) return;
    escreveNo(f, f->raiz, true, buf, tam);
}

/* ======================= catálogo ======================= */

int catalogoTamanho(void) { return CATALOGO_TAM; }

int catalogoNivel(int id) {
    static const int nivel[CATALOGO_TAM] = {
        0, 0, 0, 0,             /* 0-3   */
        1, 1, 1, 1, 1, 1,       /* 4-9   */
        2, 2, 2, 2, 2, 2,       /* 10-15 */
        3, 3, 3, 3, 3           /* 16-20 */
    };
    return (id >= 0 && id < CATALOGO_TAM) ? nivel[id] : 0;
}

Formula catalogoFormula(int id) {
    const Formula P = f_var(0), Q = f_var(1), R = f_var(2);
    switch (id) {
        /* nível 0 */
        case 0:  return P;
        case 1:  return f_nao(Q);
        case 2:  return f_e(P, Q);
        case 3:  return f_ou(P, Q);
        /* nível 1 */
        case 4:  return f_e(P, f_nao(Q));
        case 5:  return f_ou(f_nao(P), R);
        case 6:  return f_se(P, Q);
        case 7:  return f_e(Q, R);
        case 8:  return f_e(f_nao(P), f_nao(R));
        case 9:  return f_ou(f_nao(P), Q);                 /* equivalente a P -> Q (id 6) */
        /* nível 2 */
        case 10: return f_e(f_se(P, Q), R);
        case 11: return f_sse(P, Q);
        case 12: return f_e(f_ou(P, Q), f_nao(R));
        case 13: return f_nao(f_e(P, Q));
        case 14: return f_ou(f_nao(P), f_nao(Q));          /* De Morgan: equivalente ao id 13 */
        case 15: return f_e(P, f_ou(Q, R));
        /* nível 3 */
        case 16: return f_se(f_e(P, Q), R);
        case 17: return f_ou(f_sse(P, Q), R);
        case 18: return f_se(f_nao(P), f_e(Q, R));
        case 19: return f_se(f_ou(P, Q), f_nao(R));
        case 20: return f_e(f_nao(f_sse(P, R)), Q);
        default: return P;
    }
}
