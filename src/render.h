#ifndef RENDER_H
#define RENDER_H

#include <stdbool.h>
#include "mundo.h"

/* Só DESENHA: nunca altera o Mundo (const). */
void render_mundo(const Mundo *m, bool mostraTabela);
void render_menu(void);
void render_pausa(void);
void render_fim(const Mundo *m, int recorde);

#endif
