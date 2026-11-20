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
 *
 * Tema 0: início da partida.
 * Temas 1 a 7: progresso pelos portões.
 * Tema 8: cenário final.
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

    /* 4 portões: pôr do sol alaranjado */
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

static Color misturaCor(Color a, Color b, float t) {
    Color resultado;

    resultado.r = (unsigned char)(a.r + (b.r - a.r) * t);
    resultado.g = (unsigned char)(a.g + (b.g - a.g) * t);
    resultado.b = (unsigned char)(a.b + (b.b - a.b) * t);
    resultado.a = (unsigned char)(a.a + (b.a - a.a) * t);

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

static float suavizaTransicao(float t) {
    t = limita01(t);

    return t * t * (3.0f - 2.0f * t);
}

static TemaCenario misturaTema(
    TemaCenario anterior,
    TemaCenario atual,
    float t
) {
    TemaCenario resultado;

    t = suavizaTransicao(t);

    resultado.ceuTopo =
        misturaCor(anterior.ceuTopo, atual.ceuTopo, t);

    resultado.ceuBase =
        misturaCor(anterior.ceuBase, atual.ceuBase, t);

    resultado.sol =
        misturaCor(anterior.sol, atual.sol, t);

    resultado.solBrilho =
        misturaCor(anterior.solBrilho, atual.solBrilho, t);

    resultado.montanha1 =
        misturaCor(anterior.montanha1, atual.montanha1, t);

    resultado.montanha2 =
        misturaCor(anterior.montanha2, atual.montanha2, t);

    resultado.montanha3 =
        misturaCor(anterior.montanha3, atual.montanha3, t);

    resultado.chao =
        misturaCor(anterior.chao, atual.chao, t);

    resultado.chaoTopo =
        misturaCor(anterior.chaoTopo, atual.chaoTopo, t);

    resultado.chaoLuz =
        misturaCor(anterior.chaoLuz, atual.chaoLuz, t);

    resultado.nuvem =
        misturaCor(anterior.nuvem, atual.nuvem, t);

    resultado.nuvemSombra =
        misturaCor(
            anterior.nuvemSombra,
            atual.nuvemSombra,
            t
        );

    resultado.estrela =
        misturaCor(anterior.estrela, atual.estrela, t);

    resultado.tecnologia =
        misturaCor(
            anterior.tecnologia,
            atual.tecnologia,
            t
        );

    return resultado;
}

static TemaCenario temaAtual(const Mundo *m) {
    int indice = m->portoesOk;

    if (indice < 0) {
        indice = 0;
    }

    if (indice > META_PORTOES) {
        indice = META_PORTOES;
    }

    bool transicaoAtiva =
        indice > 0 &&
        m->msgTempo > 0.0f &&
        strstr(m->msg, "PORTAO ABERTO") != NULL;

    if (!transicaoAtiva) {
        return TEMAS_CENARIO[indice];
    }

    float progresso =
        1.0f -
        m->msgTempo / 1.8f;

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
        centroX - MeasureText(texto, tamanho) / 2,
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

    DrawRectangleRec(area, fundo);
    DrawRectangleLinesEx(area, espessura, borda);
}

static void linhaBrilho(
    Vector2 inicio,
    Vector2 fim,
    float espessura,
    Color cor
) {
    Color brilho = cor;
    brilho.a = 55;

    DrawLineEx(inicio, fim, espessura + 5.0f, brilho);
    DrawLineEx(inicio, fim, espessura, cor);
}

static void circuloBrilho(
    int x,
    int y,
    float raio,
    Color cor
) {
    Color brilho = cor;
    brilho.a = 45;

    DrawCircle(x, y, raio + 7.0f, brilho);
    DrawCircle(x, y, raio, cor);
}

/* ============================================================
 * CENÁRIO
 * ============================================================ */

static void desenhaCeu(const TemaCenario *tema) {
    const int faixas = 60;

    const int alturaFaixa =
        (int)CHAO_Y / faixas + 1;

    for (int i = 0; i < faixas; i++) {
        float t =
            (float)i /
            (float)(faixas - 1);

        Color cor =
            misturaCor(
                tema->ceuTopo,
                tema->ceuBase,
                t
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
        limita01(visibilidade);

    for (int i = 0; i < 28; i++) {
        float baseX =
            (float)(
                (i * 83 + 43) %
                LARGURA_TELA
            );

        float x = fmodf(
            baseX -
            m->distancia * 0.015f,
            (float)LARGURA_TELA
        );

        if (x < 0.0f) {
            x += LARGURA_TELA;
        }

        int y =
            28 +
            (i * 47) % 185;

        float raio =
            (i % 4 == 0)
                ? 1.8f
                : 1.0f;

        Color estrela =
            tema->estrela;

        estrela.a =
            (unsigned char)(
                estrela.a *
                (
                    0.25f +
                    visibilidade * 0.75f
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

    reflexo.a = 135;

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
            (int)(20.0f * escala),
        (int)y -
            (int)(8.0f * escala),
        21.0f * escala,
        tema->nuvem
    );

    DrawCircle(
        (int)x +
            (int)(43.0f * escala),
        (int)y,
        17.0f * escala,
        tema->nuvem
    );

    DrawRectangle(
        (int)x,
        (int)y,
        (int)(43.0f * escala),
        (int)(17.0f * escala),
        tema->nuvem
    );
}

static void desenhaNuvens(
    const Mundo *m,
    const TemaCenario *tema
) {
    for (int i = 0; i < 5; i++) {
        float ciclo =
            LARGURA_TELA +
            240.0f;

        float x = fmodf(
            i * 245.0f -
            m->distancia * 0.07f,
            ciclo
        );

        if (x < 0.0f) {
            x += ciclo;
        }

        x -= 100.0f;

        float y =
            72.0f +
            (float)((i * 43) % 105);

        float escala =
            0.75f +
            (float)(i % 3) * 0.12f;

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
    float deslocamento = fmodf(
        m->distancia * velocidade,
        (float)largura
    );

    for (
        int i = -2;
        i < LARGURA_TELA / largura + 3;
        i++
    ) {
        float x =
            i * largura -
            deslocamento;

        Vector2 esquerda = {
            x,
            (float)baseY
        };

        Vector2 pico = {
            x + largura * 0.5f,
            (float)(baseY - altura)
        };

        Vector2 direita = {
            x + largura,
            (float)baseY
        };

        DrawTriangle(
            esquerda,
            pico,
            direita,
            cor
        );
    }
}

static void desenhaEstruturasTecnologicas(
    const Mundo *m,
    const TemaCenario *tema
) {
    float deslocamento = fmodf(
        m->distancia * 0.18f,
        230.0f
    );

    for (int i = -1; i < 6; i++) {
        float x =
            i * 230.0f -
            deslocamento;

        int altura =
            35 +
            (i & 1) * 24;

        Color torre =
            misturaCor(
                tema->montanha3,
                COR_PAINEL,
                0.35f
            );

        DrawRectangle(
            (int)x,
            (int)CHAO_Y - altura,
            10,
            altura,
            torre
        );

        DrawRectangle(
            (int)x - 7,
            (int)CHAO_Y - altura,
            24,
            5,
            tema->montanha3
        );

        DrawCircle(
            (int)x + 5,
            (int)CHAO_Y - altura - 6,
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
        ALTURA_TELA - (int)CHAO_Y,
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

    for (int i = 0; i < 28; i++) {
        float x = fmodf(
            i * 52.0f -
            m->distancia,
            (float)LARGURA_TELA
        );

        if (x < 0.0f) {
            x += LARGURA_TELA;
        }

        int y =
            (int)CHAO_Y +
            14 +
            (i % 4) * 9;

        int largura =
            11 +
            (i % 3) * 8;

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

    for (int i = 0; i < 12; i++) {
        float x = fmodf(
            i * 95.0f -
            m->distancia * 1.25f,
            (float)LARGURA_TELA
        );

        if (x < 0.0f) {
            x += LARGURA_TELA;
        }

        DrawLineEx(
            (Vector2){
                x,
                CHAO_Y + 64.0f
            },
            (Vector2){
                x + 28.0f,
                CHAO_Y + 64.0f
            },
            2.0f,
            linhaFundo
        );
    }
}

static void desenhaCenario(const Mundo *m) {
    TemaCenario tema =
        temaAtual(m);

    int progresso =
        m->portoesOk;

    if (progresso < 0) {
        progresso = 0;
    }

    if (progresso > META_PORTOES) {
        progresso = META_PORTOES;
    }

    desenhaCeu(&tema);
    desenhaEstrelas(m, &tema);
    desenhaSol(&tema, progresso);
    desenhaNuvens(m, &tema);

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
        (Vector2){ r.x + 4.0f, r.y + r.height * 0.45f },
        (Vector2){ r.x - 27.0f, r.y + r.height * 0.52f },
        (Vector2){ r.x + 4.0f, r.y + r.height * 0.63f },
        COR_LARANJA
    );

    DrawTriangle(
        (Vector2){ r.x + 3.0f, r.y + r.height * 0.48f },
        (Vector2){ r.x - 18.0f, r.y + r.height * 0.53f },
        (Vector2){ r.x + 3.0f, r.y + r.height * 0.58f },
        COR_AMARELO
    );
}

static void desenhaJogador(const Mundo *m) {
    if (
        m->invulneravel > 0.0f &&
        ((int)(m->invulneravel * 10.0f) % 2) == 0
    ) {
        return;
    }

    Rectangle r =
        mundo_caixa_jogador(m);

    bool escudo =
        m->val.v[PROP_ESCUDO];

    bool arma =
        m->val.v[PROP_ARMA];

    bool turbo =
        m->val.v[PROP_TURBO];

    if (turbo) {
        desenhaTurbo(r);
    }

    DrawEllipse(
        (int)(r.x + 18.0f),
        (int)CHAO_Y + 3,
        34.0f,
        6.0f,
        (Color){ 19, 31, 39, 145 }
    );

    if (m->abaixado) {
        DrawEllipse(
            (int)(r.x + 18.0f),
            (int)(r.y + 16.0f),
            28.0f,
            14.0f,
            COR_VERDE_ESCURO
        );

        DrawRectangleRounded(
            (Rectangle){
                r.x + 8.0f,
                r.y + 3.0f,
                38.0f,
                22.0f
            },
            0.35f,
            6,
            COR_VERDE
        );

        DrawCircle(
            (int)r.x + 37,
            (int)r.y + 10,
            4.0f,
            WHITE
        );

        DrawCircle(
            (int)r.x + 39,
            (int)r.y + 10,
            2.0f,
            COR_PAINEL
        );
    } else {
        DrawTriangle(
            (Vector2){
                r.x + 8.0f,
                r.y + 30.0f
            },
            (Vector2){
                r.x - 25.0f,
                r.y + 40.0f
            },
            (Vector2){
                r.x + 10.0f,
                r.y + 45.0f
            },
            COR_VERDE_ESCURO
        );

        DrawEllipse(
            (int)(r.x + 17.0f),
            (int)(r.y + 33.0f),
            23.0f,
            27.0f,
            COR_VERDE
        );

        DrawRectangleRounded(
            (Rectangle){
                r.x + 16.0f,
                r.y - 11.0f,
                34.0f,
                25.0f
            },
            0.24f,
            6,
            COR_VERDE
        );

        DrawRectangle(
            (int)r.x + 34,
            (int)r.y + 5,
            17,
            10,
            COR_VERDE
        );

        DrawCircle(
            (int)r.x + 40,
            (int)r.y - 3,
            5.0f,
            WHITE
        );

        DrawCircle(
            (int)r.x + 42,
            (int)r.y - 3,
            2.5f,
            COR_PAINEL
        );

        DrawRectangle(
            (int)r.x + 42,
            (int)r.y + 10,
            8,
            2,
            COR_VERDE_ESCURO
        );

        DrawLineEx(
            (Vector2){
                r.x + 20.0f,
                r.y + 27.0f
            },
            (Vector2){
                r.x + 34.0f,
                r.y + 32.0f
            },
            4.0f,
            COR_VERDE_ESCURO
        );

        bool passo =
            m->noChao &&
            ((int)(m->tempo * 12.0f) % 2 == 0);

        int perna1 =
            passo ? 6 : 1;

        int perna2 =
            passo ? 1 : 6;

        DrawRectangle(
            (int)r.x + 8,
            (int)(r.y + r.height) - perna1,
            9,
            perna1 + 5,
            COR_VERDE
        );

        DrawRectangle(
            (int)r.x + 26,
            (int)(r.y + r.height) - perna2,
            9,
            perna2 + 5,
            COR_VERDE_ESCURO
        );

        DrawRectangle(
            (int)r.x + 5,
            (int)(r.y + r.height) - 2,
            15,
            4,
            COR_VERDE
        );

        DrawRectangle(
            (int)r.x + 23,
            (int)(r.y + r.height) - 2,
            15,
            4,
            COR_VERDE_ESCURO
        );
    }

    if (arma) {
        Rectangle armaRetangulo = {
            r.x + r.width - 1.0f,
            r.y + r.height * 0.35f,
            20.0f,
            8.0f
        };

        DrawRectangleRounded(
            armaRetangulo,
            0.3f,
            5,
            (Color){ 64, 69, 90, 255 }
        );

        DrawRectangle(
            (int)armaRetangulo.x + 13,
            (int)armaRetangulo.y + 2,
            10,
            4,
            COR_CIANO
        );
    }

    if (escudo) {
        Color preenchimento =
            COR_AZUL;

        preenchimento.a = 34;

        DrawCircle(
            (int)(r.x + r.width / 2.0f),
            (int)(r.y + r.height / 2.0f),
            49.0f,
            preenchimento
        );

        DrawCircleLines(
            (int)(r.x + r.width / 2.0f),
            (int)(r.y + r.height / 2.0f),
            49.0f,
            COR_CIANO
        );

        DrawCircleLines(
            (int)(r.x + r.width / 2.0f),
            (int)(r.y + r.height / 2.0f),
            46.0f,
            (Color){ 126, 245, 248, 115 }
        );
    }
}

/* ============================================================
 * ENTIDADES
 * ============================================================ */

static void desenhaCacto(Rectangle r) {
    Color sombra =
        { 24, 70, 62, 255 };

    Color corpo =
        { 45, 142, 104, 255 };

    Color detalhe =
        { 85, 190, 132, 255 };

    DrawEllipse(
        (int)(r.x + r.width / 2.0f),
        (int)(r.y + r.height),
        r.width * 0.68f,
        5.0f,
        (Color){ 20, 27, 38, 130 }
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x + r.width * 0.30f,
            r.y,
            r.width * 0.40f,
            r.height
        },
        0.4f,
        7,
        corpo
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x,
            r.y + r.height * 0.35f,
            r.width * 0.42f,
            8.0f
        },
        0.4f,
        6,
        sombra
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x,
            r.y + r.height * 0.22f,
            7.0f,
            r.height * 0.28f
        },
        0.4f,
        6,
        corpo
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x + r.width * 0.60f,
            r.y + r.height * 0.48f,
            r.width * 0.40f,
            8.0f
        },
        0.4f,
        6,
        corpo
    );

    DrawRectangleRounded(
        (Rectangle){
            r.x + r.width - 7.0f,
            r.y + r.height * 0.34f,
            7.0f,
            r.height * 0.27f
        },
        0.4f,
        6,
        corpo
    );

    DrawLine(
        (int)(r.x + r.width * 0.5f),
        (int)r.y + 7,
        (int)(r.x + r.width * 0.5f),
        (int)(r.y + r.height - 5.0f),
        detalhe
    );
}

static void desenhaAndarilho(const Entidade *e) {
    Rectangle r =
        e->caixa;

    Color corpo =
        e->vida >= 2
            ? COR_ROXO
            : (Color){ 218, 135, 239, 255 };

    Color escuro =
        { 91, 45, 130, 255 };

    DrawEllipse(
        (int)(r.x + r.width / 2.0f),
        (int)(r.y + r.height),
        r.width * 0.65f,
        5.0f,
        (Color){ 20, 26, 36, 130 }
    );

    DrawRectangleRounded(
        r,
        0.35f,
        7,
        corpo
    );

    DrawRectangle(
        (int)r.x + 5,
        (int)(r.y + r.height - 8.0f),
        (int)r.width - 10,
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
        (int)(r.y + r.height),
        8,
        4,
        escuro
    );

    DrawRectangle(
        (int)r.x + 25,
        (int)(r.y + r.height),
        8,
        4,
        escuro
    );
}

static void desenhaVoador(const Entidade *e) {
    Rectangle r =
        e->caixa;

    bool asaAlta =
        ((int)(e->anim * 10.0f) % 2) == 0;

    Color corpo =
        COR_LARANJA;

    Color asa =
        { 255, 180, 89, 255 };

    Color escuro =
        { 130, 58, 42, 255 };

    DrawEllipse(
        (int)(r.x + r.width / 2.0f),
        (int)(r.y + r.height / 2.0f),
        r.width * 0.5f,
        r.height * 0.5f,
        corpo
    );

    Vector2 p1 = {
        r.x + 14.0f,
        r.y + r.height / 2.0f
    };

    Vector2 p2 = {
        r.x + 25.0f,
        r.y + (asaAlta ? -16.0f : 12.0f)
    };

    Vector2 p3 = {
        r.x + 37.0f,
        r.y + r.height / 2.0f
    };

    DrawTriangle(
        p1,
        p2,
        p3,
        asa
    );

    DrawTriangle(
        (Vector2){
            r.x + r.width - 2.0f,
            r.y + r.height * 0.32f
        },
        (Vector2){
            r.x + r.width + 12.0f,
            r.y + r.height * 0.5f
        },
        (Vector2){
            r.x + r.width - 2.0f,
            r.y + r.height * 0.68f
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

static void desenhaPowerUp(const Entidade *e) {
    Rectangle r =
        e->caixa;

    Color cor =
        COR_AMARELO;

    const char *letra =
        "E";

    if (e->tipo == ENT_PU_VIDA) {
        cor = COR_VERMELHO;
        letra = "+";
    }

    if (e->tipo == ENT_PU_TAUTOLOGIA) {
        cor = COR_ROXO;
        letra = "T";
    }

    int centroX =
        (int)(r.x + r.width / 2.0f);

    int centroY =
        (int)(r.y + r.height / 2.0f);

    Color aura =
        cor;

    aura.a = 35;

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

static void desenhaPortao(const Entidade *e) {
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
        (Color){ 35, 39, 59, 255 }
    );

    DrawRectangle(
        (int)(r.x + r.width + 2.0f),
        (int)r.y,
        (int)colunaLargura,
        (int)r.height,
        (Color){ 35, 39, 59, 255 }
    );

    DrawRectangle(
        (int)r.x - 8,
        (int)r.y,
        (int)r.width + 24,
        (int)arcoAltura,
        (Color){ 35, 39, 59, 255 }
    );

    DrawRectangle(
        (int)r.x + 6,
        (int)r.y + (int)arcoAltura,
        (int)r.width - 4,
        (int)r.height - (int)arcoAltura,
        campo
    );

    linhaBrilho(
        (Vector2){
            r.x - 1.0f,
            r.y + 8.0f
        },
        (Vector2){
            r.x - 1.0f,
            r.y + r.height
        },
        2.0f,
        cor
    );

    linhaBrilho(
        (Vector2){
            r.x + r.width + 9.0f,
            r.y + 8.0f
        },
        (Vector2){
            r.x + r.width + 9.0f,
            r.y + r.height
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
            r.x + r.width + 9.0f,
            r.y + 8.0f
        },
        2.0f,
        cor
    );

    for (int i = 0; i < 5; i++) {
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
                r.x + r.width + 1.0f,
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
        catalogoFormula(e->formulaId);

    char texto[64];

    formulaParaTexto(
        &formula,
        texto,
        sizeof texto
    );

    int tamanho =
        22;

    int largura =
        MeasureText(texto, tamanho) + 28;

    int x =
        (int)(
            r.x +
            r.width / 2.0f
        ) -
        largura / 2;

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
        x + largura / 2,
        y + 5,
        tamanho,
        COR_TEXTO
    );
}

static void desenhaEntidade(const Entidade *e) {
    switch (e->tipo) {
        case ENT_CACTO:
            desenhaCacto(e->caixa);
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

static void desenhaTiro(const Tiro *tiro) {
    Rectangle r =
        tiro->caixa;

    Color brilho =
        COR_CIANO;

    brilho.a = 45;

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
            : (Color){ 82, 84, 101, 255 };

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
            (float)x - 11.0f,
            (float)y + 2.0f
        },
        (Vector2){
            (float)x + 11.0f,
            (float)y + 2.0f
        },
        (Vector2){
            (float)x,
            (float)y + 14.0f
        },
        cor
    );
}

static void desenhaTabela(
    const Mundo *m,
    const Entidade *portao
) {
    Formula formula =
        catalogoFormula(portao->formulaId);

    int x =
        LARGURA_TELA - 190;

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
        (Color){ 27, 30, 45, 242 },
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
        x + largura - 10,
        y + 45,
        (Color){ 102, 108, 132, 255 }
    );

    unsigned atual =
        mascaraDeValoracao(m->val);

    for (unsigned i = 0; i < 8; i++) {
        Valoracao v =
            valoracaoDeMascara(i);

        bool resultado =
            avalia(&formula, v);

        int linhaY =
            y + 49 + (int)i * 12;

        if (i == atual) {
            DrawRectangle(
                x + 8,
                linhaY - 1,
                largura - 16,
                12,
                (Color){ 74, 151, 255, 70 }
            );
        }

        char linha[32];

        snprintf(
            linha,
            sizeof linha,
            "%c  %c  %c  |  %c",
            v.v[0] ? 'V' : 'F',
            v.v[1] ? 'V' : 'F',
            v.v[2] ? 'V' : 'F',
            resultado ? 'V' : 'F'
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
    static const char *poderes[NUM_PROPS] = {
        "ESCUDO",
        "ARMA",
        "TURBO"
    };

    static const Color cores[NUM_PROPS] = {
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
            : (Color){ 79, 83, 103, 255 },
        ligado ? 2.0f : 1.0f
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
            : (Color){ 69, 72, 89, 255 },
        ligado
            ? WHITE
            : (Color){ 116, 120, 140, 255 },
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
            : (Color){ 92, 95, 110, 255 }
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
        (Color){ 27, 30, 44, 218 },
        (Color){ 74, 78, 104, 210 },
        1.0f
    );

    DrawText(
        "VIDAS",
        22,
        17,
        12,
        COR_TEXTO_FRACO
    );

    for (int i = 0; i < VIDAS_MAX; i++) {
        desenhaCoracao(
            29 + i * 27,
            39,
            i < m->vidas
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
        (Color){ 66, 69, 86, 255 }
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
        (Color){ 27, 30, 44, 218 },
        (Color){ 74, 78, 104, 210 },
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
        LARGURA_TELA / 2,
        16,
        27,
        COR_TEXTO
    );

    textoCentro(
        "PONTOS",
        LARGURA_TELA / 2,
        45,
        11,
        COR_TEXTO_FRACO
    );

    painel(
        746,
        10,
        202,
        54,
        (Color){ 27, 30, 44, 218 },
        (Color){ 74, 78, 104, 210 },
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
        (Color){ 67, 70, 87, 255 }
    );

    DrawRectangle(
        761,
        46,
        progresso,
        7,
        COR_CIANO
    );

    if (m->sobrecarga > 0.0f) {
        textoCentro(
            "SOBRECARGA",
            LARGURA_TELA / 2,
            72,
            17,
            COR_VERMELHO
        );
    }

    if (m->tautologia) {
        textoCentro(
            "TAUTOLOGIA ATIVA",
            LARGURA_TELA / 2,
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
            (Color){ 27, 30, 44, 228 },
            (Color){ 92, 97, 121, 220 },
            1.0f
        );

        textoCentro(
            m->msg,
            LARGURA_TELA / 2,
            102,
            19,
            COR_TEXTO
        );
    }

    for (int i = 0; i < NUM_PROPS; i++) {
        desenhaCartao(
            m,
            i,
            12 + i * 190
        );
    }

    painel(
        588,
        467,
        360,
        58,
        COR_PAINEL,
        (Color){ 79, 83, 103, 255 },
        1.0f
    );

    const Entidade *portao =
        mundo_proximo_portao(m);

    if (portao != NULL) {
        Formula formula =
            catalogoFormula(portao->formulaId);

        char formulaTexto[64];

        formulaParaTexto(
            &formula,
            formulaTexto,
            sizeof formulaTexto
        );

        if (m->decisaoPortao) {
            DrawText(
                "DESAFIO LOGICO EM ANDAMENTO",
                602,
                475,
                15,
                COR_AMARELO
            );

            DrawText(
                "1, 2 e 3 alteram  |  ENTER confirma",
                602,
                499,
                13,
                COR_TEXTO_FRACO
            );
        } else {
            bool resultado =
                m->tautologia ||
                avalia(&formula, m->val);

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
        499,
        11,
        COR_TEXTO_FRACO
    );
}

/* ============================================================
 * DESAFIO LÓGICO
 * ============================================================ */

static void desenhaDesafioLogico(const Mundo *m) {
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
        (Color){ 12, 14, 28, 180 }
    );

    painel(
        215,
        92,
        530,
        300,
        (Color){ 24, 27, 43, 250 },
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
        catalogoFormula(portao->formulaId);

    char formulaTexto[64];

    formulaParaTexto(
        &formula,
        formulaTexto,
        sizeof formulaTexto
    );

    int larguraFormula =
        MeasureText(formulaTexto, 40) + 60;

    painel(
        LARGURA_TELA / 2 - larguraFormula / 2,
        178,
        larguraFormula,
        62,
        (Color){ 38, 42, 60, 255 },
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

    static const Color cores[NUM_PROPS] = {
        { 74, 151, 255, 255 },
        { 255, 207, 79, 255 },
        { 180, 100, 230, 255 }
    };

    for (int i = 0; i < NUM_PROPS; i++) {
        int x =
            310 + i * 115;

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
                : (Color){ 77, 81, 99, 255 },
            ligado ? 2.0f : 1.0f
        );

        char valor[24];

        snprintf(
            valor,
            sizeof valor,
            "%s = %c",
            nomeProp(i),
            ligado ? 'V' : 'F'
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

    if (m->tempoDecisaoPortao <= 2.0f) {
        corTempo =
            COR_VERMELHO;
    } else if (m->tempoDecisaoPortao <= 3.5f) {
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
        (Color){ 66, 70, 88, 255 }
    );

    DrawRectangle(
        315,
        347,
        (int)(330.0f * proporcao),
        12,
        corTempo
    );

    DrawRectangleLines(
        315,
        347,
        330,
        12,
        (Color){ 160, 166, 194, 255 }
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
 * API PRINCIPAL
 * ============================================================ */

void render_mundo(
    const Mundo *m,
    bool mostraTabela
) {
    desenhaCenario(m);

    for (int i = 0; i < m->nEnts; i++) {
        desenhaEntidade(
            &m->ents[i]
        );
    }

    for (int i = 0; i < m->nTiros; i++) {
        desenhaTiro(
            &m->tiros[i]
        );
    }

    desenhaJogador(m);
    desenhaHud(m, mostraTabela);
    desenhaDesafioLogico(m);
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

    desenhaCenario(
        &demonstracao
    );

    DrawRectangle(
        0,
        0,
        LARGURA_TELA,
        ALTURA_TELA,
        (Color){ 15, 17, 31, 65 }
    );

    Rectangle jogador = {
        88.0f,
        360.0f,
        JOGADOR_L,
        JOGADOR_A
    };

    DrawTriangle(
        (Vector2){
            jogador.x + 8.0f,
            jogador.y + 29.0f
        },
        (Vector2){
            jogador.x - 27.0f,
            jogador.y + 40.0f
        },
        (Vector2){
            jogador.x + 10.0f,
            jogador.y + 45.0f
        },
        COR_VERDE_ESCURO
    );

    DrawEllipse(
        (int)jogador.x + 17,
        (int)jogador.y + 32,
        23.0f,
        27.0f,
        COR_VERDE
    );

    DrawRectangleRounded(
        (Rectangle){
            jogador.x + 16.0f,
            jogador.y - 11.0f,
            34.0f,
            25.0f
        },
        0.24f,
        6,
        COR_VERDE
    );

    DrawCircle(
        (int)jogador.x + 40,
        (int)jogador.y - 3,
        5.0f,
        WHITE
    );

    DrawCircle(
        (int)jogador.x + 42,
        (int)jogador.y - 3,
        2.5f,
        COR_PAINEL
    );
}

void render_menu(void) {
    desenhaFundoMenu();

    textoCentro(
        "RUNNER LOGICO",
        LARGURA_TELA / 2,
        40,
        54,
        COR_TEXTO
    );

    textoCentro(
        "CORRA. RACIOCINE. SOBREVIVA.",
        LARGURA_TELA / 2,
        101,
        17,
        COR_CIANO
    );

    painel(
        250,
        139,
        460,
        250,
        (Color){ 24, 27, 42, 235 },
        (Color){ 89, 96, 127, 230 },
        2.0f
    );

    DrawText(
        "MOVIMENTO",
        274,
        158,
        17,
        COR_AMARELO
    );

    DrawText(
        "ESPACO / W / SETA CIMA",
        274,
        185,
        15,
        COR_TEXTO
    );

    DrawText(
        "Pular",
        530,
        185,
        15,
        COR_TEXTO_FRACO
    );

    DrawText(
        "SETA BAIXO / S",
        274,
        209,
        15,
        COR_TEXTO
    );

    DrawText(
        "Abaixar ou cair rapido",
        530,
        209,
        15,
        COR_TEXTO_FRACO
    );

    DrawText(
        "X / Z / CTRL",
        274,
        233,
        15,
        COR_TEXTO
    );

    DrawText(
        "Atirar com Q = V",
        530,
        233,
        15,
        COR_TEXTO_FRACO
    );

    DrawLine(
        274,
        263,
        686,
        263,
        (Color){ 88, 93, 116, 255 }
    );

    DrawText(
        "LOGICA",
        274,
        277,
        17,
        COR_AMARELO
    );

    DrawText(
        "[1] P = ESCUDO",
        274,
        305,
        15,
        COR_AZUL
    );

    DrawText(
        "[2] Q = ARMA",
        435,
        305,
        15,
        COR_AMARELO
    );

    DrawText(
        "[3] R = TURBO",
        565,
        305,
        15,
        COR_ROXO
    );

    DrawText(
        "Nos portoes, voce tem 5 segundos para validar a formula.",
        274,
        337,
        14,
        COR_TEXTO
    );

    DrawText(
        "ENTER confirma  |  T tabela  |  P pausa  |  ESC menu",
        274,
        363,
        13,
        COR_TEXTO_FRACO
    );

    painel(
        342,
        416,
        276,
        54,
        (Color){ 45, 126, 91, 245 },
        COR_VERDE,
        2.0f
    );

    textoCentro(
        "ENTER PARA JOGAR",
        LARGURA_TELA / 2,
        431,
        22,
        WHITE
    );
}

/* ============================================================
 * PAUSA E FIM
 * ============================================================ */

static void fundoEscuro(void) {
    DrawRectangle(
        0,
        0,
        LARGURA_TELA,
        ALTURA_TELA,
        (Color){ 12, 14, 26, 220 }
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

void render_fim(
    const Mundo *m,
    int recorde
) {
    fundoEscuro();

    bool venceu =
        m->resultado == RES_VITORIA;

    Color destaque =
        venceu
            ? COR_VERDE
            : COR_VERMELHO;

    painel(
        230,
        112,
        500,
        310,
        COR_PAINEL,
        destaque,
        3.0f
    );

    textoCentro(
        venceu
            ? "VITORIA!"
            : "FIM DE JOGO",
        LARGURA_TELA / 2,
        143,
        48,
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
        215,
        29,
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
        252,
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
        294,
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
        323,
        17,
        COR_TEXTO_FRACO
    );

    textoCentro(
        "ENTER jogar novamente",
        LARGURA_TELA / 2,
        365,
        18,
        COR_CIANO
    );

    textoCentro(
        "ESC voltar ao menu",
        LARGURA_TELA / 2,
        393,
        14,
        COR_TEXTO_FRACO
    );
}