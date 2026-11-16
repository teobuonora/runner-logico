#include "render.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* ---------- cores ---------- */

static const Color COR_FUNDO = {
    247, 247, 242, 255
};

static const Color COR_PAINEL = {
    38, 38, 48, 255
};

static const Color COR_TEXTO = {
    45, 45, 55, 255
};

static const Color COR_VERDE = {
    46, 160, 90, 255
};

static const Color COR_VERMELHO = {
    220, 70, 70, 255
};

static const Color COR_AZUL = {
    70, 120, 255, 255
};

static const Color COR_AMARELO = {
    250, 200, 50, 255
};

static const Color COR_ROXO = {
    150, 90, 220, 255
};

static const Color COR_CINZA = {
    110, 110, 120, 255
};

/* ---------- auxiliares ---------- */

static void textoCentro(
    const char *texto,
    int centroX,
    int y,
    int tamanho,
    Color cor
) {
    int largura = MeasureText(texto, tamanho);

    DrawText(
        texto,
        centroX - largura / 2,
        y,
        tamanho,
        cor
    );
}

static void desenhaPainel(
    int x,
    int y,
    int largura,
    int altura,
    Color fundo,
    Color borda,
    float espessura
) {
    Rectangle area = {
        (float)x,
        (float)y,
        (float)largura,
        (float)altura
    };

    DrawRectangleRec(area, fundo);
    DrawRectangleLinesEx(area, espessura, borda);
}

/* ---------- cenário ---------- */

static void desenhaCenario(const Mundo *m) {
    ClearBackground(COR_FUNDO);

    /*
     * Nuvens com paralaxe.
     */
    for (int i = 0; i < 5; i++) {
        float x = fmodf(
            i * 270.0f - m->distancia * 0.12f,
            LARGURA_TELA + 140.0f
        );

        if (x < 0.0f) {
            x += LARGURA_TELA + 140.0f;
        }

        x -= 70.0f;

        float y =
            80.0f +
            (i % 3) * 38.0f;

        DrawRectangle(
            (int)x,
            (int)y,
            70,
            14,
            (Color){ 225, 228, 235, 255 }
        );

        DrawRectangle(
            (int)x + 14,
            (int)y - 10,
            40,
            12,
            (Color){ 225, 228, 235, 255 }
        );
    }

    /*
     * Chão.
     */
    DrawRectangle(
        0,
        (int)CHAO_Y,
        LARGURA_TELA,
        ALTURA_TELA - (int)CHAO_Y,
        COR_PAINEL
    );

    DrawRectangle(
        0,
        (int)CHAO_Y,
        LARGURA_TELA,
        3,
        (Color){ 90, 90, 100, 255 }
    );

    /*
     * Pontos que dão sensação de movimento.
     */
    for (int i = 0; i < 40; i++) {
        float x = fmodf(
            i * 47.0f - m->distancia,
            (float)LARGURA_TELA
        );

        if (x < 0.0f) {
            x += LARGURA_TELA;
        }

        DrawRectangle(
            (int)x,
            (int)CHAO_Y + 8 + (i % 3) * 3,
            6,
            2,
            (Color){ 90, 90, 100, 255 }
        );
    }
}

/* ---------- jogador ---------- */

static void desenhaJogador(const Mundo *m) {
    /*
     * Pisca durante a invulnerabilidade.
     */
    if (
        m->invulneravel > 0.0f &&
        ((int)(m->invulneravel * 10.0f) % 2) == 0
    ) {
        return;
    }

    Rectangle r = mundo_caixa_jogador(m);

    bool escudo = m->val.v[PROP_ESCUDO];
    bool arma = m->val.v[PROP_ARMA];
    bool turbo = m->val.v[PROP_TURBO];

    /*
     * Chama do turbo.
     */
    if (turbo) {
        DrawRectangle(
            (int)r.x - 18,
            (int)r.y + (int)r.height / 2 - 4,
            18,
            8,
            COR_AMARELO
        );
    }

    /*
     * Corpo.
     */
    DrawRectangleRec(r, COR_VERDE);

    if (!m->abaixado) {
        /*
         * Cabeça.
         */
        DrawRectangle(
            (int)r.x + 18,
            (int)r.y - 12,
            26,
            20,
            COR_VERDE
        );

        /*
         * Olho.
         */
        DrawRectangle(
            (int)r.x + 34,
            (int)r.y - 8,
            6,
            6,
            WHITE
        );

        DrawRectangle(
            (int)r.x + 37,
            (int)r.y - 6,
            3,
            3,
            BLACK
        );

        /*
         * Pernas.
         */
        bool passo =
            m->noChao &&
            ((int)(m->tempo * 12.0f) % 2 == 0);

        DrawRectangle(
            (int)r.x + 6,
            (int)(r.y + r.height) - (passo ? 4 : 0),
            10,
            4,
            (Color){ 30, 110, 60, 255 }
        );

        DrawRectangle(
            (int)r.x + 24,
            (int)(r.y + r.height) - (passo ? 0 : 4),
            10,
            4,
            (Color){ 30, 110, 60, 255 }
        );
    } else {
        /*
         * Cabeça abaixada.
         */
        DrawRectangle(
            (int)r.x + 24,
            (int)r.y + 2,
            20,
            14,
            COR_VERDE
        );

        DrawRectangle(
            (int)r.x + 36,
            (int)r.y + 5,
            4,
            4,
            BLACK
        );
    }

    /*
     * Arma.
     */
    if (arma) {
        DrawRectangle(
            (int)(r.x + r.width),
            (int)(r.y + r.height * 0.4f),
            16,
            7,
            (Color){ 60, 60, 70, 255 }
        );
    }

    /*
     * Escudo.
     */
    if (escudo) {
        Color corEscudo = COR_AZUL;
        corEscudo.a = 90;

        DrawCircle(
            (int)(r.x + r.width / 2.0f),
            (int)(r.y + r.height / 2.0f),
            46.0f,
            corEscudo
        );

        DrawCircleLines(
            (int)(r.x + r.width / 2.0f),
            (int)(r.y + r.height / 2.0f),
            46.0f,
            COR_AZUL
        );
    }
}

/* ---------- portão ---------- */

static void desenhaPortao(const Entidade *e) {
    Color preenchimento = COR_AZUL;
    Color borda = COR_AZUL;

    preenchimento.a = 150;

    if (e->avaliado) {
        if (e->resultado) {
            preenchimento = COR_VERDE;
            borda = COR_VERDE;
        } else {
            preenchimento = COR_VERMELHO;
            borda = COR_VERMELHO;
        }

        preenchimento.a = 130;
    }

    DrawRectangleRec(
        e->caixa,
        preenchimento
    );

    DrawRectangleLinesEx(
        e->caixa,
        3.0f,
        borda
    );

    Formula formula =
        catalogoFormula(e->formulaId);

    char texto[64];

    formulaParaTexto(
        &formula,
        texto,
        sizeof texto
    );

    int tamanho = 22;
    int largura = MeasureText(texto, tamanho) + 20;

    int x =
        (int)(
            e->caixa.x +
            e->caixa.width / 2.0f
        ) -
        largura / 2;

    int y =
        (int)e->caixa.y -
        34;

    DrawRectangle(
        x,
        y,
        largura,
        30,
        WHITE
    );

    DrawRectangleLines(
        x,
        y,
        largura,
        30,
        borda
    );

    DrawText(
        texto,
        x + 10,
        y + 4,
        tamanho,
        COR_TEXTO
    );
}

/* ---------- entidades ---------- */

static void desenhaEntidade(const Entidade *e) {
    Rectangle r = e->caixa;

    switch (e->tipo) {
        case ENT_CACTO:
            DrawRectangleRec(
                r,
                (Color){ 40, 120, 70, 255 }
            );

            DrawRectangle(
                (int)r.x - 6,
                (int)r.y + 14,
                8,
                6,
                (Color){ 40, 120, 70, 255 }
            );

            DrawRectangle(
                (int)(r.x + r.width) - 2,
                (int)r.y + 22,
                8,
                6,
                (Color){ 40, 120, 70, 255 }
            );
            break;

        case ENT_ANDARILHO: {
            Color cor =
                e->vida >= 2
                    ? COR_ROXO
                    : (Color){ 190, 120, 220, 255 };

            DrawRectangleRounded(
                r,
                0.3f,
                6,
                cor
            );

            DrawRectangle(
                (int)r.x + 8,
                (int)r.y + 10,
                8,
                8,
                WHITE
            );

            DrawRectangle(
                (int)r.x + 24,
                (int)r.y + 10,
                8,
                8,
                WHITE
            );

            DrawRectangle(
                (int)r.x + 11,
                (int)r.y + 13,
                4,
                4,
                BLACK
            );

            DrawRectangle(
                (int)r.x + 27,
                (int)r.y + 13,
                4,
                4,
                BLACK
            );
            break;
        }

        case ENT_VOADOR: {
            bool bateAsa =
                ((int)(e->anim * 10.0f) % 2) == 0;

            DrawRectangleRec(
                r,
                (Color){ 240, 140, 40, 255 }
            );

            DrawRectangle(
                (int)r.x + 10,
                (int)r.y + (bateAsa ? -12 : 20),
                24,
                10,
                (Color){ 210, 110, 20, 255 }
            );

            DrawRectangle(
                (int)r.x + 4,
                (int)r.y + 6,
                6,
                6,
                WHITE
            );
            break;
        }

        case ENT_PORTAO:
            desenhaPortao(e);
            break;

        case ENT_PU_ENERGIA:
        case ENT_PU_VIDA:
        case ENT_PU_TAUTOLOGIA: {
            Color cor = COR_AMARELO;
            const char *letra = "E";

            if (e->tipo == ENT_PU_VIDA) {
                cor = COR_VERMELHO;
                letra = "+";
            }

            if (e->tipo == ENT_PU_TAUTOLOGIA) {
                cor = COR_ROXO;
                letra = "T";
            }

            DrawCircle(
                (int)(r.x + r.width / 2.0f),
                (int)(r.y + r.height / 2.0f),
                16.0f,
                cor
            );

            DrawCircleLines(
                (int)(r.x + r.width / 2.0f),
                (int)(r.y + r.height / 2.0f),
                16.0f,
                COR_TEXTO
            );

            textoCentro(
                letra,
                (int)(r.x + r.width / 2.0f),
                (int)r.y + 5,
                20,
                WHITE
            );
            break;
        }
    }
}

/* ---------- tabela-verdade ---------- */

static void desenhaTabela(
    const Mundo *m,
    const Entidade *portao
) {
    Formula formula =
        catalogoFormula(portao->formulaId);

    char texto[64];

    formulaParaTexto(
        &formula,
        texto,
        sizeof texto
    );

    int x = LARGURA_TELA - 184;
    int y = 52;
    int largura = 172;
    int altura = 150;

    DrawRectangle(
        x,
        y,
        largura,
        altura,
        (Color){ 255, 255, 255, 235 }
    );

    DrawRectangleLines(
        x,
        y,
        largura,
        altura,
        COR_AZUL
    );

    DrawText(
        "TABELA-VERDADE",
        x + 10,
        y + 5,
        12,
        COR_TEXTO
    );

    DrawText(
        "P  Q  R |  f",
        x + 14,
        y + 22,
        14,
        COR_TEXTO
    );

    DrawLine(
        x + 8,
        y + 38,
        x + largura - 8,
        y + 38,
        COR_TEXTO
    );

    unsigned atual =
        mascaraDeValoracao(m->val);

    for (unsigned i = 0; i < 8; i++) {
        Valoracao valoracao =
            valoracaoDeMascara(i);

        bool resultado =
            avalia(&formula, valoracao);

        int linhaY =
            y + 42 + (int)i * 13;

        if (i == atual) {
            DrawRectangle(
                x + 6,
                linhaY - 1,
                largura - 12,
                13,
                (Color){ 250, 220, 90, 255 }
            );
        }

        char linha[32];

        snprintf(
            linha,
            sizeof linha,
            "%c  %c  %c |  %c",
            valoracao.v[0] ? 'V' : 'F',
            valoracao.v[1] ? 'V' : 'F',
            valoracao.v[2] ? 'V' : 'F',
            resultado ? 'V' : 'F'
        );

        DrawText(
            linha,
            x + 14,
            linhaY,
            12,
            resultado
                ? COR_VERDE
                : COR_VERMELHO
        );
    }
}

/* ---------- cartões das proposições ---------- */

static void desenhaCartaoProposicao(
    const Mundo *m,
    int indice,
    int x,
    int y
) {
    static const char *poderes[NUM_PROPS] = {
        "ESCUDO",
        "ARMA",
        "TURBO"
    };

    static const Color cores[NUM_PROPS] = {
        { 70, 120, 255, 255 },
        { 230, 180, 40, 255 },
        { 170, 80, 215, 255 }
    };

    const int largura = 188;
    const int altura = 52;

    bool ligado = m->val.v[indice];

    Color fundo =
        ligado
            ? cores[indice]
            : (Color){ 60, 60, 72, 255 };

    Color borda =
        ligado
            ? WHITE
            : COR_CINZA;

    desenhaPainel(
        x,
        y,
        largura,
        altura,
        fundo,
        borda,
        ligado ? 2.0f : 1.0f
    );

    char cabecalho[32];

    snprintf(
        cabecalho,
        sizeof cabecalho,
        "[%d] %s",
        indice + 1,
        nomeProp(indice)
    );

    DrawText(
        cabecalho,
        x + 10,
        y + 7,
        22,
        WHITE
    );

    DrawText(
        poderes[indice],
        x + 66,
        y + 10,
        15,
        WHITE
    );

    DrawText(
        ligado ? "VERDADEIRO" : "FALSO",
        x + 10,
        y + 33,
        13,
        ligado
            ? WHITE
            : (Color){ 200, 200, 210, 255 }
    );

    DrawCircle(
        x + largura - 18,
        y + altura / 2,
        8.0f,
        ligado
            ? COR_VERDE
            : (Color){ 120, 120, 130, 255 }
    );
}

/* ---------- painel do desafio lógico ---------- */

static void desenhaDesafioLogico(const Mundo *m) {
    if (!m->decisaoPortao) {
        return;
    }

    const Entidade *portao =
        mundo_proximo_portao(m);

    if (portao == NULL) {
        return;
    }

    /*
     * Escurece somente a área de jogo.
     */
    DrawRectangle(
        0,
        0,
        LARGURA_TELA,
        (int)CHAO_Y,
        (Color){ 20, 22, 34, 165 }
    );

    const int painelX = 230;
    const int painelY = 100;
    const int painelLargura = 500;
    const int painelAltura = 275;

    desenhaPainel(
        painelX,
        painelY,
        painelLargura,
        painelAltura,
        (Color){ 30, 32, 46, 250 },
        COR_AZUL,
        3.0f
    );

    textoCentro(
        "DESAFIO LOGICO",
        LARGURA_TELA / 2,
        118,
        28,
        COR_AMARELO
    );

    textoCentro(
        "Torne a formula verdadeira antes do tempo acabar",
        LARGURA_TELA / 2,
        153,
        15,
        (Color){ 215, 215, 225, 255 }
    );

    Formula formula =
        catalogoFormula(portao->formulaId);

    char textoFormula[64];

    formulaParaTexto(
        &formula,
        textoFormula,
        sizeof textoFormula
    );

    int larguraFormula =
        MeasureText(textoFormula, 38) + 50;

    int formulaX =
        LARGURA_TELA / 2 -
        larguraFormula / 2;

    desenhaPainel(
        formulaX,
        184,
        larguraFormula,
        58,
        WHITE,
        COR_AZUL,
        2.0f
    );

    textoCentro(
        textoFormula,
        LARGURA_TELA / 2,
        194,
        38,
        COR_TEXTO
    );

    char valores[96];

    snprintf(
        valores,
        sizeof valores,
        "P = %c       Q = %c       R = %c",
        m->val.v[PROP_ESCUDO] ? 'V' : 'F',
        m->val.v[PROP_ARMA] ? 'V' : 'F',
        m->val.v[PROP_TURBO] ? 'V' : 'F'
    );

    textoCentro(
        valores,
        LARGURA_TELA / 2,
        258,
        22,
        WHITE
    );

    float proporcao =
        m->tempoDecisaoPortao /
        TEMPO_DECISAO_PORTAO;

    if (proporcao < 0.0f) {
        proporcao = 0.0f;
    }

    if (proporcao > 1.0f) {
        proporcao = 1.0f;
    }

    Color corTempo = COR_VERDE;

    if (m->tempoDecisaoPortao <= 2.0f) {
        corTempo = COR_VERMELHO;
    } else if (m->tempoDecisaoPortao <= 3.5f) {
        corTempo = COR_AMARELO;
    }

    char textoTempo[32];

    snprintf(
        textoTempo,
        sizeof textoTempo,
        "TEMPO: %.1f s",
        m->tempoDecisaoPortao
    );

    textoCentro(
        textoTempo,
        LARGURA_TELA / 2,
        292,
        22,
        corTempo
    );

    DrawRectangle(
        330,
        324,
        300,
        12,
        (Color){ 75, 77, 92, 255 }
    );

    DrawRectangle(
        330,
        324,
        (int)(300.0f * proporcao),
        12,
        corTempo
    );

    DrawRectangleLines(
        330,
        324,
        300,
        12,
        WHITE
    );

    textoCentro(
        "[1] [2] [3] ALTERAR     [ENTER] CONFIRMAR",
        LARGURA_TELA / 2,
        347,
        15,
        WHITE
    );
}

/* ---------- HUD ---------- */

static void desenhaHud(
    const Mundo *m,
    bool mostraTabela
) {
    /*
     * Vidas.
     */
    for (int i = 0; i < VIDAS_MAX; i++) {
        Vector2 centro = {
            28.0f + i * 26.0f,
            26.0f
        };

        if (i < m->vidas) {
            DrawCircleV(
                centro,
                9.0f,
                COR_VERMELHO
            );
        } else {
            DrawCircleLinesV(
                centro,
                9.0f,
                (Color){ 170, 170, 175, 255 }
            );
        }
    }

    /*
     * Energia.
     */
    DrawText(
        "ENERGIA",
        20,
        44,
        12,
        COR_TEXTO
    );

    DrawRectangle(
        20,
        58,
        180,
        12,
        (Color){ 215, 215, 220, 255 }
    );

    Color corEnergia =
        m->energia < 25.0f
            ? COR_VERMELHO
            : COR_AMARELO;

    DrawRectangle(
        20,
        58,
        (int)(
            180.0f *
            m->energia /
            ENERGIA_MAX
        ),
        12,
        corEnergia
    );

    DrawRectangleLines(
        20,
        58,
        180,
        12,
        COR_TEXTO
    );

    if (m->sobrecarga > 0.0f) {
        DrawText(
            "SOBRECARGA!",
            210,
            56,
            16,
            COR_VERMELHO
        );
    }

    /*
     * Pontuação.
     */
    char texto[128];

    snprintf(
        texto,
        sizeof texto,
        "PONTOS %d",
        mundo_pontos(m)
    );

    textoCentro(
        texto,
        LARGURA_TELA / 2,
        14,
        28,
        COR_TEXTO
    );

    /*
     * Portões.
     */
    snprintf(
        texto,
        sizeof texto,
        "PORTOES %d/%d",
        m->portoesOk,
        META_PORTOES
    );

    DrawText(
        texto,
        LARGURA_TELA - MeasureText(texto, 22) - 14,
        14,
        22,
        COR_AZUL
    );

    if (m->tautologia) {
        textoCentro(
            "TAUTOLOGIA ATIVA: o proximo portao abre",
            LARGURA_TELA / 2,
            48,
            16,
            COR_ROXO
        );
    }

    /*
     * Mensagem temporária.
     */
    if (
        !m->decisaoPortao &&
        m->msgTempo > 0.0f &&
        m->msg[0] != '\0'
    ) {
        textoCentro(
            m->msg,
            LARGURA_TELA / 2,
            100,
            24,
            COR_TEXTO
        );
    }

    /*
     * Cartões inferiores sem as explicações longas.
     */
    for (int i = 0; i < NUM_PROPS; i++) {
        desenhaCartaoProposicao(
            m,
            i,
            20 + i * 198,
            452
        );
    }

    /*
     * Próximo portão.
     */
    const Entidade *portao =
        mundo_proximo_portao(m);

    if (portao != NULL) {
        Formula formula =
            catalogoFormula(portao->formulaId);

        char textoFormula[64];

        formulaParaTexto(
            &formula,
            textoFormula,
            sizeof textoFormula
        );

        if (m->decisaoPortao) {
            DrawText(
                "DESAFIO EM ANDAMENTO",
                626,
                454,
                16,
                COR_AMARELO
            );

            DrawText(
                "Use 1, 2 e 3. ENTER confirma.",
                626,
                476,
                13,
                WHITE
            );
        } else {
            bool resultadoAtual =
                m->tautologia ||
                avalia(&formula, m->val);

            char linha[128];

            snprintf(
                linha,
                sizeof linha,
                "Portao: %s => %s",
                textoFormula,
                resultadoAtual
                    ? "VERDADEIRO"
                    : "FALSO"
            );

            DrawText(
                linha,
                626,
                454,
                16,
                resultadoAtual
                    ? COR_VERDE
                    : COR_VERMELHO
            );
        }

        if (
            mostraTabela &&
            !m->decisaoPortao
        ) {
            desenhaTabela(
                m,
                portao
            );
        }
    } else {
        DrawText(
            "Prox. portao: em breve...",
            626,
            454,
            16,
            (Color){ 160, 160, 170, 255 }
        );
    }

    DrawText(
        "~ NAO    & E    | OU",
        626,
        494,
        12,
        (Color){ 200, 200, 210, 255 }
    );

    DrawText(
        "T: tabela-verdade    P: pausa",
        626,
        512,
        12,
        (Color){ 160, 160, 170, 255 }
    );
}

/* ---------- API ---------- */

void render_mundo(
    const Mundo *m,
    bool mostraTabela
) {
    desenhaCenario(m);

    for (int i = 0; i < m->nEnts; i++) {
        desenhaEntidade(&m->ents[i]);
    }

    for (int i = 0; i < m->nTiros; i++) {
        DrawRectangleRec(
            m->tiros[i].caixa,
            COR_VERMELHO
        );
    }

    desenhaJogador(m);
    desenhaHud(m, mostraTabela);
    desenhaDesafioLogico(m);
}

/* ---------- telas auxiliares ---------- */

static void fundoEscuro(void) {
    DrawRectangle(
        0,
        0,
        LARGURA_TELA,
        ALTURA_TELA,
        (Color){ 20, 20, 28, 225 }
    );
}

void render_menu(void) {
    ClearBackground(COR_FUNDO);

    DrawRectangle(
        0,
        0,
        LARGURA_TELA,
        ALTURA_TELA,
        COR_PAINEL
    );

    textoCentro(
        "RUNNER LOGICO",
        LARGURA_TELA / 2,
        50,
        56,
        WHITE
    );

    textoCentro(
        "corra, atire e abra os portoes com Logica Proposicional",
        LARGURA_TELA / 2,
        118,
        20,
        (Color){ 190, 190, 200, 255 }
    );

    int x = 110;
    int y = 180;

    DrawText(
        "CONTROLES",
        x,
        y,
        22,
        COR_AMARELO
    );

    DrawText(
        "ESPACO / SETA CIMA ... pular",
        x,
        y + 32,
        18,
        WHITE
    );

    DrawText(
        "SETA BAIXO ........... abaixar",
        x,
        y + 56,
        18,
        WHITE
    );

    DrawText(
        "X ou Z ............... atirar (so se Q = V)",
        x,
        y + 80,
        18,
        WHITE
    );

    DrawText(
        "1  2  3 .............. alternar P, Q e R",
        x,
        y + 104,
        18,
        WHITE
    );

    DrawText(
        "T tabela   P pausa   ESC menu",
        x,
        y + 128,
        18,
        WHITE
    );

    DrawText(
        "COMO FUNCIONA",
        520,
        y,
        22,
        COR_AMARELO
    );

    DrawText(
        "P = escudo   Q = arma   R = turbo",
        520,
        y + 32,
        18,
        WHITE
    );

    DrawText(
        "Cada proposicao V gasta energia.",
        520,
        y + 56,
        18,
        WHITE
    );

    DrawText(
        "No portao, a corrida para por 5 segundos.",
        520,
        y + 80,
        18,
        WHITE
    );

    DrawText(
        "Ajuste P, Q e R para validar a formula.",
        520,
        y + 104,
        18,
        WHITE
    );

    DrawText(
        "ENTER confirma. Formula falsa = -1 vida.",
        520,
        y + 128,
        18,
        WHITE
    );

    textoCentro(
        "ENTER para jogar",
        LARGURA_TELA / 2,
        440,
        30,
        COR_VERDE
    );
}

void render_pausa(void) {
    fundoEscuro();

    textoCentro(
        "PAUSADO",
        LARGURA_TELA / 2,
        220,
        48,
        WHITE
    );

    textoCentro(
        "P para continuar   |   ESC para o menu",
        LARGURA_TELA / 2,
        285,
        20,
        (Color){ 200, 200, 210, 255 }
    );
}

void render_fim(
    const Mundo *m,
    int recorde
) {
    fundoEscuro();

    bool venceu =
        m->resultado == RES_VITORIA;

    textoCentro(
        venceu
            ? "VITORIA!"
            : "FIM DE JOGO",
        LARGURA_TELA / 2,
        150,
        56,
        venceu
            ? COR_VERDE
            : COR_VERMELHO
    );

    char texto[128];

    snprintf(
        texto,
        sizeof texto,
        "Pontos: %d    Recorde: %d",
        mundo_pontos(m),
        recorde
    );

    textoCentro(
        texto,
        LARGURA_TELA / 2,
        225,
        26,
        WHITE
    );

    snprintf(
        texto,
        sizeof texto,
        "Portoes abertos: %d   Falsos: %d   Inimigos abatidos: %d",
        m->portoesOk,
        m->portoesErro,
        m->abatidos
    );

    textoCentro(
        texto,
        LARGURA_TELA / 2,
        265,
        18,
        (Color){ 200, 200, 210, 255 }
    );

    textoCentro(
        "ENTER para jogar de novo   |   ESC para o menu",
        LARGURA_TELA / 2,
        330,
        20,
        WHITE
    );
}