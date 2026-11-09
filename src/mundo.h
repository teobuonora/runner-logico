#ifndef MUNDO_H
#define MUNDO_H

/*
 * ESTADO DO JOGO + REGRAS (sem desenhar nada, sem ler teclado).
 *
 *   Mundo mundo_atualiza(Mundo m, Entrada in, float dt)
 *
 * recebe o estado antigo (por valor = uma cópia), a entrada do quadro e o
 * tempo decorrido, e DEVOLVE o novo estado. O game loop (main.c) só chama
 * essa função e depois manda desenhar. Assim a lógica é testável sem janela.
 */

#include <stdint.h>
#include <stdbool.h>
#include "raylib.h"      /* só para Rectangle e CheckCollisionRecs */
#include "logica.h"
#include "config.h"

typedef enum {
    ENT_CACTO,        /* obstáculo fixo: pule ou use escudo           */
    ENT_ANDARILHO,    /* inimigo terrestre (2 de vida): atire!         */
    ENT_VOADOR,       /* inimigo aéreo (1 de vida): abaixe ou atire    */
    ENT_PORTAO,       /* PORTÃO LÓGICO: abre se a fórmula for verdadeira */
    ENT_PU_ENERGIA,   /* power-up: +50 de energia                      */
    ENT_PU_VIDA,      /* power-up: +1 vida                             */
    ENT_PU_TAUTOLOGIA /* power-up: o próximo portão abre sempre        */
} TipoEnt;

typedef struct {
    TipoEnt   tipo;
    Rectangle caixa;
    float     y0;          /* altura-base (voadores e power-ups balançam) */
    float     velExtra;    /* velocidade própria além da rolagem do chão  */
    int       vida;
    bool      viva;
    float     anim;
    int       formulaId;   /* só para portões: índice no catálogo */
    bool      avaliado;    /* só para portões: já foi avaliado?   */
    bool      resultado;   /* só para portões: deu V ou F?        */
} Entidade;

typedef struct {
    Rectangle caixa;
    bool      vivo;
} Tiro;

typedef struct {
    bool pula;                    /* apertou agora        */
    bool abaixa;                  /* segurando            */
    bool atira;                   /* segurando            */
    bool alterna[NUM_PROPS];      /* apertou 1, 2 ou 3    */
} Entrada;

typedef enum { RES_JOGANDO, RES_VITORIA, RES_DERROTA } Resultado;

/* eventos do quadro (bitmask) — úteis para sons/efeitos depois */
enum {
    EV_PULO        = 1 << 0,
    EV_TIRO        = 1 << 1,
    EV_ABATE       = 1 << 2,
    EV_DANO        = 1 << 3,
    EV_PORTAO_OK   = 1 << 4,
    EV_PORTAO_ERRO = 1 << 5,
    EV_POWERUP     = 1 << 6,
    EV_SOBRECARGA  = 1 << 7,
    EV_SEM_ARMA    = 1 << 8
};

typedef struct {
    /* jogador */
    float alt, velY;              /* altura do pé acima do chão e velocidade vertical */
    bool  noChao, abaixado;
    int   vidas;
    float invulneravel;           /* segundos restantes */
    float cooldownTiro;

    /* proposições */
    Valoracao val;                /* valor atual de P, Q, R */
    float energia;
    float sobrecarga;             /* >0: não dá para mexer nas proposições */
    bool  tautologia;             /* próximo portão abre sempre */

    /* coleções de objetos */
    Entidade ents[MAX_ENTIDADES]; int nEnts;
    Tiro     tiros[MAX_TIROS];    int nTiros;

    /* progresso */
    float velocidade, distancia, tempo, pontosDist;
    int   bonus, abatidos, portoesOk, portoesErro, portoesGerados;
    float distProx, distProxPortao;   /* distância até o próximo objeto / portão */

    /* diversos */
    uint32_t  rng;
    Resultado resultado;
    unsigned  eventos;
    char      msg[56];
    float     msgTempo;
} Mundo;

Mundo mundo_novo(uint32_t semente);
Mundo mundo_atualiza(Mundo m, Entrada in, float dt);

int        mundo_pontos(const Mundo *m);
Rectangle  mundo_caixa_jogador(const Mundo *m);
const Entidade *mundo_proximo_portao(const Mundo *m);   /* portão à frente ainda não avaliado, ou NULL */

/* ---------- regras PURAS (fáceis de testar) ---------- */
float regra_velocidade_base(float tempo);
float regra_energia(float energia, int ativas, float dt);
int   regra_conta_ativas(Valoracao v);
int   regra_pontos_inimigo(TipoEnt t);
int   regra_nivel_formula(int portoesGerados);

#endif
