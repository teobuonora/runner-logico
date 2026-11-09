/*
 * Testes das regras do jogo (sem abrir janela). Rode com: make test-mundo
 * (linka com a Raylib só por causa de Rectangle/CheckCollisionRecs, que não precisam de janela.)
 */
#include <stdio.h>
#include <string.h>
#include "mundo.h"

static int total = 0, falhas = 0;
#define CHECK(cond) do { total++; if (!(cond)) { falhas++; printf("  FALHOU: %s (linha %d)\n", #cond, __LINE__); } } while (0)

#define DT (1.0f / 60.0f)

static Entrada vazia(void) { Entrada in; memset(&in, 0, sizeof in); return in; }

static int idPorTexto(const char *txt) {
    char buf[64];
    for (int id = 0; id < catalogoTamanho(); id++) {
        Formula f = catalogoFormula(id);
        formulaParaTexto(&f, buf, sizeof buf);
        if (strcmp(buf, txt) == 0) return id;
    }
    return -1;
}

/* mundo "limpo": sem spawn automático atrapalhando */
static Mundo mundoQuieto(void) {
    Mundo m = mundo_novo(123);
    m.distProx = 1e9f;
    m.distProxPortao = 1e9f;
    return m;
}

static Entidade coloca(TipoEnt tipo, float x, float y, float w, float h) {
    Entidade e; memset(&e, 0, sizeof e);
    e.tipo = tipo; e.caixa = (Rectangle){ x, y, w, h }; e.y0 = y; e.vida = 1; e.viva = true;
    return e;
}

static void testa_regras_puras(void) {
    printf("regras puras\n");
    CHECK(regra_velocidade_base(0) == VEL_INICIAL);
    CHECK(regra_velocidade_base(1e6f) == VEL_MAX);
    CHECK(regra_velocidade_base(10) > regra_velocidade_base(5));
    CHECK(regra_energia(50, 0, 1.0f) > 50);                 /* sem proposições V: recarrega */
    CHECK(regra_energia(50, 2, 1.0f) < 50);                 /* com V: gasta */
    CHECK(regra_energia(1, 3, 10.0f) == 0.0f);              /* não fica negativa */
    CHECK(regra_energia(99, 0, 10.0f) == ENERGIA_MAX);      /* não passa do máximo */
    Valoracao v = valoracaoDeMascara(7);
    CHECK(regra_conta_ativas(v) == 3);
    CHECK(regra_nivel_formula(0) == 0 && regra_nivel_formula(2) == 1 && regra_nivel_formula(99) == 3);
}

static void testa_estado_inicial(void) {
    printf("estado inicial e pureza\n");
    Mundo m = mundo_novo(1);
    CHECK(m.vidas == VIDAS_INICIAIS && m.nEnts == 0 && m.nTiros == 0);
    CHECK(m.resultado == RES_JOGANDO && m.noChao);
    Mundo n = mundo_atualiza(m, vazia(), DT);
    CHECK(m.tempo == 0.0f);          /* o original NÃO mudou (função pura) */
    CHECK(n.tempo > 0.0f);
}

static void testa_pulo(void) {
    printf("pulo\n");
    Mundo m = mundoQuieto();
    Entrada in = vazia(); in.pula = true;
    m = mundo_atualiza(m, in, DT);
    CHECK(!m.noChao && m.alt > 0.0f && (m.eventos & EV_PULO));
    float apice = 0;
    for (int i = 0; i < 90; i++) {
        m = mundo_atualiza(m, vazia(), DT);
        if (m.alt > apice) apice = m.alt;
    }
    CHECK(m.noChao && m.alt == 0.0f);          /* voltou ao chão */
    CHECK(apice > 100.0f && apice < 220.0f);   /* altura razoável */
    /* não pula duas vezes no ar */
    m = mundo_atualiza(m, in, DT);
    float v1 = m.velY;
    m = mundo_atualiza(m, in, DT);
    CHECK(m.velY < v1);
}

static void testa_proposicoes_e_energia(void) {
    printf("proposicoes e energia\n");
    Mundo m = mundoQuieto();
    Entrada in = vazia(); in.alterna[PROP_ESCUDO] = true;
    m = mundo_atualiza(m, in, DT);
    CHECK(m.val.v[PROP_ESCUDO]);
    m = mundo_atualiza(m, in, DT);
    CHECK(!m.val.v[PROP_ESCUDO]);              /* apertar de novo desliga */

    /* energia baixa + proposição ligada = sobrecarga */
    m = mundoQuieto();
    m.val.v[PROP_ESCUDO] = true; m.val.v[PROP_TURBO] = true;
    m.energia = 0.05f;
    m = mundo_atualiza(m, vazia(), DT);
    CHECK(regra_conta_ativas(m.val) == 0 && m.sobrecarga > 0.0f && (m.eventos & EV_SOBRECARGA));
    in = vazia(); in.alterna[PROP_ARMA] = true;
    m = mundo_atualiza(m, in, DT);
    CHECK(!m.val.v[PROP_ARMA]);                /* travado durante a sobrecarga */

    /* turbo aumenta a velocidade */
    Mundo a = mundoQuieto(), b = mundoQuieto();
    b.val.v[PROP_TURBO] = true;
    a = mundo_atualiza(a, vazia(), DT); b = mundo_atualiza(b, vazia(), DT);
    CHECK(b.velocidade > a.velocidade);
}

static void testa_tiro(void) {
    printf("tiro e inimigos\n");
    Mundo m = mundoQuieto();
    Entrada in = vazia(); in.atira = true;
    m = mundo_atualiza(m, in, DT);
    CHECK(m.nTiros == 0 && (m.eventos & EV_SEM_ARMA));    /* Q falsa: não atira */

    m = mundoQuieto(); m.val.v[PROP_ARMA] = true;
    m = mundo_atualiza(m, in, DT);
    CHECK(m.nTiros == 1 && (m.eventos & EV_TIRO));

    /* andarilho tem 2 de vida: dois tiros */
    m = mundoQuieto(); m.val.v[PROP_ARMA] = true;
    Entidade e = coloca(ENT_ANDARILHO, 600, CHAO_Y - 44, 40, 44); e.vida = 2;
    m.ents[m.nEnts++] = e;
    int pontos0 = m.bonus;
    for (int i = 0; i < 120 && m.nEnts > 0; i++) m = mundo_atualiza(m, in, DT);
    CHECK(m.nEnts == 0 && m.abatidos == 1 && m.bonus > pontos0);
}

static void testa_colisoes(void) {
    printf("colisoes\n");
    /* sem escudo: perde vida */
    Mundo m = mundoQuieto();
    m.ents[m.nEnts++] = coloca(ENT_CACTO, JOGADOR_X + 5, CHAO_Y - 50, 26, 50);
    m = mundo_atualiza(m, vazia(), DT);
    CHECK(m.vidas == VIDAS_INICIAIS - 1 && (m.eventos & EV_DANO) && m.nEnts == 0);

    /* durante a invulnerabilidade não perde mais */
    m.ents[m.nEnts++] = coloca(ENT_CACTO, JOGADOR_X + 5, CHAO_Y - 50, 26, 50);
    m = mundo_atualiza(m, vazia(), DT);
    CHECK(m.vidas == VIDAS_INICIAIS - 1);

    /* com escudo (P = V): destrói e não perde vida */
    m = mundoQuieto(); m.val.v[PROP_ESCUDO] = true;
    m.ents[m.nEnts++] = coloca(ENT_ANDARILHO, JOGADOR_X + 5, CHAO_Y - 44, 40, 44);
    m = mundo_atualiza(m, vazia(), DT);
    CHECK(m.vidas == VIDAS_INICIAIS && m.nEnts == 0 && m.bonus > 0);

    /* pular por cima do cacto */
    m = mundoQuieto();
    m.ents[m.nEnts++] = coloca(ENT_CACTO, 500, CHAO_Y - 50, 26, 50);
    Entrada in = vazia();
    for (int i = 0; i < 400; i++) {
        in.pula = (m.ents[0].viva && m.nEnts > 0 && m.ents[0].caixa.x < JOGADOR_X + 150 && m.noChao);
        m = mundo_atualiza(m, in, DT);
    }
    CHECK(m.vidas == VIDAS_INICIAIS);

    /* power-ups */
    m = mundoQuieto(); m.vidas = 2; m.energia = 10;
    m.ents[m.nEnts++] = coloca(ENT_PU_VIDA, JOGADOR_X, CHAO_Y - 40, 28, 28);
    m.ents[m.nEnts++] = coloca(ENT_PU_ENERGIA, JOGADOR_X, CHAO_Y - 40, 28, 28);
    m.ents[m.nEnts++] = coloca(ENT_PU_TAUTOLOGIA, JOGADOR_X, CHAO_Y - 40, 28, 28);
    m = mundo_atualiza(m, vazia(), DT);
    CHECK(m.vidas == 3 && m.energia > 50 && m.tautologia);
}

static void testa_portoes(void) {
    printf("portoes logicos\n");
    int idPeQ = idPorTexto("P & Q");
    CHECK(idPeQ >= 0);

    /* P=F, Q=F -> (P & Q) falsa: perde vida */
    Mundo m = mundoQuieto();
    Entidade g = coloca(ENT_PORTAO, JOGADOR_X - 5, CHAO_Y - 180, 26, 180); g.formulaId = idPeQ;
    m.ents[m.nEnts++] = g;
    m = mundo_atualiza(m, vazia(), DT);
    CHECK(m.portoesErro == 1 && m.vidas == VIDAS_INICIAIS - 1 && (m.eventos & EV_PORTAO_ERRO));

    /* P=V, Q=V -> verdadeira: abre e dá pontos */
    m = mundoQuieto(); m.val.v[0] = true; m.val.v[1] = true;
    m.ents[m.nEnts++] = g;
    m = mundo_atualiza(m, vazia(), DT);
    CHECK(m.portoesOk == 1 && m.vidas == VIDAS_INICIAIS && m.bonus == PONTOS_PORTAO);

    /* tautologia (power-up) abre mesmo com a fórmula falsa, e é consumida */
    m = mundoQuieto(); m.tautologia = true;
    m.ents[m.nEnts++] = g;
    m = mundo_atualiza(m, vazia(), DT);
    CHECK(m.portoesOk == 1 && !m.tautologia);

    /* o portão só é avaliado UMA vez */
    for (int i = 0; i < 30; i++) m = mundo_atualiza(m, vazia(), DT);
    CHECK(m.portoesOk == 1 && m.portoesErro == 0);

    /* vitória ao abrir META_PORTOES portões */
    m = mundoQuieto(); m.portoesOk = META_PORTOES - 1; m.val.v[0] = true; m.val.v[1] = true;
    m.ents[m.nEnts++] = g;
    m = mundo_atualiza(m, vazia(), DT);
    CHECK(m.resultado == RES_VITORIA);

    /* derrota ao zerar as vidas */
    m = mundoQuieto(); m.vidas = 1;
    m.ents[m.nEnts++] = g;
    m = mundo_atualiza(m, vazia(), DT);
    CHECK(m.resultado == RES_DERROTA && m.vidas == 0);
}

static unsigned fingerprint(const Mundo *m) {
    return (unsigned)(m->distancia * 10.0f) * 31u + (unsigned)m->nEnts * 17u + (unsigned)mundo_pontos(m)
         + (unsigned)m->vidas * 7919u + (unsigned)m->portoesGerados * 104729u;
}

static void testa_simulacao(void) {
    printf("simulacao longa (robustez e determinismo)\n");
    for (unsigned semente = 1; semente <= 5; semente++) {
        Mundo a = mundo_novo(semente), b = mundo_novo(semente);
        for (int i = 0; i < 60 * 120; i++) {        /* 2 minutos de jogo, entrada "aleatória" */
            Entrada in = vazia();
            in.pula   = (i % 37) == 0;
            in.abaixa = (i % 91) < 20;
            in.atira  = (i % 5) < 3;
            in.alterna[0] = (i % 211) == 0;
            in.alterna[1] = (i % 83) == 0;
            in.alterna[2] = (i % 157) == 0;
            a = mundo_atualiza(a, in, DT);
            b = mundo_atualiza(b, in, DT);
            if (a.nEnts > MAX_ENTIDADES || a.nTiros > MAX_TIROS || a.nEnts < 0) { CHECK(0); break; }
            if (a.energia < 0 || a.energia > ENERGIA_MAX) { CHECK(0); break; }
            if (a.resultado != RES_JOGANDO) break;
        }
        CHECK(fingerprint(&a) == fingerprint(&b));  /* mesma semente + mesma entrada = mesmo jogo */
    }
    /* sem inimigos e sem fim de jogo, os portões aparecem sozinhos */
    Mundo m = mundo_novo(7);
    m.vidas = 99;
    for (int i = 0; i < 60 * 20; i++) m = mundo_atualiza(m, vazia(), DT);
    CHECK(m.portoesGerados >= 1);
}

int main(void) {
    testa_regras_puras();
    testa_estado_inicial();
    testa_pulo();
    testa_proposicoes_e_energia();
    testa_tiro();
    testa_colisoes();
    testa_portoes();
    testa_simulacao();
    printf("\n%d verificacoes, %d falhas\n", total, falhas);
    return falhas ? 1 : 0;
}
