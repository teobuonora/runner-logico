#include "render.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* ============================================================
 * PALETA FIXA DA INTERFACE
 * ============================================================ */

static const Color COR_PAINEL       = { 28, 30, 44, 245 };
static const Color COR_PAINEL_CLARO = { 47, 50, 68, 245 };
static const Color COR_TEXTO        = { 241, 242, 248, 255 };
static const Color COR_TEXTO_FRACO  = { 184, 188, 204, 255 };

static const Color COR_VERDE        = { 75, 205, 132, 255 };
static const Color COR_VERDE_ESCURO = { 31, 121, 79, 255 };
static const Color COR_VERMELHO     = { 238, 83, 94, 255 };
static const Color COR_AZUL         = { 74, 151, 255, 255 };
static const Color COR_CIANO        = { 69, 220, 224, 255 };
static const Color COR_AMARELO      = { 255, 207, 79, 255 };
static const Color COR_LARANJA      = { 239, 127, 58, 255 };
static const Color COR_ROXO         = { 180, 100, 230, 255 };

/* ============================================================
 * TEMAS DO CENÁRIO
 * ============================================================ */

typedef struct {
    Color ceuTopo;
    Color ceuBase;

    Color sol;
    Color solBrilho;

    Color montanha1;
    Color montanha2;
    Color montanha3;

    Color chao;
    Color chaoTopo;
    Color chaoLuz;

    Color nuvem;
    Color nuvemSombra;

    Color estrela;
    Color tecnologia;
} TemaCenario;

static const TemaCenario TEMAS_CENARIO[META_PORTOES + 1] = {
    /* 0 portões: crepúsculo roxo */
    {
        { 28, 31, 72, 255 },
        { 116, 67, 118, 255 },

        { 255, 205, 92, 255 },
        { 255, 218, 125, 55 },

        { 66, 49, 92, 255 },
        { 48, 42, 72, 255 },
        { 35, 35, 55, 255 },

        { 31, 31, 45, 255 },
        { 231, 134, 74, 255 },
        { 91, 78, 98, 255 },

        { 207, 191, 213, 185 },
        { 93, 75, 112, 125 },

        { 241, 232, 220, 185 },
        { 82, 155, 190, 160 }
    },

    /* 1 portão: noite azul */
    {
        { 15, 29, 72, 255 },
        { 41, 78, 130, 255 },

        { 184, 221, 255, 255 },
        { 131, 198, 255, 48 },

        { 37, 66, 110, 255 },
        { 29, 48, 85, 255 },
        { 23, 33, 59, 255 },

        { 23, 30, 48, 255 },
        { 71, 165, 221, 255 },
        { 61, 91, 128, 255 },

        { 181, 209, 232, 180 },
        { 56, 82, 116, 125 },

        { 225, 239, 255, 210 },
        { 73, 191, 225, 170 }
    },

    /* 2 portões: violeta profundo */
    {
        { 35, 21, 76, 255 },
        { 101, 54, 145, 255 },

        { 226, 184, 255, 255 },
        { 197, 126, 255, 52 },

        { 72, 45, 111, 255 },
        { 51, 34, 83, 255 },
        { 34, 28, 57, 255 },

        { 29, 27, 45, 255 },
        { 177, 103, 228, 255 },
        { 91, 65, 119, 255 },

        { 210, 184, 229, 180 },
        { 92, 65, 124, 125 },

        { 238, 219, 255, 205 },
        { 173, 105, 225, 170 }
    },

    /* 3 portões: magenta tecnológico */
    {
        { 55, 24, 74, 255 },
        { 153, 58, 121, 255 },

        { 255, 174, 205, 255 },
        { 255, 103, 178, 52 },

        { 105, 47, 104, 255 },
        { 72, 36, 78, 255 },
        { 43, 29, 55, 255 },

        { 34, 27, 43, 255 },
        { 232, 91, 157, 255 },
        { 119, 62, 105, 255 },

        { 226, 181, 214, 180 },
        { 119, 65, 107, 125 },

        { 255, 222, 238, 200 },
        { 231, 88, 179, 170 }
    },

    /* 4 portões: pôr do sol */
    {
        { 71, 35, 76, 255 },
        { 219, 101, 70, 255 },

        { 255, 220, 112, 255 },
        { 255, 175, 73, 58 },

        { 135, 67, 80, 255 },
        { 91, 53, 72, 255 },
        { 49, 38, 57, 255 },

        { 40, 31, 42, 255 },
        { 255, 150, 68, 255 },
        { 129, 76, 77, 255 },

        { 241, 197, 183, 185 },
        { 141, 78, 85, 125 },

        { 255, 231, 196, 165 },
        { 255, 156, 68, 175 }
    },

    /* 5 portões: noite ciano */
    {
        { 12, 45, 65, 255 },
        { 24, 111, 121, 255 },

        { 172, 247, 236, 255 },
        { 70, 238, 217, 50 },

        { 33, 91, 105, 255 },
        { 25, 67, 82, 255 },
        { 20, 43, 58, 255 },

        { 21, 35, 44, 255 },
        { 47, 214, 188, 255 },
        { 53, 105, 111, 255 },

        { 175, 225, 220, 180 },
        { 55, 105, 110, 125 },

        { 213, 255, 248, 210 },
        { 52, 224, 200, 175 }
    },

    /* 6 portões: aurora verde */
    {
        { 19, 51, 60, 255 },
        { 55, 139, 103, 255 },

        { 205, 255, 178, 255 },
        { 119, 255, 154, 52 },

        { 44, 103, 84, 255 },
        { 31, 75, 68, 255 },
        { 23, 48, 52, 255 },

        { 24, 38, 40, 255 },
        { 94, 226, 137, 255 },
        { 59, 113, 83, 255 },

        { 186, 226, 198, 180 },
        { 60, 110, 82, 125 },

        { 223, 255, 225, 195 },
        { 84, 230, 147, 175 }
    },

    /* 7 portões: aurora dourada */
    {
        { 60, 56, 83, 255 },
        { 205, 139, 76, 255 },

        { 255, 239, 137, 255 },
        { 255, 214, 82, 62 },

        { 129, 91, 91, 255 },
        { 89, 67, 78, 255 },
        { 48, 45, 58, 255 },

        { 42, 37, 43, 255 },
        { 255, 198, 74, 255 },
        { 127, 98, 70, 255 },

        { 236, 216, 192, 185 },
        { 126, 99, 91, 125 },

        { 255, 242, 196, 185 },
        { 255, 197, 74, 180 }
    },

    /* 8 portões: céu final luminoso */
    {
        { 46, 99, 164, 255 },
        { 114, 211, 205, 255 },

        { 255, 242, 158, 255 },
        { 255, 236, 129, 70 },

        { 82, 150, 162, 255 },
        { 61, 112, 134, 255 },
        { 41, 72, 96, 255 },

        { 30, 47, 57, 255 },
        { 255, 215, 89, 255 },
        { 78, 139, 139, 255 },

        { 229, 241, 235, 200 },
        { 107, 165, 171, 125 },

        { 255, 248, 209, 175 },
        { 255, 223, 92, 185 }
    }
};

/* ============================================================
 * AUXILIARES
 * ============================================================ */

static Color misturaCor(
    Color a,
    Color b,
    float t
) {
    Color resultado;

    resultado.r =
        (unsigned char)(
            a.r +
            (b.r - a.r) * t
        );

    resultado.g =
        (unsigned char)(
            a.g +
            (b.g - a.g) * t
        );

    resultado.b =
        (unsigned char)(
            a.b +
            (b.b - a.b) * t
        );

    resultado.a =
        (unsigned char)(
            a.a +
            (b.a - a.a) * t
        );

    return resultado;
}

static float limita01(float valor) {
    if (valor < 0.0f) {
        return 0.0f;
    }

    if (valor > 1.0f) {
        return 1.0f;
    }

    return valor;
}

static float suavizaTransicao(float valor) {
    valor =
        limita01(valor);

    return
        valor *
        valor *
        (
            3.0f -
            2.0f * valor
        );
}

static TemaCenario misturaTema(
    TemaCenario anterior,
    TemaCenario atual,
    float valor
) {
    TemaCenario resultado;

    valor =
        suavizaTransicao(valor);

    resultado.ceuTopo =
        misturaCor(
            anterior.ceuTopo,
            atual.ceuTopo,
            valor
        );

    resultado.ceuBase =
        misturaCor(
            anterior.ceuBase,
            atual.ceuBase,
            valor
        );

    resultado.sol =
        misturaCor(
            anterior.sol,
            atual.sol,
            valor
        );

    resultado.solBrilho =
        misturaCor(
            anterior.solBrilho,
            atual.solBrilho,
            valor
        );

    resultado.montanha1 =
        misturaCor(
            anterior.montanha1,
            atual.montanha1,
            valor
        );

    resultado.montanha2 =
        misturaCor(
            anterior.montanha2,
            atual.montanha2,
            valor
        );

    resultado.montanha3 =
        misturaCor(
            anterior.montanha3,
            atual.montanha3,
            valor
        );

    resultado.chao =
        misturaCor(
            anterior.chao,
            atual.chao,
            valor
        );

    resultado.chaoTopo =
        misturaCor(
            anterior.chaoTopo,
            atual.chaoTopo,
            valor
        );

    resultado.chaoLuz =
        misturaCor(
            anterior.chaoLuz,
            atual.chaoLuz,
            valor
        );

    resultado.nuvem =
        misturaCor(
            anterior.nuvem,
            atual.nuvem,
            valor
        );

    resultado.nuvemSombra =
        misturaCor(
            anterior.nuvemSombra,
            atual.nuvemSombra,
            valor
        );

    resultado.estrela =
        misturaCor(
            anterior.estrela,
            atual.estrela,
            valor
        );

    resultado.tecnologia =
        misturaCor(
            anterior.tecnologia,
            atual.tecnologia,
            valor
        );

    return resultado;
}

static TemaCenario temaAtual(const Mundo *m) {
    int indice =
        m->portoesOk;

    if (indice < 0) {
        indice = 0;
    }

    if (indice > META_PORTOES) {
        indice =
            META_PORTOES;
    }

    bool transicaoAtiva =
        indice > 0 &&
        m->msgTempo > 0.0f &&
        strstr(
            m->msg,
            "PORTAO ABERTO"
        ) != NULL;

    if (!transicaoAtiva) {
        return
            TEMAS_CENARIO[indice];
    }

    float progresso =
        1.0f -
        m->msgTempo /
        1.8f;

    return misturaTema(
        TEMAS_CENARIO[indice - 1],
        TEMAS_CENARIO[indice],
        progresso
    );
}

static void textoCentro(
    const char *texto,
    int centroX,
    int y,
    int tamanho,
    Color cor
) {
    DrawText(
        texto,
        centroX -
            MeasureText(
                texto,
                tamanho
            ) /
            2,
        y,
        tamanho,
        cor
    );
}

static void painel(
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

    DrawRectangleRec(
        area,
        fundo
    );

    DrawRectangleLinesEx(
        area,
        espessura,
        borda
    );
}

static void linhaBrilho(
    Vector2 inicio,
    Vector2 fim,
    float espessura,
    Color cor
) {
    Color brilho =
        cor;

    brilho.a =
        55;

    DrawLineEx(
        inicio,
        fim,
        espessura + 5.0f,
        brilho
    );

    DrawLineEx(
        inicio,
        fim,
        espessura,
        cor
    );
}

static void circuloBrilho(
    int x,
    int y,
    float raio,
    Color cor
) {
    Color brilho =
        cor;

    brilho.a =
        45;

    DrawCircle(
        x,
        y,
        raio + 7.0f,
        brilho
    );

    DrawCircle(
        x,
        y,
        raio,
        cor
    );
}

/* ============================================================
 * ÁREAS CLICÁVEIS
 * ============================================================ */

Rectangle render_area_botao_ranking_menu(void) {
    return (Rectangle){
        BOTAO_RANKING_MENU_X,
        BOTAO_RANKING_MENU_Y,
        BOTAO_RANKING_MENU_L,
        BOTAO_RANKING_MENU_A
    };
}

Rectangle render_area_botao_ranking_fim(void) {
    return (Rectangle){
        BOTAO_RANKING_FIM_X,
        BOTAO_RANKING_FIM_Y,
        BOTAO_RANKING_FIM_L,
        BOTAO_RANKING_FIM_A
    };
}

Rectangle render_area_botao_voltar_ranking(void) {
    return (Rectangle){
        BOTAO_VOLTAR_RANKING_X,
        BOTAO_VOLTAR_RANKING_Y,
        BOTAO_VOLTAR_RANKING_L,
        BOTAO_VOLTAR_RANKING_A
    };
}

Rectangle render_area_botao_salvar_nome(void) {
    return (Rectangle){
        BOTAO_SALVAR_NOME_X,
        BOTAO_SALVAR_NOME_Y,
        BOTAO_SALVAR_NOME_L,
        BOTAO_SALVAR_NOME_A
    };
}

static void desenhaBotao(
    Rectangle area,
    const char *texto,
    bool hover,
    Color corPrincipal
) {
    Color fundo =
        hover
            ? misturaCor(
                COR_PAINEL_CLARO,
                corPrincipal,
                0.48f
            )
            : misturaCor(
                COR_PAINEL,
                corPrincipal,
                0.24f
            );

    Color borda =
        hover
            ? WHITE
            : corPrincipal;

    DrawRectangleRec(
        area,
        fundo
    );

    DrawRectangleLinesEx(
        area,
        hover
            ? 3.0f
            : 2.0f,
        borda
    );

    textoCentro(
        texto,
        (int)(
            area.x +
            area.width / 2.0f
        ),
        (int)(
            area.y +
            area.height / 2.0f -
            10.0f
        ),
        20,
        WHITE
    );
}

/* ============================================================
 * CENÁRIO: DIMENSÕES
 *
 * 0 - Deserto do Crepúsculo
 * 1 - Floresta Bioluminescente
 * 2 - Cidade Neon
 * 3 - Mundo Glacial
 * 4 - Dimensão Vulcânica
 * 5 - Cidade Submersa
 * 6 - Vazio Cósmico
 * 7 - Inferno Lógico
 * 8 - Reino Celestial
 * ============================================================ */

static Color corComAlpha(
    Color cor,
    float alpha
) {
    alpha =
        limita01(alpha);

    cor.a =
        (unsigned char)(
            (float)cor.a *
            alpha
        );

    return cor;
}

static float deslocamentoCiclico(
    float base,
    float distancia,
    float velocidade,
    float ciclo
) {
    float x =
        fmodf(
            base -
                distancia *
                velocidade,
            ciclo
        );

    if (x < 0.0f) {
        x += ciclo;
    }

    return x;
}

static void desenhaGradienteVertical(
    Color topo,
    Color base,
    float alpha
) {
    const int faixas =
        64;

    int alturaFaixa =
        (int)CHAO_Y /
        faixas +
        1;

    for (
        int i = 0;
        i < faixas;
        i++
    ) {
        float progresso =
            (float)i /
            (float)(
                faixas - 1
            );

        Color cor =
            misturaCor(
                topo,
                base,
                progresso
            );

        cor =
            corComAlpha(
                cor,
                alpha
            );

        DrawRectangle(
            0,
            i * alturaFaixa,
            LARGURA_TELA,
            alturaFaixa + 1,
            cor
        );
    }
}

static void desenhaChaoDimensao(
    const Mundo *m,
    Color fundo,
    Color borda,
    Color detalhe,
    float alpha
) {
    fundo =
        corComAlpha(
            fundo,
            alpha
        );

    borda =
        corComAlpha(
            borda,
            alpha
        );

    detalhe =
        corComAlpha(
            detalhe,
            alpha
        );

    DrawRectangle(
        0,
        (int)CHAO_Y,
        LARGURA_TELA,
        ALTURA_TELA -
            (int)CHAO_Y,
        fundo
    );

    DrawRectangle(
        0,
        (int)CHAO_Y,
        LARGURA_TELA,
        5,
        borda
    );

    DrawRectangle(
        0,
        (int)CHAO_Y + 5,
        LARGURA_TELA,
        3,
        misturaCor(
            fundo,
            borda,
            0.45f
        )
    );

    for (
        int i = 0;
        i < 28;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 52.0f,
                m->distancia,
                1.0f,
                (float)LARGURA_TELA
            );

        int y =
            (int)CHAO_Y +
            15 +
            (i % 4) *
            10;

        int largura =
            10 +
            (i % 3) *
            8;

        DrawRectangle(
            (int)x,
            y,
            largura,
            2,
            detalhe
        );
    }
}

static void desenhaMontanhaDimensional(
    const Mundo *m,
    float velocidade,
    int baseY,
    int altura,
    int largura,
    Color cor,
    float alpha
) {
    Color corFinal =
        corComAlpha(
            cor,
            alpha
        );

    float deslocamento =
        fmodf(
            m->distancia *
                velocidade,
            (float)largura
        );

    for (
        int i = -2;
        i <
            LARGURA_TELA /
                largura +
                3;
        i++
    ) {
        float x =
            i * largura -
            deslocamento;

        DrawTriangle(
            (Vector2){
                x,
                (float)baseY
            },
            (Vector2){
                x +
                    largura *
                    0.5f,
                (float)(
                    baseY -
                    altura
                )
            },
            (Vector2){
                x +
                    largura,
                (float)baseY
            },
            corFinal
        );
    }
}

/* ============================================================
 * DIMENSÃO 0: DESERTO DO CREPÚSCULO
 * ============================================================ */

static void desenhaDeserto(
    const Mundo *m,
    float alpha
) {
    desenhaGradienteVertical(
        (Color){
            28,
            31,
            72,
            255
        },
        (Color){
            184,
            86,
            94,
            255
        },
        alpha
    );

    Color brilho =
        corComAlpha(
            (Color){
                255,
                192,
                83,
                55
            },
            alpha
        );

    Color sol =
        corComAlpha(
            (Color){
                255,
                211,
                105,
                255
            },
            alpha
        );

    DrawCircle(
        785,
        126,
        64.0f,
        brilho
    );

    DrawCircle(
        785,
        126,
        43.0f,
        sol
    );

    desenhaMontanhaDimensional(
        m,
        0.025f,
        383,
        132,
        300,
        (Color){
            91,
            58,
            105,
            255
        },
        alpha
    );

    desenhaMontanhaDimensional(
        m,
        0.055f,
        416,
        94,
        225,
        (Color){
            68,
            48,
            81,
            255
        },
        alpha
    );

    /*
     * Dunas.
     */
    Color dunaDistante =
        corComAlpha(
            (Color){
                139,
                79,
                91,
                255
            },
            alpha
        );

    Color dunaProxima =
        corComAlpha(
            (Color){
                92,
                59,
                72,
                255
            },
            alpha
        );

    for (
        int i = -1;
        i < 7;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 190.0f,
                m->distancia,
                0.10f,
                1330.0f
            ) -
            190.0f;

        DrawEllipse(
            (int)x,
            424,
            145.0f,
            38.0f,
            dunaDistante
        );
    }

    for (
        int i = -1;
        i < 8;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 150.0f,
                m->distancia,
                0.18f,
                1200.0f
            ) -
            150.0f;

        DrawEllipse(
            (int)x,
            440,
            120.0f,
            29.0f,
            dunaProxima
        );
    }

    /*
     * Ruínas tecnológicas.
     */
    for (
        int i = 0;
        i < 6;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 220.0f,
                m->distancia,
                0.17f,
                1320.0f
            ) -
            80.0f;

        Color pedra =
            corComAlpha(
                (Color){
                    44,
                    40,
                    61,
                    255
                },
                alpha
            );

        Color luz =
            corComAlpha(
                COR_CIANO,
                alpha
            );

        DrawRectangle(
            (int)x,
            349,
            13,
            91,
            pedra
        );

        DrawRectangle(
            (int)x - 8,
            349,
            29,
            6,
            pedra
        );

        DrawCircle(
            (int)x + 6,
            361,
            3.0f,
            luz
        );
    }

    desenhaChaoDimensao(
        m,
        (Color){
            37,
            31,
            43,
            255
        },
        (Color){
            231,
            134,
            74,
            255
        },
        (Color){
            116,
            80,
            89,
            255
        },
        alpha
    );
}

/* ============================================================
 * DIMENSÃO 1: FLORESTA BIOLUMINESCENTE
 * ============================================================ */

static void desenhaFloresta(
    const Mundo *m,
    float alpha
) {
    desenhaGradienteVertical(
        (Color){
            8,
            31,
            48,
            255
        },
        (Color){
            21,
            86,
            78,
            255
        },
        alpha
    );

    Color lua =
        corComAlpha(
            (Color){
                184,
                244,
                232,
                255
            },
            alpha
        );

    Color halo =
        lua;

    halo.a =
        (unsigned char)(
            48.0f *
            alpha
        );

    DrawCircle(
        770,
        112,
        57.0f,
        halo
    );

    DrawCircle(
        770,
        112,
        34.0f,
        lua
    );

    /*
     * Árvores distantes.
     */
    for (
        int i = -1;
        i < 11;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 110.0f,
                m->distancia,
                0.045f,
                1210.0f
            ) -
            100.0f;

        int altura =
            105 +
            (i & 1) *
            35;

        Color tronco =
            corComAlpha(
                (Color){
                    13,
                    57,
                    59,
                    255
                },
                alpha
            );

        Color copa =
            corComAlpha(
                (Color){
                    17,
                    76,
                    71,
                    255
                },
                alpha
            );

        DrawRectangle(
            (int)x,
            (int)CHAO_Y -
                altura,
            15,
            altura,
            tronco
        );

        DrawCircle(
            (int)x + 7,
            (int)CHAO_Y -
                altura,
            39.0f,
            copa
        );

        DrawCircle(
            (int)x - 14,
            (int)CHAO_Y -
                altura +
                22,
            28.0f,
            copa
        );

        DrawCircle(
            (int)x + 29,
            (int)CHAO_Y -
                altura +
                18,
            30.0f,
            copa
        );
    }

    /*
     * Árvores próximas.
     */
    for (
        int i = -1;
        i < 7;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 180.0f,
                m->distancia,
                0.105f,
                1260.0f
            ) -
            100.0f;

        Color tronco =
            corComAlpha(
                (Color){
                    16,
                    46,
                    50,
                    255
                },
                alpha
            );

        DrawRectangleRounded(
            (Rectangle){
                x,
                278.0f,
                25.0f,
                162.0f
            },
            0.35f,
            7,
            tronco
        );

        DrawLineEx(
            (Vector2){
                x + 10.0f,
                310.0f
            },
            (Vector2){
                x - 34.0f,
                273.0f
            },
            11.0f,
            tronco
        );

        DrawLineEx(
            (Vector2){
                x + 17.0f,
                333.0f
            },
            (Vector2){
                x + 55.0f,
                292.0f
            },
            10.0f,
            tronco
        );
    }

    /*
     * Cogumelos luminosos.
     */
    for (
        int i = 0;
        i < 10;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 103.0f,
                m->distancia,
                0.30f,
                1030.0f
            );

        float tamanho =
            6.0f +
            (i % 3) *
            3.0f;

        Color haste =
            corComAlpha(
                (Color){
                    170,
                    220,
                    210,
                    255
                },
                alpha
            );

        Color cogumelo =
            i % 2 == 0
                ? corComAlpha(
                    COR_CIANO,
                    alpha
                )
                : corComAlpha(
                    COR_ROXO,
                    alpha
                );

        Color aura =
            cogumelo;

        aura.a =
            (unsigned char)(
                35.0f *
                alpha
            );

        DrawRectangle(
            (int)x,
            (int)CHAO_Y -
                13,
            3,
            13,
            haste
        );

        DrawCircle(
            (int)x + 1,
            (int)CHAO_Y -
                15,
            tamanho + 5.0f,
            aura
        );

        DrawEllipse(
            (int)x + 1,
            (int)CHAO_Y -
                15,
            tamanho,
            tamanho *
                0.55f,
            cogumelo
        );
    }

    /*
     * Vaga-lumes.
     */
    for (
        int i = 0;
        i < 24;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 67.0f +
                    sinf(
                        m->tempo *
                        1.7f +
                        i
                    ) *
                    15.0f,
                m->distancia,
                0.03f,
                1080.0f
            );

        float y =
            115.0f +
            (float)(
                (i * 47) %
                270
            ) +
            sinf(
                m->tempo *
                    2.0f +
                i
            ) *
                8.0f;

        Color luz =
            corComAlpha(
                (Color){
                    139,
                    255,
                    175,
                    210
                },
                alpha
            );

        DrawCircle(
            (int)x,
            (int)y,
            2.2f,
            luz
        );
    }

    desenhaChaoDimensao(
        m,
        (Color){
            15,
            42,
            43,
            255
        },
        (Color){
            58,
            211,
            140,
            255
        },
        (Color){
            46,
            98,
            71,
            255
        },
        alpha
    );
}

/* ============================================================
 * DIMENSÃO 2: CIDADE NEON
 * ============================================================ */

static void desenhaCidadeNeon(
    const Mundo *m,
    float alpha
) {
    desenhaGradienteVertical(
        (Color){
            13,
            17,
            43,
            255
        },
        (Color){
            68,
            27,
            89,
            255
        },
        alpha
    );

    /*
     * Lua tecnológica.
     */
    Color lua =
        corComAlpha(
            (Color){
                221,
                197,
                255,
                255
            },
            alpha
        );

    Color halo =
        corComAlpha(
            (Color){
                206,
                80,
                255,
                45
            },
            alpha
        );

    DrawCircle(
        785,
        106,
        62.0f,
        halo
    );

    DrawCircle(
        785,
        106,
        34.0f,
        lua
    );

    /*
     * Prédios distantes.
     */
    for (
        int i = -1;
        i < 13;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 82.0f,
                m->distancia,
                0.035f,
                1066.0f
            ) -
            70.0f;

        int largura =
            43 +
            (i % 3) *
            9;

        int altura =
            90 +
            (i * 37 & 95);

        Color predio =
            corComAlpha(
                (Color){
                    26,
                    28,
                    61,
                    255
                },
                alpha
            );

        DrawRectangle(
            (int)x,
            (int)CHAO_Y -
                altura,
            largura,
            altura,
            predio
        );

        for (
            int linha = 0;
            linha < 5;
            linha++
        ) {
            for (
                int coluna = 0;
                coluna < 3;
                coluna++
            ) {
                if (
                    (
                        linha +
                        coluna +
                        i
                    ) %
                    3 ==
                    0
                ) {
                    continue;
                }

                Color janela =
                    (
                        linha +
                        i
                    ) %
                    2 ==
                    0
                        ? COR_CIANO
                        : COR_ROXO;

                janela =
                    corComAlpha(
                        janela,
                        alpha *
                            0.75f
                    );

                DrawRectangle(
                    (int)x +
                        7 +
                        coluna *
                        12,
                    (int)CHAO_Y -
                        altura +
                        12 +
                        linha *
                        15,
                    6,
                    8,
                    janela
                );
            }
        }
    }

    /*
     * Prédios próximos e letreiros sem texto.
     */
    for (
        int i = -1;
        i < 8;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 145.0f,
                m->distancia,
                0.095f,
                1160.0f
            ) -
            110.0f;

        int altura =
            120 +
            (i & 1) *
            75;

        Color predio =
            corComAlpha(
                (Color){
                    19,
                    22,
                    45,
                    255
                },
                alpha
            );

        DrawRectangle(
            (int)x,
            (int)CHAO_Y -
                altura,
            84,
            altura,
            predio
        );

        Color borda =
            i % 2 == 0
                ? corComAlpha(
                    COR_CIANO,
                    alpha
                )
                : corComAlpha(
                    (Color){
                        255,
                        73,
                        200,
                        255
                    },
                    alpha
                );

        DrawRectangleLinesEx(
            (Rectangle){
                x,
                CHAO_Y -
                    altura,
                84.0f,
                (float)altura
            },
            2.0f,
            borda
        );

        DrawRectangle(
            (int)x + 12,
            (int)CHAO_Y -
                altura +
                23,
            59,
            26,
            corComAlpha(
                borda,
                0.28f
            )
        );

        DrawRectangleLinesEx(
            (Rectangle){
                x + 12.0f,
                CHAO_Y -
                    altura +
                    23.0f,
                59.0f,
                26.0f
            },
            2.0f,
            borda
        );
    }

    /*
     * Cabos elevados.
     */
    Color cabo =
        corComAlpha(
            (Color){
                73,
                84,
                116,
                255
            },
            alpha
        );

    for (
        int i = 0;
        i < 4;
        i++
    ) {
        float y =
            188.0f +
            i * 37.0f;

        DrawLineEx(
            (Vector2){
                0.0f,
                y
            },
            (Vector2){
                (float)LARGURA_TELA,
                y +
                    sinf(
                        m->tempo +
                        i
                    ) *
                    5.0f
            },
            2.0f,
            cabo
        );
    }

    desenhaChaoDimensao(
        m,
        (Color){
            20,
            23,
            38,
            255
        },
        (Color){
            69,
            220,
            224,
            255
        },
        (Color){
            180,
            100,
            230,
            255
        },
        alpha
    );
}

/* ============================================================
 * DIMENSÃO 3: MUNDO GLACIAL
 * ============================================================ */

static void desenhaGelo(
    const Mundo *m,
    float alpha
) {
    desenhaGradienteVertical(
        (Color){
            18,
            48,
            93,
            255
        },
        (Color){
            91,
            174,
            197,
            255
        },
        alpha
    );

    /*
     * Aurora boreal.
     */
    for (
        int i = 0;
        i < 5;
        i++
    ) {
        Color aurora =
            i % 2 == 0
                ? (Color){
                    104,
                    255,
                    206,
                    38
                }
                : (Color){
                    145,
                    125,
                    255,
                    34
                };

        aurora =
            corComAlpha(
                aurora,
                alpha
            );

        float y =
            38.0f +
            i * 23.0f;

        for (
            int x = -30;
            x < LARGURA_TELA;
            x += 18
        ) {
            float ondulacao =
                sinf(
                    x * 0.016f +
                    m->tempo *
                        0.55f +
                    i
                ) *
                17.0f;

            DrawLineEx(
                (Vector2){
                    (float)x,
                    y +
                        ondulacao
                },
                (Vector2){
                    (float)x +
                        20.0f,
                    y +
                        ondulacao +
                        7.0f
                },
                9.0f,
                aurora
            );
        }
    }

    /*
     * Montanhas de gelo.
     */
    desenhaMontanhaDimensional(
        m,
        0.023f,
        388,
        175,
        270,
        (Color){
            133,
            195,
            218,
            255
        },
        alpha
    );

    desenhaMontanhaDimensional(
        m,
        0.052f,
        420,
        126,
        210,
        (Color){
            80,
            145,
            180,
            255
        },
        alpha
    );

    /*
     * Neve nos picos.
     */
    Color neve =
        corComAlpha(
            (Color){
                228,
                248,
                255,
                220
            },
            alpha
        );

    float deslocamento =
        fmodf(
            m->distancia *
                0.023f,
            270.0f
        );

    for (
        int i = -2;
        i < 6;
        i++
    ) {
        float x =
            i * 270.0f -
            deslocamento;

        DrawTriangle(
            (Vector2){
                x + 106.0f,
                258.0f
            },
            (Vector2){
                x + 135.0f,
                213.0f
            },
            (Vector2){
                x + 165.0f,
                258.0f
            },
            neve
        );
    }

    /*
     * Cristais próximos.
     */
    for (
        int i = 0;
        i < 9;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 128.0f,
                m->distancia,
                0.23f,
                1152.0f
            );

        float altura =
            31.0f +
            (i % 4) *
            13.0f;

        Color cristal =
            i % 2 == 0
                ? corComAlpha(
                    COR_CIANO,
                    alpha
                )
                : corComAlpha(
                    (Color){
                        150,
                        205,
                        255,
                        255
                    },
                    alpha
                );

        DrawTriangle(
            (Vector2){
                x - 10.0f,
                CHAO_Y
            },
            (Vector2){
                x,
                CHAO_Y -
                    altura
            },
            (Vector2){
                x + 10.0f,
                CHAO_Y
            },
            cristal
        );
    }

    /*
     * Flocos de neve.
     */
    for (
        int i = 0;
        i < 56;
        i++
    ) {
        float cicloY =
            CHAO_Y +
            30.0f;

        float y =
            fmodf(
                i * 43.0f +
                    m->tempo *
                    (
                        31.0f +
                        i % 5
                    ),
                cicloY
            );

        float x =
            fmodf(
                i * 89.0f -
                    m->distancia *
                    0.01f +
                    sinf(
                        m->tempo +
                        i
                    ) *
                    13.0f,
                (float)LARGURA_TELA
            );

        if (x < 0.0f) {
            x +=
                LARGURA_TELA;
        }

        DrawCircle(
            (int)x,
            (int)y,
            i % 4 == 0
                ? 2.0f
                : 1.2f,
            corComAlpha(
                (Color){
                    239,
                    251,
                    255,
                    210
                },
                alpha
            )
        );
    }

    desenhaChaoDimensao(
        m,
        (Color){
            47,
            92,
            120,
            255
        },
        (Color){
            208,
            247,
            255,
            255
        },
        (Color){
            115,
            193,
            214,
            255
        },
        alpha
    );
}

/* ============================================================
 * DIMENSÃO 4: VULCÂNICA
 * ============================================================ */

static void desenhaVulcao(
    const Mundo *m,
    float alpha
) {
    desenhaGradienteVertical(
        (Color){
            30,
            9,
            17,
            255
        },
        (Color){
            135,
            38,
            27,
            255
        },
        alpha
    );

    /*
     * Sol vermelho.
     */
    DrawCircle(
        770,
        111,
        62.0f,
        corComAlpha(
            (Color){
                255,
                66,
                37,
                43
            },
            alpha
        )
    );

    DrawCircle(
        770,
        111,
        37.0f,
        corComAlpha(
            (Color){
                255,
                112,
                52,
                255
            },
            alpha
        )
    );

    /*
     * Vulcões.
     */
    float deslocamento =
        fmodf(
            m->distancia *
                0.034f,
            330.0f
        );

    for (
        int i = -1;
        i < 5;
        i++
    ) {
        float x =
            i * 330.0f -
            deslocamento;

        Color rocha =
            corComAlpha(
                (Color){
                    54,
                    24,
                    27,
                    255
                },
                alpha
            );

        Color lava =
            corComAlpha(
                (Color){
                    255,
                    79,
                    28,
                    255
                },
                alpha
            );

        DrawTriangle(
            (Vector2){
                x,
                420.0f
            },
            (Vector2){
                x + 165.0f,
                212.0f
            },
            (Vector2){
                x + 330.0f,
                420.0f
            },
            rocha
        );

        DrawTriangle(
            (Vector2){
                x + 144.0f,
                244.0f
            },
            (Vector2){
                x + 165.0f,
                212.0f
            },
            (Vector2){
                x + 186.0f,
                244.0f
            },
            lava
        );

        DrawLineEx(
            (Vector2){
                x + 165.0f,
                235.0f
            },
            (Vector2){
                x + 125.0f,
                398.0f
            },
            8.0f,
            lava
        );

        DrawLineEx(
            (Vector2){
                x + 171.0f,
                239.0f
            },
            (Vector2){
                x + 218.0f,
                399.0f
            },
            5.0f,
            lava
        );
    }

    /*
     * Fumaça.
     */
    for (
        int i = 0;
        i < 10;
        i++
    ) {
        float x =
            165.0f +
            (i % 3) *
            330.0f -
            deslocamento;

        float y =
            190.0f -
            fmodf(
                m->tempo *
                    18.0f +
                i * 23.0f,
                130.0f
            );

        Color fumaca =
            corComAlpha(
                (Color){
                    42,
                    30,
                    34,
                    125
                },
                alpha
            );

        DrawCircle(
            (int)x +
                (i % 2) *
                13,
            (int)y,
            15.0f +
                (i % 3) *
                4.0f,
            fumaca
        );
    }

    /*
     * Brasas.
     */
    for (
        int i = 0;
        i < 42;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 71.0f +
                    sinf(
                        m->tempo +
                        i
                    ) *
                    14.0f,
                m->distancia,
                0.015f,
                1040.0f
            );

        float y =
            CHAO_Y -
            fmodf(
                i * 37.0f +
                    m->tempo *
                    (
                        27.0f +
                        i % 7
                    ),
                330.0f
            );

        DrawCircle(
            (int)x,
            (int)y,
            i % 4 == 0
                ? 2.4f
                : 1.5f,
            corComAlpha(
                i % 2 == 0
                    ? COR_LARANJA
                    : COR_AMARELO,
                alpha
            )
        );
    }

    desenhaChaoDimensao(
        m,
        (Color){
            38,
            27,
            29,
            255
        },
        (Color){
            255,
            79,
            28,
            255
        },
        (Color){
            132,
            50,
            35,
            255
        },
        alpha
    );

    /*
     * Rachaduras de lava.
     */
    Color rachadura =
        corComAlpha(
            (Color){
                255,
                100,
                29,
                255
            },
            alpha
        );

    for (
        int i = 0;
        i < 12;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 93.0f,
                m->distancia,
                1.1f,
                1116.0f
            );

        DrawLineEx(
            (Vector2){
                x,
                CHAO_Y + 17.0f
            },
            (Vector2){
                x + 12.0f,
                CHAO_Y + 29.0f
            },
            2.0f,
            rachadura
        );

        DrawLineEx(
            (Vector2){
                x + 12.0f,
                CHAO_Y + 29.0f
            },
            (Vector2){
                x + 5.0f,
                CHAO_Y + 42.0f
            },
            2.0f,
            rachadura
        );
    }
}

/* ============================================================
 * DIMENSÃO 5: CIDADE SUBMERSA
 * ============================================================ */

static void desenhaSubmerso(
    const Mundo *m,
    float alpha
) {
    desenhaGradienteVertical(
        (Color){
            5,
            40,
            70,
            255
        },
        (Color){
            16,
            108,
            115,
            255
        },
        alpha
    );

    /*
     * Raios de luz.
     */
    for (
        int i = 0;
        i < 5;
        i++
    ) {
        Color raio =
            corComAlpha(
                (Color){
                    177,
                    255,
                    235,
                    22
                },
                alpha
            );

        DrawTriangle(
            (Vector2){
                90.0f +
                    i * 220.0f,
                0.0f
            },
            (Vector2){
                180.0f +
                    i * 220.0f,
                0.0f
            },
            (Vector2){
                260.0f +
                    i * 220.0f,
                CHAO_Y
            },
            raio
        );
    }

    /*
     * Ruínas submersas.
     */
    for (
        int i = -1;
        i < 7;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 185.0f,
                m->distancia,
                0.075f,
                1295.0f
            ) -
            110.0f;

        int altura =
            76 +
            (i & 1) *
            51;

        Color pedra =
            corComAlpha(
                (Color){
                    31,
                    78,
                    86,
                    255
                },
                alpha
            );

        DrawRectangle(
            (int)x,
            (int)CHAO_Y -
                altura,
            21,
            altura,
            pedra
        );

        DrawRectangle(
            (int)x - 9,
            (int)CHAO_Y -
                altura,
            39,
            8,
            pedra
        );

        DrawRectangle(
            (int)x - 6,
            (int)CHAO_Y - 10,
            33,
            10,
            pedra
        );

        if (i % 2 == 0) {
            DrawLineEx(
                (Vector2){
                    x + 4.0f,
                    CHAO_Y -
                        altura +
                        11.0f
                },
                (Vector2){
                    x + 12.0f,
                    CHAO_Y -
                        altura +
                        31.0f
                },
                2.0f,
                corComAlpha(
                    (Color){
                        75,
                        151,
                        145,
                        255
                    },
                    alpha
                )
            );
        }
    }

    /*
     * Algas.
     */
    for (
        int i = 0;
        i < 20;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 61.0f,
                m->distancia,
                0.20f,
                1220.0f
            );

        float altura =
            23.0f +
            (i % 5) *
            8.0f;

        Color alga =
            corComAlpha(
                i % 2 == 0
                    ? (Color){
                        45,
                        163,
                        118,
                        255
                    }
                    : (Color){
                        37,
                        126,
                        119,
                        255
                    },
                alpha
            );

        Vector2 anterior = {
            x,
            CHAO_Y
        };

        for (
            int parte = 1;
            parte <= 4;
            parte++
        ) {
            float progresso =
                (float)parte /
                4.0f;

            Vector2 atual = {
                x +
                    sinf(
                        m->tempo *
                            1.7f +
                        i +
                        parte
                    ) *
                    5.0f *
                    progresso,
                CHAO_Y -
                    altura *
                    progresso
            };

            DrawLineEx(
                anterior,
                atual,
                4.0f,
                alga
            );

            anterior =
                atual;
        }
    }

    /*
     * Peixes distantes.
     */
    for (
        int i = 0;
        i < 9;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 151.0f,
                m->distancia,
                0.035f,
                1360.0f
            );

        float y =
            112.0f +
            (i * 59) %
                240;

        Color peixe =
            corComAlpha(
                (Color){
                    105,
                    185,
                    180,
                    120
                },
                alpha
            );

        DrawEllipse(
            (int)x,
            (int)y,
            12.0f,
            5.0f,
            peixe
        );

        DrawTriangle(
            (Vector2){
                x + 10.0f,
                y
            },
            (Vector2){
                x + 19.0f,
                y - 7.0f
            },
            (Vector2){
                x + 19.0f,
                y + 7.0f
            },
            peixe
        );
    }

    /*
     * Bolhas.
     */
    for (
        int i = 0;
        i < 44;
        i++
    ) {
        float x =
            (float)(
                (i * 83 + 29) %
                LARGURA_TELA
            ) +
            sinf(
                m->tempo +
                i
            ) *
                5.0f;

        float y =
            CHAO_Y -
            fmodf(
                i * 47.0f +
                    m->tempo *
                    (
                        22.0f +
                        i % 7
                    ),
                CHAO_Y +
                    20.0f
            );

        float raio =
            1.5f +
            (i % 4) *
            0.8f;

        DrawCircleLines(
            (int)x,
            (int)y,
            raio,
            corComAlpha(
                (Color){
                    195,
                    246,
                    242,
                    155
                },
                alpha
            )
        );
    }

    desenhaChaoDimensao(
        m,
        (Color){
            31,
            72,
            72,
            255
        },
        (Color){
            101,
            204,
            179,
            255
        },
        (Color){
            71,
            118,
            99,
            255
        },
        alpha
    );
}

/* ============================================================
 * DIMENSÃO 6: VAZIO CÓSMICO
 * ============================================================ */

static void desenhaEspaco(
    const Mundo *m,
    float alpha
) {
    desenhaGradienteVertical(
        (Color){
            4,
            6,
            24,
            255
        },
        (Color){
            27,
            18,
            67,
            255
        },
        alpha
    );

    /*
     * Nebulosas.
     */
    for (
        int i = 0;
        i < 5;
        i++
    ) {
        float x =
            110.0f +
            i * 225.0f -
            fmodf(
                m->distancia *
                    0.008f,
                180.0f
            );

        float y =
            90.0f +
            (i % 3) *
            82.0f;

        Color nebulosa =
            i % 2 == 0
                ? (Color){
                    145,
                    65,
                    220,
                    28
                }
                : (Color){
                    53,
                    165,
                    225,
                    25
                };

        nebulosa =
            corComAlpha(
                nebulosa,
                alpha
            );

        DrawCircle(
            (int)x,
            (int)y,
            75.0f,
            nebulosa
        );

        DrawCircle(
            (int)x + 45,
            (int)y + 15,
            54.0f,
            nebulosa
        );
    }

    /*
     * Estrelas com paralaxe.
     */
    for (
        int camada = 0;
        camada < 3;
        camada++
    ) {
        for (
            int i = 0;
            i < 34;
            i++
        ) {
            float velocidade =
                0.008f +
                camada *
                0.012f;

            float x =
                deslocamentoCiclico(
                    i * 89.0f +
                        camada *
                        37.0f,
                    m->distancia,
                    velocidade,
                    1100.0f
                );

            float y =
                25.0f +
                (float)(
                    (
                        i * 53 +
                        camada * 29
                    ) %
                    390
                );

            float raio =
                0.8f +
                camada *
                0.65f;

            DrawCircle(
                (int)x,
                (int)y,
                raio,
                corComAlpha(
                    camada == 2
                        ? COR_CIANO
                        : WHITE,
                    alpha *
                        (
                            0.65f +
                            camada *
                            0.12f
                        )
                )
            );
        }
    }

    /*
     * Planetas.
     */
    DrawCircle(
        770,
        112,
        54.0f,
        corComAlpha(
            (Color){
                76,
                83,
                190,
                255
            },
            alpha
        )
    );

    DrawCircle(
        753,
        97,
        22.0f,
        corComAlpha(
            (Color){
                106,
                158,
                225,
                120
            },
            alpha
        )
    );

    DrawEllipseLines(
        770,
        112,
        82.0f,
        19.0f,
        corComAlpha(
            (Color){
                219,
                175,
                255,
                190
            },
            alpha
        )
    );

    /*
     * Ilhas rochosas flutuantes.
     */
    for (
        int i = -1;
        i < 7;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 178.0f,
                m->distancia,
                0.08f,
                1246.0f
            ) -
            120.0f;

        float y =
            328.0f +
            (i % 3) *
            29.0f;

        Color rocha =
            corComAlpha(
                (Color){
                    49,
                    45,
                    82,
                    255
                },
                alpha
            );

        DrawEllipse(
            (int)x,
            (int)y,
            77.0f,
            21.0f,
            rocha
        );

        DrawTriangle(
            (Vector2){
                x - 54.0f,
                y + 5.0f
            },
            (Vector2){
                x,
                y + 76.0f
            },
            (Vector2){
                x + 53.0f,
                y + 5.0f
            },
            rocha
        );

        DrawCircle(
            (int)x,
            (int)y - 4,
            5.0f,
            corComAlpha(
                COR_ROXO,
                alpha
            )
        );
    }

    /*
     * Meteoros.
     */
    for (
        int i = 0;
        i < 5;
        i++
    ) {
        float ciclo =
            LARGURA_TELA +
            300.0f;

        float x =
            fmodf(
                i * 271.0f +
                    m->tempo *
                    63.0f,
                ciclo
            ) -
            100.0f;

        float y =
            63.0f +
            i * 58.0f +
            x * 0.08f;

        Color rastro =
            corComAlpha(
                (Color){
                    144,
                    185,
                    255,
                    110
                },
                alpha
            );

        DrawLineEx(
            (Vector2){
                x - 38.0f,
                y - 17.0f
            },
            (Vector2){
                x,
                y
            },
            3.0f,
            rastro
        );

        DrawCircle(
            (int)x,
            (int)y,
            3.0f,
            corComAlpha(
                WHITE,
                alpha
            )
        );
    }

    desenhaChaoDimensao(
        m,
        (Color){
            25,
            24,
            47,
            255
        },
        (Color){
            139,
            98,
            235,
            255
        },
        (Color){
            71,
            89,
            139,
            255
        },
        alpha
    );

    /*
     * Cristais espaciais no chão.
     */
    for (
        int i = 0;
        i < 11;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 97.0f,
                m->distancia,
                1.0f,
                1067.0f
            );

        Color cristal =
            corComAlpha(
                i % 2 == 0
                    ? COR_ROXO
                    : COR_CIANO,
                alpha
            );

        DrawTriangle(
            (Vector2){
                x - 5.0f,
                CHAO_Y + 4.0f
            },
            (Vector2){
                x,
                CHAO_Y - 14.0f
            },
            (Vector2){
                x + 5.0f,
                CHAO_Y + 4.0f
            },
            cristal
        );
    }
}

/* ============================================================
 * DIMENSÃO 7: INFERNO LÓGICO
 * ============================================================ */

static void desenhaInferno(
    const Mundo *m,
    float alpha
) {
    desenhaGradienteVertical(
        (Color){
            15,
            2,
            11,
            255
        },
        (Color){
            96,
            14,
            21,
            255
        },
        alpha
    );

    /*
     * Lua eclipsada.
     */
    DrawCircle(
        780,
        106,
        56.0f,
        corComAlpha(
            (Color){
                255,
                45,
                25,
                44
            },
            alpha
        )
    );

    DrawCircle(
        780,
        106,
        36.0f,
        corComAlpha(
            (Color){
                202,
                34,
                31,
                255
            },
            alpha
        )
    );

    DrawCircle(
        770,
        99,
        30.0f,
        corComAlpha(
            (Color){
                25,
                7,
                18,
                255
            },
            alpha
        )
    );

    /*
     * Castelo distante.
     */
    Color castelo =
        corComAlpha(
            (Color){
                30,
                13,
                25,
                255
            },
            alpha
        );

    float casteloX =
        670.0f -
        fmodf(
            m->distancia *
                0.015f,
            90.0f
        );

    DrawRectangle(
        (int)casteloX,
        235,
        192,
        205,
        castelo
    );

    for (
        int i = 0;
        i < 4;
        i++
    ) {
        float x =
            casteloX +
            i * 55.0f;

        DrawRectangle(
            (int)x,
            183 +
                (i % 2) *
                31,
            32,
            257 -
                (i % 2) *
                31,
            castelo
        );

        DrawTriangle(
            (Vector2){
                x - 5.0f,
                184.0f +
                    (i % 2) *
                    31.0f
            },
            (Vector2){
                x + 16.0f,
                142.0f +
                    (i % 2) *
                    31.0f
            },
            (Vector2){
                x + 37.0f,
                184.0f +
                    (i % 2) *
                    31.0f
            },
            castelo
        );

        DrawRectangle(
            (int)x + 12,
            250 +
                (i % 2) *
                19,
            7,
            18,
            corComAlpha(
                COR_VERMELHO,
                alpha *
                    0.75f
            )
        );
    }

    /*
     * Pilares e correntes.
     */
    for (
        int i = -1;
        i < 7;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 190.0f,
                m->distancia,
                0.11f,
                1330.0f
            ) -
            100.0f;

        Color pedra =
            corComAlpha(
                (Color){
                    45,
                    25,
                    35,
                    255
                },
                alpha
            );

        DrawRectangle(
            (int)x,
            267,
            24,
            173,
            pedra
        );

        DrawRectangle(
            (int)x - 9,
            267,
            42,
            9,
            pedra
        );

        Color corrente =
            corComAlpha(
                (Color){
                    102,
                    74,
                    79,
                    255
                },
                alpha
            );

        for (
            int elo = 0;
            elo < 5;
            elo++
        ) {
            float correnteX =
                x +
                12.0f +
                sinf(
                    m->tempo +
                    elo
                ) *
                    3.0f;

            DrawCircleLines(
                (int)correnteX,
                289 +
                    elo *
                    17,
                6.0f,
                corrente
            );
        }
    }

    /*
     * Símbolos lógicos flutuantes.
     */
    static const char *simbolos[] = {
        "~",
        "&",
        "|",
        "->",
        "<->"
    };

    for (
        int i = 0;
        i < 13;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 97.0f,
                m->distancia,
                0.018f,
                1261.0f
            );

        float y =
            72.0f +
            (i * 71) %
                290 +
            sinf(
                m->tempo *
                    1.3f +
                i
            ) *
                10.0f;

        Color simbolo =
            corComAlpha(
                i % 2 == 0
                    ? COR_VERMELHO
                    : COR_LARANJA,
                alpha *
                    0.67f
            );

        DrawText(
            simbolos[
                i %
                5
            ],
            (int)x,
            (int)y,
            19,
            simbolo
        );
    }

    /*
     * Brasas.
     */
    for (
        int i = 0;
        i < 54;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 61.0f +
                    sinf(
                        m->tempo *
                            0.8f +
                        i
                    ) *
                    13.0f,
                m->distancia,
                0.01f,
                1120.0f
            );

        float y =
            CHAO_Y -
            fmodf(
                i * 31.0f +
                    m->tempo *
                    (
                        32.0f +
                        i % 8
                    ),
                390.0f
            );

        DrawCircle(
            (int)x,
            (int)y,
            i % 4 == 0
                ? 2.8f
                : 1.4f,
            corComAlpha(
                i % 3 == 0
                    ? COR_AMARELO
                    : COR_LARANJA,
                alpha
            )
        );
    }

    desenhaChaoDimensao(
        m,
        (Color){
            35,
            22,
            27,
            255
        },
        (Color){
            255,
            58,
            35,
            255
        },
        (Color){
            120,
            42,
            39,
            255
        },
        alpha
    );

    /*
     * Chamas no fundo do chão.
     */
    for (
        int i = 0;
        i < 18;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 59.0f,
                m->distancia,
                0.42f,
                1062.0f
            );

        float altura =
            17.0f +
            fabsf(
                sinf(
                    m->tempo *
                        5.0f +
                    i
                )
            ) *
                24.0f;

        DrawTriangle(
            (Vector2){
                x - 8.0f,
                CHAO_Y
            },
            (Vector2){
                x,
                CHAO_Y -
                    altura
            },
            (Vector2){
                x + 8.0f,
                CHAO_Y
            },
            corComAlpha(
                COR_LARANJA,
                alpha *
                    0.78f
            )
        );

        DrawTriangle(
            (Vector2){
                x - 4.0f,
                CHAO_Y
            },
            (Vector2){
                x,
                CHAO_Y -
                    altura *
                    0.62f
            },
            (Vector2){
                x + 4.0f,
                CHAO_Y
            },
            corComAlpha(
                COR_AMARELO,
                alpha *
                    0.88f
            )
        );
    }
}

/* ============================================================
 * DIMENSÃO 8: REINO CELESTIAL
 * ============================================================ */

static void desenhaCeuFinal(
    const Mundo *m,
    float alpha
) {
    desenhaGradienteVertical(
        (Color){
            65,
            135,
            207,
            255
        },
        (Color){
            178,
            232,
            225,
            255
        },
        alpha
    );

    /*
     * Sol dourado.
     */
    DrawCircle(
        785,
        94,
        82.0f,
        corComAlpha(
            (Color){
                255,
                237,
                137,
                62
            },
            alpha
        )
    );

    DrawCircle(
        785,
        94,
        45.0f,
        corComAlpha(
            (Color){
                255,
                244,
                174,
                255
            },
            alpha
        )
    );

    /*
     * Raios celestiais.
     */
    for (
        int i = 0;
        i < 7;
        i++
    ) {
        DrawTriangle(
            (Vector2){
                700.0f +
                    i * 22.0f,
                100.0f
            },
            (Vector2){
                730.0f +
                    i * 45.0f,
                CHAO_Y
            },
            (Vector2){
                770.0f +
                    i * 45.0f,
                CHAO_Y
            },
            corComAlpha(
                (Color){
                    255,
                    244,
                    180,
                    19
                },
                alpha
            )
        );
    }

    /*
     * Nuvens distantes.
     */
    for (
        int i = -1;
        i < 8;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 180.0f,
                m->distancia,
                0.025f,
                1440.0f
            ) -
            100.0f;

        float y =
            155.0f +
            (i % 3) *
            42.0f;

        Color sombra =
            corComAlpha(
                (Color){
                    122,
                    181,
                    200,
                    110
                },
                alpha
            );

        Color nuvem =
            corComAlpha(
                (Color){
                    241,
                    251,
                    247,
                    210
                },
                alpha
            );

        DrawEllipse(
            (int)x + 31,
            (int)y + 12,
            51.0f,
            13.0f,
            sombra
        );

        DrawCircle(
            (int)x,
            (int)y,
            19.0f,
            nuvem
        );

        DrawCircle(
            (int)x + 26,
            (int)y - 10,
            28.0f,
            nuvem
        );

        DrawCircle(
            (int)x + 59,
            (int)y,
            23.0f,
            nuvem
        );

        DrawRectangle(
            (int)x,
            (int)y,
            60,
            22,
            nuvem
        );
    }

    /*
     * Ilhas flutuantes.
     */
    for (
        int i = -1;
        i < 7;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 205.0f,
                m->distancia,
                0.072f,
                1435.0f
            ) -
            120.0f;

        float y =
            321.0f +
            (i % 3) *
            26.0f;

        Color grama =
            corComAlpha(
                (Color){
                    122,
                    205,
                    151,
                    255
                },
                alpha
            );

        Color pedra =
            corComAlpha(
                (Color){
                    106,
                    136,
                    147,
                    255
                },
                alpha
            );

        DrawEllipse(
            (int)x,
            (int)y,
            84.0f,
            21.0f,
            grama
        );

        DrawTriangle(
            (Vector2){
                x - 65.0f,
                y + 7.0f
            },
            (Vector2){
                x,
                y + 86.0f
            },
            (Vector2){
                x + 65.0f,
                y + 7.0f
            },
            pedra
        );

        /*
         * Templo sobre algumas ilhas.
         */
        if (i % 2 == 0) {
            Color templo =
                corComAlpha(
                    (Color){
                        237,
                        235,
                        215,
                        255
                    },
                    alpha
                );

            DrawRectangle(
                (int)x - 38,
                (int)y - 54,
                76,
                9,
                templo
            );

            for (
                int coluna = 0;
                coluna < 4;
                coluna++
            ) {
                DrawRectangle(
                    (int)x -
                        31 +
                        coluna *
                        20,
                    (int)y - 45,
                    7,
                    45,
                    templo
                );
            }

            DrawTriangle(
                (Vector2){
                    x - 46.0f,
                    y - 54.0f
                },
                (Vector2){
                    x,
                    y - 85.0f
                },
                (Vector2){
                    x + 46.0f,
                    y - 54.0f
                },
                templo
            );
        }
    }

    /*
     * Partículas douradas.
     */
    for (
        int i = 0;
        i < 45;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 79.0f +
                    sinf(
                        m->tempo +
                        i
                    ) *
                    11.0f,
                m->distancia,
                0.012f,
                1100.0f
            );

        float y =
            35.0f +
            (i * 61) %
                380 +
            sinf(
                m->tempo *
                    1.4f +
                i
            ) *
                8.0f;

        DrawCircle(
            (int)x,
            (int)y,
            i % 4 == 0
                ? 2.5f
                : 1.4f,
            corComAlpha(
                (Color){
                    255,
                    230,
                    118,
                    210
                },
                alpha
            )
        );
    }

    desenhaChaoDimensao(
        m,
        (Color){
            48,
            74,
            80,
            255
        },
        (Color){
            255,
            221,
            105,
            255
        },
        (Color){
            105,
            157,
            145,
            255
        },
        alpha
    );

    /*
     * Faixa de nuvens abaixo do chão.
     */
    Color nuvemBase =
        corComAlpha(
            (Color){
                230,
                246,
                240,
                170
            },
            alpha
        );

    for (
        int i = -1;
        i < 12;
        i++
    ) {
        float x =
            deslocamentoCiclico(
                i * 95.0f,
                m->distancia,
                0.26f,
                1235.0f
            ) -
            70.0f;

        DrawCircle(
            (int)x,
            (int)CHAO_Y + 48,
            41.0f,
            nuvemBase
        );
    }
}

/* ============================================================
 * SELEÇÃO E TRANSIÇÃO ENTRE DIMENSÕES
 * ============================================================ */

static void desenhaDimensao(
    const Mundo *m,
    int dimensao,
    float alpha
) {
    switch (dimensao) {
        case 0:
            desenhaDeserto(
                m,
                alpha
            );
            break;

        case 1:
            desenhaFloresta(
                m,
                alpha
            );
            break;

        case 2:
            desenhaCidadeNeon(
                m,
                alpha
            );
            break;

        case 3:
            desenhaGelo(
                m,
                alpha
            );
            break;

        case 4:
            desenhaVulcao(
                m,
                alpha
            );
            break;

        case 5:
            desenhaSubmerso(
                m,
                alpha
            );
            break;

        case 6:
            desenhaEspaco(
                m,
                alpha
            );
            break;

        case 7:
            desenhaInferno(
                m,
                alpha
            );
            break;

        default:
            desenhaCeuFinal(
                m,
                alpha
            );
            break;
    }
}

static void desenhaCenario(
    const Mundo *m
) {
    int dimensao =
        m->portoesOk;

    if (dimensao < 0) {
        dimensao = 0;
    }

    if (
        dimensao >
        META_PORTOES
    ) {
        dimensao =
            META_PORTOES;
    }

    bool transicaoAtiva =
        dimensao > 0 &&
        m->msgTempo > 0.0f &&
        strstr(
            m->msg,
            "PORTAO ABERTO"
        ) != NULL;

    if (!transicaoAtiva) {
        desenhaDimensao(
            m,
            dimensao,
            1.0f
        );

        return;
    }

    /*
     * A dimensão nova é desenhada primeiro.
     */
    desenhaDimensao(
        m,
        dimensao,
        1.0f
    );

    /*
     * A dimensão anterior desaparece gradualmente.
     */
    float progresso =
        1.0f -
        m->msgTempo /
        1.8f;

    progresso =
        suavizaTransicao(
            progresso
        );

    desenhaDimensao(
        m,
        dimensao - 1,
        1.0f -
            progresso
    );

    /*
     * Clarão no momento da passagem.
     */
    float centro =
        1.0f -
        fabsf(
            progresso -
            0.5f
        ) *
        2.0f;

    Color clarão =
        {
            225,
            244,
            255,
            (unsigned char)(
                60.0f *
                centro
            )
        };

    DrawRectangle(
        0,
        0,
        LARGURA_TELA,
        ALTURA_TELA,
        clarão
    );
}

/* ============================================================
 * JOGADOR
 * ============================================================ */

static void desenhaTurbo(Rectangle r) {
    DrawTriangle(
        (Vector2){
            r.x + 2.0f,
            r.y + r.height * 0.45f
        },
        (Vector2){
            r.x - 31.0f,
            r.y + r.height * 0.52f
        },
        (Vector2){
            r.x + 2.0f,
            r.y + r.height * 0.64f
        },
        COR_LARANJA
    );

    DrawTriangle(
        (Vector2){
            r.x + 1.0f,
            r.y + r.height * 0.49f
        },
        (Vector2){
            r.x - 21.0f,
            r.y + r.height * 0.53f
        },
        (Vector2){
            r.x + 1.0f,
            r.y + r.height * 0.59f
        },
        COR_AMARELO
    );
}

static float progressoComemoracao(
    const Mundo *m
) {
    if (!m->sequenciaVitoria) {
        return 0.0f;
    }

    float progresso =
        (
            m->tempoSequenciaVitoria -
            TEMPO_CORRIDA_VITORIA
        ) /
        TEMPO_COMEMORACAO_VITORIA;

    return limita01(progresso);
}

static void desenhaDinossauroAbaixado(
    Rectangle r
) {
    const Color corpo =
        COR_VERDE;

    const Color escuro =
        COR_VERDE_ESCURO;

    const Color barriga =
        { 119, 228, 162, 255 };

    const Color contorno =
        { 20, 70, 57, 255 };

    /*
     * Cauda mais fina e alongada.
     */
    DrawTriangle(
        (Vector2){
            r.x + 10.0f,
            r.y + 12.0f
        },
        (Vector2){
            r.x - 35.0f,
            r.y + 19.0f
        },
        (Vector2){
            r.x + 11.0f,
            r.y + 23.0f
        },
        escuro
    );

    DrawTriangle(
        (Vector2){
            r.x + 8.0f,
            r.y + 14.0f
        },
        (Vector2){
            r.x - 29.0f,
            r.y + 19.0f
        },
        (Vector2){
            r.x + 9.0f,
            r.y + 20.0f
        },
        corpo
    );

    /*
     * Corpo horizontal mais estreito.
     */
    DrawEllipse(
        (int)(r.x + 18.0f),
        (int)(r.y + 17.0f),
        23.0f,
        12.5f,
        contorno
    );

    DrawEllipse(
        (int)(r.x + 19.0f),
        (int)(r.y + 15.5f),
        21.0f,
        10.5f,
        corpo
    );

    /*
     * Barriga menor.
     */
    DrawEllipse(
        (int)(r.x + 23.0f),
        (int)(r.y + 18.0f),
        8.0f,
        7.0f,
        barriga
    );

    /*
     * Pescoço.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x + 22.0f,
            r.y + 4.0f,
            17.0f,
            19.0f
        },
        0.35f,
        6,
        corpo
    );

    /*
     * Cabeça.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x + 30.0f,
            r.y + 2.0f,
            28.0f,
            18.0f
        },
        0.28f,
        6,
        corpo
    );

    /*
     * Focinho.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x + 43.0f,
            r.y + 8.0f,
            21.0f,
            12.0f
        },
        0.30f,
        5,
        corpo
    );

    /*
     * Mandíbula.
     */
    DrawLineEx(
        (Vector2){
            r.x + 46.0f,
            r.y + 18.0f
        },
        (Vector2){
            r.x + 61.0f,
            r.y + 18.0f
        },
        1.5f,
        contorno
    );

    /*
     * Olho.
     */
    DrawCircle(
        (int)r.x + 45,
        (int)r.y + 7,
        4.5f,
        WHITE
    );

    DrawCircle(
        (int)r.x + 47,
        (int)r.y + 7,
        2.2f,
        COR_PAINEL
    );

    /*
     * Narina.
     */
    DrawCircle(
        (int)r.x + 59,
        (int)r.y + 13,
        1.5f,
        contorno
    );

    /*
     * Braço curto.
     */
    DrawLineEx(
        (Vector2){
            r.x + 28.0f,
            r.y + 17.0f
        },
        (Vector2){
            r.x + 39.0f,
            r.y + 22.0f
        },
        3.0f,
        escuro
    );

    /*
     * Pernas recolhidas.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x + 8.0f,
            r.y + 22.0f,
            13.0f,
            6.0f
        },
        0.35f,
        4,
        escuro
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x + 25.0f,
            r.y + 22.0f,
            13.0f,
            6.0f
        },
        0.35f,
        4,
        corpo
    );
}

static void desenhaDinossauroEmPe(
    const Mundo *m,
    Rectangle r,
    bool comemorando
) {
    const Color corpo =
        COR_VERDE;

    const Color escuro =
        COR_VERDE_ESCURO;

    const Color barriga =
        { 119, 228, 162, 255 };

    const Color contorno =
        { 20, 70, 57, 255 };

    float fase =
        m->tempo *
        13.0f;

    float oscilacao =
        m->noChao
            ? sinf(fase) * 1.4f
            : 0.0f;

    float corpoY =
        r.y +
        31.0f +
        oscilacao;

    float comemoracao =
        progressoComemoracao(m);

    float saltoVisual =
        0.0f;

    if (comemorando) {
        saltoVisual =
            sinf(
                comemoracao *
                PI
            ) *
            34.0f;

        corpoY -=
            saltoVisual;
    }

    /*
     * Sombra.
     */
    float larguraSombra =
        comemorando
            ? 25.0f -
                saltoVisual *
                0.18f
            : (
                !m->noChao
                    ? 18.0f
                    : 32.0f
            );

    if (larguraSombra < 9.0f) {
        larguraSombra =
            9.0f;
    }

    DrawEllipse(
        (int)(r.x + 18.0f),
        (int)CHAO_Y + 3,
        larguraSombra,
        5.0f,
        (Color){
            15,
            22,
            31,
            145
        }
    );

    /*
     * Movimento da cauda.
     */
    float movimentoCauda =
        comemorando
            ? sinf(
                comemoracao *
                PI *
                5.0f
            ) *
                7.0f
            : sinf(
                fase *
                0.55f
            ) *
                3.0f;

    /*
     * Cauda mais fina.
     */
    DrawTriangle(
        (Vector2){
            r.x + 10.0f,
            corpoY - 5.0f
        },
        (Vector2){
            r.x - 39.0f,
            corpoY +
                movimentoCauda
        },
        (Vector2){
            r.x + 10.0f,
            corpoY + 10.0f
        },
        escuro
    );

    DrawTriangle(
        (Vector2){
            r.x + 8.0f,
            corpoY - 2.0f
        },
        (Vector2){
            r.x - 33.0f,
            corpoY +
                movimentoCauda
        },
        (Vector2){
            r.x + 9.0f,
            corpoY + 6.0f
        },
        corpo
    );

    /*
     * Corpo mais fino.
     *
     * Antes ele usava aproximadamente 26 x 27.
     * Agora utiliza 20 x 26.
     */
    DrawEllipse(
        (int)(r.x + 17.0f),
        (int)corpoY,
        21.0f,
        27.0f,
        contorno
    );

    DrawEllipse(
        (int)(r.x + 18.0f),
        (int)(corpoY - 2.0f),
        19.0f,
        25.0f,
        corpo
    );

    /*
     * Barriga menor.
     */
    DrawEllipse(
        (int)(r.x + 22.0f),
        (int)(corpoY + 5.0f),
        8.0f,
        15.0f,
        barriga
    );

    /*
     * Pescoço mais fino.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x + 21.0f,
            corpoY - 34.0f,
            16.0f,
            40.0f
        },
        0.42f,
        7,
        corpo
    );

    /*
     * Conexão entre corpo e pescoço.
     */
    DrawTriangle(
        (Vector2){
            r.x + 19.0f,
            corpoY - 4.0f
        },
        (Vector2){
            r.x + 35.0f,
            corpoY - 34.0f
        },
        (Vector2){
            r.x + 37.0f,
            corpoY + 1.0f
        },
        corpo
    );

    /*
     * Cabeça.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x + 24.0f,
            corpoY - 46.0f,
            33.0f,
            27.0f
        },
        0.30f,
        7,
        corpo
    );

    /*
     * Focinho.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x + 39.0f,
            corpoY - 36.0f,
            27.0f,
            17.0f
        },
        0.27f,
        6,
        corpo
    );

    /*
     * Mandíbula.
     */
    DrawLineEx(
        (Vector2){
            r.x + 43.0f,
            corpoY - 21.0f
        },
        (Vector2){
            r.x + 64.0f,
            corpoY - 21.0f
        },
        1.8f,
        contorno
    );

    /*
     * Olho.
     */
    DrawCircle(
        (int)r.x + 44,
        (int)(corpoY - 38.0f),
        5.2f,
        WHITE
    );

    DrawCircle(
        (int)r.x + 46,
        (int)(corpoY - 38.0f),
        2.6f,
        COR_PAINEL
    );

    /*
     * Sobrancelha.
     */
    DrawLineEx(
        (Vector2){
            r.x + 38.0f,
            corpoY - 44.0f
        },
        (Vector2){
            r.x + 49.0f,
            corpoY - 43.0f
        },
        2.0f,
        contorno
    );

    /*
     * Narina.
     */
    DrawCircle(
        (int)r.x + 60,
        (int)(corpoY - 30.0f),
        1.7f,
        contorno
    );

    /*
     * Braços.
     */
    float bracoFrenteY =
        comemorando
            ? corpoY - 28.0f
            : corpoY - 11.0f;

    float bracoTrasY =
        comemorando
            ? corpoY - 25.0f
            : corpoY - 7.0f;

    DrawLineEx(
        (Vector2){
            r.x + 27.0f,
            corpoY - 10.0f
        },
        (Vector2){
            r.x + 41.0f,
            bracoFrenteY
        },
        3.5f,
        corpo
    );

    DrawLineEx(
        (Vector2){
            r.x + 41.0f,
            bracoFrenteY
        },
        (Vector2){
            r.x + 48.0f,
            bracoFrenteY - 2.0f
        },
        2.3f,
        corpo
    );

    DrawLineEx(
        (Vector2){
            r.x + 19.0f,
            corpoY - 8.0f
        },
        (Vector2){
            r.x + 9.0f,
            bracoTrasY
        },
        3.5f,
        escuro
    );

    /*
     * Pernas durante salto ou comemoração.
     */
    if (
        !m->noChao ||
        comemorando
    ) {
        DrawLineEx(
            (Vector2){
                r.x + 9.0f,
                corpoY + 17.0f
            },
            (Vector2){
                r.x + 3.0f,
                corpoY + 30.0f
            },
            7.5f,
            escuro
        );

        DrawLineEx(
            (Vector2){
                r.x + 3.0f,
                corpoY + 30.0f
            },
            (Vector2){
                r.x + 15.0f,
                corpoY + 34.0f
            },
            5.5f,
            escuro
        );

        DrawLineEx(
            (Vector2){
                r.x + 25.0f,
                corpoY + 16.0f
            },
            (Vector2){
                r.x + 33.0f,
                corpoY + 28.0f
            },
            7.5f,
            corpo
        );

        DrawLineEx(
            (Vector2){
                r.x + 33.0f,
                corpoY + 28.0f
            },
            (Vector2){
                r.x + 45.0f,
                corpoY + 24.0f
            },
            5.5f,
            corpo
        );
    } else {
        /*
         * Pernas durante a corrida.
         */
        float ciclo =
            sinf(fase);

        float frente =
            ciclo *
            7.0f;

        float tras =
            -ciclo *
            7.0f;

        float joelhoY =
            r.y +
            r.height -
            14.0f;

        float peY =
            r.y +
            r.height +
            1.0f;

        DrawLineEx(
            (Vector2){
                r.x + 10.0f,
                corpoY + 17.0f
            },
            (Vector2){
                r.x + 8.0f +
                    tras,
                joelhoY
            },
            7.5f,
            escuro
        );

        DrawLineEx(
            (Vector2){
                r.x + 8.0f +
                    tras,
                joelhoY
            },
            (Vector2){
                r.x + 11.0f -
                    tras *
                    0.45f,
                peY
            },
            5.5f,
            escuro
        );

        DrawLineEx(
            (Vector2){
                r.x + 8.0f -
                    tras *
                    0.45f,
                peY
            },
            (Vector2){
                r.x + 22.0f -
                    tras *
                    0.45f,
                peY
            },
            4.5f,
            escuro
        );

        DrawLineEx(
            (Vector2){
                r.x + 24.0f,
                corpoY + 16.0f
            },
            (Vector2){
                r.x + 27.0f +
                    frente,
                joelhoY
            },
            7.5f,
            corpo
        );

        DrawLineEx(
            (Vector2){
                r.x + 27.0f +
                    frente,
                joelhoY
            },
            (Vector2){
                r.x + 29.0f -
                    frente *
                    0.45f,
                peY
            },
            5.5f,
            corpo
        );

        DrawLineEx(
            (Vector2){
                r.x + 27.0f -
                    frente *
                    0.45f,
                peY
            },
            (Vector2){
                r.x + 43.0f -
                    frente *
                    0.45f,
                peY
            },
            4.5f,
            corpo
        );
    }
}

static void desenhaJogador(
    const Mundo *m
) {
    if (
        !m->sequenciaVitoria &&
        m->invulneravel > 0.0f &&
        (
            (int)(
                m->invulneravel *
                10.0f
            ) %
            2
        ) ==
        0
    ) {
        return;
    }

    Rectangle r =
        mundo_caixa_jogador(m);

    bool comemorando =
        m->sequenciaVitoria &&
        m->tempoSequenciaVitoria >=
            TEMPO_CORRIDA_VITORIA;

    if (
        m->val.v[
            PROP_TURBO
        ]
    ) {
        desenhaTurbo(r);
    }

    if (
        m->abaixado &&
        !m->sequenciaVitoria
    ) {
        desenhaDinossauroAbaixado(
            r
        );
    } else {
        desenhaDinossauroEmPe(
            m,
            r,
            comemorando
        );
    }

    /*
     * Arma.
     */
    if (
        m->val.v[
            PROP_ARMA
        ] &&
        !m->sequenciaVitoria
    ) {
        Rectangle arma = {
            r.x +
                r.width +
                7.0f,
            r.y +
                r.height *
                0.36f,
            22.0f,
            8.0f
        };

        DrawRectangleRounded(
            arma,
            0.3f,
            5,
            (Color){
                64,
                69,
                90,
                255
            }
        );

        DrawRectangle(
            (int)arma.x + 14,
            (int)arma.y + 2,
            11,
            4,
            COR_CIANO
        );
    }

    /*
     * Escudo.
     */
    if (
        m->val.v[
            PROP_ESCUDO
        ] &&
        !m->sequenciaVitoria
    ) {
        Color preenchimento =
            COR_AZUL;

        preenchimento.a =
            34;

        DrawCircle(
            (int)(
                r.x +
                r.width /
                2.0f
            ),
            (int)(
                r.y +
                r.height /
                2.0f
            ),
            52.0f,
            preenchimento
        );

        DrawCircleLines(
            (int)(
                r.x +
                r.width /
                2.0f
            ),
            (int)(
                r.y +
                r.height /
                2.0f
            ),
            52.0f,
            COR_CIANO
        );

        DrawCircleLines(
            (int)(
                r.x +
                r.width /
                2.0f
            ),
            (int)(
                r.y +
                r.height /
                2.0f
            ),
            48.0f,
            (Color){
                126,
                245,
                248,
                115
            }
        );
    }
}

/* ============================================================
 * ENTIDADES
 * ============================================================ */

static void desenhaCacto(
    Rectangle r
) {
    Color sombra =
        { 24, 70, 62, 255 };

    Color corpo =
        { 45, 142, 104, 255 };

    Color detalhe =
        { 85, 190, 132, 255 };

    DrawEllipse(
        (int)(
            r.x +
            r.width /
            2.0f
        ),
        (int)(
            r.y +
            r.height
        ),
        r.width *
            0.68f,
        5.0f,
        (Color){
            20,
            27,
            38,
            130
        }
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x +
                r.width *
                0.30f,
            r.y,
            r.width *
                0.40f,
            r.height
        },
        0.4f,
        7,
        corpo
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x,
            r.y +
                r.height *
                0.35f,
            r.width *
                0.42f,
            8.0f
        },
        0.4f,
        6,
        sombra
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x,
            r.y +
                r.height *
                0.22f,
            7.0f,
            r.height *
                0.28f
        },
        0.4f,
        6,
        corpo
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x +
                r.width *
                0.60f,
            r.y +
                r.height *
                0.48f,
            r.width *
                0.40f,
            8.0f
        },
        0.4f,
        6,
        corpo
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x +
                r.width -
                7.0f,
            r.y +
                r.height *
                0.34f,
            7.0f,
            r.height *
                0.27f
        },
        0.4f,
        6,
        corpo
    );

    DrawLine(
        (int)(
            r.x +
            r.width *
            0.5f
        ),
        (int)r.y + 7,
        (int)(
            r.x +
            r.width *
            0.5f
        ),
        (int)(
            r.y +
            r.height -
            5.0f
        ),
        detalhe
    );
}

static void desenhaAndarilho(
    const Entidade *e
) {
    Rectangle r =
        e->caixa;

    bool passo =
        ((int)(e->anim * 9.0f) % 2) == 0;

    float balanco =
        sinf(e->anim * 9.0f) *
        1.5f;

    Color corpo =
        e->vida >= 2
            ? COR_ROXO
            : (Color){
                218,
                135,
                239,
                255
            };

    Color corpoClaro =
        e->vida >= 2
            ? (Color){
                205,
                133,
                245,
                255
            }
            : (Color){
                238,
                172,
                250,
                255
            };

    Color escuro =
        { 91, 45, 130, 255 };

    Color contorno =
        { 58, 31, 84, 255 };

    Color boca =
        { 50, 25, 66, 255 };

    /*
     * Sombra.
     */
    DrawEllipse(
        (int)(
            r.x +
            r.width /
            2.0f
        ),
        (int)(
            r.y +
            r.height +
            2.0f
        ),
        r.width *
            0.70f,
        5.0f,
        (Color){
            20,
            26,
            36,
            130
        }
    );

    /*
     * Chifres.
     */
    DrawTriangle(
        (Vector2){
            r.x + 5.0f,
            r.y + 8.0f +
                balanco
        },
        (Vector2){
            r.x + 8.0f,
            r.y - 9.0f +
                balanco
        },
        (Vector2){
            r.x + 16.0f,
            r.y + 7.0f +
                balanco
        },
        (Color){
            245,
            210,
            126,
            255
        }
    );

    DrawTriangle(
        (Vector2){
            r.x +
                r.width -
                16.0f,
            r.y + 7.0f +
                balanco
        },
        (Vector2){
            r.x +
                r.width -
                8.0f,
            r.y - 10.0f +
                balanco
        },
        (Vector2){
            r.x +
                r.width -
                4.0f,
            r.y + 9.0f +
                balanco
        },
        (Color){
            245,
            210,
            126,
            255
        }
    );

    /*
     * Orelhas laterais.
     */
    DrawTriangle(
        (Vector2){
            r.x + 3.0f,
            r.y + 14.0f
        },
        (Vector2){
            r.x - 9.0f,
            r.y + 20.0f
        },
        (Vector2){
            r.x + 4.0f,
            r.y + 27.0f
        },
        escuro
    );

    DrawTriangle(
        (Vector2){
            r.x +
                r.width -
                3.0f,
            r.y + 14.0f
        },
        (Vector2){
            r.x +
                r.width +
                9.0f,
            r.y + 20.0f
        },
        (Vector2){
            r.x +
                r.width -
                4.0f,
            r.y + 27.0f
        },
        escuro
    );

    /*
     * Corpo com contorno.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x - 2.0f,
            r.y + balanco,
            r.width + 4.0f,
            r.height
        },
        0.36f,
        8,
        contorno
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x + 1.0f,
            r.y + 2.0f +
                balanco,
            r.width - 2.0f,
            r.height - 4.0f
        },
        0.36f,
        8,
        corpo
    );

    /*
     * Mancha clara central.
     */
    DrawEllipse(
        (int)(
            r.x +
            r.width /
            2.0f
        ),
        (int)(
            r.y +
            r.height *
            0.62f +
            balanco
        ),
        r.width *
            0.29f,
        r.height *
            0.31f,
        corpoClaro
    );

    /*
     * Sobrancelhas inclinadas.
     */
    DrawLineEx(
        (Vector2){
            r.x + 7.0f,
            r.y + 10.0f +
                balanco
        },
        (Vector2){
            r.x + 18.0f,
            r.y + 14.0f +
                balanco
        },
        3.0f,
        contorno
    );

    DrawLineEx(
        (Vector2){
            r.x +
                r.width -
                7.0f,
            r.y + 10.0f +
                balanco
        },
        (Vector2){
            r.x +
                r.width -
                18.0f,
            r.y + 14.0f +
                balanco
        },
        3.0f,
        contorno
    );

    /*
     * Olhos.
     */
    DrawCircle(
        (int)r.x + 13,
        (int)(
            r.y +
            18.0f +
            balanco
        ),
        7.0f,
        WHITE
    );

    DrawCircle(
        (int)(
            r.x +
            r.width -
            13.0f
        ),
        (int)(
            r.y +
            18.0f +
            balanco
        ),
        7.0f,
        WHITE
    );

    DrawCircle(
        (int)r.x + 15,
        (int)(
            r.y +
            19.0f +
            balanco
        ),
        3.2f,
        COR_PAINEL
    );

    DrawCircle(
        (int)(
            r.x +
            r.width -
            15.0f
        ),
        (int)(
            r.y +
            19.0f +
            balanco
        ),
        3.2f,
        COR_PAINEL
    );

    /*
     * Boca.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x + 10.0f,
            r.y + 28.0f +
                balanco,
            r.width - 20.0f,
            10.0f
        },
        0.45f,
        6,
        boca
    );

    /*
     * Dentes.
     */
    DrawTriangle(
        (Vector2){
            r.x + 14.0f,
            r.y + 28.0f +
                balanco
        },
        (Vector2){
            r.x + 20.0f,
            r.y + 28.0f +
                balanco
        },
        (Vector2){
            r.x + 17.0f,
            r.y + 34.0f +
                balanco
        },
        WHITE
    );

    DrawTriangle(
        (Vector2){
            r.x +
                r.width -
                20.0f,
            r.y + 28.0f +
                balanco
        },
        (Vector2){
            r.x +
                r.width -
                14.0f,
            r.y + 28.0f +
                balanco
        },
        (Vector2){
            r.x +
                r.width -
                17.0f,
            r.y + 34.0f +
                balanco
        },
        WHITE
    );

    /*
     * Braços curtos.
     */
    float braco =
        passo
            ? 3.0f
            : -2.0f;

    DrawLineEx(
        (Vector2){
            r.x + 4.0f,
            r.y + 27.0f
        },
        (Vector2){
            r.x - 7.0f,
            r.y + 33.0f +
                braco
        },
        5.0f,
        escuro
    );

    DrawLineEx(
        (Vector2){
            r.x +
                r.width -
                4.0f,
            r.y + 27.0f
        },
        (Vector2){
            r.x +
                r.width +
                7.0f,
            r.y + 33.0f -
                braco
        },
        5.0f,
        escuro
    );

    /*
     * Pernas e pés alternados.
     */
    int pernaEsquerda =
        passo
            ? 2
            : 6;

    int pernaDireita =
        passo
            ? 6
            : 2;

    DrawRectangleRounded(
        (Rectangle){
            r.x + 7.0f,
            r.y +
                r.height -
                5.0f,
            10.0f,
            7.0f +
                pernaEsquerda
        },
        0.35f,
        4,
        escuro
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x +
                r.width -
                17.0f,
            r.y +
                r.height -
                5.0f,
            10.0f,
            7.0f +
                pernaDireita
        },
        0.35f,
        4,
        escuro
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x + 3.0f,
            r.y +
                r.height +
                pernaEsquerda,
            17.0f,
            5.0f
        },
        0.4f,
        4,
        escuro
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x +
                r.width -
                20.0f,
            r.y +
                r.height +
                pernaDireita,
            17.0f,
            5.0f
        },
        0.4f,
        4,
        escuro
    );
}

static void desenhaVoador(
    const Entidade *e
) {
    Rectangle r =
        e->caixa;

    /*
     * cicloAsa varia continuamente entre -1 e 1.
     *
     * Valores positivos:
     * asa levantada.
     *
     * Valores negativos:
     * asa abaixada.
     */
    float cicloAsa =
        sinf(
            e->anim *
            11.0f
        );

    float cicloAsaSecundaria =
        sinf(
            e->anim *
            11.0f +
            0.45f
        );

    float oscilacaoCorpo =
        sinf(
            e->anim *
            5.0f
        ) *
        1.5f;

    Color corpo =
        COR_LARANJA;

    Color corpoClaro =
        { 255, 177, 91, 255 };

    Color asa =
        { 255, 197, 99, 255 };

    Color asaEscura =
        { 201, 81, 49, 255 };

    Color contorno =
        { 114, 53, 47, 255 };

    Color bico =
        { 255, 216, 72, 255 };

    float centroX =
        r.x +
        r.width *
        0.52f;

    float centroY =
        r.y +
        r.height *
        0.52f +
        oscilacaoCorpo;

    /*
     * A ponta da asa percorre aproximadamente 38 pixels entre
     * a posição mais alta e a posição mais baixa.
     */
    float pontaAsaY =
        centroY -
        cicloAsa *
        19.0f;

    float pontaAsaTraseiraY =
        centroY -
        cicloAsaSecundaria *
        13.0f;

    /*
     * Inclinação horizontal deixa a asa menos rígida.
     */
    float pontaAsaX =
        centroX +
        2.0f +
        fabsf(cicloAsa) *
        4.0f;

    /*
     * Sombra aérea.
     */
    DrawEllipse(
        (int)centroX,
        (int)(
            r.y +
            r.height +
            11.0f
        ),
        r.width *
            0.42f,
        3.0f,
        (Color){
            22,
            27,
            37,
            75
        }
    );

    /*
     * Cauda.
     */
    DrawTriangle(
        (Vector2){
            r.x +
                r.width -
                8.0f,
            centroY - 4.0f
        },
        (Vector2){
            r.x +
                r.width +
                18.0f,
            centroY - 11.0f
        },
        (Vector2){
            r.x +
                r.width -
                3.0f,
            centroY + 2.0f
        },
        asaEscura
    );

    DrawTriangle(
        (Vector2){
            r.x +
                r.width -
                6.0f,
            centroY
        },
        (Vector2){
            r.x +
                r.width +
                21.0f,
            centroY + 2.0f
        },
        (Vector2){
            r.x +
                r.width -
                4.0f,
            centroY + 7.0f
        },
        corpoClaro
    );

    DrawTriangle(
        (Vector2){
            r.x +
                r.width -
                8.0f,
            centroY + 4.0f
        },
        (Vector2){
            r.x +
                r.width +
                16.0f,
            centroY + 13.0f
        },
        (Vector2){
            r.x +
                r.width -
                4.0f,
            centroY + 8.0f
        },
        asaEscura
    );

    /*
     * Asa traseira. É desenhada antes do corpo.
     */
    DrawTriangle(
        (Vector2){
            centroX + 1.0f,
            centroY - 1.0f
        },
        (Vector2){
            centroX + 12.0f,
            pontaAsaTraseiraY
        },
        (Vector2){
            centroX + 19.0f,
            centroY + 6.0f
        },
        asaEscura
    );

    DrawTriangle(
        (Vector2){
            centroX + 4.0f,
            centroY
        },
        (Vector2){
            centroX + 11.0f,
            pontaAsaTraseiraY + 3.0f
        },
        (Vector2){
            centroX + 16.0f,
            centroY + 4.0f
        },
        corpo
    );

    /*
     * Corpo com contorno.
     */
    DrawEllipse(
        (int)centroX,
        (int)centroY,
        r.width *
            0.52f,
        r.height *
            0.53f,
        contorno
    );

    DrawEllipse(
        (int)(
            centroX -
            1.0f
        ),
        (int)(
            centroY -
            1.0f
        ),
        r.width *
            0.46f,
        r.height *
            0.47f,
        corpo
    );

    /*
     * Barriga.
     */
    DrawEllipse(
        (int)(
            centroX -
            4.0f
        ),
        (int)(
            centroY +
            3.0f
        ),
        r.width *
            0.25f,
        r.height *
            0.30f,
        corpoClaro
    );

    /*
     * Asa principal animada.
     *
     * A asa possui três triângulos sobrepostos. Isso cria uma
     * aparência de penas e deixa a batida mais evidente.
     */
    DrawTriangle(
        (Vector2){
            centroX - 4.0f,
            centroY
        },
        (Vector2){
            pontaAsaX,
            pontaAsaY -
                8.0f
        },
        (Vector2){
            centroX + 15.0f,
            centroY + 6.0f
        },
        contorno
    );

    DrawTriangle(
        (Vector2){
            centroX - 2.0f,
            centroY
        },
        (Vector2){
            pontaAsaX,
            pontaAsaY -
                5.0f
        },
        (Vector2){
            centroX + 13.0f,
            centroY + 4.0f
        },
        asaEscura
    );

    DrawTriangle(
        (Vector2){
            centroX,
            centroY
        },
        (Vector2){
            pontaAsaX + 1.0f,
            pontaAsaY
        },
        (Vector2){
            centroX + 10.0f,
            centroY + 3.0f
        },
        asa
    );

    /*
     * Linhas das penas acompanham o movimento da asa.
     */
    DrawLineEx(
        (Vector2){
            centroX + 2.0f,
            centroY
        },
        (Vector2){
            pontaAsaX,
            pontaAsaY + 1.0f
        },
        1.5f,
        corpoClaro
    );

    DrawLineEx(
        (Vector2){
            centroX + 5.0f,
            centroY + 2.0f
        },
        (Vector2){
            pontaAsaX + 5.0f,
            pontaAsaY + 6.0f
        },
        1.3f,
        corpoClaro
    );

    /*
     * Cabeça.
     */
    DrawCircle(
        (int)(
            r.x +
            11.0f
        ),
        (int)(
            centroY -
            5.0f
        ),
        11.0f,
        contorno
    );

    DrawCircle(
        (int)(
            r.x +
            11.0f
        ),
        (int)(
            centroY -
            6.0f
        ),
        9.2f,
        corpo
    );

    /*
     * Topete.
     */
    DrawTriangle(
        (Vector2){
            r.x + 7.0f,
            centroY - 13.0f
        },
        (Vector2){
            r.x + 8.0f,
            centroY - 24.0f
        },
        (Vector2){
            r.x + 13.0f,
            centroY - 14.0f
        },
        asaEscura
    );

    DrawTriangle(
        (Vector2){
            r.x + 11.0f,
            centroY - 14.0f
        },
        (Vector2){
            r.x + 16.0f,
            centroY - 22.0f
        },
        (Vector2){
            r.x + 16.0f,
            centroY - 11.0f
        },
        corpoClaro
    );

    /*
     * Bico.
     */
    DrawTriangle(
        (Vector2){
            r.x + 4.0f,
            centroY - 7.0f
        },
        (Vector2){
            r.x - 12.0f,
            centroY - 1.0f
        },
        (Vector2){
            r.x + 4.0f,
            centroY + 2.0f
        },
        bico
    );

    DrawLineEx(
        (Vector2){
            r.x - 10.0f,
            centroY - 1.0f
        },
        (Vector2){
            r.x + 3.0f,
            centroY - 1.0f
        },
        1.3f,
        contorno
    );

    /*
     * Olho.
     */
    DrawCircle(
        (int)(
            r.x +
            9.0f
        ),
        (int)(
            centroY -
            8.0f
        ),
        4.6f,
        WHITE
    );

    DrawCircle(
        (int)(
            r.x +
            7.8f
        ),
        (int)(
            centroY -
            8.0f
        ),
        2.3f,
        COR_PAINEL
    );

    /*
     * Sobrancelha.
     */
    DrawLineEx(
        (Vector2){
            r.x + 4.0f,
            centroY - 13.0f
        },
        (Vector2){
            r.x + 13.0f,
            centroY - 14.0f
        },
        1.8f,
        contorno
    );

    /*
     * Pernas recolhidas.
     */
    DrawLineEx(
        (Vector2){
            centroX - 4.0f,
            centroY + 9.0f
        },
        (Vector2){
            centroX - 7.0f,
            centroY + 16.0f
        },
        2.5f,
        contorno
    );

    DrawLineEx(
        (Vector2){
            centroX + 5.0f,
            centroY + 9.0f
        },
        (Vector2){
            centroX + 8.0f,
            centroY + 16.0f
        },
        2.5f,
        contorno
    );

    DrawLineEx(
        (Vector2){
            centroX - 11.0f,
            centroY + 16.0f
        },
        (Vector2){
            centroX - 4.0f,
            centroY + 16.0f
        },
        2.0f,
        bico
    );

    DrawLineEx(
        (Vector2){
            centroX + 5.0f,
            centroY + 16.0f
        },
        (Vector2){
            centroX + 12.0f,
            centroY + 16.0f
        },
        2.0f,
        bico
    );
}

static void desenhaPowerUp(
    const Entidade *e
) {
    Rectangle r =
        e->caixa;

    Color cor =
        COR_AMARELO;

    const char *letra =
        "E";

    if (
        e->tipo ==
        ENT_PU_VIDA
    ) {
        cor =
            COR_VERMELHO;

        letra =
            "+";
    }

    if (
        e->tipo ==
        ENT_PU_TAUTOLOGIA
    ) {
        cor =
            COR_ROXO;

        letra =
            "T";
    }

    int centroX =
        (int)(
            r.x +
            r.width /
            2.0f
        );

    int centroY =
        (int)(
            r.y +
            r.height /
            2.0f
        );

    Color aura =
        cor;

    aura.a =
        35;

    DrawCircle(
        centroX,
        centroY,
        25.0f,
        aura
    );

    DrawCircle(
        centroX,
        centroY,
        17.0f,
        cor
    );

    DrawCircleLines(
        centroX,
        centroY,
        17.0f,
        WHITE
    );

    DrawCircleLines(
        centroX,
        centroY,
        21.0f,
        (Color){
            cor.r,
            cor.g,
            cor.b,
            100
        }
    );

    textoCentro(
        letra,
        centroX,
        centroY - 10,
        20,
        WHITE
    );
}

/* ============================================================
 * PORTÃO
 * ============================================================ */

static void desenhaPortao(
    const Entidade *e
) {
    Rectangle r =
        e->caixa;

    Color cor =
        COR_AZUL;

    if (e->avaliado) {
        cor =
            e->resultado
                ? COR_VERDE
                : COR_VERMELHO;
    }

    float colunaLargura =
        14.0f;

    float arcoAltura =
        17.0f;

    Color campo =
        cor;

    campo.a =
        e->avaliado
            ? 70
            : 45;

    DrawRectangle(
        (int)r.x - 8,
        (int)r.y,
        (int)colunaLargura,
        (int)r.height,
        (Color){
            35,
            39,
            59,
            255
        }
    );

    DrawRectangle(
        (int)(
            r.x +
            r.width +
            2.0f
        ),
        (int)r.y,
        (int)colunaLargura,
        (int)r.height,
        (Color){
            35,
            39,
            59,
            255
        }
    );

    DrawRectangle(
        (int)r.x - 8,
        (int)r.y,
        (int)r.width + 24,
        (int)arcoAltura,
        (Color){
            35,
            39,
            59,
            255
        }
    );

    DrawRectangle(
        (int)r.x + 6,
        (int)r.y +
            (int)arcoAltura,
        (int)r.width - 4,
        (int)r.height -
            (int)arcoAltura,
        campo
    );

    linhaBrilho(
        (Vector2){
            r.x - 1.0f,
            r.y + 8.0f
        },
        (Vector2){
            r.x - 1.0f,
            r.y +
                r.height
        },
        2.0f,
        cor
    );

    linhaBrilho(
        (Vector2){
            r.x +
                r.width +
                9.0f,
            r.y + 8.0f
        },
        (Vector2){
            r.x +
                r.width +
                9.0f,
            r.y +
                r.height
        },
        2.0f,
        cor
    );

    linhaBrilho(
        (Vector2){
            r.x - 1.0f,
            r.y + 8.0f
        },
        (Vector2){
            r.x +
                r.width +
                9.0f,
            r.y + 8.0f
        },
        2.0f,
        cor
    );

    for (
        int i = 0;
        i < 5;
        i++
    ) {
        float y =
            r.y +
            30.0f +
            i * 28.0f;

        DrawLineEx(
            (Vector2){
                r.x + 7.0f,
                y
            },
            (Vector2){
                r.x +
                    r.width +
                    1.0f,
                y + 10.0f
            },
            1.0f,
            (Color){
                cor.r,
                cor.g,
                cor.b,
                90
            }
        );
    }

    Formula formula =
        catalogoFormula(
            e->formulaId
        );

    char texto[64];

    formulaParaTexto(
        &formula,
        texto,
        sizeof texto
    );

    int tamanho =
        22;

    int largura =
        MeasureText(
            texto,
            tamanho
        ) +
        28;

    int x =
        (int)(
            r.x +
            r.width /
            2.0f
        ) -
        largura /
        2;

    int y =
        (int)r.y -
        41;

    painel(
        x,
        y,
        largura,
        32,
        COR_PAINEL,
        cor,
        2.0f
    );

    textoCentro(
        texto,
        x +
            largura /
            2,
        y + 5,
        tamanho,
        COR_TEXTO
    );
}

static void desenhaEntidade(
    const Entidade *e
) {
    switch (e->tipo) {
        case ENT_CACTO:
            desenhaCacto(
                e->caixa
            );
            break;

        case ENT_ANDARILHO:
            desenhaAndarilho(e);
            break;

        case ENT_VOADOR:
            desenhaVoador(e);
            break;

        case ENT_PORTAO:
            desenhaPortao(e);
            break;

        case ENT_PU_ENERGIA:
        case ENT_PU_VIDA:
        case ENT_PU_TAUTOLOGIA:
            desenhaPowerUp(e);
            break;
    }
}

static void desenhaTiro(
    const Tiro *tiro
) {
    Rectangle r =
        tiro->caixa;

    Color brilho =
        COR_CIANO;

    brilho.a =
        45;

    DrawRectangle(
        (int)r.x - 8,
        (int)r.y - 4,
        (int)r.width + 16,
        (int)r.height + 8,
        brilho
    );

    DrawRectangleRec(
        r,
        COR_CIANO
    );

    DrawRectangle(
        (int)r.x + 5,
        (int)r.y + 1,
        (int)r.width - 5,
        2,
        WHITE
    );
}

/* ============================================================
 * HUD
 * ============================================================ */

static void desenhaCoracao(
    int x,
    int y,
    bool cheio
) {
    Color cor =
        cheio
            ? COR_VERMELHO
            : (Color){
                82,
                84,
                101,
                255
            };

    DrawCircle(
        x - 5,
        y,
        6.0f,
        cor
    );

    DrawCircle(
        x + 5,
        y,
        6.0f,
        cor
    );

    DrawTriangle(
        (Vector2){
            (float)x -
                11.0f,
            (float)y +
                2.0f
        },
        (Vector2){
            (float)x +
                11.0f,
            (float)y +
                2.0f
        },
        (Vector2){
            (float)x,
            (float)y +
                14.0f
        },
        cor
    );
}

static void desenhaTabela(
    const Mundo *m,
    const Entidade *portao
) {
    Formula formula =
        catalogoFormula(
            portao->formulaId
        );

    int x =
        LARGURA_TELA -
        190;

    int y =
        79;

    int largura =
        176;

    int altura =
        151;

    painel(
        x,
        y,
        largura,
        altura,
        (Color){
            27,
            30,
            45,
            242
        },
        COR_AZUL,
        2.0f
    );

    DrawText(
        "TABELA-VERDADE",
        x + 13,
        y + 8,
        13,
        COR_AMARELO
    );

    DrawText(
        "P  Q  R  |  f",
        x + 18,
        y + 28,
        14,
        COR_TEXTO
    );

    DrawLine(
        x + 10,
        y + 45,
        x +
            largura -
            10,
        y + 45,
        (Color){
            102,
            108,
            132,
            255
        }
    );

    unsigned atual =
        mascaraDeValoracao(
            m->val
        );

    for (
        unsigned i = 0;
        i < 8;
        i++
    ) {
        Valoracao valoracao =
            valoracaoDeMascara(i);

        bool resultado =
            avalia(
                &formula,
                valoracao
            );

        int linhaY =
            y +
            49 +
            (int)i *
            12;

        if (i == atual) {
            DrawRectangle(
                x + 8,
                linhaY - 1,
                largura - 16,
                12,
                (Color){
                    74,
                    151,
                    255,
                    70
                }
            );
        }

        char linha[32];

        snprintf(
            linha,
            sizeof linha,
            "%c  %c  %c  |  %c",
            valoracao.v[0]
                ? 'V'
                : 'F',
            valoracao.v[1]
                ? 'V'
                : 'F',
            valoracao.v[2]
                ? 'V'
                : 'F',
            resultado
                ? 'V'
                : 'F'
        );

        DrawText(
            linha,
            x + 18,
            linhaY,
            12,
            resultado
                ? COR_VERDE
                : COR_VERMELHO
        );
    }
}

static void desenhaCartao(
    const Mundo *m,
    int indice,
    int x
) {
    static const char *poderes[
        NUM_PROPS
    ] = {
        "ESCUDO",
        "ARMA",
        "TURBO"
    };

    static const Color cores[
        NUM_PROPS
    ] = {
        { 74, 151, 255, 255 },
        { 255, 207, 79, 255 },
        { 180, 100, 230, 255 }
    };

    bool ligado =
        m->val.v[indice];

    Color fundo =
        ligado
            ? misturaCor(
                COR_PAINEL_CLARO,
                cores[indice],
                0.35f
            )
            : COR_PAINEL_CLARO;

    painel(
        x,
        467,
        182,
        58,
        fundo,
        ligado
            ? cores[indice]
            : (Color){
                79,
                83,
                103,
                255
            },
        ligado
            ? 2.0f
            : 1.0f
    );

    char tecla[8];

    snprintf(
        tecla,
        sizeof tecla,
        "%d",
        indice + 1
    );

    painel(
        x + 8,
        476,
        32,
        32,
        ligado
            ? cores[indice]
            : (Color){
                69,
                72,
                89,
                255
            },
        ligado
            ? WHITE
            : (Color){
                116,
                120,
                140,
                255
            },
        1.0f
    );

    textoCentro(
        tecla,
        x + 24,
        482,
        18,
        WHITE
    );

    char titulo[40];

    snprintf(
        titulo,
        sizeof titulo,
        "%s  %s",
        nomeProp(indice),
        poderes[indice]
    );

    DrawText(
        titulo,
        x + 49,
        474,
        16,
        COR_TEXTO
    );

    DrawText(
        ligado
            ? "VERDADEIRO"
            : "FALSO",
        x + 49,
        497,
        13,
        ligado
            ? COR_VERDE
            : COR_TEXTO_FRACO
    );

    circuloBrilho(
        x + 165,
        496,
        6.0f,
        ligado
            ? cores[indice]
            : (Color){
                92,
                95,
                110,
                255
            }
    );
}

static void desenhaHud(
    const Mundo *m,
    bool mostraTabela
) {
    painel(
        12,
        10,
        208,
        64,
        (Color){
            27,
            30,
            44,
            218
        },
        (Color){
            74,
            78,
            104,
            210
        },
        1.0f
    );

    DrawText(
        "VIDAS",
        22,
        17,
        12,
        COR_TEXTO_FRACO
    );

    for (
        int i = 0;
        i < VIDAS_MAX;
        i++
    ) {
        desenhaCoracao(
            29 +
                i *
                27,
            39,
            i <
                m->vidas
        );
    }

    DrawText(
        "ENERGIA",
        22,
        57,
        11,
        COR_TEXTO_FRACO
    );

    DrawRectangle(
        79,
        58,
        126,
        9,
        (Color){
            66,
            69,
            86,
            255
        }
    );

    Color energia =
        m->energia < 25.0f
            ? COR_VERMELHO
            : COR_AMARELO;

    DrawRectangle(
        79,
        58,
        (int)(
            126.0f *
            m->energia /
            ENERGIA_MAX
        ),
        9,
        energia
    );

    painel(
        378,
        10,
        204,
        54,
        (Color){
            27,
            30,
            44,
            218
        },
        (Color){
            74,
            78,
            104,
            210
        },
        1.0f
    );

    char texto[96];

    snprintf(
        texto,
        sizeof texto,
        "%d",
        mundo_pontos(m)
    );

    textoCentro(
        texto,
        LARGURA_TELA /
            2,
        16,
        27,
        COR_TEXTO
    );

    textoCentro(
        "PONTOS",
        LARGURA_TELA /
            2,
        45,
        11,
        COR_TEXTO_FRACO
    );

    painel(
        746,
        10,
        202,
        54,
        (Color){
            27,
            30,
            44,
            218
        },
        (Color){
            74,
            78,
            104,
            210
        },
        1.0f
    );

    snprintf(
        texto,
        sizeof texto,
        "PORTOES  %d / %d",
        m->portoesOk,
        META_PORTOES
    );

    textoCentro(
        texto,
        847,
        18,
        20,
        COR_CIANO
    );

    int progresso =
        (int)(
            172.0f *
            m->portoesOk /
            META_PORTOES
        );

    DrawRectangle(
        761,
        46,
        172,
        7,
        (Color){
            67,
            70,
            87,
            255
        }
    );

    DrawRectangle(
        761,
        46,
        progresso,
        7,
        COR_CIANO
    );

    if (
        m->sobrecarga >
        0.0f
    ) {
        textoCentro(
            "SOBRECARGA",
            LARGURA_TELA /
                2,
            72,
            17,
            COR_VERMELHO
        );
    }

    if (m->tautologia) {
        textoCentro(
            "TAUTOLOGIA ATIVA",
            LARGURA_TELA /
                2,
            72,
            17,
            COR_ROXO
        );
    }

    if (
        !m->decisaoPortao &&
        m->msgTempo > 0.0f &&
        m->msg[0] != '\0'
    ) {
        painel(
            280,
            94,
            400,
            38,
            (Color){
                27,
                30,
                44,
                228
            },
            (Color){
                92,
                97,
                121,
                220
            },
            1.0f
        );

        textoCentro(
            m->msg,
            LARGURA_TELA /
                2,
            102,
            19,
            COR_TEXTO
        );
    }

    for (
        int i = 0;
        i < NUM_PROPS;
        i++
    ) {
        desenhaCartao(
            m,
            i,
            12 +
                i *
                190
        );
    }

    painel(
        588,
        467,
        360,
        58,
        COR_PAINEL,
        (Color){
            79,
            83,
            103,
            255
        },
        1.0f
    );

    const Entidade *portao =
        mundo_proximo_portao(m);

    if (portao != NULL) {
        Formula formula =
            catalogoFormula(
                portao->formulaId
            );

        char formulaTexto[64];

        formulaParaTexto(
            &formula,
            formulaTexto,
            sizeof formulaTexto
        );

        if (
            m->decisaoPortao
        ) {
            DrawText(
                "DESAFIO LOGICO EM ANDAMENTO",
                602,
                475,
                15,
                COR_AMARELO
            );

            DrawText(
                "1, 2 e 3 alteram | ENTER confirma",
                602,
                499,
                13,
                COR_TEXTO_FRACO
            );
        } else {
            bool resultado =
                m->tautologia ||
                avalia(
                    &formula,
                    m->val
                );

            char linha[120];

            snprintf(
                linha,
                sizeof linha,
                "PROXIMO: %s",
                formulaTexto
            );

            DrawText(
                linha,
                602,
                475,
                16,
                COR_TEXTO
            );

            DrawText(
                resultado
                    ? "ESTADO ATUAL: VERDADEIRO"
                    : "ESTADO ATUAL: FALSO",
                602,
                498,
                13,
                resultado
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
    } else if (
        m->sequenciaVitoria
    ) {
        DrawText(
            "TODOS OS PORTOES ABERTOS",
            602,
            475,
            15,
            COR_AMARELO
        );

        DrawText(
            "Sequencia final em andamento",
            602,
            499,
            13,
            COR_TEXTO_FRACO
        );
    } else {
        DrawText(
            "PROXIMO PORTAO",
            602,
            475,
            15,
            COR_TEXTO
        );

        DrawText(
            "Aguardando novo desafio...",
            602,
            499,
            13,
            COR_TEXTO_FRACO
        );
    }

    DrawText(
        "T: tabela   P: pausa",
        799,
        511,
        11,
        COR_TEXTO_FRACO
    );
}
/* ============================================================
 * DESAFIO LÓGICO
 * ============================================================ */

static void desenhaDesafioLogico(
    const Mundo *m
) {
    if (!m->decisaoPortao) {
        return;
    }

    const Entidade *portao =
        mundo_proximo_portao(m);

    if (portao == NULL) {
        return;
    }

    DrawRectangle(
        0,
        0,
        LARGURA_TELA,
        (int)CHAO_Y,
        (Color){
            12,
            14,
            28,
            180
        }
    );

    painel(
        215,
        92,
        530,
        300,
        (Color){
            24,
            27,
            43,
            250
        },
        COR_CIANO,
        3.0f
    );

    textoCentro(
        "DESAFIO LOGICO",
        LARGURA_TELA / 2,
        110,
        29,
        COR_AMARELO
    );

    textoCentro(
        "Torne a formula verdadeira",
        LARGURA_TELA / 2,
        146,
        16,
        COR_TEXTO_FRACO
    );

    Formula formula =
        catalogoFormula(
            portao->formulaId
        );

    char formulaTexto[64];

    formulaParaTexto(
        &formula,
        formulaTexto,
        sizeof formulaTexto
    );

    int larguraFormula =
        MeasureText(
            formulaTexto,
            40
        ) +
        60;

    painel(
        LARGURA_TELA / 2 -
            larguraFormula / 2,
        178,
        larguraFormula,
        62,
        (Color){
            38,
            42,
            60,
            255
        },
        COR_AZUL,
        2.0f
    );

    textoCentro(
        formulaTexto,
        LARGURA_TELA / 2,
        188,
        40,
        COR_TEXTO
    );

    static const Color cores[
        NUM_PROPS
    ] = {
        { 74, 151, 255, 255 },
        { 255, 207, 79, 255 },
        { 180, 100, 230, 255 }
    };

    for (
        int i = 0;
        i < NUM_PROPS;
        i++
    ) {
        int x =
            310 +
            i * 115;

        bool ligado =
            m->val.v[i];

        painel(
            x,
            258,
            105,
            48,
            ligado
                ? misturaCor(
                    COR_PAINEL_CLARO,
                    cores[i],
                    0.38f
                )
                : COR_PAINEL_CLARO,
            ligado
                ? cores[i]
                : (Color){
                    77,
                    81,
                    99,
                    255
                },
            ligado
                ? 2.0f
                : 1.0f
        );

        char valor[24];

        snprintf(
            valor,
            sizeof valor,
            "%s = %c",
            nomeProp(i),
            ligado
                ? 'V'
                : 'F'
        );

        textoCentro(
            valor,
            x + 52,
            269,
            20,
            ligado
                ? WHITE
                : COR_TEXTO_FRACO
        );
    }

    float proporcao =
        m->tempoDecisaoPortao /
        TEMPO_DECISAO_PORTAO;

    proporcao =
        limita01(proporcao);

    Color corTempo =
        COR_VERDE;

    if (
        m->tempoDecisaoPortao <=
        2.0f
    ) {
        corTempo =
            COR_VERMELHO;
    } else if (
        m->tempoDecisaoPortao <=
        3.5f
    ) {
        corTempo =
            COR_AMARELO;
    }

    char tempo[32];

    snprintf(
        tempo,
        sizeof tempo,
        "%.1f s",
        m->tempoDecisaoPortao
    );

    textoCentro(
        tempo,
        LARGURA_TELA / 2,
        316,
        24,
        corTempo
    );

    DrawRectangle(
        315,
        347,
        330,
        12,
        (Color){
            66,
            70,
            88,
            255
        }
    );

    DrawRectangle(
        315,
        347,
        (int)(
            330.0f *
            proporcao
        ),
        12,
        corTempo
    );

    DrawRectangleLines(
        315,
        347,
        330,
        12,
        (Color){
            160,
            166,
            194,
            255
        }
    );

    textoCentro(
        "[1] [2] [3] ALTERAR     [ENTER] CONFIRMAR",
        LARGURA_TELA / 2,
        369,
        14,
        COR_TEXTO
    );
}

/* ============================================================
 * SEQUÊNCIA VISUAL DE VITÓRIA
 * ============================================================ */

static void desenhaEstrelaVitoria(
    float x,
    float y,
    float tamanho,
    Color cor
) {
    DrawLineEx(
        (Vector2){
            x - tamanho,
            y
        },
        (Vector2){
            x + tamanho,
            y
        },
        2.0f,
        cor
    );

    DrawLineEx(
        (Vector2){
            x,
            y - tamanho
        },
        (Vector2){
            x,
            y + tamanho
        },
        2.0f,
        cor
    );

    DrawLineEx(
        (Vector2){
            x -
                tamanho *
                0.65f,
            y -
                tamanho *
                0.65f
        },
        (Vector2){
            x +
                tamanho *
                0.65f,
            y +
                tamanho *
                0.65f
        },
        1.5f,
        cor
    );

    DrawLineEx(
        (Vector2){
            x +
                tamanho *
                0.65f,
            y -
                tamanho *
                0.65f
        },
        (Vector2){
            x -
                tamanho *
                0.65f,
            y +
                tamanho *
                0.65f
        },
        1.5f,
        cor
    );
}

static void desenhaSequenciaVitoria(
    const Mundo *m
) {
    if (!m->sequenciaVitoria) {
        return;
    }

    float progresso =
        m->tempoSequenciaVitoria /
        TEMPO_SEQUENCIA_VITORIA;

    progresso =
        limita01(progresso);

    Color brilho =
        COR_AMARELO;

    brilho.a =
        (unsigned char)(
            18.0f +
            47.0f *
            sinf(
                progresso *
                PI
            )
        );

    DrawRectangle(
        0,
        0,
        LARGURA_TELA,
        (int)CHAO_Y,
        brilho
    );

    if (
        m->tempoSequenciaVitoria <
        TEMPO_CORRIDA_VITORIA
    ) {
        painel(
            285,
            90,
            390,
            43,
            (Color){
                25,
                30,
                43,
                225
            },
            COR_VERDE,
            2.0f
        );

        textoCentro(
            "8 PORTOES ABERTOS!",
            LARGURA_TELA / 2,
            100,
            22,
            COR_VERDE
        );

        return;
    }

    float comemoracao =
        progressoComemoracao(m);

    for (
        int i = 0;
        i < 18;
        i++
    ) {
        float angulo =
            i * 2.39996f +
            comemoracao *
            3.0f;

        float distancia =
            65.0f +
            (float)(i % 5) *
            23.0f;

        float x =
            LARGURA_TELA /
                2.0f +
            cosf(angulo) *
                distancia;

        float y =
            235.0f +
            sinf(angulo) *
                distancia *
                0.55f;

        float tamanho =
            3.0f +
            (float)(i % 3);

        Color cor =
            i % 2 == 0
                ? COR_AMARELO
                : COR_CIANO;

        cor.a =
            (unsigned char)(
                120.0f +
                110.0f *
                sinf(
                    comemoracao *
                    PI
                )
            );

        desenhaEstrelaVitoria(
            x,
            y,
            tamanho,
            cor
        );
    }

    painel(
        260,
        92,
        440,
        83,
        (Color){
            25,
            30,
            43,
            235
        },
        COR_AMARELO,
        3.0f
    );

    textoCentro(
        "TODOS OS PORTOES ABERTOS!",
        LARGURA_TELA / 2,
        106,
        26,
        COR_AMARELO
    );

    textoCentro(
        "CORRIDA LOGICA CONCLUIDA",
        LARGURA_TELA / 2,
        143,
        15,
        COR_TEXTO
    );
}

/* ============================================================
 * API PRINCIPAL
 * ============================================================ */

void render_mundo(
    const Mundo *m,
    bool mostraTabela
) {
    desenhaCenario(m);

    for (
        int i = 0;
        i < m->nEnts;
        i++
    ) {
        desenhaEntidade(
            &m->ents[i]
        );
    }

    for (
        int i = 0;
        i < m->nTiros;
        i++
    ) {
        desenhaTiro(
            &m->tiros[i]
        );
    }

    desenhaJogador(m);

    desenhaHud(
        m,
        mostraTabela
    );

    desenhaDesafioLogico(m);

    desenhaSequenciaVitoria(m);
}

/* ============================================================
 * MENU
 * ============================================================ */

static void desenhaFundoMenu(void) {
    Mundo demonstracao;

    memset(
        &demonstracao,
        0,
        sizeof demonstracao
    );

    demonstracao.noChao =
        true;

    demonstracao.energia =
        ENERGIA_MAX;

    demonstracao.vidas =
        VIDAS_INICIAIS;

    demonstracao.velocidade =
        VEL_INICIAL;

    demonstracao.tempo =
        (float)GetTime();

    /*
     * O personagem precisa ficar mais à esquerda no menu.
     * A função de desenho usa JOGADOR_X, então deslocamos
     * somente o desenho do mundo pelo valor da distância.
     */
    desenhaCenario(
        &demonstracao
    );

    DrawRectangle(
        0,
        0,
        LARGURA_TELA,
        ALTURA_TELA,
        (Color){
            15,
            17,
            31,
            65
        }
    );

    /*
     * Usa o mesmo dinossauro da partida.
     */
    desenhaJogador(
        &demonstracao
    );
}

void render_menu(
    bool rankingHover
) {
    desenhaFundoMenu();

    textoCentro(
        "RUNNER LOGICO",
        LARGURA_TELA / 2,
        30,
        50,
        COR_TEXTO
    );

    textoCentro(
        "CORRA. RACIOCINE. SOBREVIVA.",
        LARGURA_TELA / 2,
        87,
        16,
        COR_CIANO
    );

    painel(
        250,
        120,
        460,
        262,
        (Color){
            24,
            27,
            42,
            235
        },
        (Color){
            89,
            96,
            127,
            230
        },
        2.0f
    );

    DrawText(
        "MOVIMENTO",
        274,
        139,
        17,
        COR_AMARELO
    );

    DrawText(
        "ESPACO / W / SETA CIMA",
        274,
        166,
        15,
        COR_TEXTO
    );

    DrawText(
        "Pular",
        530,
        166,
        15,
        COR_TEXTO_FRACO
    );

    DrawText(
        "SETA BAIXO / S",
        274,
        190,
        15,
        COR_TEXTO
    );

    DrawText(
        "Abaixar ou cair rapido",
        530,
        190,
        15,
        COR_TEXTO_FRACO
    );

    DrawText(
        "X / Z / CTRL",
        274,
        214,
        15,
        COR_TEXTO
    );

    DrawText(
        "Atirar com Q = V",
        530,
        214,
        15,
        COR_TEXTO_FRACO
    );

    DrawLine(
        274,
        244,
        686,
        244,
        (Color){
            88,
            93,
            116,
            255
        }
    );

    DrawText(
        "LOGICA",
        274,
        258,
        17,
        COR_AMARELO
    );

    DrawText(
        "[1] P = ESCUDO",
        274,
        286,
        15,
        COR_AZUL
    );

    DrawText(
        "[2] Q = ARMA",
        435,
        286,
        15,
        COR_AMARELO
    );

    DrawText(
        "[3] R = TURBO",
        565,
        286,
        15,
        COR_ROXO
    );

    DrawText(
        "Nos portoes, voce tem 5 segundos para validar a formula.",
        274,
        318,
        14,
        COR_TEXTO
    );

    DrawText(
        "ENTER confirma | T tabela | P pausa | ESC menu",
        274,
        344,
        13,
        COR_TEXTO_FRACO
    );

    painel(
        342,
        401,
        276,
        54,
        (Color){
            45,
            126,
            91,
            245
        },
        COR_VERDE,
        2.0f
    );

    textoCentro(
        "ENTER PARA JOGAR",
        LARGURA_TELA / 2,
        416,
        22,
        WHITE
    );

    desenhaBotao(
        render_area_botao_ranking_menu(),
        "RANKING TOP 10",
        rankingHover,
        COR_AMARELO
    );

    textoCentro(
        "Clique ou pressione R",
        LARGURA_TELA / 2,
        522,
        11,
        COR_TEXTO_FRACO
    );
}

/* ============================================================
 * PAUSA E FUNDO ESCURO
 * ============================================================ */

static void fundoEscuro(void) {
    DrawRectangle(
        0,
        0,
        LARGURA_TELA,
        ALTURA_TELA,
        (Color){
            12,
            14,
            26,
            220
        }
    );
}

void render_pausa(void) {
    fundoEscuro();

    painel(
        260,
        173,
        440,
        180,
        COR_PAINEL,
        COR_AZUL,
        2.0f
    );

    textoCentro(
        "PAUSADO",
        LARGURA_TELA / 2,
        207,
        43,
        COR_TEXTO
    );

    textoCentro(
        "P para continuar",
        LARGURA_TELA / 2,
        274,
        19,
        COR_CIANO
    );

    textoCentro(
        "ESC para voltar ao menu",
        LARGURA_TELA / 2,
        307,
        15,
        COR_TEXTO_FRACO
    );
}

/* ============================================================
 * TELA FINAL
 * ============================================================ */

void render_fim(
    const Mundo *m,
    int recorde,
    bool rankingHover
) {
    fundoEscuro();

    bool venceu =
        m->resultado ==
        RES_VITORIA;

    Color destaque =
        venceu
            ? COR_VERDE
            : COR_VERMELHO;

    painel(
        230,
        74,
        500,
        448,
        COR_PAINEL,
        destaque,
        3.0f
    );

    textoCentro(
        venceu
            ? "VITORIA!"
            : "FIM DE JOGO",
        LARGURA_TELA / 2,
        101,
        45,
        destaque
    );

    char texto[128];

    snprintf(
        texto,
        sizeof texto,
        "PONTOS  %d",
        mundo_pontos(m)
    );

    textoCentro(
        texto,
        LARGURA_TELA / 2,
        169,
        28,
        COR_TEXTO
    );

    snprintf(
        texto,
        sizeof texto,
        "RECORDE  %d",
        recorde
    );

    textoCentro(
        texto,
        LARGURA_TELA / 2,
        207,
        17,
        COR_AMARELO
    );

    snprintf(
        texto,
        sizeof texto,
        "Portoes corretos: %d    Erros: %d",
        m->portoesOk,
        m->portoesErro
    );

    textoCentro(
        texto,
        LARGURA_TELA / 2,
        247,
        17,
        COR_TEXTO_FRACO
    );

    snprintf(
        texto,
        sizeof texto,
        "Inimigos abatidos: %d",
        m->abatidos
    );

    textoCentro(
        texto,
        LARGURA_TELA / 2,
        277,
        17,
        COR_TEXTO_FRACO
    );

    painel(
        340,
        326,
        280,
        48,
        (Color){
            45,
            126,
            91,
            245
        },
        COR_VERDE,
        2.0f
    );

    textoCentro(
        "ENTER JOGAR NOVAMENTE",
        LARGURA_TELA / 2,
        340,
        17,
        WHITE
    );

    desenhaBotao(
        render_area_botao_ranking_fim(),
        "VER RANKING",
        rankingHover,
        COR_AMARELO
    );

    textoCentro(
        "R: ranking   |   ESC: menu",
        LARGURA_TELA / 2,
        482,
        14,
        COR_TEXTO_FRACO
    );
}

/* ============================================================
 * RANKING
 * ============================================================ */

void render_ranking(
    const Ranking *ranking,
    bool voltarHover
) {
    fundoEscuro();

    painel(
        145,
        30,
        670,
        500,
        (Color){
            22,
            25,
            39,
            250
        },
        COR_AMARELO,
        3.0f
    );

    textoCentro(
        "RANKING TOP 10",
        LARGURA_TELA / 2,
        51,
        34,
        COR_AMARELO
    );

    textoCentro(
        "As maiores pontuacoes do Runner Logico",
        LARGURA_TELA / 2,
        92,
        14,
        COR_TEXTO_FRACO
    );

    DrawRectangle(
        172,
        122,
        616,
        31,
        (Color){
            48,
            52,
            71,
            255
        }
    );

    DrawText(
        "#",
        186,
        130,
        15,
        COR_TEXTO_FRACO
    );

    DrawText(
        "JOGADOR",
        226,
        130,
        15,
        COR_TEXTO_FRACO
    );

    DrawText(
        "PONTOS",
        505,
        130,
        15,
        COR_TEXTO_FRACO
    );

    DrawText(
        "PORTOES",
        616,
        130,
        15,
        COR_TEXTO_FRACO
    );

    DrawText(
        "ABATES",
        712,
        130,
        15,
        COR_TEXTO_FRACO
    );

    int quantidade =
        ranking != NULL
            ? ranking->quantidade
            : 0;

    if (quantidade <= 0) {
        textoCentro(
            "O ranking ainda esta vazio.",
            LARGURA_TELA / 2,
            240,
            22,
            COR_TEXTO
        );

        textoCentro(
            "Complete uma partida para registrar a primeira pontuacao.",
            LARGURA_TELA / 2,
            277,
            14,
            COR_TEXTO_FRACO
        );
    } else {
        for (
            int i = 0;
            i < quantidade &&
                i <
                RANKING_MAX_REGISTROS;
            i++
        ) {
            const RegistroRanking *registro =
                &ranking->registros[i];

            int y =
                160 +
                i * 29;

            Color fundoLinha =
                i % 2 == 0
                    ? (Color){
                        34,
                        37,
                        53,
                        235
                    }
                    : (Color){
                        28,
                        31,
                        46,
                        235
                    };

            Color corPosicao =
                COR_TEXTO;

            if (i == 0) {
                fundoLinha =
                    (Color){
                        104,
                        82,
                        31,
                        235
                    };

                corPosicao =
                    COR_AMARELO;
            } else if (i == 1) {
                fundoLinha =
                    (Color){
                        70,
                        75,
                        88,
                        235
                    };

                corPosicao =
                    (Color){
                        220,
                        226,
                        237,
                        255
                    };
            } else if (i == 2) {
                fundoLinha =
                    (Color){
                        91,
                        57,
                        40,
                        235
                    };

                corPosicao =
                    (Color){
                        228,
                        148,
                        94,
                        255
                    };
            }

            DrawRectangle(
                172,
                y,
                616,
                25,
                fundoLinha
            );

            char texto[64];

            snprintf(
                texto,
                sizeof texto,
                "%d",
                i + 1
            );

            DrawText(
                texto,
                187,
                y + 5,
                15,
                corPosicao
            );

            DrawText(
                registro->nome,
                226,
                y + 5,
                15,
                i < 3
                    ? WHITE
                    : COR_TEXTO
            );

            snprintf(
                texto,
                sizeof texto,
                "%d",
                registro->pontos
            );

            DrawText(
                texto,
                505,
                y + 5,
                15,
                i == 0
                    ? COR_AMARELO
                    : COR_TEXTO
            );

            snprintf(
                texto,
                sizeof texto,
                "%d",
                registro->portoes
            );

            DrawText(
                texto,
                638,
                y + 5,
                15,
                COR_CIANO
            );

            snprintf(
                texto,
                sizeof texto,
                "%d",
                registro->abatidos
            );

            DrawText(
                texto,
                733,
                y + 5,
                15,
                COR_VERDE
            );
        }
    }

    desenhaBotao(
        render_area_botao_voltar_ranking(),
        "VOLTAR",
        voltarHover,
        COR_CIANO
    );

    textoCentro(
        "ENTER, R ou ESC tambem voltam",
        LARGURA_TELA / 2,
        520,
        11,
        COR_TEXTO_FRACO
    );
}

/* ============================================================
 * ENTRADA DO NOME
 * ============================================================ */

void render_entrada_nome(
    const char *nome,
    int pontos,
    int posicaoEstimada,
    bool salvarHover,
    const char *aviso
) {
    fundoEscuro();

    painel(
        235,
        93,
        490,
        384,
        (Color){
            24,
            27,
            43,
            252
        },
        COR_AMARELO,
        3.0f
    );

    textoCentro(
        "NOVO RECORDE!",
        LARGURA_TELA / 2,
        117,
        34,
        COR_AMARELO
    );

    textoCentro(
        "Sua pontuacao entrou no TOP 10",
        LARGURA_TELA / 2,
        164,
        17,
        COR_TEXTO
    );

    char texto[96];

    snprintf(
        texto,
        sizeof texto,
        "Posicao estimada: %do lugar",
        posicaoEstimada
    );

    textoCentro(
        texto,
        LARGURA_TELA / 2,
        197,
        16,
        COR_CIANO
    );

    snprintf(
        texto,
        sizeof texto,
        "Pontos: %d",
        pontos
    );

    textoCentro(
        texto,
        LARGURA_TELA / 2,
        224,
        16,
        COR_VERDE
    );

    DrawText(
        "DIGITE SEU NOME",
        315,
        270,
        15,
        COR_TEXTO_FRACO
    );

    Rectangle campoNome = {
        315.0f,
        294.0f,
        330.0f,
        58.0f
    };

    DrawRectangleRec(
        campoNome,
        (Color){
            42,
            46,
            64,
            255
        }
    );

    DrawRectangleLinesEx(
        campoNome,
        2.0f,
        COR_CIANO
    );

    bool nomePreenchido =
        nome != NULL &&
        nome[0] != '\0';

    const char *nomeVisivel =
        nomePreenchido
            ? nome
            : "Seu nome...";

    DrawText(
        nomeVisivel,
        330,
        311,
        23,
        nomePreenchido
            ? WHITE
            : (Color){
                117,
                122,
                142,
                255
            }
    );

    /*
     * Cursor piscante.
     */
    if (
        nome != NULL &&
        (
            (int)(
                GetTime() *
                2.0
            ) %
            2
        ) ==
        0
    ) {
        int xCursor =
            330 +
            MeasureText(
                nome,
                23
            ) +
            2;

        DrawRectangle(
            xCursor,
            309,
            2,
            27,
            COR_CIANO
        );
    }

    textoCentro(
        "Maximo de 15 caracteres",
        LARGURA_TELA / 2,
        363,
        12,
        COR_TEXTO_FRACO
    );

    if (
        aviso != NULL &&
        aviso[0] != '\0'
    ) {
        textoCentro(
            aviso,
            LARGURA_TELA / 2,
            389,
            12,
            COR_VERMELHO
        );
    }

    desenhaBotao(
        render_area_botao_salvar_nome(),
        "SALVAR NO RANKING",
        salvarHover,
        COR_VERDE
    );

    textoCentro(
        "ENTER salva | ESC usa o nome PLAYER",
        LARGURA_TELA / 2,
        466,
        12,
        COR_TEXTO_FRACO
    );
}