#include "mundo.h"
#include "funcional.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* ============================================================
 * Números aleatórios determinísticos (xorshift32)
 *
 * A semente mora dentro do Mundo:
 * mesma semente + mesmas entradas = exatamente o mesmo jogo.
 * ============================================================ */

static uint32_t xorshift(uint32_t s) {
    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    return s;
}

static float sorteia01(Mundo *m) {
    m->rng = xorshift(m->rng);
    return (float)(m->rng & 0xFFFFFFu) / 16777216.0f;
}

static float sorteiaEntre(Mundo *m, float a, float b) {
    return a + (b - a) * sorteia01(m);
}

static int sorteiaInt(Mundo *m, int n) {
    int r = (int)(sorteia01(m) * (float)n);
    return r >= n ? n - 1 : r;
}

/* ============================================================
 * Regras PURAS
 *
 * Recebem os dados de entrada e devolvem um resultado,
 * sem alterar o estado externo.
 * ============================================================ */

float regra_velocidade_base(float tempo) {
    float v = VEL_INICIAL + 7.0f * tempo;
    return v > VEL_MAX ? VEL_MAX : v;
}

float regra_energia(float energia, int ativas, float dt) {
    float taxa = (ativas == 0)
        ? REGEN_ENERGIA
        : -CUSTO_ENERGIA * (float)ativas;

    float e = energia + taxa * dt;

    if (e < 0.0f) {
        e = 0.0f;
    }

    if (e > ENERGIA_MAX) {
        e = ENERGIA_MAX;
    }

    return e;
}

int regra_conta_ativas(Valoracao v) {
    int n = 0;

    for (int i = 0; i < NUM_PROPS; i++) {
        if (v.v[i]) {
            n++;
        }
    }

    return n;
}

int regra_pontos_inimigo(TipoEnt t) {
    switch (t) {
        case ENT_CACTO:
            return 10;

        case ENT_ANDARILHO:
            return 30;

        case ENT_VOADOR:
            return 40;

        default:
            return 0;
    }
}

int regra_nivel_formula(int portoesGerados) {
    int n = portoesGerados / 2;
    return n > 3 ? 3 : n;
}

int mundo_pontos(const Mundo *m) {
    return (int)m->pontosDist + m->bonus;
}

Rectangle mundo_caixa_jogador(const Mundo *m) {
    float h = m->abaixado
        ? JOGADOR_A_ABAIXADO
        : JOGADOR_A;

    return (Rectangle){
        JOGADOR_X,
        CHAO_Y - m->alt - h,
        JOGADOR_L,
        h
    };
}

/*
 * A caixa de colisão é um pouco menor que o desenho.
 * Isso torna as colisões mais justas para o jogador.
 */
static Rectangle caixaDano(const Mundo *m) {
    Rectangle r = mundo_caixa_jogador(m);

    return (Rectangle){
        r.x + 5.0f,
        r.y + 4.0f,
        r.width - 10.0f,
        r.height - 4.0f
    };
}

/* ============================================================
 * Callbacks para map / filter / fold
 * ============================================================ */

typedef struct {
    float scroll;
    float dt;
} CtxMov;

/* MAP: move cada entidade */
static void moveEntidade(void *p, void *c) {
    Entidade *e = (Entidade *)p;
    const CtxMov *ctx = (const CtxMov *)c;

    e->caixa.x -= (ctx->scroll + e->velExtra) * ctx->dt;
    e->anim += ctx->dt;

    if (e->tipo == ENT_VOADOR) {
        e->caixa.y =
            e->y0 +
            sinf(e->anim * 5.0f) * 10.0f;
    } else if (e->tipo >= ENT_PU_ENERGIA) {
        e->caixa.y =
            e->y0 +
            sinf(e->anim * 4.0f) * 6.0f;
    }
}

/* MAP: move cada tiro */
static void moveTiro(void *p, void *c) {
    Tiro *t = (Tiro *)p;
    const CtxMov *ctx = (const CtxMov *)c;

    t->caixa.x += VEL_TIRO * ctx->dt;
}

/* FILTER: entidades que continuam no jogo */
static bool entidadeAtiva(const void *p, const void *c) {
    const Entidade *e = (const Entidade *)p;
    (void)c;

    return
        e->viva &&
        (e->caixa.x + e->caixa.width) > -60.0f;
}

/* FILTER: tiros que continuam no jogo */
static bool tiroAtivo(const void *p, const void *c) {
    const Tiro *t = (const Tiro *)p;
    (void)c;

    return
        t->vivo &&
        t->caixa.x < LARGURA_TELA + 40.0f;
}

/* FOLD: identifica os portões vivos */
static bool ehPortaoVivo(const void *p, const void *c) {
    const Entidade *e = (const Entidade *)p;
    (void)c;

    return
        e->viva &&
        e->tipo == ENT_PORTAO;
}

static bool ehInimigo(const Entidade *e) {
    return
        e->viva &&
        (
            e->tipo == ENT_ANDARILHO ||
            e->tipo == ENT_VOADOR
        );
}

/* ============================================================
 * Mensagens temporárias
 * ============================================================ */

static void mensagem(Mundo *m, const char *txt) {
    snprintf(
        m->msg,
        sizeof m->msg,
        "%s",
        txt
    );

    m->msgTempo = 1.8f;
}

/* ============================================================
 * Busca de portões
 * ============================================================ */

/*
 * Procura o portão vivo, ainda não avaliado, com o menor X.
 *
 * Não guardamos ponteiros permanentes para as entidades porque
 * o vetor pode ser reorganizado pela operação de filter.
 */
static Entidade *buscaPortaoPendente(Mundo *m) {
    Entidade *melhor = NULL;

    for (int i = 0; i < m->nEnts; i++) {
        Entidade *e = &m->ents[i];

        if (
            e->viva &&
            e->tipo == ENT_PORTAO &&
            !e->avaliado
        ) {
            if (
                melhor == NULL ||
                e->caixa.x < melhor->caixa.x
            ) {
                melhor = e;
            }
        }
    }

    return melhor;
}

/*
 * Distância horizontal entre a frente do jogador e o começo
 * do portão.
 */
static float distanciaAtePortao(const Entidade *portao) {
    float frenteJogador = JOGADOR_X + JOGADOR_L;
    return portao->caixa.x - frenteJogador;
}

/* ============================================================
 * Tiros contra inimigos
 * ============================================================ */

/*
 * MAP sobre os tiros:
 * cada tiro procura um inimigo para acertar.
 */
static void tiroContraInimigos(void *p, void *c) {
    Tiro *t = (Tiro *)p;
    Mundo *m = (Mundo *)c;

    if (!t->vivo) {
        return;
    }

    for (int i = 0; i < m->nEnts; i++) {
        Entidade *e = &m->ents[i];

        if (
            ehInimigo(e) &&
            CheckCollisionRecs(t->caixa, e->caixa)
        ) {
            t->vivo = false;
            e->vida--;

            if (e->vida <= 0) {
                e->viva = false;
                m->bonus += regra_pontos_inimigo(e->tipo);
                m->abatidos++;
                m->eventos |= EV_ABATE;
            }

            break;
        }
    }
}

/* ============================================================
 * Jogador contra entidades
 * ============================================================ */

/*
 * MAP sobre as entidades:
 * verifica se o jogador encostou em alguma entidade.
 */
static void jogadorContraEntidade(void *p, void *c) {
    Entidade *e = (Entidade *)p;
    Mundo *m = (Mundo *)c;

    if (
        !e->viva ||
        e->tipo == ENT_PORTAO
    ) {
        return;
    }

    if (!CheckCollisionRecs(caixaDano(m), e->caixa)) {
        return;
    }

    switch (e->tipo) {
        case ENT_PU_ENERGIA:
            m->energia += 50.0f;

            if (m->energia > ENERGIA_MAX) {
                m->energia = ENERGIA_MAX;
            }

            e->viva = false;
            m->eventos |= EV_POWERUP;
            mensagem(m, "+ENERGIA");
            break;

        case ENT_PU_VIDA:
            if (m->vidas < VIDAS_MAX) {
                m->vidas++;
            }

            e->viva = false;
            m->eventos |= EV_POWERUP;
            mensagem(m, "+1 VIDA");
            break;

        case ENT_PU_TAUTOLOGIA:
            m->tautologia = true;
            e->viva = false;
            m->eventos |= EV_POWERUP;
            mensagem(
                m,
                "TAUTOLOGIA! O proximo portao abre."
            );
            break;

        default:
            /*
             * Cacto, andarilho ou voador.
             */
            if (m->val.v[PROP_ESCUDO]) {
                /*
                 * P verdadeira:
                 * o escudo destrói o inimigo.
                 */
                e->viva = false;
                m->bonus += regra_pontos_inimigo(e->tipo);
                m->eventos |= EV_ABATE;
            } else if (m->invulneravel <= 0.0f) {
                /*
                 * P falsa:
                 * o jogador recebe dano.
                 */
                m->vidas--;
                m->invulneravel = TEMPO_INVULNERAVEL;
                e->viva = false;
                m->eventos |= EV_DANO;
                mensagem(m, "Ai! -1 vida");
            }
            break;
    }
}

/* ============================================================
 * Desafio lógico do portão
 * ============================================================ */

/*
 * Avalia definitivamente um portão.
 *
 * Esta função só deve ser chamada quando:
 * - o jogador confirmar com Enter;
 * - o contador chegar a zero;
 * - houver uma tautologia ativa.
 */
static void finalizaPortao(Mundo *m, Entidade *portao) {
    if (
        portao == NULL ||
        portao->tipo != ENT_PORTAO ||
        !portao->viva ||
        portao->avaliado
    ) {
        m->decisaoPortao = false;
        m->tempoDecisaoPortao = 0.0f;
        return;
    }

    Formula f = catalogoFormula(portao->formulaId);

    bool ok =
        m->tautologia ||
        avalia(&f, m->val);

    /*
     * A tautologia vale somente para um portão.
     */
    m->tautologia = false;

    portao->avaliado = true;
    portao->resultado = ok;

    /*
     * Encerra o modo de decisão antes de registrar o resultado.
     */
    m->decisaoPortao = false;
    m->tempoDecisaoPortao = 0.0f;

    if (ok) {
        m->portoesOk++;
        m->bonus += PONTOS_PORTAO;
        m->eventos |= EV_PORTAO_OK;
        mensagem(m, "PORTAO ABERTO! +100");
    } else {
        m->portoesErro++;
        m->vidas--;
        m->invulneravel = TEMPO_INVULNERAVEL;
        m->eventos |= EV_PORTAO_ERRO;
        mensagem(m, "FORMULA FALSA! -1 vida");
    }
}

/*
 * Verifica se um portão entrou na área de decisão.
 *
 * O desafio começa quando o portão está a uma distância igual
 * ou menor que DISTANCIA_DECISAO_PORTAO da frente do jogador.
 */
static bool tentaIniciarDecisaoPortao(Mundo *m) {
    if (m->decisaoPortao) {
        return true;
    }

    Entidade *portao = buscaPortaoPendente(m);

    if (portao == NULL) {
        return false;
    }

    float distancia = distanciaAtePortao(portao);

    if (distancia > DISTANCIA_DECISAO_PORTAO) {
        return false;
    }

    /*
     * O power-up de tautologia torna o próximo portão sempre
     * verdadeiro. Nesse caso, não é necessário obrigar o jogador
     * a esperar cinco segundos ou confirmar uma resposta.
     */
    if (m->tautologia) {
        finalizaPortao(m, portao);
        return false;
    }

    m->decisaoPortao = true;
    m->tempoDecisaoPortao = TEMPO_DECISAO_PORTAO;
    m->abaixado = false;

    /*
     * Se o jogador estava no ar, mantemos a posição congelada
     * durante o desafio. O movimento volta normalmente depois.
     */
    mensagem(m, "DESAFIO LOGICO! Ajuste P, Q e R.");

    return true;
}

/*
 * Atualiza apenas os controles permitidos durante o desafio.
 *
 * Durante a decisão:
 * - P, Q e R podem ser alterados;
 * - a energia não é consumida nem regenerada;
 * - pulo, tiro e movimento ficam congelados;
 * - a sobrecarga continua contando o próprio tempo.
 */
static void passoDecisaoPortao(
    Mundo *m,
    Entrada in,
    float dt
) {
    Entidade *portao = buscaPortaoPendente(m);

    if (portao == NULL) {
        m->decisaoPortao = false;
        m->tempoDecisaoPortao = 0.0f;
        return;
    }

    /*
     * A sobrecarga ainda impede temporariamente a alteração das
     * proposições, mas seu contador continua diminuindo.
     */
    if (m->sobrecarga > 0.0f) {
        m->sobrecarga -= dt;

        if (m->sobrecarga < 0.0f) {
            m->sobrecarga = 0.0f;
        }
    } else {
        for (int i = 0; i < NUM_PROPS; i++) {
            if (in.alterna[i]) {
                m->val.v[i] = !m->val.v[i];
            }
        }
    }

    m->tempoDecisaoPortao -= dt;

    if (m->tempoDecisaoPortao < 0.0f) {
        m->tempoDecisaoPortao = 0.0f;
    }

    /*
     * A resposta é confirmada quando:
     * - Enter é pressionado;
     * - o contador chega a zero.
     */
    if (
        in.confirma ||
        m->tempoDecisaoPortao <= 0.0f
    ) {
        finalizaPortao(m, portao);
    }
}

/* ============================================================
 * Geração de objetos
 * ============================================================ */

static Entidade novaEnt(
    TipoEnt tipo,
    float x,
    float y,
    float w,
    float h
) {
    Entidade e;
    memset(&e, 0, sizeof e);

    e.tipo = tipo;
    e.caixa = (Rectangle){ x, y, w, h };
    e.y0 = y;
    e.vida = 1;
    e.viva = true;

    return e;
}

static void adiciona(Mundo *m, Entidade e) {
    if (m->nEnts < MAX_ENTIDADES) {
        m->ents[m->nEnts++] = e;
    }
}

static int sorteiaFormulaId(Mundo *m) {
    int nivel = regra_nivel_formula(m->portoesGerados);

    int cand[64];
    int nc = 0;

    for (
        int id = 0;
        id < catalogoTamanho() && nc < 64;
        id++
    ) {
        int nv = catalogoNivel(id);

        if (
            nv == nivel ||
            nv == nivel - 1
        ) {
            cand[nc++] = id;
        }
    }

    return cand[sorteiaInt(m, nc)];
}

static void geraPortao(Mundo *m) {
    Entidade e = novaEnt(
        ENT_PORTAO,
        LARGURA_TELA + 40.0f,
        CHAO_Y - 180.0f,
        26.0f,
        180.0f
    );

    e.formulaId = sorteiaFormulaId(m);

    adiciona(m, e);

    m->portoesGerados++;

    m->distProxPortao =
        sorteiaEntre(m, 8.0f, 11.0f) *
        VEL_INICIAL *
        1.3f;
}

static void geraObjeto(Mundo *m) {
    float x = LARGURA_TELA + 40.0f;
    float r = sorteia01(m);

    if (r < 0.12f) {
        /*
         * Power-up.
         */
        float s = sorteia01(m);
        TipoEnt t = ENT_PU_TAUTOLOGIA;

        if (s < 0.45f) {
            t = ENT_PU_ENERGIA;
        } else if (
            s < 0.65f &&
            m->vidas < VIDAS_MAX
        ) {
            t = ENT_PU_VIDA;
        }

        float y =
            sorteia01(m) < 0.5f
                ? CHAO_Y - 50.0f
                : CHAO_Y - 150.0f;

        adiciona(
            m,
            novaEnt(
                t,
                x,
                y,
                28.0f,
                28.0f
            )
        );
    } else if (r < 0.50f) {
        /*
         * Cacto simples ou duplo.
         */
        bool duplo = sorteia01(m) < 0.3f;
        float w = duplo ? 52.0f : 26.0f;
        float h = sorteiaEntre(m, 44.0f, 70.0f);

        adiciona(
            m,
            novaEnt(
                ENT_CACTO,
                x,
                CHAO_Y - h,
                w,
                h
            )
        );
    } else if (r < 0.78f) {
        /*
         * Andarilho.
         */
        Entidade e = novaEnt(
            ENT_ANDARILHO,
            x,
            CHAO_Y - 44.0f,
            40.0f,
            44.0f
        );

        e.vida = 2;
        e.velExtra = 110.0f;

        adiciona(m, e);
    } else {
        /*
         * Voador em altura baixa ou alta.
         */
        float y =
            sorteia01(m) < 0.6f
                ? CHAO_Y - 70.0f
                : CHAO_Y - 150.0f;

        Entidade e = novaEnt(
            ENT_VOADOR,
            x,
            y,
            44.0f,
            26.0f
        );

        e.velExtra = 80.0f;

        adiciona(m, e);
    }
}

/* ============================================================
 * Passos do update
 *
 * Cada função possui uma única responsabilidade.
 * ============================================================ */

static void passoProposicoes(
    Mundo *m,
    Entrada in,
    float dt
) {
    if (m->sobrecarga > 0.0f) {
        m->sobrecarga -= dt;

        if (m->sobrecarga < 0.0f) {
            m->sobrecarga = 0.0f;
        }
    } else {
        for (int i = 0; i < NUM_PROPS; i++) {
            if (in.alterna[i]) {
                m->val.v[i] = !m->val.v[i];
            }
        }
    }

    int ativas = regra_conta_ativas(m->val);

    m->energia = regra_energia(
        m->energia,
        ativas,
        dt
    );

    /*
     * Se a energia acaba, todas as proposições voltam para F
     * e o jogador entra em sobrecarga.
     */
    if (
        ativas > 0 &&
        m->energia <= 0.0f
    ) {
        m->val = (Valoracao){
            .v = { false, false, false }
        };

        m->sobrecarga = TEMPO_SOBRECARGA;
        m->eventos |= EV_SOBRECARGA;

        mensagem(
            m,
            "SOBRECARGA! Tudo voltou a ser FALSO"
        );
    }
}

static void passoJogador(
    Mundo *m,
    Entrada in,
    float dt
) {
    m->abaixado =
        in.abaixa &&
        m->noChao;

    if (
        in.pula &&
        m->noChao
    ) {
        m->velY = VEL_PULO;
        m->noChao = false;
        m->abaixado = false;
        m->eventos |= EV_PULO;
    }

    if (!m->noChao) {
        float g =
            GRAVIDADE +
            (in.abaixa ? GRAVIDADE_QUEDA : 0.0f);

        m->velY -= g * dt;
        m->alt += m->velY * dt;

        if (m->alt <= 0.0f) {
            m->alt = 0.0f;
            m->velY = 0.0f;
            m->noChao = true;
        }
    }
}

static void passoTiro(
    Mundo *m,
    Entrada in,
    float dt
) {
    if (m->cooldownTiro > 0.0f) {
        m->cooldownTiro -= dt;
    }

    if (!in.atira) {
        return;
    }

    if (!m->val.v[PROP_ARMA]) {
        m->eventos |= EV_SEM_ARMA;

        if (m->msgTempo <= 0.0f) {
            mensagem(
                m,
                "Arma desligada: Q esta FALSA (tecla 2)"
            );
        }

        return;
    }

    if (
        m->cooldownTiro <= 0.0f &&
        m->nTiros < MAX_TIROS
    ) {
        Rectangle j = mundo_caixa_jogador(m);

        m->tiros[m->nTiros++] = (Tiro){
            {
                JOGADOR_X + JOGADOR_L,
                j.y + j.height * 0.4f,
                18.0f,
                6.0f
            },
            true
        };

        m->cooldownTiro = COOLDOWN_TIRO;
        m->eventos |= EV_TIRO;
    }
}

static void passoSpawn(
    Mundo *m,
    float scroll,
    float dt
) {
    int portoes = conta(
        m->ents,
        m->nEnts,
        sizeof(Entidade),
        ehPortaoVivo,
        NULL
    );

    m->distProxPortao -= scroll * dt;

    if (m->distProx > 0.0f) {
        m->distProx -= scroll * dt;
    }

    /*
     * Evita inimigos muito próximos do portão.
     */
    bool zonaQuieta =
        portoes > 0 ||
        m->distProxPortao < 1.0f * scroll;

    if (
        m->distProx <= 0.0f &&
        !zonaQuieta
    ) {
        geraObjeto(m);

        m->distProx =
            sorteiaEntre(m, 1.1f, 2.1f) *
            m->velocidade;
    }

    if (
        m->distProxPortao <= 0.0f &&
        portoes == 0
    ) {
        geraPortao(m);
    }
}

/* ============================================================
 * API pública
 * ============================================================ */

Mundo mundo_novo(uint32_t semente) {
    Mundo m;
    memset(&m, 0, sizeof m);

    m.rng =
        semente
            ? semente
            : 0x9E3779B9u;

    m.noChao = true;
    m.vidas = VIDAS_INICIAIS;
    m.energia = ENERGIA_MAX;
    m.velocidade = VEL_INICIAL;

    m.distProx =
        1.6f *
        VEL_INICIAL;

    m.distProxPortao =
        6.0f *
        VEL_INICIAL;

    m.decisaoPortao = false;
    m.tempoDecisaoPortao = 0.0f;
    m.resultado = RES_JOGANDO;

    return m;
}

Mundo mundo_atualiza(
    Mundo m,
    Entrada in,
    float dt
) {
    if (
        m.resultado != RES_JOGANDO ||
        dt <= 0.0f
    ) {
        return m;
    }

    /*
     * Robustez contra quadros excessivamente longos.
     */
    if (dt > 0.1f) {
        dt = 0.1f;
    }

    m.eventos = 0;

    if (m.msgTempo > 0.0f) {
        m.msgTempo -= dt;

        if (m.msgTempo < 0.0f) {
            m.msgTempo = 0.0f;
        }
    }

    if (m.invulneravel > 0.0f) {
        m.invulneravel -= dt;

        if (m.invulneravel < 0.0f) {
            m.invulneravel = 0.0f;
        }
    }

    /*
     * Se o desafio já estiver ativo, somente a lógica específica
     * da decisão será atualizada.
     *
     * Não avançamos:
     * - tempo geral;
     * - entidades;
     * - tiros;
     * - distância;
     * - pontuação;
     * - spawn;
     * - energia.
     */
    if (m.decisaoPortao) {
        passoDecisaoPortao(&m, in, dt);

        if (m.vidas <= 0) {
            m.vidas = 0;
            m.resultado = RES_DERROTA;
        } else if (m.portoesOk >= META_PORTOES) {
            m.resultado = RES_VITORIA;
        }

        return m;
    }

    /*
     * Detecta se o portão já entrou na área de decisão antes de
     * continuar o movimento normal do quadro.
     */
    if (tentaIniciarDecisaoPortao(&m)) {
        passoDecisaoPortao(&m, in, dt);
        return m;
    }

    /*
     * Caso uma tautologia tenha avaliado o portão imediatamente,
     * verificamos vitória antes de continuar.
     */
    if (m.portoesOk >= META_PORTOES) {
        m.resultado = RES_VITORIA;
        return m;
    }

    m.tempo += dt;

    passoProposicoes(&m, in, dt);
    passoJogador(&m, in, dt);

    m.velocidade =
        regra_velocidade_base(m.tempo) *
        (
            m.val.v[PROP_TURBO]
                ? BONUS_TURBO
                : 1.0f
        );

    float scroll = m.velocidade;

    passoTiro(&m, in, dt);

    /*
     * MAP: movimenta as coleções.
     */
    CtxMov ctx = {
        scroll,
        dt
    };

    mapa(
        m.ents,
        m.nEnts,
        sizeof(Entidade),
        moveEntidade,
        &ctx
    );

    mapa(
        m.tiros,
        m.nTiros,
        sizeof(Tiro),
        moveTiro,
        &ctx
    );

    /*
     * Colisões e regras.
     */
    mapa(
        m.tiros,
        m.nTiros,
        sizeof(Tiro),
        tiroContraInimigos,
        &m
    );

    mapa(
        m.ents,
        m.nEnts,
        sizeof(Entidade),
        jogadorContraEntidade,
        &m
    );

    /*
     * FILTER:
     * remove entidades mortas, entidades que saíram da tela
     * e tiros que não estão mais ativos.
     */
    m.nEnts = filtra(
        m.ents,
        m.nEnts,
        sizeof(Entidade),
        entidadeAtiva,
        NULL
    );

    m.nTiros = filtra(
        m.tiros,
        m.nTiros,
        sizeof(Tiro),
        tiroAtivo,
        NULL
    );

    passoSpawn(
        &m,
        scroll,
        dt
    );

    /*
     * Pontuação por distância.
     * Com turbo ativo, os pontos por distância valem o dobro.
     */
    m.distancia += scroll * dt;

    m.pontosDist +=
        scroll *
        dt *
        0.1f *
        (
            m.val.v[PROP_TURBO]
                ? 2.0f
                : 1.0f
        );

    /*
     * Depois do movimento deste quadro, verifica novamente se o
     * portão acabou de entrar na área de decisão.
     *
     * A decisão será exibida imediatamente no próximo desenho.
     */
    tentaIniciarDecisaoPortao(&m);

    /*
     * Condições de fim de jogo.
     */
    if (m.vidas <= 0) {
        m.vidas = 0;
        m.resultado = RES_DERROTA;
    } else if (m.portoesOk >= META_PORTOES) {
        m.resultado = RES_VITORIA;
    }

    return m;
}

const Entidade *mundo_proximo_portao(const Mundo *m) {
    const Entidade *melhor = NULL;

    for (int i = 0; i < m->nEnts; i++) {
        const Entidade *e = &m->ents[i];

        if (
            e->viva &&
            e->tipo == ENT_PORTAO &&
            !e->avaliado
        ) {
            if (
                melhor == NULL ||
                e->caixa.x < melhor->caixa.x
            ) {
                melhor = e;
            }
        }
    }

    return melhor;
}