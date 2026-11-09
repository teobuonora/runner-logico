/* Testes da lógica pura (sem Raylib, sem janela). Rode com: make test-logica */
#include <stdio.h>
#include <string.h>
#include "logica.h"
#include "funcional.h"

static int total = 0, falhas = 0;
#define CHECK(cond) do { total++; if (!(cond)) { falhas++; printf("  FALHOU: %s (linha %d)\n", #cond, __LINE__); } } while (0)

static int idPorTexto(const char *txt) {
    char buf[64];
    for (int id = 0; id < catalogoTamanho(); id++) {
        Formula f = catalogoFormula(id);
        formulaParaTexto(&f, buf, sizeof buf);
        if (strcmp(buf, txt) == 0) return id;
    }
    return -1;
}

static void testa_conectivos(void) {
    printf("conectivos e tabelas-verdade\n");
    Formula P = f_var(0), Q = f_var(1);
    Formula e = f_e(P, Q), ou = f_ou(P, Q), se = f_se(P, Q), sse = f_sse(P, Q), nao = f_nao(P);
    for (unsigned m = 0; m < 4; m++) {
        Valoracao v = valoracaoDeMascara(m << 1);     /* P=bit2, Q=bit1, R=0 */
        bool p = v.v[0], q = v.v[1];
        CHECK(avalia(&e,   v) == (p && q));
        CHECK(avalia(&ou,  v) == (p || q));
        CHECK(avalia(&se,  v) == (!p || q));
        CHECK(avalia(&sse, v) == (p == q));
        CHECK(avalia(&nao, v) == !p);
    }
    CHECK(contaModelos(&e) == 2);
    CHECK(contaModelos(&ou) == 6);
    CHECK(contaModelos(&se) == 6);
    CHECK(contaModelos(&sse) == 4);
}

static void testa_propriedades(void) {
    printf("tautologia, contradicao, equivalencia\n");
    Formula P = f_var(0);
    Formula taut = f_ou(P, f_nao(P));
    Formula contra = f_e(P, f_nao(P));
    CHECK(ehTautologia(&taut) && !ehContradicao(&taut));
    CHECK(ehContradicao(&contra) && !ehTautologia(&contra));

    /* De Morgan: ~(P & Q) == ~P | ~Q */
    int a = idPorTexto("~(P & Q)"), b = idPorTexto("~P | ~Q");
    CHECK(a >= 0 && b >= 0);
    Formula fa = catalogoFormula(a), fb = catalogoFormula(b);
    CHECK(saoEquivalentes(&fa, &fb));

    /* condicional: P -> Q == ~P | Q */
    int c = idPorTexto("P -> Q"), d = idPorTexto("~P | Q");
    CHECK(c >= 0 && d >= 0);
    Formula fc = catalogoFormula(c), fd = catalogoFormula(d);
    CHECK(saoEquivalentes(&fc, &fd));

    /* P -> Q NÃO é equivalente a Q -> P */
    Formula Q = f_var(1);
    Formula pq = f_se(P, Q), qp = f_se(Q, P);
    CHECK(!saoEquivalentes(&pq, &qp));
}

static void testa_texto(void) {
    printf("formulaParaTexto\n");
    char buf[64];
    Formula P = f_var(0), Q = f_var(1), R = f_var(2);
    Formula f1 = f_e(P, f_nao(Q));
    formulaParaTexto(&f1, buf, sizeof buf);  CHECK(strcmp(buf, "P & ~Q") == 0);
    Formula f2 = f_se(f_e(P, Q), R);
    formulaParaTexto(&f2, buf, sizeof buf);  CHECK(strcmp(buf, "(P & Q) -> R") == 0);
    Formula f3 = f_nao(f_ou(P, Q));
    formulaParaTexto(&f3, buf, sizeof buf);  CHECK(strcmp(buf, "~(P | Q)") == 0);
    Formula f4 = f_sse(P, R);
    formulaParaTexto(&f4, buf, sizeof buf);  CHECK(strcmp(buf, "P <-> R") == 0);
    /* buffer minúsculo não pode estourar */
    char pequeno[4];
    formulaParaTexto(&f2, pequeno, sizeof pequeno);
    CHECK(strlen(pequeno) < sizeof pequeno);
}

static void testa_catalogo(void) {
    printf("catalogo de portoes\n");
    for (int id = 0; id < catalogoTamanho(); id++) {
        Formula f = catalogoFormula(id);
        int modelos = contaModelos(&f);
        CHECK(modelos > 0);                  /* todo portão tem solução (não é contradição) */
        CHECK(modelos < 8);                  /* e não é tautologia (precisa de decisão)   */
        CHECK(catalogoNivel(id) >= 0 && catalogoNivel(id) <= 3);
    }
    /* níveis em ordem crescente */
    for (int id = 1; id < catalogoTamanho(); id++)
        CHECK(catalogoNivel(id) >= catalogoNivel(id - 1));
    /* fora do intervalo não quebra */
    Formula f = catalogoFormula(999);
    CHECK(f.n > 0);
}

static void testa_mascara(void) {
    printf("mascara <-> valoracao\n");
    for (unsigned m = 0; m < 8; m++)
        CHECK(mascaraDeValoracao(valoracaoDeMascara(m)) == m);
    Valoracao v = valoracaoDeMascara(5);     /* 101 -> P=V Q=F R=V */
    CHECK(v.v[0] && !v.v[1] && v.v[2]);
}

/* ---------- map / filter / fold ---------- */
typedef struct { int vida; int valor; } Alvo;
static void dobraVida(void *p, void *c)           { (void)c; ((Alvo *)p)->vida *= 2; }
static bool estaVivo(const void *p, const void *c){ (void)c; return ((const Alvo *)p)->vida > 0; }
static float somaValor(float acc, const void *p, const void *c) { (void)c; return acc + (float)((const Alvo *)p)->valor; }

static void testa_funcional(void) {
    printf("mapa / filtra / dobra\n");
    Alvo a[4] = { {1, 10}, {0, 20}, {3, 30}, {0, 40} };

    mapa(a, 4, sizeof(Alvo), dobraVida, NULL);
    CHECK(a[0].vida == 2 && a[2].vida == 6);

    CHECK(conta(a, 4, sizeof(Alvo), estaVivo, NULL) == 2);
    CHECK(dobra(a, 4, sizeof(Alvo), 0.0f, somaValor, NULL) == 100.0f);

    int novo = filtra(a, 4, sizeof(Alvo), estaVivo, NULL);
    CHECK(novo == 2);
    CHECK(a[0].valor == 10 && a[1].valor == 30);     /* compactou mantendo a ordem */

    /* bordas: vetor vazio / NULL */
    CHECK(filtra(a, 0, sizeof(Alvo), estaVivo, NULL) == 0);
    CHECK(conta(NULL, 3, sizeof(Alvo), estaVivo, NULL) == 0);
    CHECK(dobra(a, 0, sizeof(Alvo), 7.0f, somaValor, NULL) == 7.0f);
    mapa(NULL, 3, sizeof(Alvo), dobraVida, NULL);    /* não pode quebrar */
}

int main(void) {
    testa_conectivos();
    testa_propriedades();
    testa_texto();
    testa_catalogo();
    testa_mascara();
    testa_funcional();
    printf("\n%d verificacoes, %d falhas\n", total, falhas);
    return falhas ? 1 : 0;
}
