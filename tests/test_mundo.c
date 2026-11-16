/*
 * Testes das regras do jogo sem abrir janela.
 *
 * Rode com:
 *     mingw32-make test-mundo
 *
 * Ou, dependendo do Makefile:
 *     mingw32-make test
 *
 * O teste é linkado com a Raylib somente por causa de
 * Rectangle e CheckCollisionRecs.
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "mundo.h"

static int total = 0;
static int falhas = 0;

#define CHECK(cond)                                                        \
    do {                                                                   \
        total++;                                                           \
        if (!(cond)) {                                                     \
            falhas++;                                                      \
            printf(                                                        \
                "  FALHOU: %s (linha %d)\n",                               \
                #cond,                                                     \
                __LINE__                                                   \
            );                                                             \
        }                                                                  \
    } while (0)

#define DT (1.0f / 60.0f)

static Entrada vazia(void) {
    Entrada in;
    memset(&in, 0, sizeof in);
    return in;
}

static bool quaseIgual(float a, float b, float tolerancia) {
    return fabsf(a - b) <= tolerancia;
}

static int idPorTexto(const char *texto) {
    char buffer[64];

    for (int id = 0; id < catalogoTamanho(); id++) {
        Formula formula = catalogoFormula(id);

        formulaParaTexto(
            &formula,
            buffer,
            sizeof buffer
        );

        if (strcmp(buffer, texto) == 0) {
            return id;
        }
    }

    return -1;
}

/*
 * Mundo sem spawn automático atrapalhando os testes.
 */
static Mundo mundoQuieto(void) {
    Mundo m = mundo_novo(123);

    m.distProx = 1e9f;
    m.distProxPortao = 1e9f;

    return m;
}

static Entidade coloca(
    TipoEnt tipo,
    float x,
    float y,
    float largura,
    float altura
) {
    Entidade e;
    memset(&e, 0, sizeof e);

    e.tipo = tipo;

    e.caixa = (Rectangle){
        x,
        y,
        largura,
        altura
    };

    e.y0 = y;
    e.vida = 1;
    e.viva = true;

    return e;
}

static Entidade criaPortao(
    int formulaId,
    float x
) {
    Entidade portao = coloca(
        ENT_PORTAO,
        x,
        CHAO_Y - 180.0f,
        26.0f,
        180.0f
    );

    portao.formulaId = formulaId;

    return portao;
}

/*
 * Inicia o desafio lógico de um portão que já está dentro
 * da distância de decisão.
 */
static Mundo iniciaDesafio(
    Mundo m,
    Entidade portao
) {
    m.ents[m.nEnts++] = portao;
    return mundo_atualiza(m, vazia(), DT);
}

/*
 * Confirma imediatamente a valoração atual.
 */
static Mundo confirmaDesafio(Mundo m) {
    Entrada in = vazia();
    in.confirma = true;

    return mundo_atualiza(m, in, DT);
}

/* ============================================================
 * Regras puras
 * ============================================================ */

static void testa_regras_puras(void) {
    printf("regras puras\n");

    CHECK(
        regra_velocidade_base(0.0f) ==
        VEL_INICIAL
    );

    CHECK(
        regra_velocidade_base(1e6f) ==
        VEL_MAX
    );

    CHECK(
        regra_velocidade_base(10.0f) >
        regra_velocidade_base(5.0f)
    );

    /*
     * Sem proposições verdadeiras, a energia recarrega.
     */
    CHECK(
        regra_energia(50.0f, 0, 1.0f) >
        50.0f
    );

    /*
     * Com proposições verdadeiras, a energia diminui.
     */
    CHECK(
        regra_energia(50.0f, 2, 1.0f) <
        50.0f
    );

    CHECK(
        regra_energia(1.0f, 3, 10.0f) ==
        0.0f
    );

    CHECK(
        regra_energia(99.0f, 0, 10.0f) ==
        ENERGIA_MAX
    );

    Valoracao v = valoracaoDeMascara(7);

    CHECK(
        regra_conta_ativas(v) == 3
    );

    CHECK(
        regra_nivel_formula(0) == 0 &&
        regra_nivel_formula(2) == 1 &&
        regra_nivel_formula(99) == 3
    );
}

/* ============================================================
 * Estado inicial e pureza
 * ============================================================ */

static void testa_estado_inicial(void) {
    printf("estado inicial e pureza\n");

    Mundo m = mundo_novo(1);

    CHECK(
        m.vidas == VIDAS_INICIAIS &&
        m.nEnts == 0 &&
        m.nTiros == 0
    );

    CHECK(
        m.resultado == RES_JOGANDO &&
        m.noChao
    );

    CHECK(
        !m.decisaoPortao &&
        m.tempoDecisaoPortao == 0.0f
    );

    Mundo n = mundo_atualiza(
        m,
        vazia(),
        DT
    );

    /*
     * O estado original não pode ser modificado.
     */
    CHECK(
        m.tempo == 0.0f
    );

    CHECK(
        n.tempo > 0.0f
    );
}

/* ============================================================
 * Pulo
 * ============================================================ */

static void testa_pulo(void) {
    printf("pulo\n");

    Mundo m = mundoQuieto();

    Entrada in = vazia();
    in.pula = true;

    m = mundo_atualiza(
        m,
        in,
        DT
    );

    CHECK(
        !m.noChao &&
        m.alt > 0.0f &&
        (m.eventos & EV_PULO)
    );

    float apice = 0.0f;

    for (int i = 0; i < 90; i++) {
        m = mundo_atualiza(
            m,
            vazia(),
            DT
        );

        if (m.alt > apice) {
            apice = m.alt;
        }
    }

    CHECK(
        m.noChao &&
        m.alt == 0.0f
    );

    CHECK(
        apice > 100.0f &&
        apice < 220.0f
    );

    /*
     * Não pode pular uma segunda vez no ar.
     */
    m = mundo_atualiza(
        m,
        in,
        DT
    );

    float velocidade1 = m.velY;

    m = mundo_atualiza(
        m,
        in,
        DT
    );

    CHECK(
        m.velY < velocidade1
    );
}

/* ============================================================
 * Proposições e energia
 * ============================================================ */

static void testa_proposicoes_e_energia(void) {
    printf("proposicoes e energia\n");

    Mundo m = mundoQuieto();

    Entrada in = vazia();
    in.alterna[PROP_ESCUDO] = true;

    m = mundo_atualiza(
        m,
        in,
        DT
    );

    CHECK(
        m.val.v[PROP_ESCUDO]
    );

    m = mundo_atualiza(
        m,
        in,
        DT
    );

    CHECK(
        !m.val.v[PROP_ESCUDO]
    );

    /*
     * Energia baixa e proposições ativas causam sobrecarga.
     */
    m = mundoQuieto();

    m.val.v[PROP_ESCUDO] = true;
    m.val.v[PROP_TURBO] = true;
    m.energia = 0.05f;

    m = mundo_atualiza(
        m,
        vazia(),
        DT
    );

    CHECK(
        regra_conta_ativas(m.val) == 0 &&
        m.sobrecarga > 0.0f &&
        (m.eventos & EV_SOBRECARGA)
    );

    in = vazia();
    in.alterna[PROP_ARMA] = true;

    m = mundo_atualiza(
        m,
        in,
        DT
    );

    CHECK(
        !m.val.v[PROP_ARMA]
    );

    /*
     * Turbo aumenta a velocidade.
     */
    Mundo a = mundoQuieto();
    Mundo b = mundoQuieto();

    b.val.v[PROP_TURBO] = true;

    a = mundo_atualiza(
        a,
        vazia(),
        DT
    );

    b = mundo_atualiza(
        b,
        vazia(),
        DT
    );

    CHECK(
        b.velocidade > a.velocidade
    );
}

/* ============================================================
 * Tiros e inimigos
 * ============================================================ */

static void testa_tiro(void) {
    printf("tiro e inimigos\n");

    Mundo m = mundoQuieto();

    Entrada in = vazia();
    in.atira = true;

    m = mundo_atualiza(
        m,
        in,
        DT
    );

    CHECK(
        m.nTiros == 0 &&
        (m.eventos & EV_SEM_ARMA)
    );

    m = mundoQuieto();
    m.val.v[PROP_ARMA] = true;

    m = mundo_atualiza(
        m,
        in,
        DT
    );

    CHECK(
        m.nTiros == 1 &&
        (m.eventos & EV_TIRO)
    );

    /*
     * O andarilho possui dois pontos de vida.
     */
    m = mundoQuieto();
    m.val.v[PROP_ARMA] = true;

    Entidade inimigo = coloca(
        ENT_ANDARILHO,
        600.0f,
        CHAO_Y - 44.0f,
        40.0f,
        44.0f
    );

    inimigo.vida = 2;

    m.ents[m.nEnts++] = inimigo;

    int pontosIniciais = m.bonus;

    for (
        int i = 0;
        i < 120 && m.nEnts > 0;
        i++
    ) {
        m = mundo_atualiza(
            m,
            in,
            DT
        );
    }

    CHECK(
        m.nEnts == 0 &&
        m.abatidos == 1 &&
        m.bonus > pontosIniciais
    );
}

/* ============================================================
 * Colisões
 * ============================================================ */

static void testa_colisoes(void) {
    printf("colisoes\n");

    /*
     * Sem escudo, o jogador perde uma vida.
     */
    Mundo m = mundoQuieto();

    m.ents[m.nEnts++] = coloca(
        ENT_CACTO,
        JOGADOR_X + 5.0f,
        CHAO_Y - 50.0f,
        26.0f,
        50.0f
    );

    m = mundo_atualiza(
        m,
        vazia(),
        DT
    );

    CHECK(
        m.vidas == VIDAS_INICIAIS - 1 &&
        (m.eventos & EV_DANO) &&
        m.nEnts == 0
    );

    /*
     * Durante a invulnerabilidade, não perde outra vida.
     */
    m.ents[m.nEnts++] = coloca(
        ENT_CACTO,
        JOGADOR_X + 5.0f,
        CHAO_Y - 50.0f,
        26.0f,
        50.0f
    );

    m = mundo_atualiza(
        m,
        vazia(),
        DT
    );

    CHECK(
        m.vidas == VIDAS_INICIAIS - 1
    );

    /*
     * Com escudo, o inimigo é destruído.
     */
    m = mundoQuieto();
    m.val.v[PROP_ESCUDO] = true;

    m.ents[m.nEnts++] = coloca(
        ENT_ANDARILHO,
        JOGADOR_X + 5.0f,
        CHAO_Y - 44.0f,
        40.0f,
        44.0f
    );

    m = mundo_atualiza(
        m,
        vazia(),
        DT
    );

    CHECK(
        m.vidas == VIDAS_INICIAIS &&
        m.nEnts == 0 &&
        m.bonus > 0
    );

    /*
     * Pular por cima do cacto.
     */
    m = mundoQuieto();

    m.ents[m.nEnts++] = coloca(
        ENT_CACTO,
        500.0f,
        CHAO_Y - 50.0f,
        26.0f,
        50.0f
    );

    Entrada in = vazia();

    for (int i = 0; i < 400; i++) {
        in = vazia();

        if (
            m.nEnts > 0 &&
            m.ents[0].viva &&
            m.ents[0].caixa.x < JOGADOR_X + 150.0f &&
            m.noChao
        ) {
            in.pula = true;
        }

        m = mundo_atualiza(
            m,
            in,
            DT
        );
    }

    CHECK(
        m.vidas == VIDAS_INICIAIS
    );

    /*
     * Power-ups.
     */
    m = mundoQuieto();

    m.vidas = 2;
    m.energia = 10.0f;

    m.ents[m.nEnts++] = coloca(
        ENT_PU_VIDA,
        JOGADOR_X,
        CHAO_Y - 40.0f,
        28.0f,
        28.0f
    );

    m.ents[m.nEnts++] = coloca(
        ENT_PU_ENERGIA,
        JOGADOR_X,
        CHAO_Y - 40.0f,
        28.0f,
        28.0f
    );

    m.ents[m.nEnts++] = coloca(
        ENT_PU_TAUTOLOGIA,
        JOGADOR_X,
        CHAO_Y - 40.0f,
        28.0f,
        28.0f
    );

    m = mundo_atualiza(
        m,
        vazia(),
        DT
    );

    CHECK(
        m.vidas == 3 &&
        m.energia > 50.0f &&
        m.tautologia
    );
}

/* ============================================================
 * Desafio lógico dos portões
 * ============================================================ */

static void testa_inicio_desafio_portao(void) {
    printf("inicio do desafio do portao\n");

    int idPeQ = idPorTexto("P & Q");

    CHECK(
        idPeQ >= 0
    );

    Mundo m = mundoQuieto();

    float xPortao =
        JOGADOR_X +
        JOGADOR_L +
        DISTANCIA_DECISAO_PORTAO -
        10.0f;

    Entidade portao =
        criaPortao(idPeQ, xPortao);

    float distanciaAntes = m.distancia;
    float energiaAntes = m.energia;

    m = iniciaDesafio(
        m,
        portao
    );

    CHECK(
        m.decisaoPortao
    );

    CHECK(
        m.tempoDecisaoPortao > 0.0f &&
        m.tempoDecisaoPortao <= TEMPO_DECISAO_PORTAO
    );

    CHECK(
        m.portoesOk == 0 &&
        m.portoesErro == 0
    );

    CHECK(
        quaseIgual(
            m.distancia,
            distanciaAntes,
            0.001f
        )
    );

    CHECK(
        quaseIgual(
            m.energia,
            energiaAntes,
            0.001f
        )
    );
}

static void testa_congelamento_desafio(void) {
    printf("congelamento durante o desafio\n");

    int idPeQ = idPorTexto("P & Q");

    CHECK(
        idPeQ >= 0
    );

    Mundo m = mundoQuieto();

    Entidade portao = criaPortao(
        idPeQ,
        JOGADOR_X + JOGADOR_L + 100.0f
    );

    m = iniciaDesafio(
        m,
        portao
    );

    CHECK(
        m.decisaoPortao
    );

    float distanciaAntes = m.distancia;
    float pontosAntes = m.pontosDist;
    float energiaAntes = m.energia;
    float tempoAntes = m.tempo;
    float xAntes = m.ents[0].caixa.x;
    float tempoDecisaoAntes =
        m.tempoDecisaoPortao;

    m.val.v[PROP_ESCUDO] = true;

    m = mundo_atualiza(
        m,
        vazia(),
        DT
    );

    CHECK(
        m.decisaoPortao
    );

    CHECK(
        quaseIgual(
            m.distancia,
            distanciaAntes,
            0.001f
        )
    );

    CHECK(
        quaseIgual(
            m.pontosDist,
            pontosAntes,
            0.001f
        )
    );

    CHECK(
        quaseIgual(
            m.energia,
            energiaAntes,
            0.001f
        )
    );

    CHECK(
        quaseIgual(
            m.tempo,
            tempoAntes,
            0.001f
        )
    );

    CHECK(
        quaseIgual(
            m.ents[0].caixa.x,
            xAntes,
            0.001f
        )
    );

    CHECK(
        m.tempoDecisaoPortao <
        tempoDecisaoAntes
    );
}

static void testa_alteracao_durante_desafio(void) {
    printf("proposicoes durante o desafio\n");

    int idPeQ = idPorTexto("P & Q");

    CHECK(
        idPeQ >= 0
    );

    Mundo m = mundoQuieto();

    Entidade portao = criaPortao(
        idPeQ,
        JOGADOR_X + JOGADOR_L + 100.0f
    );

    m = iniciaDesafio(
        m,
        portao
    );

    CHECK(
        m.decisaoPortao
    );

    float energiaAntes = m.energia;

    Entrada in = vazia();

    in.alterna[PROP_ESCUDO] = true;
    in.alterna[PROP_ARMA] = true;

    m = mundo_atualiza(
        m,
        in,
        DT
    );

    CHECK(
        m.val.v[PROP_ESCUDO]
    );

    CHECK(
        m.val.v[PROP_ARMA]
    );

    CHECK(
        !m.val.v[PROP_TURBO]
    );

    CHECK(
        quaseIgual(
            m.energia,
            energiaAntes,
            0.001f
        )
    );
}

static void testa_confirmacao_verdadeira(void) {
    printf("confirmacao verdadeira do portao\n");

    int idPeQ = idPorTexto("P & Q");

    CHECK(
        idPeQ >= 0
    );

    Mundo m = mundoQuieto();

    Entidade portao = criaPortao(
        idPeQ,
        JOGADOR_X + JOGADOR_L + 100.0f
    );

    m = iniciaDesafio(
        m,
        portao
    );

    Entrada in = vazia();

    in.alterna[PROP_ESCUDO] = true;
    in.alterna[PROP_ARMA] = true;

    m = mundo_atualiza(
        m,
        in,
        DT
    );

    CHECK(
        m.decisaoPortao
    );

    m = confirmaDesafio(m);

    CHECK(
        !m.decisaoPortao
    );

    CHECK(
        m.portoesOk == 1
    );

    CHECK(
        m.portoesErro == 0
    );

    CHECK(
        m.vidas == VIDAS_INICIAIS
    );

    CHECK(
        m.bonus == PONTOS_PORTAO
    );

    CHECK(
        m.eventos & EV_PORTAO_OK
    );

    CHECK(
        m.nEnts == 1 &&
        m.ents[0].avaliado &&
        m.ents[0].resultado
    );
}

static void testa_confirmacao_falsa(void) {
    printf("confirmacao falsa do portao\n");

    int idPeQ = idPorTexto("P & Q");

    CHECK(
        idPeQ >= 0
    );

    Mundo m = mundoQuieto();

    Entidade portao = criaPortao(
        idPeQ,
        JOGADOR_X + JOGADOR_L + 100.0f
    );

    m = iniciaDesafio(
        m,
        portao
    );

    CHECK(
        !m.val.v[PROP_ESCUDO] &&
        !m.val.v[PROP_ARMA]
    );

    m = confirmaDesafio(m);

    CHECK(
        !m.decisaoPortao
    );

    CHECK(
        m.portoesOk == 0
    );

    CHECK(
        m.portoesErro == 1
    );

    CHECK(
        m.vidas == VIDAS_INICIAIS - 1
    );

    CHECK(
        m.eventos & EV_PORTAO_ERRO
    );

    CHECK(
        m.nEnts == 1 &&
        m.ents[0].avaliado &&
        !m.ents[0].resultado
    );
}

static void testa_confirmacao_automatica(void) {
    printf("confirmacao automatica por tempo\n");

    int idPeQ = idPorTexto("P & Q");

    CHECK(
        idPeQ >= 0
    );

    Mundo m = mundoQuieto();

    Entidade portao = criaPortao(
        idPeQ,
        JOGADOR_X + JOGADOR_L + 100.0f
    );

    m = iniciaDesafio(
        m,
        portao
    );

    /*
     * Deixa P e Q verdadeiras para que a resposta automática
     * seja considerada correta.
     */
    Entrada altera = vazia();

    altera.alterna[PROP_ESCUDO] = true;
    altera.alterna[PROP_ARMA] = true;

    m = mundo_atualiza(
        m,
        altera,
        DT
    );

    int limite =
        (int)(
            TEMPO_DECISAO_PORTAO /
            DT
        ) +
        10;

    for (
        int i = 0;
        i < limite && m.decisaoPortao;
        i++
    ) {
        m = mundo_atualiza(
            m,
            vazia(),
            DT
        );
    }

    CHECK(
        !m.decisaoPortao
    );

    CHECK(
        m.tempoDecisaoPortao == 0.0f
    );

    CHECK(
        m.portoesOk == 1
    );

    CHECK(
        m.portoesErro == 0
    );
}

static void testa_tautologia_portao(void) {
    printf("tautologia no portao\n");

    int idPeQ = idPorTexto("P & Q");

    CHECK(
        idPeQ >= 0
    );

    Mundo m = mundoQuieto();
    m.tautologia = true;

    Entidade portao = criaPortao(
        idPeQ,
        JOGADOR_X + JOGADOR_L + 100.0f
    );

    m.ents[m.nEnts++] = portao;

    m = mundo_atualiza(
        m,
        vazia(),
        DT
    );

    /*
     * A tautologia resolve imediatamente o próximo portão.
     */
    CHECK(
        !m.decisaoPortao
    );

    CHECK(
        m.portoesOk == 1
    );

    CHECK(
        m.portoesErro == 0
    );

    CHECK(
        !m.tautologia
    );

    CHECK(
        m.ents[0].avaliado &&
        m.ents[0].resultado
    );
}

static void testa_portao_avaliado_uma_vez(void) {
    printf("portao avaliado uma unica vez\n");

    int idPeQ = idPorTexto("P & Q");

    CHECK(
        idPeQ >= 0
    );

    Mundo m = mundoQuieto();

    m.val.v[PROP_ESCUDO] = true;
    m.val.v[PROP_ARMA] = true;

    Entidade portao = criaPortao(
        idPeQ,
        JOGADOR_X + JOGADOR_L + 100.0f
    );

    m = iniciaDesafio(
        m,
        portao
    );

    m = confirmaDesafio(m);

    CHECK(
        m.portoesOk == 1
    );

    for (int i = 0; i < 30; i++) {
        m = mundo_atualiza(
            m,
            vazia(),
            DT
        );
    }

    CHECK(
        m.portoesOk == 1 &&
        m.portoesErro == 0
    );
}

static void testa_vitoria_e_derrota_portoes(void) {
    printf("vitoria e derrota pelos portoes\n");

    int idPeQ = idPorTexto("P & Q");

    CHECK(
        idPeQ >= 0
    );

    /*
     * Vitória no último portão necessário.
     */
    Mundo m = mundoQuieto();

    m.portoesOk = META_PORTOES - 1;
    m.val.v[PROP_ESCUDO] = true;
    m.val.v[PROP_ARMA] = true;

    Entidade portao = criaPortao(
        idPeQ,
        JOGADOR_X + JOGADOR_L + 100.0f
    );

    m = iniciaDesafio(
        m,
        portao
    );

    m = confirmaDesafio(m);

    CHECK(
        m.portoesOk == META_PORTOES
    );

    CHECK(
        m.resultado == RES_VITORIA
    );

    /*
     * Derrota ao errar um portão com uma única vida.
     */
    m = mundoQuieto();
    m.vidas = 1;

    portao = criaPortao(
        idPeQ,
        JOGADOR_X + JOGADOR_L + 100.0f
    );

    m = iniciaDesafio(
        m,
        portao
    );

    m = confirmaDesafio(m);

    CHECK(
        m.portoesErro == 1
    );

    CHECK(
        m.vidas == 0
    );

    CHECK(
        m.resultado == RES_DERROTA
    );
}

static void testa_portoes(void) {
    testa_inicio_desafio_portao();
    testa_congelamento_desafio();
    testa_alteracao_durante_desafio();
    testa_confirmacao_verdadeira();
    testa_confirmacao_falsa();
    testa_confirmacao_automatica();
    testa_tautologia_portao();
    testa_portao_avaliado_uma_vez();
    testa_vitoria_e_derrota_portoes();
}

/* ============================================================
 * Simulação longa, robustez e determinismo
 * ============================================================ */

static unsigned fingerprint(const Mundo *m) {
    return
        (unsigned)(m->distancia * 10.0f) * 31u +
        (unsigned)m->nEnts * 17u +
        (unsigned)mundo_pontos(m) +
        (unsigned)m->vidas * 7919u +
        (unsigned)m->portoesGerados * 104729u +
        (unsigned)m->portoesOk * 37u +
        (unsigned)m->portoesErro * 43u +
        (unsigned)m->decisaoPortao * 97u;
}

static void testa_simulacao(void) {
    printf("simulacao longa, robustez e determinismo\n");

    for (
        unsigned semente = 1;
        semente <= 5;
        semente++
    ) {
        Mundo a = mundo_novo(semente);
        Mundo b = mundo_novo(semente);

        /*
         * Mantém vidas suficientes para a simulação continuar
         * mesmo se várias fórmulas forem respondidas incorretamente.
         */
        a.vidas = 99;
        b.vidas = 99;

        for (int i = 0; i < 60 * 120; i++) {
            Entrada in = vazia();

            in.pula =
                (i % 37) == 0;

            in.abaixa =
                (i % 91) < 20;

            in.atira =
                (i % 5) < 3;

            in.alterna[PROP_ESCUDO] =
                (i % 211) == 0;

            in.alterna[PROP_ARMA] =
                (i % 83) == 0;

            in.alterna[PROP_TURBO] =
                (i % 157) == 0;

            /*
             * Confirma rapidamente os desafios para que a
             * simulação cubra vários portões.
             */
            in.confirma =
                a.decisaoPortao;

            a = mundo_atualiza(
                a,
                in,
                DT
            );

            b = mundo_atualiza(
                b,
                in,
                DT
            );

            if (
                a.nEnts > MAX_ENTIDADES ||
                a.nTiros > MAX_TIROS ||
                a.nEnts < 0 ||
                a.nTiros < 0
            ) {
                CHECK(0);
                break;
            }

            if (
                a.energia < 0.0f ||
                a.energia > ENERGIA_MAX
            ) {
                CHECK(0);
                break;
            }

            if (
                a.tempoDecisaoPortao < 0.0f ||
                a.tempoDecisaoPortao >
                    TEMPO_DECISAO_PORTAO
            ) {
                CHECK(0);
                break;
            }

            if (a.resultado != RES_JOGANDO) {
                break;
            }
        }

        CHECK(
            fingerprint(&a) ==
            fingerprint(&b)
        );

        CHECK(
            a.rng == b.rng
        );

        CHECK(
            a.portoesOk ==
            b.portoesOk
        );

        CHECK(
            a.portoesErro ==
            b.portoesErro
        );
    }

    /*
     * Sem fim de jogo, os portões devem surgir normalmente.
     */
    Mundo m = mundo_novo(7);
    m.vidas = 99;

    for (int i = 0; i < 60 * 20; i++) {
        Entrada in = vazia();

        if (m.decisaoPortao) {
            in.confirma = true;
        }

        m = mundo_atualiza(
            m,
            in,
            DT
        );
    }

    CHECK(
        m.portoesGerados >= 1
    );
}

/* ============================================================
 * Execução
 * ============================================================ */

int main(void) {
    testa_regras_puras();
    testa_estado_inicial();
    testa_pulo();
    testa_proposicoes_e_energia();
    testa_tiro();
    testa_colisoes();
    testa_portoes();
    testa_simulacao();

    printf(
        "\n%d verificacoes, %d falhas\n",
        total,
        falhas
    );

    return falhas ? 1 : 0;
}