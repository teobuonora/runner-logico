#ifndef FUNCIONAL_H
#define FUNCIONAL_H

/*
 * Mini-biblioteca "funcional" para C (Aulas 13-16):
 *   mapa   -> aplica uma função a CADA elemento   (map)
 *   filtra -> mantém só os que passam no teste     (filter)
 *   dobra  -> combina a coleção em um único valor  (fold/reduce)
 *
 * Funciona com qualquer vetor de structs: basta passar o tamanho do
 * elemento (sizeof) e uma função (ponteiro de função) que sabe tratá-lo.
 */

#include <stdbool.h>
#include <stddef.h>

typedef void  (*FuncaoMapa)(void *elem, void *ctx);
typedef bool  (*Predicado)(const void *elem, const void *ctx);
typedef float (*FuncaoDobra)(float acc, const void *elem, const void *ctx);

/* map: chama f em cada elemento (altera o vetor in-place) */
void  mapa(void *arr, int n, size_t tam, FuncaoMapa f, void *ctx);

/* filter: compacta o vetor mantendo só quem passa em p. Retorna o novo tamanho. */
int   filtra(void *arr, int n, size_t tam, Predicado p, const void *ctx);

/* fold: acc = f(acc, elem) para cada elemento, começando em 'inicial'. */
float dobra(const void *arr, int n, size_t tam, float inicial,
            FuncaoDobra f, const void *ctx);

/* caso particular de fold: quantos elementos passam em p */
int   conta(const void *arr, int n, size_t tam, Predicado p, const void *ctx);

#endif
