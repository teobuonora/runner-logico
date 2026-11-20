/*
 * RUNNER LÓGICO — Projeto Integrador (PIF) + Lógica Proposicional
 *
 * main.c = GAME LOOP:
 *     entrada -> atualizar (mundo_atualiza) -> desenhar (render_*)
 *
 * Nenhuma regra do jogo mora aqui.
 * As regras ficam em mundo.c e logica.c.
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

/*
 * Tamanho inicial da janela.
 *
 * O jogo continua usando internamente LARGURA_TELA x ALTURA_TELA,
 * definidos em config.h. A imagem é ampliada proporcionalmente.
 */
#define JANELA_INICIAL_LARGURA 1280
#define JANELA_INICIAL_ALTURA   720

/* ============================================================
 * ENTRADA
 * ============================================================ */

static Entrada lerEntrada(void) {
    Entrada in;

    memset(
        &in,
        0,
        sizeof in
    );

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
     * Durante o desafio lógico, Enter confirma a resposta atual.
     */
    in.confirma =
        IsKeyPressed(KEY_ENTER);

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
    return
        (uint32_t)time(NULL) ^
        (uint32_t)(GetTime() * 1000.0);
}

/* ============================================================
 * ESCALA DA JANELA
 * ============================================================ */

/*
 * Calcula a maior escala possível sem distorcer a proporção
 * original de 960 x 540.
 */
static float calculaEscala(void) {
    float escalaX =
        (float)GetScreenWidth() /
        (float)LARGURA_TELA;

    float escalaY =
        (float)GetScreenHeight() /
        (float)ALTURA_TELA;

    return
        escalaX < escalaY
            ? escalaX
            : escalaY;
}

/*
 * Cria uma câmera que amplia e centraliza a área lógica do jogo.
 *
 * Exemplo:
 *
 * Janela:        1280 x 720
 * Jogo interno:   960 x 540
 * Escala:              1,333
 *
 * Todo o desenho continua usando as coordenadas originais.
 */
static Camera2D criaCameraEscalada(void) {
    float escala =
        calculaEscala();

    float larguraDesenho =
        LARGURA_TELA *
        escala;

    float alturaDesenho =
        ALTURA_TELA *
        escala;

    float margemX =
        (
            GetScreenWidth() -
            larguraDesenho
        ) /
        2.0f;

    float margemY =
        (
            GetScreenHeight() -
            alturaDesenho
        ) /
        2.0f;

    Camera2D camera;

    memset(
        &camera,
        0,
        sizeof camera
    );

    camera.target =
        (Vector2){ 0.0f, 0.0f };

    camera.offset =
        (Vector2){
            margemX,
            margemY
        };

    camera.rotation =
        0.0f;

    camera.zoom =
        escala;

    return camera;
}

/* ============================================================
 * PROGRAMA PRINCIPAL
 * ============================================================ */

int main(void) {
    /*
     * Esta configuração precisa ser definida antes de InitWindow.
     *
     * FLAG_WINDOW_RESIZABLE permite:
     * - redimensionar pelas bordas;
     * - usar o botão de maximizar do Windows;
     * - adaptar o jogo ao novo tamanho.
     */
    SetConfigFlags(
        FLAG_WINDOW_RESIZABLE
    );

    InitWindow(
        JANELA_INICIAL_LARGURA,
        JANELA_INICIAL_ALTURA,
        "Runner Logico"
    );

    /*
     * Impede que a janela seja redimensionada para um tamanho
     * menor que a resolução lógica original.
     */
    SetWindowMinSize(
        LARGURA_TELA,
        ALTURA_TELA
    );

    /*
     * ESC não fecha diretamente a janela.
     * Durante o jogo, ESC volta ao menu.
     */
    SetExitKey(KEY_NULL);

    SetTargetFPS(60);

    Tela tela =
        TELA_MENU;

    Mundo mundo =
        mundo_novo(
            novaSemente()
        );

    int recorde =
        0;

    bool mostraTabela =
        true;

    bool sair =
        false;

    while (
        !sair &&
        !WindowShouldClose()
    ) {
        float dt =
            GetFrameTime();

        /*
         * Evita saltos grandes na simulação caso a janela trave,
         * seja arrastada ou perca o foco.
         */
        if (dt > 0.05f) {
            dt = 0.05f;
        }

        /*
         * F11 alterna entre janela e tela cheia.
         */
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        /* ====================================================
         * 1) ENTRADA
         * 2) ATUALIZAÇÃO
         * ==================================================== */

        switch (tela) {
            case TELA_MENU:
                if (IsKeyPressed(KEY_ENTER)) {
                    mundo =
                        mundo_novo(
                            novaSemente()
                        );

                    tela =
                        TELA_JOGO;
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    sair =
                        true;
                }
                break;

            case TELA_JOGO:
                if (IsKeyPressed(KEY_T)) {
                    mostraTabela =
                        !mostraTabela;
                }

                if (IsKeyPressed(KEY_P)) {
                    tela =
                        TELA_PAUSA;

                    break;
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    tela =
                        TELA_MENU;

                    break;
                }

                mundo =
                    mundo_atualiza(
                        mundo,
                        lerEntrada(),
                        dt
                    );

                if (
                    mundo.resultado !=
                    RES_JOGANDO
                ) {
                    int pontos =
                        mundo_pontos(
                            &mundo
                        );

                    if (pontos > recorde) {
                        recorde =
                            pontos;
                    }

                    tela =
                        TELA_FIM;
                }
                break;

            case TELA_PAUSA:
                if (IsKeyPressed(KEY_P)) {
                    tela =
                        TELA_JOGO;
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    tela =
                        TELA_MENU;
                }
                break;

            case TELA_FIM:
                if (IsKeyPressed(KEY_ENTER)) {
                    mundo =
                        mundo_novo(
                            novaSemente()
                        );

                    tela =
                        TELA_JOGO;
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    tela =
                        TELA_MENU;
                }
                break;
        }

        /* ====================================================
         * 3) DESENHO
         * ==================================================== */

        Camera2D camera =
            criaCameraEscalada();

        BeginDrawing();

        /*
         * Caso a proporção da janela seja diferente de 16:9,
         * essas barras escuras aparecem nas áreas restantes.
         */
        ClearBackground(
            (Color){ 10, 11, 20, 255 }
        );

        /*
         * Tudo que for desenhado entre BeginMode2D e EndMode2D
         * será ampliado e centralizado proporcionalmente.
         */
        BeginMode2D(camera);

        switch (tela) {
            case TELA_MENU:
                render_menu();
                break;

            case TELA_JOGO:
                render_mundo(
                    &mundo,
                    mostraTabela
                );
                break;

            case TELA_PAUSA:
                render_mundo(
                    &mundo,
                    mostraTabela
                );

                render_pausa();
                break;

            case TELA_FIM:
                render_mundo(
                    &mundo,
                    mostraTabela
                );

                render_fim(
                    &mundo,
                    recorde
                );
                break;
        }

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}