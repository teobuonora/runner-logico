#ifndef CONFIG_H
#define CONFIG_H

/* ---------- Tela lógica do jogo ---------- */

#define LARGURA_TELA 960
#define ALTURA_TELA  540
#define CHAO_Y       440.0f

/*
 * A janela física pode ser maior e redimensionável,
 * mas o jogo continua sendo desenhado nesta resolução lógica.
 */

/* ---------- Jogador ---------- */

#define JOGADOR_X           130.0f
#define JOGADOR_L           40.0f
#define JOGADOR_A           56.0f
#define JOGADOR_A_ABAIXADO  30.0f

#define GRAVIDADE           2300.0f
#define GRAVIDADE_QUEDA     3400.0f
#define VEL_PULO            820.0f

#define VIDAS_INICIAIS      3
#define VIDAS_MAX           5
#define TEMPO_INVULNERAVEL  1.4f

/* ---------- Proposições P, Q e R ---------- */

#define PROP_ESCUDO 0
#define PROP_ARMA   1
#define PROP_TURBO  2

#define ENERGIA_MAX       100.0f
#define CUSTO_ENERGIA     7.0f
#define REGEN_ENERGIA     14.0f
#define TEMPO_SOBRECARGA  1.5f
#define BONUS_TURBO       1.35f

/* ---------- Velocidade ---------- */

#define VEL_INICIAL 300.0f
#define VEL_MAX     620.0f

/* ---------- Armas e coleções ---------- */

#define VEL_TIRO       900.0f
#define COOLDOWN_TIRO  0.28f

#define MAX_ENTIDADES  48
#define MAX_TIROS      24

/* ---------- Portões lógicos ---------- */

/*
 * O desafio começa quando a frente do jogador fica a esta
 * distância horizontal do portão.
 */
#define DISTANCIA_DECISAO_PORTAO 180.0f

/*
 * Tempo disponível para configurar P, Q e R.
 */
#define TEMPO_DECISAO_PORTAO 5.0f

/* ---------- Objetivo ---------- */

#define META_PORTOES   8
#define PONTOS_PORTAO  100

/* ---------- Sequência final de vitória ---------- */

/*
 * Depois que o oitavo portão é resolvido corretamente, o jogo
 * não termina imediatamente.
 *
 * A sequência possui duas etapas:
 *
 * 1. O dinossauro continua correndo e atravessa o portão.
 * 2. O dinossauro desacelera e realiza a comemoração.
 */
#define TEMPO_SEQUENCIA_VITORIA 4.5f

/*
 * Durante os primeiros 2,8 segundos, o cenário continua em
 * movimento normal para que o portão passe pelo jogador.
 */
#define TEMPO_CORRIDA_VITORIA 2.8f

/*
 * Nos segundos finais, a velocidade vai diminuindo e o
 * renderizador apresenta a animação de comemoração.
 */
#define TEMPO_COMEMORACAO_VITORIA \
    (TEMPO_SEQUENCIA_VITORIA - TEMPO_CORRIDA_VITORIA)

#endif
