#include "mundo.h"
#include "funcional.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* ============================================================
 * Números aleatórios determinísticos
 * ============================================================ */

static uint32_t xorshift(uint32_t s) {
    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;

    return s;
}

static float sorteia01(Mundo *m) {
    m->rng = xorshift(m->rng);

    return
        (float)(m->rng & 0xFFFFFFu) /
        16777216.0f;
}

static float sorteiaEntre(
    Mundo *m,
    float a,
    float b
) {
    return
        a +
        (b - a) *
        sorteia01(m);
}

static int sorteiaInt(
    Mundo *m,
    int n
) {
    int r =
        (int)(
            sorteia01(m) *
            (float)n
        );

    return
        r >= n
            ? n - 1
            : r;
}

/* ============================================================
 * Regras puras
 * ============================================================ */

float regra_velocidade_base(float tempo) {
    float velocidade =
        VEL_INICIAL +
        7.0f * tempo;

    return
        velocidade > VEL_MAX
            ? VEL_MAX
            : velocidade;
}

float regra_energia(
    float energia,
    int ativas,
    float dt
) {
    float taxa =
        ativas == 0
            ? REGEN_ENERGIA
            : -CUSTO_ENERGIA *
                (float)ativas;

    float novaEnergia =
        energia +
        taxa * dt;

    if (novaEnergia < 0.0f) {
        novaEnergia = 0.0f;
    }

    if (novaEnergia > ENERGIA_MAX) {
        novaEnergia = ENERGIA_MAX;
    }

    return novaEnergia;
}

int regra_conta_ativas(Valoracao v) {
    int quantidade = 0;

    for (int i = 0; i < NUM_PROPS; i++) {
        if (v.v[i]) {
            quantidade++;
        }
    }

    return quantidade;
}

int regra_pontos_inimigo(TipoEnt tipo) {
    switch (tipo) {
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
    int nivel =
        portoesGerados / 2;

    return
        nivel > 3
            ? 3
            : nivel;
}

int mundo_pontos(const Mundo *m) {
    return
        (int)m->pontosDist +
        m->bonus;
}

Rectangle mundo_caixa_jogador(const Mundo *m) {
    float altura =
        m->abaixado
            ? JOGADOR_A_ABAIXADO
            : JOGADOR_A;

    return (Rectangle){
        JOGADOR_X,
        CHAO_Y - m->alt - altura,
        JOGADOR_L,
        altura
    };
}

static Rectangle caixaDano(const Mundo *m) {
    Rectangle caixa =
        mundo_caixa_jogador(m);

    return (Rectangle){
        caixa.x + 5.0f,
        caixa.y + 4.0f,
        caixa.width - 10.0f,
        caixa.height - 4.0f
    };
}

/* ============================================================
 * Callbacks de map, filter e fold
 * ============================================================ */

typedef struct {
    float scroll;
    float dt;
} CtxMov;

static void moveEntidade(
    void *elemento,
    void *contexto
) {
    Entidade *entidade =
        (Entidade *)elemento;

    const CtxMov *movimento =
        (const CtxMov *)contexto;

    entidade->caixa.x -=
        (
            movimento->scroll +
            entidade->velExtra
        ) *
        movimento->dt;

    entidade->anim +=
        movimento->dt;

    if (entidade->tipo == ENT_VOADOR) {
        entidade->caixa.y =
            entidade->y0 +
            sinf(
                entidade->anim *
                5.0f
            ) *
            10.0f;
    } else if (
        entidade->tipo >=
        ENT_PU_ENERGIA
    ) {
        entidade->caixa.y =
            entidade->y0 +
            sinf(
                entidade->anim *
                4.0f
            ) *
            6.0f;
    }
}

static void moveTiro(
    void *elemento,
    void *contexto
) {
    Tiro *tiro =
        (Tiro *)elemento;

    const CtxMov *movimento =
        (const CtxMov *)contexto;

    tiro->caixa.x +=
        VEL_TIRO *
        movimento->dt;
}

static bool entidadeAtiva(
    const void *elemento,
    const void *contexto
) {
    const Entidade *entidade =
        (const Entidade *)elemento;

    (void)contexto;

    return
        entidade->viva &&
        (
            entidade->caixa.x +
            entidade->caixa.width
        ) >
        -60.0f;
}

static bool tiroAtivo(
    const void *elemento,
    const void *contexto
) {
    const Tiro *tiro =
        (const Tiro *)elemento;

    (void)contexto;

    return
        tiro->vivo &&
        tiro->caixa.x <
            LARGURA_TELA + 40.0f;
}

static bool ehPortaoVivo(
    const void *elemento,
    const void *contexto
) {
    const Entidade *entidade =
        (const Entidade *)elemento;

    (void)contexto;

    return
        entidade->viva &&
        entidade->tipo ==
            ENT_PORTAO;
}

static bool ehInimigo(
    const Entidade *entidade
) {
    return
        entidade->viva &&
        (
            entidade->tipo ==
                ENT_ANDARILHO ||
            entidade->tipo ==
                ENT_VOADOR
        );
}

/* ============================================================
 * Mensagens
 * ============================================================ */

static void mensagem(
    Mundo *m,
    const char *texto
) {
    snprintf(
        m->msg,
        sizeof m->msg,
        "%s",
        texto
    );

    m->msgTempo = 1.8f;
}

/* ============================================================
 * Busca de portões
 * ============================================================ */

static Entidade *buscaPortaoPendente(
    Mundo *m
) {
    Entidade *melhor = NULL;

    for (int i = 0; i < m->nEnts; i++) {
        Entidade *entidade =
            &m->ents[i];

        if (
            entidade->viva &&
            entidade->tipo ==
                ENT_PORTAO &&
            !entidade->avaliado
        ) {
            if (
                melhor == NULL ||
                entidade->caixa.x <
                    melhor->caixa.x
            ) {
                melhor =
                    entidade;
            }
        }
    }

    return melhor;
}

static float distanciaAtePortao(
    const Entidade *portao
) {
    float frenteJogador =
        JOGADOR_X +
        JOGADOR_L;

    return
        portao->caixa.x -
        frenteJogador;
}

/* ============================================================
 * Tiros contra inimigos
 * ============================================================ */

static void tiroContraInimigos(
    void *elemento,
    void *contexto
) {
    Tiro *tiro =
        (Tiro *)elemento;

    Mundo *m =
        (Mundo *)contexto;

    if (!tiro->vivo) {
        return;
    }

    for (int i = 0; i < m->nEnts; i++) {
        Entidade *entidade =
            &m->ents[i];

        if (
            ehInimigo(entidade) &&
            CheckCollisionRecs(
                tiro->caixa,
                entidade->caixa
            )
        ) {
            tiro->vivo = false;
            entidade->vida--;

            if (entidade->vida <= 0) {
                entidade->viva =
                    false;

                m->bonus +=
                    regra_pontos_inimigo(
                        entidade->tipo
                    );

                m->abatidos++;

                m->eventos |=
                    EV_ABATE;
            }

            break;
        }
    }
}

/* ============================================================
 * Jogador contra entidades
 * ============================================================ */

static void jogadorContraEntidade(
    void *elemento,
    void *contexto
) {
    Entidade *entidade =
        (Entidade *)elemento;

    Mundo *m =
        (Mundo *)contexto;

    if (
        !entidade->viva ||
        entidade->tipo ==
            ENT_PORTAO
    ) {
        return;
    }

    if (
        !CheckCollisionRecs(
            caixaDano(m),
            entidade->caixa
        )
    ) {
        return;
    }

    switch (entidade->tipo) {
        case ENT_PU_ENERGIA:
            m->energia += 50.0f;

            if (
                m->energia >
                ENERGIA_MAX
            ) {
                m->energia =
                    ENERGIA_MAX;
            }

            entidade->viva =
                false;

            m->eventos |=
                EV_POWERUP;

            mensagem(
                m,
                "+ENERGIA"
            );
            break;

        case ENT_PU_VIDA:
            if (
                m->vidas <
                VIDAS_MAX
            ) {
                m->vidas++;
            }

            entidade->viva =
                false;

            m->eventos |=
                EV_POWERUP;

            mensagem(
                m,
                "+1 VIDA"
            );
            break;

        case ENT_PU_TAUTOLOGIA:
            m->tautologia =
                true;

            entidade->viva =
                false;

            m->eventos |=
                EV_POWERUP;

            mensagem(
                m,
                "TAUTOLOGIA! O proximo portao abre."
            );
            break;

        default:
            if (
                m->val.v[
                    PROP_ESCUDO
                ]
            ) {
                entidade->viva =
                    false;

                m->bonus +=
                    regra_pontos_inimigo(
                        entidade->tipo
                    );

                m->eventos |=
                    EV_ABATE;
            } else if (
                m->invulneravel <=
                0.0f
            ) {
                m->vidas--;

                m->invulneravel =
                    TEMPO_INVULNERAVEL;

                entidade->viva =
                    false;

                m->eventos |=
                    EV_DANO;

                mensagem(
                    m,
                    "Ai! -1 vida"
                );
            }
            break;
    }
}

/* ============================================================
 * Sequência final de vitória
 * ============================================================ */

static void iniciaSequenciaVitoria(
    Mundo *m
) {
    if (m->sequenciaVitoria) {
        return;
    }

    m->sequenciaVitoria =
        true;

    m->tempoSequenciaVitoria =
        0.0f;

    /*
     * Os poderes são desligados para a corrida final ficar
     * totalmente controlada pela sequência.
     */
    m->val = (Valoracao){
        .v = {
            false,
            false,
            false
        }
    };

    m->tautologia =
        false;

    m->sobrecarga =
        0.0f;

    m->abaixado =
        false;

    m->cooldownTiro =
        0.0f;

    /*
     * Não são criados novos objetos durante a sequência.
     */
    m->distProx =
        1e9f;

    m->distProxPortao =
        1e9f;

    mensagem(
        m,
        "TODOS OS PORTOES FORAM ABERTOS!"
    );
}

/*
 * Calcula a velocidade da sequência final.
 *
 * Durante a primeira etapa, a corrida continua normalmente.
 * Na comemoração, a velocidade diminui de forma gradual.
 */
static float velocidadeSequenciaVitoria(
    const Mundo *m
) {
    if (
        m->tempoSequenciaVitoria <=
        TEMPO_CORRIDA_VITORIA
    ) {
        float velocidade =
            regra_velocidade_base(
                m->tempo
            );

        if (
            velocidade <
            VEL_INICIAL
        ) {
            velocidade =
                VEL_INICIAL;
        }

        return velocidade;
    }

    float tempoComemoracao =
        m->tempoSequenciaVitoria -
        TEMPO_CORRIDA_VITORIA;

    float progresso =
        tempoComemoracao /
        TEMPO_COMEMORACAO_VITORIA;

    if (progresso < 0.0f) {
        progresso = 0.0f;
    }

    if (progresso > 1.0f) {
        progresso = 1.0f;
    }

    /*
     * Smoothstep deixa a desaceleração menos abrupta.
     */
    float suavizado =
        progresso *
        progresso *
        (
            3.0f -
            2.0f * progresso
        );

    return
        VEL_INICIAL *
        (
            1.0f -
            0.82f * suavizado
        );
}

/*
 * Atualiza somente o necessário durante a sequência final.
 */
static void passoSequenciaVitoria(
    Mundo *m,
    float dt
) {
    m->tempoSequenciaVitoria +=
        dt;

    m->tempo +=
        dt;

    /*
     * Caso a decisão do último portão tenha acontecido durante
     * um salto, o jogador volta ao chão naturalmente.
     */
    m->abaixado =
        false;

    if (!m->noChao) {
        m->velY -=
            GRAVIDADE *
            dt;

        m->alt +=
            m->velY *
            dt;

        if (m->alt <= 0.0f) {
            m->alt =
                0.0f;

            m->velY =
                0.0f;

            m->noChao =
                true;
        }
    }

    m->velocidade =
        velocidadeSequenciaVitoria(m);

    float scroll =
        m->velocidade;

    CtxMov contexto = {
        scroll,
        dt
    };

    /*
     * Entidades continuam saindo pela esquerda.
     *
     * Não há colisão durante a sequência final.
     */
    mapa(
        m->ents,
        m->nEnts,
        sizeof(Entidade),
        moveEntidade,
        &contexto
    );

    mapa(
        m->tiros,
        m->nTiros,
        sizeof(Tiro),
        moveTiro,
        &contexto
    );

    m->nEnts = filtra(
        m->ents,
        m->nEnts,
        sizeof(Entidade),
        entidadeAtiva,
        NULL
    );

    m->nTiros = filtra(
        m->tiros,
        m->nTiros,
        sizeof(Tiro),
        tiroAtivo,
        NULL
    );

    /*
     * A distância continua avançando para manter o paralaxe.
     * Não são concedidos novos pontos por distância.
     */
    m->distancia +=
        scroll *
        dt;

    if (
        m->tempoSequenciaVitoria >=
        TEMPO_SEQUENCIA_VITORIA
    ) {
        m->tempoSequenciaVitoria =
            TEMPO_SEQUENCIA_VITORIA;

        m->velocidade =
            0.0f;

        m->resultado =
            RES_VITORIA;
    }
}

/* ============================================================
 * Desafio lógico do portão
 * ============================================================ */

static void finalizaPortao(
    Mundo *m,
    Entidade *portao
) {
    if (
        portao == NULL ||
        portao->tipo !=
            ENT_PORTAO ||
        !portao->viva ||
        portao->avaliado
    ) {
        m->decisaoPortao =
            false;

        m->tempoDecisaoPortao =
            0.0f;

        return;
    }

    Formula formula =
        catalogoFormula(
            portao->formulaId
        );

    bool correto =
        m->tautologia ||
        avalia(
            &formula,
            m->val
        );

    m->tautologia =
        false;

    portao->avaliado =
        true;

    portao->resultado =
        correto;

    m->decisaoPortao =
        false;

    m->tempoDecisaoPortao =
        0.0f;

    if (correto) {
        m->portoesOk++;

        m->bonus +=
            PONTOS_PORTAO;

        m->eventos |=
            EV_PORTAO_OK;

        mensagem(
            m,
            "PORTAO ABERTO! +100"
        );

        /*
         * O oitavo portão inicia a sequência final, mas ainda
         * não muda o resultado para RES_VITORIA.
         */
        if (
            m->portoesOk >=
            META_PORTOES
        ) {
            iniciaSequenciaVitoria(m);
        }
    } else {
        m->portoesErro++;

        m->vidas--;

        m->invulneravel =
            TEMPO_INVULNERAVEL;

        m->eventos |=
            EV_PORTAO_ERRO;

        mensagem(
            m,
            "FORMULA FALSA! -1 vida"
        );
    }
}

static bool tentaIniciarDecisaoPortao(
    Mundo *m
) {
    if (
        m->decisaoPortao ||
        m->sequenciaVitoria
    ) {
        return
            m->decisaoPortao;
    }

    Entidade *portao =
        buscaPortaoPendente(m);

    if (portao == NULL) {
        return false;
    }

    float distancia =
        distanciaAtePortao(
            portao
        );

    if (
        distancia >
        DISTANCIA_DECISAO_PORTAO
    ) {
        return false;
    }

    if (m->tautologia) {
        finalizaPortao(
            m,
            portao
        );

        return false;
    }

    m->decisaoPortao =
        true;

    m->tempoDecisaoPortao =
        TEMPO_DECISAO_PORTAO;

    m->abaixado =
        false;

    mensagem(
        m,
        "DESAFIO LOGICO! Ajuste P, Q e R."
    );

    return true;
}

static void passoDecisaoPortao(
    Mundo *m,
    Entrada entrada,
    float dt
) {
    Entidade *portao =
        buscaPortaoPendente(m);

    if (portao == NULL) {
        m->decisaoPortao =
            false;

        m->tempoDecisaoPortao =
            0.0f;

        return;
    }

    if (m->sobrecarga > 0.0f) {
        m->sobrecarga -=
            dt;

        if (m->sobrecarga < 0.0f) {
            m->sobrecarga =
                0.0f;
        }
    } else {
        for (
            int i = 0;
            i < NUM_PROPS;
            i++
        ) {
            if (
                entrada.alterna[i]
            ) {
                m->val.v[i] =
                    !m->val.v[i];
            }
        }
    }

    m->tempoDecisaoPortao -=
        dt;

    if (
        m->tempoDecisaoPortao <
        0.0f
    ) {
        m->tempoDecisaoPortao =
            0.0f;
    }

    if (
        entrada.confirma ||
        m->tempoDecisaoPortao <=
            0.0f
    ) {
        finalizaPortao(
            m,
            portao
        );
    }
}

/* ============================================================
 * Geração de objetos
 * ============================================================ */

static Entidade novaEnt(
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

static void adiciona(
    Mundo *m,
    Entidade entidade
) {
    if (
        m->nEnts <
        MAX_ENTIDADES
    ) {
        m->ents[m->nEnts++] =
            entidade;
    }
}

static int sorteiaFormulaId(
    Mundo *m
) {
    int nivel =
        regra_nivel_formula(
            m->portoesGerados
        );

    int candidatos[64];
    int quantidade = 0;

    for (
        int id = 0;
        id < catalogoTamanho() &&
            quantidade < 64;
        id++
    ) {
        int nivelFormula =
            catalogoNivel(id);

        if (
            nivelFormula == nivel ||
            nivelFormula ==
                nivel - 1
        ) {
            candidatos[quantidade++] =
                id;
        }
    }

    return
        candidatos[
            sorteiaInt(
                m,
                quantidade
            )
        ];
}

static void geraPortao(Mundo *m) {
    Entidade entidade =
        novaEnt(
            ENT_PORTAO,
            LARGURA_TELA + 40.0f,
            CHAO_Y - 180.0f,
            26.0f,
            180.0f
        );

    entidade.formulaId =
        sorteiaFormulaId(m);

    adiciona(
        m,
        entidade
    );

    m->portoesGerados++;

    m->distProxPortao =
        sorteiaEntre(
            m,
            8.0f,
            11.0f
        ) *
        VEL_INICIAL *
        1.3f;
}

static void geraObjeto(Mundo *m) {
    float x =
        LARGURA_TELA +
        40.0f;

    float sorteio =
        sorteia01(m);

    if (sorteio < 0.12f) {
        float tipoSorteado =
            sorteia01(m);

        TipoEnt tipo =
            ENT_PU_TAUTOLOGIA;

        if (tipoSorteado < 0.45f) {
            tipo =
                ENT_PU_ENERGIA;
        } else if (
            tipoSorteado < 0.65f &&
            m->vidas < VIDAS_MAX
        ) {
            tipo =
                ENT_PU_VIDA;
        }

        float y =
            sorteia01(m) < 0.5f
                ? CHAO_Y - 50.0f
                : CHAO_Y - 150.0f;

        adiciona(
            m,
            novaEnt(
                tipo,
                x,
                y,
                28.0f,
                28.0f
            )
        );
    } else if (sorteio < 0.50f) {
        bool duplo =
            sorteia01(m) <
            0.3f;

        float largura =
            duplo
                ? 52.0f
                : 26.0f;

        float altura =
            sorteiaEntre(
                m,
                44.0f,
                70.0f
            );

        adiciona(
            m,
            novaEnt(
                ENT_CACTO,
                x,
                CHAO_Y - altura,
                largura,
                altura
            )
        );
    } else if (sorteio < 0.78f) {
        Entidade entidade =
            novaEnt(
                ENT_ANDARILHO,
                x,
                CHAO_Y - 44.0f,
                40.0f,
                44.0f
            );

        entidade.vida =
            2;

        entidade.velExtra =
            110.0f;

        adiciona(
            m,
            entidade
        );
    } else {
        float y =
            sorteia01(m) < 0.6f
                ? CHAO_Y - 70.0f
                : CHAO_Y - 150.0f;

        Entidade entidade =
            novaEnt(
                ENT_VOADOR,
                x,
                y,
                44.0f,
                26.0f
            );

        entidade.velExtra =
            80.0f;

        adiciona(
            m,
            entidade
        );
    }
}

/* ============================================================
 * Passos normais da atualização
 * ============================================================ */

static void passoProposicoes(
    Mundo *m,
    Entrada entrada,
    float dt
) {
    if (m->sobrecarga > 0.0f) {
        m->sobrecarga -=
            dt;

        if (m->sobrecarga < 0.0f) {
            m->sobrecarga =
                0.0f;
        }
    } else {
        for (
            int i = 0;
            i < NUM_PROPS;
            i++
        ) {
            if (
                entrada.alterna[i]
            ) {
                m->val.v[i] =
                    !m->val.v[i];
            }
        }
    }

    int ativas =
        regra_conta_ativas(
            m->val
        );

    m->energia =
        regra_energia(
            m->energia,
            ativas,
            dt
        );

    if (
        ativas > 0 &&
        m->energia <= 0.0f
    ) {
        m->val =
            (Valoracao){
                .v = {
                    false,
                    false,
                    false
                }
            };

        m->sobrecarga =
            TEMPO_SOBRECARGA;

        m->eventos |=
            EV_SOBRECARGA;

        mensagem(
            m,
            "SOBRECARGA! Tudo voltou a ser FALSO"
        );
    }
}

static void passoJogador(
    Mundo *m,
    Entrada entrada,
    float dt
) {
    m->abaixado =
        entrada.abaixa &&
        m->noChao;

    if (
        entrada.pula &&
        m->noChao
    ) {
        m->velY =
            VEL_PULO;

        m->noChao =
            false;

        m->abaixado =
            false;

        m->eventos |=
            EV_PULO;
    }

    if (!m->noChao) {
        float gravidade =
            GRAVIDADE +
            (
                entrada.abaixa
                    ? GRAVIDADE_QUEDA
                    : 0.0f
            );

        m->velY -=
            gravidade *
            dt;

        m->alt +=
            m->velY *
            dt;

        if (m->alt <= 0.0f) {
            m->alt =
                0.0f;

            m->velY =
                0.0f;

            m->noChao =
                true;
        }
    }
}

static void passoTiro(
    Mundo *m,
    Entrada entrada,
    float dt
) {
    if (m->cooldownTiro > 0.0f) {
        m->cooldownTiro -=
            dt;
    }

    if (!entrada.atira) {
        return;
    }

    if (
        !m->val.v[
            PROP_ARMA
        ]
    ) {
        m->eventos |=
            EV_SEM_ARMA;

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
        Rectangle jogador =
            mundo_caixa_jogador(m);

        m->tiros[m->nTiros++] =
            (Tiro){
                {
                    JOGADOR_X +
                        JOGADOR_L,
                    jogador.y +
                        jogador.height *
                        0.4f,
                    18.0f,
                    6.0f
                },
                true
            };

        m->cooldownTiro =
            COOLDOWN_TIRO;

        m->eventos |=
            EV_TIRO;
    }
}

static void passoSpawn(
    Mundo *m,
    float scroll,
    float dt
) {
    /*
     * Nenhum novo elemento pode ser criado na sequência final.
     */
    if (m->sequenciaVitoria) {
        return;
    }

    int portoes =
        conta(
            m->ents,
            m->nEnts,
            sizeof(Entidade),
            ehPortaoVivo,
            NULL
        );

    m->distProxPortao -=
        scroll *
        dt;

    if (m->distProx > 0.0f) {
        m->distProx -=
            scroll *
            dt;
    }

    bool zonaQuieta =
        portoes > 0 ||
        m->distProxPortao <
            1.0f * scroll;

    if (
        m->distProx <= 0.0f &&
        !zonaQuieta
    ) {
        geraObjeto(m);

        m->distProx =
            sorteiaEntre(
                m,
                1.1f,
                2.1f
            ) *
            m->velocidade;
    }

    if (
        m->distProxPortao <= 0.0f &&
        portoes == 0 &&
        m->portoesGerados <
            META_PORTOES
    ) {
        geraPortao(m);
    }
}

/* ============================================================
 * API pública
 * ============================================================ */

Mundo mundo_novo(uint32_t semente) {
    Mundo m;

    memset(
        &m,
        0,
        sizeof m
    );

    m.rng =
        semente
            ? semente
            : 0x9E3779B9u;

    m.noChao =
        true;

    m.vidas =
        VIDAS_INICIAIS;

    m.energia =
        ENERGIA_MAX;

    m.velocidade =
        VEL_INICIAL;

    m.distProx =
        1.6f *
        VEL_INICIAL;

    m.distProxPortao =
        6.0f *
        VEL_INICIAL;

    m.decisaoPortao =
        false;

    m.tempoDecisaoPortao =
        0.0f;

    m.sequenciaVitoria =
        false;

    m.tempoSequenciaVitoria =
        0.0f;

    m.resultado =
        RES_JOGANDO;

    return m;
}

Mundo mundo_atualiza(
    Mundo m,
    Entrada entrada,
    float dt
) {
    if (
        m.resultado !=
            RES_JOGANDO ||
        dt <= 0.0f
    ) {
        return m;
    }

    if (dt > 0.1f) {
        dt = 0.1f;
    }

    m.eventos =
        0;

    if (m.msgTempo > 0.0f) {
        m.msgTempo -=
            dt;

        if (m.msgTempo < 0.0f) {
            m.msgTempo =
                0.0f;
        }
    }

    if (m.invulneravel > 0.0f) {
        m.invulneravel -=
            dt;

        if (m.invulneravel < 0.0f) {
            m.invulneravel =
                0.0f;
        }
    }

    /*
     * A sequência final possui prioridade sobre todas as outras
     * atualizações.
     */
    if (m.sequenciaVitoria) {
        passoSequenciaVitoria(
            &m,
            dt
        );

        return m;
    }

    /*
     * Durante o desafio do portão, somente P, Q, R e o contador
     * são atualizados.
     */
    if (m.decisaoPortao) {
        passoDecisaoPortao(
            &m,
            entrada,
            dt
        );

        /*
         * O oitavo portão pode ter iniciado a sequência.
         * A sequência começa a avançar no próximo quadro.
         */
        if (m.sequenciaVitoria) {
            return m;
        }

        if (m.vidas <= 0) {
            m.vidas =
                0;

            m.resultado =
                RES_DERROTA;
        }

        return m;
    }

    /*
     * Verifica se algum portão entrou na área de decisão.
     */
    if (
        tentaIniciarDecisaoPortao(
            &m
        )
    ) {
        passoDecisaoPortao(
            &m,
            entrada,
            dt
        );

        return m;
    }

    /*
     * A tautologia pode avaliar o oitavo portão sem abrir o
     * painel de decisão.
     */
    if (m.sequenciaVitoria) {
        return m;
    }

    m.tempo +=
        dt;

    passoProposicoes(
        &m,
        entrada,
        dt
    );

    passoJogador(
        &m,
        entrada,
        dt
    );

    m.velocidade =
        regra_velocidade_base(
            m.tempo
        ) *
        (
            m.val.v[
                PROP_TURBO
            ]
                ? BONUS_TURBO
                : 1.0f
        );

    float scroll =
        m.velocidade;

    passoTiro(
        &m,
        entrada,
        dt
    );

    CtxMov contexto = {
        scroll,
        dt
    };

    mapa(
        m.ents,
        m.nEnts,
        sizeof(Entidade),
        moveEntidade,
        &contexto
    );

    mapa(
        m.tiros,
        m.nTiros,
        sizeof(Tiro),
        moveTiro,
        &contexto
    );

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

    m.nEnts =
        filtra(
            m.ents,
            m.nEnts,
            sizeof(Entidade),
            entidadeAtiva,
            NULL
        );

    m.nTiros =
        filtra(
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

    m.distancia +=
        scroll *
        dt;

    m.pontosDist +=
        scroll *
        dt *
        0.1f *
        (
            m.val.v[
                PROP_TURBO
            ]
                ? 2.0f
                : 1.0f
        );

    tentaIniciarDecisaoPortao(
        &m
    );

    if (m.vidas <= 0) {
        m.vidas =
            0;

        m.resultado =
            RES_DERROTA;
    }

    /*
     * A vitória não é mais definida apenas por portoesOk.
     * Ela será definida ao final de passoSequenciaVitoria().
     */

    return m;
}

const Entidade *mundo_proximo_portao(
    const Mundo *m
) {
    const Entidade *melhor =
        NULL;

    for (int i = 0; i < m->nEnts; i++) {
        const Entidade *entidade =
            &m->ents[i];

        if (
            entidade->viva &&
            entidade->tipo ==
                ENT_PORTAO &&
            !entidade->avaliado
        ) {
            if (
                melhor == NULL ||
                entidade->caixa.x <
                    melhor->caixa.x
            ) {
                melhor =
                    entidade;
            }
        }
    }

    return melhor;
}