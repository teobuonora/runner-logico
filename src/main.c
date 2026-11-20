/*
 * RUNNER LÓGICO
 *
 * main.c = GAME LOOP:
 *     entrada -> atualizar -> desenhar
 *
 * As regras da partida ficam em mundo.c.
 * O ranking persistente fica em ranking.c.
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "raylib.h"
#include "config.h"
#include "mundo.h"
#include "ranking.h"
#include "render.h"

/* ============================================================
 * TELAS
 * ============================================================ */

typedef enum {
    TELA_MENU,
    TELA_JOGO,
    TELA_PAUSA,
    TELA_FIM,
    TELA_RANKING,
    TELA_ENTRADA_NOME
} Tela;

/*
 * Indica para onde o jogador deve retornar ao fechar o ranking.
 */
typedef enum {
    RETORNO_RANKING_MENU,
    RETORNO_RANKING_FIM
} RetornoRanking;

/* ============================================================
 * JANELA
 * ============================================================ */

#define JANELA_INICIAL_LARGURA 1280
#define JANELA_INICIAL_ALTURA   720

/* ============================================================
 * ENTRADA DA PARTIDA
 * ============================================================ */

static Entrada lerEntrada(void) {
    Entrada entrada;

    memset(
        &entrada,
        0,
        sizeof entrada
    );

    entrada.pula =
        IsKeyPressed(KEY_SPACE) ||
        IsKeyPressed(KEY_UP) ||
        IsKeyPressed(KEY_W);

    entrada.abaixa =
        IsKeyDown(KEY_DOWN) ||
        IsKeyDown(KEY_S);

    entrada.atira =
        IsKeyDown(KEY_X) ||
        IsKeyDown(KEY_Z) ||
        IsKeyDown(KEY_LEFT_CONTROL);

    entrada.confirma =
        IsKeyPressed(KEY_ENTER);

    entrada.alterna[PROP_ESCUDO] =
        IsKeyPressed(KEY_ONE) ||
        IsKeyPressed(KEY_KP_1);

    entrada.alterna[PROP_ARMA] =
        IsKeyPressed(KEY_TWO) ||
        IsKeyPressed(KEY_KP_2);

    entrada.alterna[PROP_TURBO] =
        IsKeyPressed(KEY_THREE) ||
        IsKeyPressed(KEY_KP_3);

    return entrada;
}

static uint32_t novaSemente(void) {
    return
        (uint32_t)time(NULL) ^
        (uint32_t)(GetTime() * 1000.0);
}

/* ============================================================
 * ESCALA DA JANELA
 * ============================================================ */

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
        (Vector2){
            0.0f,
            0.0f
        };

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

/*
 * Converte o mouse da janela física para a resolução lógica
 * de 960 x 540.
 *
 * Isso mantém os botões corretos quando a janela está:
 * - em 1280 x 720;
 * - maximizada;
 * - redimensionada;
 * - em tela cheia com F11.
 */
static Vector2 mouseLogico(void) {
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

    Vector2 mouse =
        GetMousePosition();

    Vector2 resultado = {
        (
            mouse.x -
            margemX
        ) /
        escala,

        (
            mouse.y -
            margemY
        ) /
        escala
    };

    return resultado;
}

static bool mouseSobre(Rectangle area) {
    return
        CheckCollisionPointRec(
            mouseLogico(),
            area
        );
}

static bool botaoClicado(Rectangle area) {
    return
        mouseSobre(area) &&
        IsMouseButtonPressed(
            MOUSE_BUTTON_LEFT
        );
}

/* ============================================================
 * RANKING
 * ============================================================ */

static int recordeDoRanking(
    const Ranking *ranking
) {
    if (
        ranking == NULL ||
        ranking->quantidade <= 0
    ) {
        return 0;
    }

    return
        ranking->registros[0].pontos;
}

/*
 * Calcula a posição prevista da nova pontuação.
 *
 * O resultado fica entre 1 e 10.
 */
static int posicaoEstimadaRanking(
    const Ranking *ranking,
    int pontos,
    int portoes,
    int abatidos
) {
    if (ranking == NULL) {
        return 1;
    }

    int posicao =
        1;

    for (
        int i = 0;
        i < ranking->quantidade;
        i++
    ) {
        const RegistroRanking *registro =
            &ranking->registros[i];

        bool novoFicaDepois =
            false;

        if (pontos < registro->pontos) {
            novoFicaDepois =
                true;
        } else if (
            pontos ==
            registro->pontos
        ) {
            if (
                portoes <
                registro->portoes
            ) {
                novoFicaDepois =
                    true;
            } else if (
                portoes ==
                    registro->portoes &&
                abatidos <=
                    registro->abatidos
            ) {
                /*
                 * Em empate completo, o registro antigo
                 * permanece na frente.
                 */
                novoFicaDepois =
                    true;
            }
        }

        if (novoFicaDepois) {
            posicao++;
        } else {
            break;
        }
    }

    if (posicao < 1) {
        posicao = 1;
    }

    if (
        posicao >
        RANKING_MAX_REGISTROS
    ) {
        posicao =
            RANKING_MAX_REGISTROS;
    }

    return posicao;
}

/* ============================================================
 * ENTRADA DO NOME
 * ============================================================ */

static bool caractereNomePermitido(int caractere) {
    if (
        caractere >= 'A' &&
        caractere <= 'Z'
    ) {
        return true;
    }

    if (
        caractere >= 'a' &&
        caractere <= 'z'
    ) {
        return true;
    }

    if (
        caractere >= '0' &&
        caractere <= '9'
    ) {
        return true;
    }

    return
        caractere == ' ' ||
        caractere == '-' ||
        caractere == '_';
}

static void atualizaEntradaNome(
    char nome[RANKING_NOME_MAX + 1]
) {
    int caractere =
        GetCharPressed();

    while (caractere > 0) {
        int tamanho =
            (int)strlen(nome);

        if (
            tamanho <
                RANKING_NOME_MAX &&
            caractereNomePermitido(
                caractere
            )
        ) {
            /*
             * Evita que o nome comece com espaço ou acumule
             * vários espaços seguidos.
             */
            bool espacoInvalido =
                caractere == ' ' &&
                (
                    tamanho == 0 ||
                    nome[tamanho - 1] == ' '
                );

            if (!espacoInvalido) {
                nome[tamanho] =
                    (char)caractere;

                nome[tamanho + 1] =
                    '\0';
            }
        }

        caractere =
            GetCharPressed();
    }

    if (
        IsKeyPressed(KEY_BACKSPACE)
    ) {
        int tamanho =
            (int)strlen(nome);

        if (tamanho > 0) {
            nome[tamanho - 1] =
                '\0';
        }
    }
}

static bool nomePossuiConteudo(
    const char *nome
) {
    if (nome == NULL) {
        return false;
    }

    for (
        int i = 0;
        nome[i] != '\0';
        i++
    ) {
        if (
            !isspace(
                (unsigned char)nome[i]
            )
        ) {
            return true;
        }
    }

    return false;
}

/* ============================================================
 * INÍCIO DE PARTIDA
 * ============================================================ */

static void iniciaNovaPartida(
    Mundo *mundo
) {
    *mundo =
        mundo_novo(
            novaSemente()
        );
}

/* ============================================================
 * PROGRAMA PRINCIPAL
 * ============================================================ */

int main(void) {
    SetConfigFlags(
        FLAG_WINDOW_RESIZABLE
    );

    InitWindow(
        JANELA_INICIAL_LARGURA,
        JANELA_INICIAL_ALTURA,
        "Runner Logico"
    );

    SetWindowMinSize(
        LARGURA_TELA,
        ALTURA_TELA
    );

    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    Ranking ranking =
        ranking_carrega();

    Tela tela =
        TELA_MENU;

    RetornoRanking retornoRanking =
        RETORNO_RANKING_MENU;

    Mundo mundo =
        mundo_novo(
            novaSemente()
        );

    int recorde =
        recordeDoRanking(
            &ranking
        );

    bool mostraTabela =
        true;

    bool sair =
        false;

    /*
     * Impede que o mesmo resultado seja processado mais de uma
     * vez enquanto a tela final permanece aberta.
     */
    bool resultadoProcessado =
        false;

    /*
     * Dados temporários para registrar uma nova colocação.
     */
    char nomeJogador[
        RANKING_NOME_MAX + 1
    ] = "";

    int pontosPendentes =
        0;

    int portoesPendentes =
        0;

    int abatidosPendentes =
        0;

    int posicaoEstimada =
        1;

    char avisoRanking[96] =
        "";

    while (
        !sair &&
        !WindowShouldClose()
    ) {
        float dt =
            GetFrameTime();

        if (dt > 0.05f) {
            dt = 0.05f;
        }

        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        bool hoverRankingMenu =
            mouseSobre(
                render_area_botao_ranking_menu()
            );

        bool hoverRankingFim =
            mouseSobre(
                render_area_botao_ranking_fim()
            );

        bool hoverVoltarRanking =
            mouseSobre(
                render_area_botao_voltar_ranking()
            );

        bool hoverSalvarNome =
            mouseSobre(
                render_area_botao_salvar_nome()
            );

        /* ====================================================
         * ENTRADA E ATUALIZAÇÃO
         * ==================================================== */

        switch (tela) {
            case TELA_MENU:
                if (
                    IsKeyPressed(KEY_R) ||
                    botaoClicado(
                        render_area_botao_ranking_menu()
                    )
                ) {
                    retornoRanking =
                        RETORNO_RANKING_MENU;

                    tela =
                        TELA_RANKING;

                    break;
                }

                if (IsKeyPressed(KEY_ENTER)) {
                    iniciaNovaPartida(
                        &mundo
                    );

                    resultadoProcessado =
                        false;

                    avisoRanking[0] =
                        '\0';

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

                    /*
                     * O resultado é analisado uma única vez.
                     */
                    if (!resultadoProcessado) {
                        resultadoProcessado =
                            true;

                        pontosPendentes =
                            pontos;

                        portoesPendentes =
                            mundo.portoesOk;

                        abatidosPendentes =
                            mundo.abatidos;

                        /*
                         * Só pedimos o nome quando a pontuação
                         * realmente pode entrar no Top 10.
                         */
                        if (
                            ranking_pontuacao_entra(
                                &ranking,
                                pontosPendentes
                            )
                        ) {
                            nomeJogador[0] =
                                '\0';

                            avisoRanking[0] =
                                '\0';

                            posicaoEstimada =
                                posicaoEstimadaRanking(
                                    &ranking,
                                    pontosPendentes,
                                    portoesPendentes,
                                    abatidosPendentes
                                );

                            tela =
                                TELA_ENTRADA_NOME;
                        } else {
                            tela =
                                TELA_FIM;
                        }
                    }
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
                if (
                    IsKeyPressed(KEY_R) ||
                    botaoClicado(
                        render_area_botao_ranking_fim()
                    )
                ) {
                    retornoRanking =
                        RETORNO_RANKING_FIM;

                    tela =
                        TELA_RANKING;

                    break;
                }

                if (IsKeyPressed(KEY_ENTER)) {
                    iniciaNovaPartida(
                        &mundo
                    );

                    resultadoProcessado =
                        false;

                    avisoRanking[0] =
                        '\0';

                    tela =
                        TELA_JOGO;
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    tela =
                        TELA_MENU;
                }
                break;

            case TELA_RANKING:
                if (
                    IsKeyPressed(KEY_ESCAPE) ||
                    IsKeyPressed(KEY_R) ||
                    IsKeyPressed(KEY_ENTER) ||
                    botaoClicado(
                        render_area_botao_voltar_ranking()
                    )
                ) {
                    tela =
                        retornoRanking ==
                            RETORNO_RANKING_FIM
                                ? TELA_FIM
                                : TELA_MENU;
                }
                break;

            case TELA_ENTRADA_NOME: {
                atualizaEntradaNome(
                    nomeJogador
                );

                bool salvar =
                    IsKeyPressed(KEY_ENTER) ||
                    botaoClicado(
                        render_area_botao_salvar_nome()
                    );

                /*
                 * Esc registra o nome padrão PLAYER para que o
                 * jogador não fique preso nesta tela.
                 */
                if (IsKeyPressed(KEY_ESCAPE)) {
                    snprintf(
                        nomeJogador,
                        sizeof nomeJogador,
                        "%s",
                        "PLAYER"
                    );

                    salvar =
                        true;
                }

                if (salvar) {
                    if (
                        !nomePossuiConteudo(
                            nomeJogador
                        )
                    ) {
                        snprintf(
                            avisoRanking,
                            sizeof avisoRanking,
                            "%s",
                            "Digite um nome antes de salvar."
                        );
                    } else {
                        bool entrou =
                            ranking_adiciona(
                                &ranking,
                                nomeJogador,
                                pontosPendentes,
                                portoesPendentes,
                                abatidosPendentes
                            );

                        if (!entrou) {
                            snprintf(
                                avisoRanking,
                                sizeof avisoRanking,
                                "%s",
                                "A pontuacao nao entrou no ranking."
                            );

                            tela =
                                TELA_FIM;

                            break;
                        }

                        bool salvou =
                            ranking_salva(
                                &ranking
                            );

                        recorde =
                            recordeDoRanking(
                                &ranking
                            );

                        if (!salvou) {
                            /*
                             * O resultado continua disponível
                             * nesta execução, mesmo se o arquivo
                             * não puder ser escrito.
                             */
                            snprintf(
                                avisoRanking,
                                sizeof avisoRanking,
                                "%s",
                                "Ranking atualizado, mas ranking.txt nao foi salvo."
                            );
                        } else {
                            avisoRanking[0] =
                                '\0';
                        }

                        retornoRanking =
                            RETORNO_RANKING_FIM;

                        /*
                         * Depois de registrar, mostramos o Top 10
                         * para o jogador ver a nova posição.
                         */
                        tela =
                            TELA_RANKING;
                    }
                }
                break;
            }
        }

        /* ====================================================
         * DESENHO
         * ==================================================== */

        Camera2D camera =
            criaCameraEscalada();

        BeginDrawing();

        ClearBackground(
            (Color){ 10, 11, 20, 255 }
        );

        BeginMode2D(camera);

        switch (tela) {
            case TELA_MENU:
                render_menu(
                    hoverRankingMenu
                );
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
                    recorde,
                    hoverRankingFim
                );
                break;

            case TELA_RANKING:
                /*
                 * Mantém um fundo coerente com a tela de origem.
                 */
                if (
                    retornoRanking ==
                    RETORNO_RANKING_FIM
                ) {
                    render_mundo(
                        &mundo,
                        mostraTabela
                    );
                } else {
                    render_menu(false);
                }

                render_ranking(
                    &ranking,
                    hoverVoltarRanking
                );
                break;

            case TELA_ENTRADA_NOME:
                render_mundo(
                    &mundo,
                    mostraTabela
                );

                render_entrada_nome(
                    nomeJogador,
                    pontosPendentes,
                    posicaoEstimada,
                    hoverSalvarNome,
                    avisoRanking
                );
                break;
        }

        EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}