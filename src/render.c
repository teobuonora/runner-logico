#include "render.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* ---------- cores ---------- */
static const Color COR_FUNDO   = { 247, 247, 242, 255 };
static const Color COR_PAINEL  = { 38, 38, 48, 255 };
static const Color COR_TEXTO   = { 45, 45, 55, 255 };
static const Color COR_VERDE   = { 46, 160, 90, 255 };
static const Color COR_VERMELHO= { 220, 70, 70, 255 };
static const Color COR_AZUL    = { 70, 120, 255, 255 };
static const Color COR_AMARELO = { 250, 200, 50, 255 };
static const Color COR_ROXO    = { 150, 90, 220, 255 };

static void textoCentro(const char *t, int cx, int y, int tam, Color c) {
    DrawText(t, cx - MeasureText(t, tam) / 2, y, tam, c);
}

/* ---------- cenário ---------- */
static void desenhaCenario(const Mundo *m) {
    ClearBackground(COR_FUNDO);

    for (int i = 0; i < 5; i++) {                       /* nuvens (parallax) */
        float x = fmodf(i * 270.0f - m->distancia * 0.12f, LARGURA_TELA + 140.0f);
        if (x < 0) x += LARGURA_TELA + 140.0f;
        x -= 70.0f;
        float y = 80.0f + (i % 3) * 38.0f;
        DrawRectangle((int)x, (int)y, 70, 14, (Color){ 225, 228, 235, 255 });
        DrawRectangle((int)x + 14, (int)y - 10, 40, 12, (Color){ 225, 228, 235, 255 });
    }

    DrawRectangle(0, (int)CHAO_Y, LARGURA_TELA, ALTURA_TELA - (int)CHAO_Y, COR_PAINEL);
    DrawRectangle(0, (int)CHAO_Y, LARGURA_TELA, 3, (Color){ 90, 90, 100, 255 });
    for (int i = 0; i < 40; i++) {                      /* pontinhos do chão */
        float x = fmodf(i * 47.0f - m->distancia, (float)LARGURA_TELA);
        if (x < 0) x += LARGURA_TELA;
        DrawRectangle((int)x, (int)CHAO_Y + 8 + (i % 3) * 3, 6, 2, (Color){ 90, 90, 100, 255 });
    }
}

/* ---------- jogador ---------- */
static void desenhaJogador(const Mundo *m) {
    if (m->invulneravel > 0.0f && ((int)(m->invulneravel * 10.0f) % 2) == 0) return;   /* pisca */

    Rectangle r = mundo_caixa_jogador(m);
    bool escudo = m->val.v[PROP_ESCUDO];
    bool arma   = m->val.v[PROP_ARMA];
    bool turbo  = m->val.v[PROP_TURBO];

    if (turbo)                                          /* chama do turbo */
        DrawRectangle((int)r.x - 18, (int)r.y + (int)r.height / 2 - 4, 18, 8, COR_AMARELO);

    DrawRectangleRec(r, COR_VERDE);
    if (!m->abaixado) {                                 /* cabeça + olho + pernas */
        DrawRectangle((int)r.x + 18, (int)r.y - 12, 26, 20, COR_VERDE);
        DrawRectangle((int)r.x + 34, (int)r.y - 8, 6, 6, WHITE);
        DrawRectangle((int)r.x + 37, (int)r.y - 6, 3, 3, BLACK);
        bool passo = m->noChao && ((int)(m->tempo * 12.0f) % 2 == 0);
        DrawRectangle((int)r.x + 6,  (int)(r.y + r.height) - (passo ? 4 : 0), 10, 4, (Color){ 30, 110, 60, 255 });
        DrawRectangle((int)r.x + 24, (int)(r.y + r.height) - (passo ? 0 : 4), 10, 4, (Color){ 30, 110, 60, 255 });
    } else {
        DrawRectangle((int)r.x + 24, (int)r.y + 2, 20, 14, COR_VERDE);
        DrawRectangle((int)r.x + 36, (int)r.y + 5, 4, 4, BLACK);
    }
    if (arma)                                           /* arma na frente */
        DrawRectangle((int)(r.x + r.width), (int)(r.y + r.height * 0.4f), 16, 7, (Color){ 60, 60, 70, 255 });
    if (escudo) {
        Color c = COR_AZUL; c.a = 90;
        DrawCircle((int)(r.x + r.width / 2), (int)(r.y + r.height / 2), 46.0f, c);
        DrawCircleLines((int)(r.x + r.width / 2), (int)(r.y + r.height / 2), 46.0f, COR_AZUL);
    }
}

/* ---------- entidades ---------- */
static void desenhaPortao(const Entidade *e) {
    Color c = COR_AZUL; c.a = 150;
    if (e->avaliado) { c = e->resultado ? COR_VERDE : COR_VERMELHO; c.a = 130; }

    DrawRectangleRec(e->caixa, c);
    DrawRectangleLinesEx(e->caixa, 3, e->avaliado ? (e->resultado ? COR_VERDE : COR_VERMELHO) : COR_AZUL);

    char txt[64];
    Formula f = catalogoFormula(e->formulaId);
    formulaParaTexto(&f, txt, sizeof txt);
    int tam = 22, w = MeasureText(txt, tam) + 20;
    int x = (int)(e->caixa.x + e->caixa.width / 2) - w / 2;
    int y = (int)e->caixa.y - 34;
    DrawRectangle(x, y, w, 30, WHITE);
    DrawRectangleLines(x, y, w, 30, COR_AZUL);
    DrawText(txt, x + 10, y + 4, tam, COR_TEXTO);
}

static void desenhaEntidade(const Entidade *e) {
    Rectangle r = e->caixa;
    switch (e->tipo) {
        case ENT_CACTO:
            DrawRectangleRec(r, (Color){ 40, 120, 70, 255 });
            DrawRectangle((int)r.x - 6, (int)r.y + 14, 8, 6, (Color){ 40, 120, 70, 255 });
            DrawRectangle((int)(r.x + r.width) - 2, (int)r.y + 22, 8, 6, (Color){ 40, 120, 70, 255 });
            break;
        case ENT_ANDARILHO: {
            Color c = (e->vida >= 2) ? COR_ROXO : (Color){ 190, 120, 220, 255 };
            DrawRectangleRounded(r, 0.3f, 6, c);
            DrawRectangle((int)r.x + 8,  (int)r.y + 10, 8, 8, WHITE);
            DrawRectangle((int)r.x + 24, (int)r.y + 10, 8, 8, WHITE);
            DrawRectangle((int)r.x + 11, (int)r.y + 13, 4, 4, BLACK);
            DrawRectangle((int)r.x + 27, (int)r.y + 13, 4, 4, BLACK);
            break;
        }
        case ENT_VOADOR: {
            bool bate = ((int)(e->anim * 10.0f) % 2) == 0;
            DrawRectangleRec(r, (Color){ 240, 140, 40, 255 });
            DrawRectangle((int)r.x + 10, (int)r.y + (bate ? -12 : 20), 24, 10, (Color){ 210, 110, 20, 255 });
            DrawRectangle((int)r.x + 4,  (int)r.y + 6, 6, 6, WHITE);
            break;
        }
        case ENT_PORTAO:
            desenhaPortao(e);
            break;
        case ENT_PU_ENERGIA:
        case ENT_PU_VIDA:
        case ENT_PU_TAUTOLOGIA: {
            Color c = COR_AMARELO; const char *l = "E";
            if (e->tipo == ENT_PU_VIDA)       { c = COR_VERMELHO; l = "+"; }
            if (e->tipo == ENT_PU_TAUTOLOGIA) { c = COR_ROXO;     l = "T"; }
            DrawCircle((int)(r.x + r.width / 2), (int)(r.y + r.height / 2), 16.0f, c);
            DrawCircleLines((int)(r.x + r.width / 2), (int)(r.y + r.height / 2), 16.0f, COR_TEXTO);
            textoCentro(l, (int)(r.x + r.width / 2), (int)r.y + 5, 20, WHITE);
            break;
        }
    }
}

/* ---------- HUD ---------- */
static void desenhaTabela(const Mundo *m, const Entidade *portao) {
    Formula f = catalogoFormula(portao->formulaId);
    char txt[64];
    formulaParaTexto(&f, txt, sizeof txt);

    int x = LARGURA_TELA - 184, y = 52, w = 172, h = 150;
    DrawRectangle(x, y, w, h, (Color){ 255, 255, 255, 220 });
    DrawRectangleLines(x, y, w, h, COR_AZUL);
    DrawText("TABELA-VERDADE", x + 10, y + 5, 12, COR_TEXTO);
    DrawText("P  Q  R |  f", x + 14, y + 22, 14, COR_TEXTO);
    DrawLine(x + 8, y + 38, x + w - 8, y + 38, COR_TEXTO);

    unsigned atual = mascaraDeValoracao(m->val);
    for (unsigned i = 0; i < 8; i++) {
        Valoracao v = valoracaoDeMascara(i);
        bool res = avalia(&f, v);
        int ry = y + 42 + (int)i * 13;
        if (i == atual) DrawRectangle(x + 6, ry - 1, w - 12, 13, (Color){ 250, 220, 90, 255 });
        char linha[32];
        snprintf(linha, sizeof linha, "%c  %c  %c |  %c",
                 v.v[0] ? 'V' : 'F', v.v[1] ? 'V' : 'F', v.v[2] ? 'V' : 'F', res ? 'V' : 'F');
        DrawText(linha, x + 14, ry, 12, res ? COR_VERDE : COR_VERMELHO);
    }
}

static void desenhaHud(const Mundo *m, bool mostraTabela) {
    /* vidas */
    for (int i = 0; i < VIDAS_MAX; i++) {
        Vector2 c = { 28.0f + i * 26.0f, 26.0f };
        if (i < m->vidas) DrawCircleV(c, 9.0f, COR_VERMELHO);
        else              DrawCircleLinesV(c, 9.0f, (Color){ 170, 170, 175, 255 });
    }
    /* energia */
    DrawText("ENERGIA", 20, 44, 12, COR_TEXTO);
    DrawRectangle(20, 58, 180, 12, (Color){ 215, 215, 220, 255 });
    Color ce = m->energia < 25.0f ? COR_VERMELHO : COR_AMARELO;
    DrawRectangle(20, 58, (int)(180.0f * m->energia / ENERGIA_MAX), 12, ce);
    DrawRectangleLines(20, 58, 180, 12, COR_TEXTO);
    if (m->sobrecarga > 0.0f) DrawText("SOBRECARGA!", 210, 56, 16, COR_VERMELHO);

    /* pontos e portões */
    char t[64];
    snprintf(t, sizeof t, "PONTOS %d", mundo_pontos(m));
    textoCentro(t, LARGURA_TELA / 2, 14, 28, COR_TEXTO);
    snprintf(t, sizeof t, "PORTOES %d/%d", m->portoesOk, META_PORTOES);
    DrawText(t, LARGURA_TELA - MeasureText(t, 22) - 14, 14, 22, COR_AZUL);

    if (m->tautologia) textoCentro("TAUTOLOGIA ATIVA: o proximo portao abre", LARGURA_TELA / 2, 48, 16, COR_ROXO);

    /* mensagem */
    if (m->msgTempo > 0.0f && m->msg[0])
        textoCentro(m->msg, LARGURA_TELA / 2, 100, 24, COR_TEXTO);

    /* painel de proposições */
    static const char *efeito[NUM_PROPS] = { "ESCUDO: destroi inimigos", "ARMA: permite atirar (X)", "TURBO: +vel, pontos x2" };
    for (int i = 0; i < NUM_PROPS; i++) {
        int x = 20 + i * 198, y = 452, w = 190, h = 52;
        bool on = m->val.v[i];
        DrawRectangle(x, y, w, h, on ? (Color){ 40, 120, 70, 255 } : (Color){ 60, 60, 72, 255 });
        DrawRectangleLines(x, y, w, h, on ? COR_VERDE : (Color){ 110, 110, 120, 255 });
        char a[32];
        snprintf(a, sizeof a, "[%d] %s = %s", i + 1, nomeProp(i), on ? "V" : "F");
        DrawText(a, x + 10, y + 7, 24, WHITE);
        DrawText(efeito[i], x + 10, y + 36, 12, (Color){ 220, 220, 225, 255 });
    }

    /* próximo portão: fórmula + valor agora */
    const Entidade *p = mundo_proximo_portao(m);
    if (p) {
        Formula f = catalogoFormula(p->formulaId);
        char txt[64], linha[128];
        formulaParaTexto(&f, txt, sizeof txt);
        bool agora = m->tautologia || avalia(&f, m->val);
        snprintf(linha, sizeof linha, "Portao: %s => %s", txt, agora ? "VERDADEIRO" : "FALSO");
        DrawText(linha, 626, 454, 16, agora ? COR_VERDE : COR_VERMELHO);
        if (mostraTabela) desenhaTabela(m, p);
    } else {
        DrawText("Prox. portao: em breve...", 626, 454, 16, (Color){ 160, 160, 170, 255 });
    }
    DrawText("~ NAO    & E    | OU", 626, 478, 12, (Color){ 200, 200, 210, 255 });
    DrawText("-> SE..ENTAO    <-> SE E SOMENTE SE", 626, 494, 12, (Color){ 200, 200, 210, 255 });
    DrawText("T: tabela-verdade    P: pausa", 626, 512, 12, (Color){ 160, 160, 170, 255 });
}

/* ---------- API ---------- */
void render_mundo(const Mundo *m, bool mostraTabela) {
    desenhaCenario(m);
    for (int i = 0; i < m->nEnts; i++) desenhaEntidade(&m->ents[i]);
    for (int i = 0; i < m->nTiros; i++) DrawRectangleRec(m->tiros[i].caixa, COR_VERMELHO);
    desenhaJogador(m);
    desenhaHud(m, mostraTabela);
}

static void fundoEscuro(void) {
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){ 20, 20, 28, 225 });
}

void render_menu(void) {
    ClearBackground(COR_FUNDO);
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, COR_PAINEL);
    textoCentro("RUNNER LOGICO", LARGURA_TELA / 2, 50, 56, WHITE);
    textoCentro("corra, atire e abra os portoes com Logica Proposicional", LARGURA_TELA / 2, 118, 20, (Color){ 190, 190, 200, 255 });

    int x = 120, y = 180;
    DrawText("CONTROLES", x, y, 22, COR_AMARELO);
    DrawText("ESPACO / SETA CIMA ... pular", x, y + 32, 18, WHITE);
    DrawText("SETA BAIXO ........... abaixar (no ar: cair rapido)", x, y + 56, 18, WHITE);
    DrawText("X ou Z ............... atirar (so se Q = V)", x, y + 80, 18, WHITE);
    DrawText("1  2  3 .............. liga/desliga P, Q, R", x, y + 104, 18, WHITE);
    DrawText("T tabela-verdade   P pausa   ESC menu", x, y + 128, 18, WHITE);

    DrawText("COMO FUNCIONA", 520, y, 22, COR_AMARELO);
    DrawText("P = escudo   Q = arma   R = turbo", 520, y + 32, 18, WHITE);
    DrawText("Cada proposicao V gasta energia.", 520, y + 56, 18, WHITE);
    DrawText("PORTOES mostram uma formula (ex.: P & ~Q).", 520, y + 80, 18, WHITE);
    DrawText("Ao cruzar, ela precisa dar VERDADEIRO!", 520, y + 104, 18, WHITE);
    DrawText("Formula falsa = -1 vida. Abra 8 portoes.", 520, y + 128, 18, WHITE);

    textoCentro("ENTER para jogar", LARGURA_TELA / 2, 440, 30, COR_VERDE);
}

void render_pausa(void) {
    fundoEscuro();
    textoCentro("PAUSADO", LARGURA_TELA / 2, 220, 48, WHITE);
    textoCentro("P para continuar   |   ESC para o menu", LARGURA_TELA / 2, 285, 20, (Color){ 200, 200, 210, 255 });
}

void render_fim(const Mundo *m, int recorde) {
    fundoEscuro();
    bool venceu = (m->resultado == RES_VITORIA);
    textoCentro(venceu ? "VITORIA!" : "FIM DE JOGO", LARGURA_TELA / 2, 150, 56, venceu ? COR_VERDE : COR_VERMELHO);
    char t[96];
    snprintf(t, sizeof t, "Pontos: %d    Recorde: %d", mundo_pontos(m), recorde);
    textoCentro(t, LARGURA_TELA / 2, 225, 26, WHITE);
    snprintf(t, sizeof t, "Portoes abertos: %d   Falsos: %d   Inimigos abatidos: %d", m->portoesOk, m->portoesErro, m->abatidos);
    textoCentro(t, LARGURA_TELA / 2, 265, 18, (Color){ 200, 200, 210, 255 });
    textoCentro("ENTER para jogar de novo   |   ESC para o menu", LARGURA_TELA / 2, 330, 20, WHITE);
}
