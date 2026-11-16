/*
 * RUNNER LÓGICO — Projeto Integrador (PIF) + Lógica Proposicional
 *
 * main.c = o GAME LOOP (parte imperativa):
 *     entrada -> atualizar (mundo_atualiza) -> desenhar (render_*)
 * Nenhuma regra do jogo mora aqui: elas ficam em mundo.c / logica.c.
 */

#include <time.h>
#include <string.h>
#include "raylib.h"
#include "config.h"
#include "mundo.h"
#include "render.h"

typedef enum {
    TELA_MENU,
    TELA_JOGO,
    TELA_PAUSA,
    TELA_FIM
} Tela;

static Entrada lerEntrada(void) {
    Entrada in;
    memset(&in, 0, sizeof in);

    in.pula =
        IsKeyPressed(KEY_SPACE) ||
        IsKeyPressed(KEY_UP) ||
        IsKeyPressed(KEY_W);

    in.abaixa =
        IsKeyDown(KEY_DOWN) ||
        IsKeyDown(KEY_S);

    in.atira =
        IsKeyDown(KEY_X) ||
        IsKeyDown(KEY_Z) ||
        IsKeyDown(KEY_LEFT_CONTROL);

    /*
     * Durante o desafio lógico, Enter confirma a valoração atual
     * antes que os cinco segundos terminem.
     *
     * A decisão sobre aceitar ou ignorar essa entrada continua em
     * mundo.c, mantendo a regra fora do game loop.
     */
    in.confirma = IsKeyPressed(KEY_ENTER);

    in.alterna[0] =
        IsKeyPressed(KEY_ONE) ||
        IsKeyPressed(KEY_KP_1);

    in.alterna[1] =
        IsKeyPressed(KEY_TWO) ||
        IsKeyPressed(KEY_KP_2);

    in.alterna[2] =
        IsKeyPressed(KEY_THREE) ||
        IsKeyPressed(KEY_KP_3);

    return in;
}

static uint32_t novaSemente(void) {
    return (uint32_t)time(NULL) ^ (uint32_t)(GetTime() * 1000.0);
}

int main(void) {
    InitWindow(
        LARGURA_TELA,
        ALTURA_TELA,
        "Runner Logico"
    );

    /*
     * ESC não fecha diretamente a janela.
     * Durante o jogo, ESC volta ao menu.
     */
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    Tela tela = TELA_MENU;
    Mundo mundo = mundo_novo(novaSemente());

    int recorde = 0;
    bool mostraTabela = true;
    bool sair = false;

    while (!sair && !WindowShouldClose()) {
        float dt = GetFrameTime();

        /*
         * Evita saltos muito grandes na simulação caso a janela
         * seja arrastada, perca foco ou trave por alguns instantes.
         */
        if (dt > 0.05f) {
            dt = 0.05f;
        }

        /* ---------- 1) ENTRADA + 2) ATUALIZAR ---------- */

        switch (tela) {
            case TELA_MENU:
                if (IsKeyPressed(KEY_ENTER)) {
                    mundo = mundo_novo(novaSemente());
                    tela = TELA_JOGO;
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    sair = true;
                }
                break;

            case TELA_JOGO:
                if (IsKeyPressed(KEY_T)) {
                    mostraTabela = !mostraTabela;
                }

                if (IsKeyPressed(KEY_P)) {
                    tela = TELA_PAUSA;
                    break;
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    tela = TELA_MENU;
                    break;
                }

                /*
                 * O Enter usado em lerEntrada() só será considerado
                 * por mundo_atualiza() quando houver um desafio lógico.
                 */
                mundo = mundo_atualiza(
                    mundo,
                    lerEntrada(),
                    dt
                );

                if (mundo.resultado != RES_JOGANDO) {
                    int pontos = mundo_pontos(&mundo);

                    if (pontos > recorde) {
                        recorde = pontos;
                    }

                    tela = TELA_FIM;
                }
                break;

            case TELA_PAUSA:
                if (IsKeyPressed(KEY_P)) {
                    tela = TELA_JOGO;
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    tela = TELA_MENU;
                }
                break;

            case TELA_FIM:
                if (IsKeyPressed(KEY_ENTER)) {
                    mundo = mundo_novo(novaSemente());
                    tela = TELA_JOGO;
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    tela = TELA_MENU;
                }
                break;
        }

        /* ---------- 3) DESENHAR ---------- */
        /*
         * As funções de renderização recebem o Mundo como const.
         * Portanto, desenhar nunca altera o estado do jogo.
         */

        BeginDrawing();

        switch (tela) {
            case TELA_MENU:
                render_menu();
                break;

            case TELA_JOGO:
                render_mundo(&mundo, mostraTabela);
                break;

            case TELA_PAUSA:
                render_mundo(&mundo, mostraTabela);
                render_pausa();
                break;

            case TELA_FIM:
                render_mundo(&mundo, mostraTabela);
                render_fim(&mundo, recorde);
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}