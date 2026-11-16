#ifndef CONFIG_H
#define CONFIG_H

/* ---------- Tela e mundo ---------- */
#define LARGURA_TELA 960
#define ALTURA_TELA  540
#define CHAO_Y       440.0f   /* y do chão (topo da faixa escura) */

/* ---------- Jogador ---------- */
#define JOGADOR_X           130.0f
#define JOGADOR_L           40.0f
#define JOGADOR_A           56.0f
#define JOGADOR_A_ABAIXADO  30.0f
#define GRAVIDADE           2300.0f
#define GRAVIDADE_QUEDA     3400.0f  /* gravidade extra ao apertar "baixo" no ar */
#define VEL_PULO            820.0f
#define VIDAS_INICIAIS      3
#define VIDAS_MAX           5
#define TEMPO_INVULNERAVEL  1.4f

/* ---------- Proposições (P, Q, R) e seus efeitos ---------- */
#define PROP_ESCUDO 0   /* P: escudo -> destrói o que encostar em você */
#define PROP_ARMA   1   /* Q: arma -> só atira com Q verdadeira         */
#define PROP_TURBO  2   /* R: turbo -> +35% velocidade, pontos x2       */

#define ENERGIA_MAX       100.0f
#define CUSTO_ENERGIA     7.0f    /* por proposição verdadeira, por segundo */
#define REGEN_ENERGIA     14.0f   /* por segundo, quando todas são falsas   */
#define TEMPO_SOBRECARGA  1.5f
#define BONUS_TURBO       1.35f

/* ---------- Velocidade ---------- */
#define VEL_INICIAL 300.0f
#define VEL_MAX     620.0f

/* ---------- Armas / coleções ---------- */
#define VEL_TIRO       900.0f
#define COOLDOWN_TIRO  0.28f
#define MAX_ENTIDADES  48
#define MAX_TIROS      24

/* ---------- Portões lógicos ---------- */

/*
 * Quando o portão fica a esta distância do jogador, a corrida é
 * temporariamente congelada para iniciar o desafio lógico.
 */
#define DISTANCIA_DECISAO_PORTAO 180.0f

/*
 * Tempo disponível para alterar P, Q e R antes que a valoração
 * atual seja confirmada automaticamente.
 */
#define TEMPO_DECISAO_PORTAO 5.0f

/* ---------- Objetivo do jogo ---------- */
#define META_PORTOES   8      /* vitória: abrir 8 portões lógicos */
#define PONTOS_PORTAO  100

#endif