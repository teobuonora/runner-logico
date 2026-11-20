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
 * CENÁRIO
 * ============================================================ */

static void desenhaCeu(
    const TemaCenario *tema
) {
    const int faixas =
        60;

    const int alturaFaixa =
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
            (float)(faixas - 1);

        Color cor =
            misturaCor(
                tema->ceuTopo,
                tema->ceuBase,
                progresso
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

static void desenhaEstrelas(
    const Mundo *m,
    const TemaCenario *tema
) {
    int luminosidade =
        (
            tema->ceuTopo.r +
            tema->ceuTopo.g +
            tema->ceuTopo.b
        ) /
        3;

    float visibilidade =
        1.0f -
        (float)luminosidade /
        210.0f;

    visibilidade =
        limita01(
            visibilidade
        );

    for (
        int i = 0;
        i < 28;
        i++
    ) {
        float baseX =
            (float)(
                (i * 83 + 43) %
                LARGURA_TELA
            );

        float x =
            fmodf(
                baseX -
                    m->distancia *
                    0.015f,
                (float)LARGURA_TELA
            );

        if (x < 0.0f) {
            x +=
                LARGURA_TELA;
        }

        int y =
            28 +
            (i * 47) %
            185;

        float raio =
            i % 4 == 0
                ? 1.8f
                : 1.0f;

        Color estrela =
            tema->estrela;

        estrela.a =
            (unsigned char)(
                estrela.a *
                (
                    0.25f +
                    visibilidade *
                    0.75f
                )
            );

        DrawCircle(
            (int)x,
            y,
            raio,
            estrela
        );
    }
}

static void desenhaSol(
    const TemaCenario *tema,
    int progresso
) {
    int solX =
        785 -
        progresso * 11;

    int solY =
        126 -
        progresso * 4;

    float raio =
        44.0f +
        progresso * 0.8f;

    DrawCircle(
        solX,
        solY,
        raio + 19.0f,
        tema->solBrilho
    );

    DrawCircle(
        solX,
        solY,
        raio,
        tema->sol
    );

    Color reflexo =
        misturaCor(
            tema->sol,
            WHITE,
            0.45f
        );

    reflexo.a =
        135;

    DrawCircle(
        solX - 15,
        solY - 15,
        13.0f,
        reflexo
    );
}

static void desenhaNuvem(
    float x,
    float y,
    float escala,
    const TemaCenario *tema
) {
    DrawEllipse(
        (int)x + 28,
        (int)y + 10,
        35.0f * escala,
        10.0f * escala,
        tema->nuvemSombra
    );

    DrawCircle(
        (int)x,
        (int)y,
        15.0f * escala,
        tema->nuvem
    );

    DrawCircle(
        (int)x +
            (int)(
                20.0f *
                escala
            ),
        (int)y -
            (int)(
                8.0f *
                escala
            ),
        21.0f * escala,
        tema->nuvem
    );

    DrawCircle(
        (int)x +
            (int)(
                43.0f *
                escala
            ),
        (int)y,
        17.0f * escala,
        tema->nuvem
    );

    DrawRectangle(
        (int)x,
        (int)y,
        (int)(
            43.0f *
            escala
        ),
        (int)(
            17.0f *
            escala
        ),
        tema->nuvem
    );
}

static void desenhaNuvens(
    const Mundo *m,
    const TemaCenario *tema
) {
    for (
        int i = 0;
        i < 5;
        i++
    ) {
        float ciclo =
            LARGURA_TELA +
            240.0f;

        float x =
            fmodf(
                i * 245.0f -
                    m->distancia *
                    0.07f,
                ciclo
            );

        if (x < 0.0f) {
            x += ciclo;
        }

        x -=
            100.0f;

        float y =
            72.0f +
            (float)(
                (i * 43) %
                105
            );

        float escala =
            0.75f +
            (float)(i % 3) *
            0.12f;

        desenhaNuvem(
            x,
            y,
            escala,
            tema
        );
    }
}

static void desenhaMontanhas(
    const Mundo *m,
    float velocidade,
    int baseY,
    int altura,
    int largura,
    Color cor
) {
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
            cor
        );
    }
}

static void desenhaEstruturasTecnologicas(
    const Mundo *m,
    const TemaCenario *tema
) {
    float deslocamento =
        fmodf(
            m->distancia *
                0.18f,
            230.0f
        );

    for (
        int i = -1;
        i < 6;
        i++
    ) {
        float x =
            i * 230.0f -
            deslocamento;

        int altura =
            35 +
            (i & 1) *
            24;

        Color torre =
            misturaCor(
                tema->montanha3,
                COR_PAINEL,
                0.35f
            );

        DrawRectangle(
            (int)x,
            (int)CHAO_Y -
                altura,
            10,
            altura,
            torre
        );

        DrawRectangle(
            (int)x - 7,
            (int)CHAO_Y -
                altura,
            24,
            5,
            tema->montanha3
        );

        DrawCircle(
            (int)x + 5,
            (int)CHAO_Y -
                altura -
                6,
            5.0f,
            tema->tecnologia
        );
    }
}

static void desenhaChao(
    const Mundo *m,
    const TemaCenario *tema
) {
    DrawRectangle(
        0,
        (int)CHAO_Y,
        LARGURA_TELA,
        ALTURA_TELA -
            (int)CHAO_Y,
        tema->chao
    );

    DrawRectangle(
        0,
        (int)CHAO_Y,
        LARGURA_TELA,
        4,
        tema->chaoTopo
    );

    Color faixaInferior =
        misturaCor(
            tema->chao,
            tema->chaoTopo,
            0.42f
        );

    DrawRectangle(
        0,
        (int)CHAO_Y + 4,
        LARGURA_TELA,
        3,
        faixaInferior
    );

    for (
        int i = 0;
        i < 28;
        i++
    ) {
        float x =
            fmodf(
                i * 52.0f -
                    m->distancia,
                (float)LARGURA_TELA
            );

        if (x < 0.0f) {
            x +=
                LARGURA_TELA;
        }

        int y =
            (int)CHAO_Y +
            14 +
            (i % 4) *
            9;

        int largura =
            11 +
            (i % 3) *
            8;

        DrawRectangle(
            (int)x,
            y,
            largura,
            2,
            tema->chaoLuz
        );
    }

    Color linhaFundo =
        misturaCor(
            tema->chao,
            tema->chaoLuz,
            0.45f
        );

    for (
        int i = 0;
        i < 12;
        i++
    ) {
        float x =
            fmodf(
                i * 95.0f -
                    m->distancia *
                    1.25f,
                (float)LARGURA_TELA
            );

        if (x < 0.0f) {
            x +=
                LARGURA_TELA;
        }

        DrawLineEx(
            (Vector2){
                x,
                CHAO_Y +
                    64.0f
            },
            (Vector2){
                x + 28.0f,
                CHAO_Y +
                    64.0f
            },
            2.0f,
            linhaFundo
        );
    }
}

static void desenhaCenario(
    const Mundo *m
) {
    TemaCenario tema =
        temaAtual(m);

    int progresso =
        m->portoesOk;

    if (progresso < 0) {
        progresso = 0;
    }

    if (
        progresso >
        META_PORTOES
    ) {
        progresso =
            META_PORTOES;
    }

    desenhaCeu(
        &tema
    );

    desenhaEstrelas(
        m,
        &tema
    );

    desenhaSol(
        &tema,
        progresso
    );

    desenhaNuvens(
        m,
        &tema
    );

    desenhaMontanhas(
        m,
        0.025f,
        380,
        135,
        300,
        tema.montanha1
    );

    desenhaMontanhas(
        m,
        0.055f,
        410,
        108,
        230,
        tema.montanha2
    );

    desenhaMontanhas(
        m,
        0.105f,
        440,
        78,
        175,
        tema.montanha3
    );

    desenhaEstruturasTecnologicas(
        m,
        &tema
    );

    desenhaChao(
        m,
        &tema
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

    const Color contorno =
        { 20, 70, 57, 255 };

    /*
     * Cauda.
     */
    DrawTriangle(
        (Vector2){
            r.x + 11.0f,
            r.y + 10.0f
        },
        (Vector2){
            r.x - 35.0f,
            r.y + 18.0f
        },
        (Vector2){
            r.x + 12.0f,
            r.y + 25.0f
        },
        escuro
    );

    DrawTriangle(
        (Vector2){
            r.x + 9.0f,
            r.y + 13.0f
        },
        (Vector2){
            r.x - 28.0f,
            r.y + 18.0f
        },
        (Vector2){
            r.x + 10.0f,
            r.y + 21.0f
        },
        corpo
    );

    /*
     * Corpo horizontal.
     */
    DrawEllipse(
        (int)(r.x + 18.0f),
        (int)(r.y + 17.0f),
        29.0f,
        15.0f,
        contorno
    );

    DrawEllipse(
        (int)(r.x + 19.0f),
        (int)(r.y + 15.0f),
        27.0f,
        13.0f,
        corpo
    );

    /*
     * Pescoço.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x + 21.0f,
            r.y + 3.0f,
            22.0f,
            21.0f
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
            r.y + 1.0f,
            29.0f,
            19.0f
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
     * Linha da mandíbula.
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
     * Pernas recolhidas.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x + 7.0f,
            r.y + 22.0f,
            16.0f,
            7.0f
        },
        0.35f,
        4,
        escuro
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x + 25.0f,
            r.y + 22.0f,
            16.0f,
            7.0f
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
            ? 27.0f -
                saltoVisual *
                0.20f
            : (
                !m->noChao
                    ? 20.0f
                    : 38.0f
            );

    if (larguraSombra < 10.0f) {
        larguraSombra =
            10.0f;
    }

    DrawEllipse(
        (int)(r.x + 18.0f),
        (int)CHAO_Y + 3,
        larguraSombra,
        6.0f,
        (Color){
            15,
            22,
            31,
            145
        }
    );

    /*
     * Cauda.
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

    DrawTriangle(
        (Vector2){
            r.x + 9.0f,
            corpoY - 7.0f
        },
        (Vector2){
            r.x - 39.0f,
            corpoY +
                movimentoCauda
        },
        (Vector2){
            r.x + 10.0f,
            corpoY + 13.0f
        },
        escuro
    );

    DrawTriangle(
        (Vector2){
            r.x + 7.0f,
            corpoY - 3.0f
        },
        (Vector2){
            r.x - 32.0f,
            corpoY +
                movimentoCauda
        },
        (Vector2){
            r.x + 9.0f,
            corpoY + 8.0f
        },
        corpo
    );

    /*
     * Corpo principal.
     */
    DrawEllipse(
        (int)(r.x + 16.0f),
        (int)corpoY,
        26.0f,
        27.0f,
        contorno
    );

    DrawEllipse(
        (int)(r.x + 17.0f),
        (int)(corpoY - 2.0f),
        24.0f,
        25.0f,
        corpo
    );

    /*
     * Barriga.
     */
    DrawEllipse(
        (int)(r.x + 22.0f),
        (int)(corpoY + 5.0f),
        11.0f,
        16.0f,
        barriga
    );

    /*
     * Pescoço.
     */
    DrawRectangleRounded(
        (Rectangle){
            r.x + 20.0f,
            corpoY - 34.0f,
            19.0f,
            41.0f
        },
        0.42f,
        7,
        corpo
    );

    DrawTriangle(
        (Vector2){
            r.x + 18.0f,
            corpoY - 5.0f
        },
        (Vector2){
            r.x + 36.0f,
            corpoY - 34.0f
        },
        (Vector2){
            r.x + 39.0f,
            corpoY + 2.0f
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
            r.x + 42.0f,
            bracoFrenteY
        },
        4.0f,
        corpo
    );

    DrawLineEx(
        (Vector2){
            r.x + 42.0f,
            bracoFrenteY
        },
        (Vector2){
            r.x + 49.0f,
            bracoFrenteY -
                2.0f
        },
        2.5f,
        corpo
    );

    DrawLineEx(
        (Vector2){
            r.x + 19.0f,
            corpoY - 8.0f
        },
        (Vector2){
            r.x + 8.0f,
            bracoTrasY
        },
        4.0f,
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
                r.x + 8.0f,
                corpoY + 17.0f
            },
            (Vector2){
                r.x + 2.0f,
                corpoY + 30.0f
            },
            9.0f,
            escuro
        );

        DrawLineEx(
            (Vector2){
                r.x + 2.0f,
                corpoY + 30.0f
            },
            (Vector2){
                r.x + 15.0f,
                corpoY + 34.0f
            },
            6.0f,
            escuro
        );

        DrawLineEx(
            (Vector2){
                r.x + 26.0f,
                corpoY + 16.0f
            },
            (Vector2){
                r.x + 34.0f,
                corpoY + 28.0f
            },
            9.0f,
            corpo
        );

        DrawLineEx(
            (Vector2){
                r.x + 34.0f,
                corpoY + 28.0f
            },
            (Vector2){
                r.x + 46.0f,
                corpoY + 24.0f
            },
            6.0f,
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
                r.x + 9.0f,
                corpoY + 17.0f
            },
            (Vector2){
                r.x + 7.0f +
                    tras,
                joelhoY
            },
            9.0f,
            escuro
        );

        DrawLineEx(
            (Vector2){
                r.x + 7.0f +
                    tras,
                joelhoY
            },
            (Vector2){
                r.x + 10.0f -
                    tras *
                    0.45f,
                peY
            },
            6.0f,
            escuro
        );

        DrawLineEx(
            (Vector2){
                r.x + 7.0f -
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
            5.0f,
            escuro
        );

        DrawLineEx(
            (Vector2){
                r.x + 26.0f,
                corpoY + 16.0f
            },
            (Vector2){
                r.x + 28.0f +
                    frente,
                joelhoY
            },
            9.0f,
            corpo
        );

        DrawLineEx(
            (Vector2){
                r.x + 28.0f +
                    frente,
                joelhoY
            },
            (Vector2){
                r.x + 30.0f -
                    frente *
                    0.45f,
                peY
            },
            6.0f,
            corpo
        );

        DrawLineEx(
            (Vector2){
                r.x + 28.0f -
                    frente *
                    0.45f,
                peY
            },
            (Vector2){
                r.x + 45.0f -
                    frente *
                    0.45f,
                peY
            },
            5.0f,
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

    Color corpo =
        e->vida >= 2
            ? COR_ROXO
            : (Color){
                218,
                135,
                239,
                255
            };

    Color escuro =
        { 91, 45, 130, 255 };

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
            0.65f,
        5.0f,
        (Color){
            20,
            26,
            36,
            130
        }
    );

    DrawRectangleRounded(
        r,
        0.35f,
        7,
        corpo
    );

    DrawRectangle(
        (int)r.x + 5,
        (int)(
            r.y +
            r.height -
            8.0f
        ),
        (int)r.width -
            10,
        8,
        escuro
    );

    DrawCircle(
        (int)r.x + 13,
        (int)r.y + 15,
        7.0f,
        WHITE
    );

    DrawCircle(
        (int)r.x + 29,
        (int)r.y + 15,
        7.0f,
        WHITE
    );

    DrawCircle(
        (int)r.x + 15,
        (int)r.y + 16,
        3.0f,
        COR_PAINEL
    );

    DrawCircle(
        (int)r.x + 27,
        (int)r.y + 16,
        3.0f,
        COR_PAINEL
    );

    DrawRectangle(
        (int)r.x + 10,
        (int)(
            r.y +
            r.height
        ),
        8,
        4,
        escuro
    );

    DrawRectangle(
        (int)r.x + 25,
        (int)(
            r.y +
            r.height
        ),
        8,
        4,
        escuro
    );
}

static void desenhaVoador(
    const Entidade *e
) {
    Rectangle r =
        e->caixa;

    bool asaAlta =
        (
            (int)(
                e->anim *
                10.0f
            ) %
            2
        ) ==
        0;

    Color corpo =
        COR_LARANJA;

    Color asa =
        { 255, 180, 89, 255 };

    Color escuro =
        { 130, 58, 42, 255 };

    DrawEllipse(
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
        r.width *
            0.5f,
        r.height *
            0.5f,
        corpo
    );

    DrawTriangle(
        (Vector2){
            r.x + 14.0f,
            r.y +
                r.height /
                2.0f
        },
        (Vector2){
            r.x + 25.0f,
            r.y +
                (
                    asaAlta
                        ? -16.0f
                        : 12.0f
                )
        },
        (Vector2){
            r.x + 37.0f,
            r.y +
                r.height /
                2.0f
        },
        asa
    );

    DrawTriangle(
        (Vector2){
            r.x +
                r.width -
                2.0f,
            r.y +
                r.height *
                0.32f
        },
        (Vector2){
            r.x +
                r.width +
                12.0f,
            r.y +
                r.height *
                0.5f
        },
        (Vector2){
            r.x +
                r.width -
                2.0f,
            r.y +
                r.height *
                0.68f
        },
        escuro
    );

    DrawCircle(
        (int)r.x + 10,
        (int)r.y + 9,
        5.0f,
        WHITE
    );

    DrawCircle(
        (int)r.x + 9,
        (int)r.y + 9,
        2.0f,
        COR_PAINEL
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