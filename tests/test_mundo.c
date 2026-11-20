/*
 * Testes das regras do jogo sem abrir janela.
 *
 * Windows:
 *     mingw32-make test-mundo
 *
 * Linux/macOS:
 *     make test-mundo
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "mundo.h"

static int total = 0;
static int falhas = 0;

#define CHECK(cond)                                                       \
    do {                                                                  \
        total++;                                                          \
        if (!(cond)) {                                                    \
            falhas++;                                                     \
            printf("  FALHOU: %s (linha %d)\n", #cond, __LINE__);         \
        }                                                                 \
    } while (0)

#define DT (1.0f / 60.0f)

/* ============================================================
 * Auxiliares
 * ============================================================ */

static Entrada vazia(void) {
    Entrada entrada;

    memset(
        &entrada,
        0,
        sizeof entrada
    );

    return entrada;
}

static bool quaseIgual(
    float a,
    float b,
    float tolerancia
) {
    return
        fabsf(a - b) <=
        tolerancia;
}

static int idPorTexto(const char *texto) {
    char buffer[64];

    for (
        int id = 0;
        id < catalogoTamanho();
        id++
    ) {
        Formula formula =
            catalogoFormula(id);

        formulaParaTexto(
            &formula,
            buffer,
            sizeof buffer
        );

        if (
            strcmp(
                buffer,
                texto
            ) == 0
        ) {
            return id;
        }
    }

    return -1;
}

static Mundo mundoQuieto(void) {
    Mundo m =
        mundo_novo(123);

    m.distProx =
        1e9f;

    m.distProxPortao =
        1e9f;

    return m;
}

static Entidade coloca(
    TipoEnt tipo,
    float x,
    float y,
    float largura,
    float altura
) {
    Entidade entidade;

    memset(
        &entidade,
        0,
        sizeof entidade
    );

    entidade.tipo =
        tipo;

    entidade.caixa =
        (Rectangle){
            x,
            y,
            largura,
            altura
        };

    entidade.y0 =
        y;

    entidade.vida =
        1;

    entidade.viva =
        true;

    return entidade;
}

static Entidade criaPortao(
    int formulaId,
    float x
) {
    Entidade portao =
        coloca(
            ENT_PORTAO,
            x,
            CHAO_Y - 180.0f,
            26.0f,
            180.0f
        );

    portao.formulaId =
        formulaId;

    return portao;
}

static Mundo iniciaDesafio(
    Mundo m,
    Entidade portao
) {
    m.ents[m.nEnts++] =
        portao;

    return
        mundo_atualiza(
            m,
            vazia(),
            DT
        );
}

static Mundo confirmaDesafio(Mundo m) {
    Entrada entrada =
        vazia();

    entrada.confirma =
        true;

    return
        mundo_atualiza(
            m,
            entrada,
            DT
        );
}

static Mundo avancaSegundos(
    Mundo m,
    float segundos
) {
    int quadros =
        (int)(
            segundos /
            DT
        ) +
        2;

    for (
        int i = 0;
        i < quadros;
        i++
    ) {
        m = mundo_atualiza(
            m,
            vazia(),
            DT
        );
    }

    return m;
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

    CHECK(
        regra_energia(
            50.0f,
            0,
            1.0f
        ) >
        50.0f
    );

    CHECK(
        regra_energia(
            50.0f,
            2,
            1.0f
        ) <
        50.0f
    );

    CHECK(
        regra_energia(
            1.0f,
            3,
            10.0f
        ) ==
        0.0f
    );

    CHECK(
        regra_energia(
            99.0f,
            0,
            10.0f
        ) ==
        ENERGIA_MAX
    );

    Valoracao valoracao =
        valoracaoDeMascara(7);

    CHECK(
        regra_conta_ativas(
            valoracao
        ) ==
        3
    );

    CHECK(
        regra_nivel_formula(0) == 0 &&
        regra_nivel_formula(2) == 1 &&
        regra_nivel_formula(99) == 3
    );
}

/* ============================================================
 * Estado inicial
 * ============================================================ */

static void testa_estado_inicial(void) {
    printf("estado inicial e pureza\n");

    Mundo m =
        mundo_novo(1);

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

    CHECK(
        !m.sequenciaVitoria &&
        m.tempoSequenciaVitoria == 0.0f
    );

    Mundo atualizado =
        mundo_atualiza(
            m,
            vazia(),
            DT
        );

    CHECK(
        m.tempo == 0.0f
    );

    CHECK(
        atualizado.tempo > 0.0f
    );
}

/* ============================================================
 * Pulo
 * ============================================================ */

static void testa_pulo(void) {
    printf("pulo\n");

    Mundo m =
        mundoQuieto();

    Entrada entrada =
        vazia();

    entrada.pula =
        true;

    m = mundo_atualiza(
        m,
        entrada,
        DT
    );

    CHECK(
        !m.noChao &&
        m.alt > 0.0f &&
        (m.eventos & EV_PULO)
    );

    float apice =
        0.0f;

    for (int i = 0; i < 90; i++) {
        m = mundo_atualiza(
            m,
            vazia(),
            DT
        );

        if (m.alt > apice) {
            apice =
                m.alt;
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

    m = mundo_atualiza(
        m,
        entrada,
        DT
    );

    float velocidadeAntes =
        m.velY;

    m = mundo_atualiza(
        m,
        entrada,
        DT
    );

    CHECK(
        m.velY <
        velocidadeAntes
    );
}

/* ============================================================
 * Proposições e energia
 * ============================================================ */

static void testa_proposicoes_e_energia(void) {
    printf("proposicoes e energia\n");

    Mundo m =
        mundoQuieto();

    Entrada entrada =
        vazia();

    entrada.alterna[PROP_ESCUDO] =
        true;

    m = mundo_atualiza(
        m,
        entrada,
        DT
    );

    CHECK(
        m.val.v[PROP_ESCUDO]
    );

    m = mundo_atualiza(
        m,
        entrada,
        DT
    );

    CHECK(
        !m.val.v[PROP_ESCUDO]
    );

    m = mundoQuieto();

    m.val.v[PROP_ESCUDO] =
        true;

    m.val.v[PROP_TURBO] =
        true;

    m.energia =
        0.05f;

    m = mundo_atualiza(
        m,
        vazia(),
        DT
    );

    CHECK(
        regra_conta_ativas(
            m.val
        ) == 0
    );

    CHECK(
        m.sobrecarga > 0.0f &&
        (m.eventos & EV_SOBRECARGA)
    );

    entrada =
        vazia();

    entrada.alterna[PROP_ARMA] =
        true;

    m = mundo_atualiza(
        m,
        entrada,
        DT
    );

    CHECK(
        !m.val.v[PROP_ARMA]
    );

    Mundo normal =
        mundoQuieto();

    Mundo turbo =
        mundoQuieto();

    turbo.val.v[PROP_TURBO] =
        true;

    normal = mundo_atualiza(
        normal,
        vazia(),
        DT
    );

    turbo = mundo_atualiza(
        turbo,
        vazia(),
        DT
    );

    CHECK(
        turbo.velocidade >
        normal.velocidade
    );
}

/* ============================================================
 * Tiros
 * ============================================================ */

static void testa_tiro(void) {
    printf("tiro e inimigos\n");

    Mundo m =
        mundoQuieto();

    Entrada entrada =
        vazia();

    entrada.atira =
        true;

    m = mundo_atualiza(
        m,
        entrada,
        DT
    );

    CHECK(
        m.nTiros == 0 &&
        (m.eventos & EV_SEM_ARMA)
    );

    m = mundoQuieto();

    m.val.v[PROP_ARMA] =
        true;

    m = mundo_atualiza(
        m,
        entrada,
        DT
    );

    CHECK(
        m.nTiros == 1 &&
        (m.eventos & EV_TIRO)
    );

    m = mundoQuieto();

    m.val.v[PROP_ARMA] =
        true;

    Entidade inimigo =
        coloca(
            ENT_ANDARILHO,
            600.0f,
            CHAO_Y - 44.0f,
            40.0f,
            44.0f
        );

    inimigo.vida =
        2;

    m.ents[m.nEnts++] =
        inimigo;

    for (
        int i = 0;
        i < 120 && m.nEnts > 0;
        i++
    ) {
        m = mundo_atualiza(
            m,
            entrada,
            DT
        );
    }

    CHECK(
        m.nEnts == 0 &&
        m.abatidos == 1
    );

    CHECK(
        m.bonus >=
        regra_pontos_inimigo(
            ENT_ANDARILHO
        )
    );
}

/* ============================================================
 * Colisões e power-ups
 * ============================================================ */

static void testa_colisoes(void) {
    printf("colisoes\n");

    Mundo m =
        mundoQuieto();

    m.ents[m.nEnts++] =
        coloca(
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
        m.vidas ==
        VIDAS_INICIAIS - 1
    );

    CHECK(
        (m.eventos & EV_DANO) &&
        m.nEnts == 0
    );

    m.ents[m.nEnts++] =
        coloca(
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
        m.vidas ==
        VIDAS_INICIAIS - 1
    );

    m = mundoQuieto();

    m.val.v[PROP_ESCUDO] =
        true;

    m.ents[m.nEnts++] =
        coloca(
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

    m = mundoQuieto();

    m.vidas =
        2;

    m.energia =
        10.0f;

    m.ents[m.nEnts++] =
        coloca(
            ENT_PU_VIDA,
            JOGADOR_X,
            CHAO_Y - 40.0f,
            28.0f,
            28.0f
        );

    m.ents[m.nEnts++] =
        coloca(
            ENT_PU_ENERGIA,
            JOGADOR_X,
            CHAO_Y - 40.0f,
            28.0f,
            28.0f
        );

    m.ents[m.nEnts++] =
        coloca(
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
 * Portões
 * ============================================================ */

static void testa_inicio_desafio_portao(void) {
    printf("inicio do desafio do portao\n");

    int formulaId =
        idPorTexto("P & Q");

    CHECK(
        formulaId >= 0
    );

    Mundo m =
        mundoQuieto();

    float xPortao =
        JOGADOR_X +
        JOGADOR_L +
        DISTANCIA_DECISAO_PORTAO -
        10.0f;

    float distanciaAntes =
        m.distancia;

    float energiaAntes =
        m.energia;

    m = iniciaDesafio(
        m,
        criaPortao(
            formulaId,
            xPortao
        )
    );

    CHECK(
        m.decisaoPortao
    );

    CHECK(
        m.tempoDecisaoPortao > 0.0f &&
        m.tempoDecisaoPortao <=
            TEMPO_DECISAO_PORTAO
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

    int formulaId =
        idPorTexto("P & Q");

    CHECK(
        formulaId >= 0
    );

    Mundo m =
        mundoQuieto();

    m = iniciaDesafio(
        m,
        criaPortao(
            formulaId,
            JOGADOR_X +
                JOGADOR_L +
                100.0f
        )
    );

    float distanciaAntes =
        m.distancia;

    float pontosAntes =
        m.pontosDist;

    float energiaAntes =
        m.energia;

    float tempoAntes =
        m.tempo;

    float xAntes =
        m.ents[0].caixa.x;

    float contadorAntes =
        m.tempoDecisaoPortao;

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
        contadorAntes
    );
}

static void testa_respostas_portao(void) {
    printf("respostas dos portoes\n");

    int formulaId =
        idPorTexto("P & Q");

    CHECK(
        formulaId >= 0
    );

    /*
     * Resposta verdadeira.
     */
    Mundo correta =
        mundoQuieto();

    correta = iniciaDesafio(
        correta,
        criaPortao(
            formulaId,
            JOGADOR_X +
                JOGADOR_L +
                100.0f
        )
    );

    Entrada altera =
        vazia();

    altera.alterna[PROP_ESCUDO] =
        true;

    altera.alterna[PROP_ARMA] =
        true;

    float energiaAntes =
        correta.energia;

    correta = mundo_atualiza(
        correta,
        altera,
        DT
    );

    CHECK(
        correta.val.v[PROP_ESCUDO] &&
        correta.val.v[PROP_ARMA]
    );

    CHECK(
        quaseIgual(
            correta.energia,
            energiaAntes,
            0.001f
        )
    );

    correta =
        confirmaDesafio(correta);

    CHECK(
        correta.portoesOk == 1 &&
        correta.portoesErro == 0
    );

    CHECK(
        correta.bonus ==
        PONTOS_PORTAO
    );

    CHECK(
        correta.eventos &
        EV_PORTAO_OK
    );

    /*
     * Resposta falsa.
     */
    Mundo falsa =
        mundoQuieto();

    falsa = iniciaDesafio(
        falsa,
        criaPortao(
            formulaId,
            JOGADOR_X +
                JOGADOR_L +
                100.0f
        )
    );

    falsa =
        confirmaDesafio(falsa);

    CHECK(
        falsa.portoesOk == 0 &&
        falsa.portoesErro == 1
    );

    CHECK(
        falsa.vidas ==
        VIDAS_INICIAIS - 1
    );

    CHECK(
        falsa.eventos &
        EV_PORTAO_ERRO
    );
}

static void testa_confirmacao_automatica(void) {
    printf("confirmacao automatica por tempo\n");

    int formulaId =
        idPorTexto("P & Q");

    CHECK(
        formulaId >= 0
    );

    Mundo m =
        mundoQuieto();

    m = iniciaDesafio(
        m,
        criaPortao(
            formulaId,
            JOGADOR_X +
                JOGADOR_L +
                100.0f
        )
    );

    Entrada altera =
        vazia();

    altera.alterna[PROP_ESCUDO] =
        true;

    altera.alterna[PROP_ARMA] =
        true;

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
        i < limite &&
            m.decisaoPortao;
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
        m.tempoDecisaoPortao ==
        0.0f
    );

    CHECK(
        m.portoesOk == 1 &&
        m.portoesErro == 0
    );
}

static void testa_tautologia(void) {
    printf("tautologia no portao\n");

    int formulaId =
        idPorTexto("P & Q");

    CHECK(
        formulaId >= 0
    );

    Mundo m =
        mundoQuieto();

    m.tautologia =
        true;

    m.ents[m.nEnts++] =
        criaPortao(
            formulaId,
            JOGADOR_X +
                JOGADOR_L +
                100.0f
        );

    m = mundo_atualiza(
        m,
        vazia(),
        DT
    );

    CHECK(
        !m.decisaoPortao
    );

    CHECK(
        m.portoesOk == 1 &&
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

/* ============================================================
 * Sequência de vitória
 * ============================================================ */

static Mundo criaSequenciaVitoria(
    int formulaId
) {
    Mundo m =
        mundoQuieto();

    m.portoesOk =
        META_PORTOES - 1;

    m.portoesGerados =
        META_PORTOES;

    m.val.v[PROP_ESCUDO] =
        true;

    m.val.v[PROP_ARMA] =
        true;

    m.val.v[PROP_TURBO] =
        true;

    m = iniciaDesafio(
        m,
        criaPortao(
            formulaId,
            JOGADOR_X +
                JOGADOR_L +
                100.0f
        )
    );

    return
        confirmaDesafio(m);
}

static void testa_inicio_sequencia_vitoria(void) {
    printf("inicio da sequencia de vitoria\n");

    int formulaId =
        idPorTexto("P & Q");

    CHECK(
        formulaId >= 0
    );

    Mundo m =
        criaSequenciaVitoria(
            formulaId
        );

    CHECK(
        m.portoesOk ==
        META_PORTOES
    );

    CHECK(
        m.sequenciaVitoria
    );

    CHECK(
        m.resultado ==
        RES_JOGANDO
    );

    CHECK(
        m.tempoSequenciaVitoria ==
        0.0f
    );

    CHECK(
        regra_conta_ativas(
            m.val
        ) == 0
    );

    CHECK(
        !m.tautologia &&
        m.sobrecarga == 0.0f
    );

    CHECK(
        m.distProx > 1e8f &&
        m.distProxPortao > 1e8f
    );
}

static void testa_movimento_sequencia_vitoria(void) {
    printf("movimento da sequencia de vitoria\n");

    int formulaId =
        idPorTexto("P & Q");

    CHECK(
        formulaId >= 0
    );

    Mundo m =
        criaSequenciaVitoria(
            formulaId
        );

    float xPortaoAntes =
        m.ents[0].caixa.x;

    float distanciaAntes =
        m.distancia;

    float pontosAntes =
        m.pontosDist;

    int entidadesAntes =
        m.nEnts;

    Entrada entrada =
        vazia();

    entrada.pula =
        true;

    entrada.atira =
        true;

    entrada.alterna[PROP_ESCUDO] =
        true;

    entrada.alterna[PROP_ARMA] =
        true;

    entrada.alterna[PROP_TURBO] =
        true;

    m = mundo_atualiza(
        m,
        entrada,
        DT
    );

    CHECK(
        m.sequenciaVitoria &&
        m.resultado == RES_JOGANDO
    );

    CHECK(
        m.tempoSequenciaVitoria >
        0.0f
    );

    CHECK(
        m.distancia >
        distanciaAntes
    );

    CHECK(
        m.ents[0].caixa.x <
        xPortaoAntes
    );

    CHECK(
        quaseIgual(
            m.pontosDist,
            pontosAntes,
            0.001f
        )
    );

    CHECK(
        regra_conta_ativas(
            m.val
        ) == 0
    );

    CHECK(
        m.nTiros == 0
    );

    CHECK(
        m.nEnts <=
        entidadesAntes
    );
}

static void testa_desaceleracao_vitoria(void) {
    printf("desaceleracao da sequencia de vitoria\n");

    int formulaId =
        idPorTexto("P & Q");

    CHECK(
        formulaId >= 0
    );

    Mundo m =
        criaSequenciaVitoria(
            formulaId
        );

    m = avancaSegundos(
        m,
        TEMPO_CORRIDA_VITORIA -
            0.2f
    );

    CHECK(
        m.sequenciaVitoria &&
        m.resultado == RES_JOGANDO
    );

    float velocidadeCorrida =
        m.velocidade;

    m = avancaSegundos(
        m,
        0.9f
    );

    CHECK(
        m.sequenciaVitoria
    );

    CHECK(
        m.tempoSequenciaVitoria >
        TEMPO_CORRIDA_VITORIA
    );

    CHECK(
        m.velocidade <
        velocidadeCorrida
    );
}

static void testa_fim_sequencia_vitoria(void) {
    printf("fim da sequencia de vitoria\n");

    int formulaId =
        idPorTexto("P & Q");

    CHECK(
        formulaId >= 0
    );

    Mundo m =
        criaSequenciaVitoria(
            formulaId
        );

    m = avancaSegundos(
        m,
        TEMPO_SEQUENCIA_VITORIA +
            0.2f
    );

    CHECK(
        m.resultado ==
        RES_VITORIA
    );

    CHECK(
        m.sequenciaVitoria
    );

    CHECK(
        quaseIgual(
            m.tempoSequenciaVitoria,
            TEMPO_SEQUENCIA_VITORIA,
            0.001f
        )
    );

    CHECK(
        m.velocidade ==
        0.0f
    );

    Mundo depois =
        mundo_atualiza(
            m,
            vazia(),
            DT
        );

    CHECK(
        quaseIgual(
            depois.distancia,
            m.distancia,
            0.001f
        )
    );
}

static void testa_derrota_portao(void) {
    printf("derrota por erro no portao\n");

    int formulaId =
        idPorTexto("P & Q");

    CHECK(
        formulaId >= 0
    );

    Mundo m =
        mundoQuieto();

    m.vidas =
        1;

    m = iniciaDesafio(
        m,
        criaPortao(
            formulaId,
            JOGADOR_X +
                JOGADOR_L +
                100.0f
        )
    );

    m =
        confirmaDesafio(m);

    CHECK(
        m.portoesErro == 1
    );

    CHECK(
        m.vidas == 0
    );

    CHECK(
        m.resultado ==
        RES_DERROTA
    );

    CHECK(
        !m.sequenciaVitoria
    );
}

static void testa_portoes(void) {
    testa_inicio_desafio_portao();
    testa_congelamento_desafio();
    testa_respostas_portao();
    testa_confirmacao_automatica();
    testa_tautologia();

    testa_inicio_sequencia_vitoria();
    testa_movimento_sequencia_vitoria();
    testa_desaceleracao_vitoria();
    testa_fim_sequencia_vitoria();
    testa_derrota_portao();
}

/* ============================================================
 * Determinismo e robustez
 * ============================================================ */

static unsigned fingerprint(
    const Mundo *m
) {
    return
        (unsigned)(
            m->distancia *
            10.0f
        ) *
        31u +
        (unsigned)m->nEnts *
        17u +
        (unsigned)mundo_pontos(m) +
        (unsigned)m->vidas *
        7919u +
        (unsigned)m->portoesGerados *
        104729u +
        (unsigned)m->portoesOk *
        37u +
        (unsigned)m->portoesErro *
        43u +
        (unsigned)m->decisaoPortao *
        97u +
        (unsigned)m->sequenciaVitoria *
        193u +
        (unsigned)(
            m->tempoSequenciaVitoria *
            100.0f
        ) *
        389u;
}

static void testa_simulacao(void) {
    printf("simulacao longa, robustez e determinismo\n");

    for (
        unsigned semente = 1;
        semente <= 5;
        semente++
    ) {
        Mundo a =
            mundo_novo(semente);

        Mundo b =
            mundo_novo(semente);

        a.vidas =
            99;

        b.vidas =
            99;

        for (
            int i = 0;
            i < 60 * 120;
            i++
        ) {
            Entrada entrada =
                vazia();

            entrada.pula =
                (i % 37) == 0;

            entrada.abaixa =
                (i % 91) < 20;

            entrada.atira =
                (i % 5) < 3;

            entrada.alterna[PROP_ESCUDO] =
                (i % 211) == 0;

            entrada.alterna[PROP_ARMA] =
                (i % 83) == 0;

            entrada.alterna[PROP_TURBO] =
                (i % 157) == 0;

            entrada.confirma =
                a.decisaoPortao;

            a = mundo_atualiza(
                a,
                entrada,
                DT
            );

            b = mundo_atualiza(
                b,
                entrada,
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

            if (
                a.tempoSequenciaVitoria < 0.0f ||
                a.tempoSequenciaVitoria >
                    TEMPO_SEQUENCIA_VITORIA
            ) {
                CHECK(0);
                break;
            }

            if (
                a.resultado !=
                RES_JOGANDO
            ) {
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

        CHECK(
            a.sequenciaVitoria ==
            b.sequenciaVitoria
        );
    }

    Mundo m =
        mundo_novo(7);

    m.vidas =
        99;

    for (
        int i = 0;
        i < 60 * 20;
        i++
    ) {
        Entrada entrada =
            vazia();

        if (m.decisaoPortao) {
            entrada.confirma =
                true;
        }

        m = mundo_atualiza(
            m,
            entrada,
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

    return
        falhas
            ? 1
            : 0;
}