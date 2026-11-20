#ifndef MUNDO_H
#define MUNDO_H

/*
 * ESTADO DO JOGO + REGRAS
 *
 * Este módulo não desenha e não lê diretamente o teclado.
 *
 *     Mundo mundo_atualiza(Mundo m, Entrada in, float dt)
 *
 * A função recebe o estado antigo por valor e devolve um novo
 * estado. Isso permite testar a lógica sem abrir a janela.
 */

#include <stdint.h>
#include <stdbool.h>
#include "raylib.h"
#include "logica.h"
#include "config.h"

/* ============================================================
 * ENTIDADES
 * ============================================================ */

typedef enum {
    ENT_CACTO,
    ENT_ANDARILHO,
    ENT_VOADOR,
    ENT_PORTAO,
    ENT_PU_ENERGIA,
    ENT_PU_VIDA,
    ENT_PU_TAUTOLOGIA
} TipoEnt;

typedef struct {
    TipoEnt tipo;

    Rectangle caixa;

    /*
     * Altura-base utilizada por entidades que oscilam
     * verticalmente.
     */
    float y0;

    /*
     * Velocidade própria além da rolagem do cenário.
     */
    float velExtra;

    int vida;
    bool viva;
    float anim;

    /*
     * Campos utilizados principalmente pelo portão.
     */
    int  formulaId;
    bool avaliado;
    bool resultado;
} Entidade;

typedef struct {
    Rectangle caixa;
    bool vivo;
} Tiro;

/* ============================================================
 * ENTRADA
 * ============================================================ */

typedef struct {
    bool pula;
    bool abaixa;
    bool atira;

    /*
     * Enter confirma a resposta durante o desafio lógico.
     */
    bool confirma;

    /*
     * Teclas 1, 2 e 3.
     */
    bool alterna[NUM_PROPS];
} Entrada;

/* ============================================================
 * RESULTADO
 * ============================================================ */

typedef enum {
    RES_JOGANDO,
    RES_VITORIA,
    RES_DERROTA
} Resultado;

/* ============================================================
 * EVENTOS DO QUADRO
 *
 * Os eventos podem ser utilizados posteriormente para sons,
 * partículas e outros efeitos.
 * ============================================================ */

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

/* ============================================================
 * MUNDO
 * ============================================================ */

typedef struct {
    /* ---------- Jogador ---------- */

    float alt;
    float velY;

    bool noChao;
    bool abaixado;

    int vidas;

    float invulneravel;
    float cooldownTiro;

    /* ---------- Proposições ---------- */

    Valoracao val;

    float energia;
    float sobrecarga;

    bool tautologia;

    /* ---------- Desafio lógico ---------- */

    /*
     * Quando decisaoPortao é verdadeira, a corrida fica
     * congelada para o jogador configurar P, Q e R.
     */
    bool decisaoPortao;

    /*
     * Segundos restantes para confirmar a resposta.
     */
    float tempoDecisaoPortao;

    /* ---------- Sequência de vitória ---------- */

    /*
     * É ativada quando o oitavo portão é resolvido corretamente.
     *
     * Enquanto estiver ativa:
     * - o resultado continua como RES_JOGANDO;
     * - não surgem novos inimigos ou portões;
     * - os controles de combate são ignorados;
     * - P, Q e R permanecem falsas;
     * - o cenário continua avançando;
     * - o portão pode passar pelo dinossauro;
     * - o renderizador realiza a comemoração.
     */
    bool sequenciaVitoria;

    /*
     * Tempo já transcorrido desde o início da sequência.
     *
     * Começa em zero e avança até TEMPO_SEQUENCIA_VITORIA.
     */
    float tempoSequenciaVitoria;

    /* ---------- Coleções ---------- */

    Entidade ents[MAX_ENTIDADES];
    int nEnts;

    Tiro tiros[MAX_TIROS];
    int nTiros;

    /* ---------- Progresso ---------- */

    float velocidade;
    float distancia;
    float tempo;
    float pontosDist;

    int bonus;
    int abatidos;

    int portoesOk;
    int portoesErro;
    int portoesGerados;

    float distProx;
    float distProxPortao;

    /* ---------- Diversos ---------- */

    uint32_t rng;
    Resultado resultado;
    unsigned eventos;

    char msg[56];
    float msgTempo;
} Mundo;

/* ============================================================
 * API PÚBLICA
 * ============================================================ */

Mundo mundo_novo(uint32_t semente);

Mundo mundo_atualiza(
    Mundo m,
    Entrada in,
    float dt
);

int mundo_pontos(const Mundo *m);

Rectangle mundo_caixa_jogador(
    const Mundo *m
);

/*
 * Retorna o portão ainda não avaliado mais próximo do jogador.
 * Retorna NULL quando não existe portão pendente.
 */
const Entidade *mundo_proximo_portao(
    const Mundo *m
);

/* ============================================================
 * REGRAS PURAS
 * ============================================================ */

float regra_velocidade_base(float tempo);

float regra_energia(
    float energia,
    int ativas,
    float dt
);

int regra_conta_ativas(Valoracao v);

int regra_pontos_inimigo(TipoEnt t);

int regra_nivel_formula(int portoesGerados);

#endif