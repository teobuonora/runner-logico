#include "funcional.h"
#include <string.h>

void mapa(void *arr, int n, size_t tam, FuncaoMapa f, void *ctx) {
    if (arr == NULL || f == NULL || n <= 0) return;      /* robustez: lista vazia */
    char *base = (char *)arr;
    for (int i = 0; i < n; i++)
        f(base + (size_t)i * tam, ctx);
}

int filtra(void *arr, int n, size_t tam, Predicado p, const void *ctx) {
    if (arr == NULL || p == NULL || n <= 0) return 0;
    char *base = (char *)arr;
    int j = 0;
    for (int i = 0; i < n; i++) {
        char *elem = base + (size_t)i * tam;
        if (p(elem, ctx)) {
            if (i != j) memmove(base + (size_t)j * tam, elem, tam);  /* compacta os que passam */
            j++;
        }
    }
    return j;                                                       /* novo tamanho */
}

float dobra(const void *arr, int n, size_t tam, float inicial,
            FuncaoDobra f, const void *ctx) {
    if (arr == NULL || f == NULL || n <= 0) return inicial;
    const char *base = (const char *)arr;
    float acc = inicial;
    for (int i = 0; i < n; i++)
        acc = f(acc, base + (size_t)i * tam, ctx);
    return acc;
}

int conta(const void *arr, int n, size_t tam, Predicado p, const void *ctx) {
    if (arr == NULL || p == NULL || n <= 0) return 0;
    const char *base = (const char *)arr;
    int total = 0;                      /* fold com acumulador inteiro */
    for (int i = 0; i < n; i++)
        if (p(base + (size_t)i * tam, ctx)) total++;
    return total;
}
