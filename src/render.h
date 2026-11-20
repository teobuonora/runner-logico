#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include "raylib.h"
#include "mundo.h"
#include "ranking.h"

/* ============================================================
 * ÁREAS CLICÁVEIS
 *
 * Todas as coordenadas utilizam a resolução lógica de
 * 960 x 540. O main.c converte a posição real do mouse.
 * ============================================================ */

/* Botão do ranking no menu inicial. */
#define BOTAO_RANKING_MENU_X 342
#define BOTAO_RANKING_MENU_Y 478
#define BOTAO_RANKING_MENU_L 276
#define BOTAO_RANKING_MENU_A 42

/* Botão do ranking na tela final. */
#define BOTAO_RANKING_FIM_X 340
#define BOTAO_RANKING_FIM_Y 426
#define BOTAO_RANKING_FIM_L 280
#define BOTAO_RANKING_FIM_A 42

/* Botão de retorno da tela de ranking. */
#define BOTAO_VOLTAR_RANKING_X 340
#define BOTAO_VOLTAR_RANKING_Y 474
#define BOTAO_VOLTAR_RANKING_L 280
#define BOTAO_VOLTAR_RANKING_A 42

/* Botão para salvar o nome no ranking. */
#define BOTAO_SALVAR_NOME_X 340
#define BOTAO_SALVAR_NOME_Y 414
#define BOTAO_SALVAR_NOME_L 280
#define BOTAO_SALVAR_NOME_A 44

/* ============================================================
 * ÁREAS DOS BOTÕES
 * ============================================================ */

Rectangle render_area_botao_ranking_menu(void);

Rectangle render_area_botao_ranking_fim(void);

Rectangle render_area_botao_voltar_ranking(void);

Rectangle render_area_botao_salvar_nome(void);

/* ============================================================
 * JOGO
 * ============================================================ */

/*
 * Desenha o estado atual da partida.
 * O renderizador nunca modifica o Mundo.
 */
void render_mundo(
    const Mundo *m,
    bool mostraTabela
);

/* ============================================================
 * TELAS PRINCIPAIS
 * ============================================================ */

/*
 * Menu inicial.
 *
 * rankingHover deve ser verdadeiro quando o mouse estiver
 * sobre o botão do ranking.
 */
void render_menu(
    bool rankingHover
);

/* Tela de pausa. */
void render_pausa(void);

/*
 * Tela de vitória ou derrota.
 *
 * rankingHover deve ser verdadeiro quando o mouse estiver
 * sobre o botão VER RANKING.
 */
void render_fim(
    const Mundo *m,
    int recorde,
    bool rankingHover
);

/* ============================================================
 * RANKING
 * ============================================================ */

/*
 * Exibe as dez maiores pontuações.
 */
void render_ranking(
    const Ranking *ranking,
    bool voltarHover
);

/*
 * Exibe a tela para o jogador informar o nome quando uma
 * pontuação entra no Top 10.
 */
void render_entrada_nome(
    const char *nome,
    int pontos,
    int posicaoEstimada,
    bool salvarHover,
    const char *aviso
);

#endif