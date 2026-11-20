#include "ranking.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* ============================================================
 * Funções auxiliares internas
 * ============================================================ */

/*
 * Mantém o nome seguro para o formato do arquivo:
 *
 * - remove espaços no começo;
 * - remove espaços no final;
 * - troca o separador "|" por espaço;
 * - aceita letras, números, espaço, hífen e sublinhado;
 * - limita o nome a RANKING_NOME_MAX caracteres;
 * - usa "PLAYER" quando o nome estiver vazio.
 */
static void normalizaNome(
    const char *origem,
    char destino[RANKING_NOME_MAX + 1]
) {
    int inicio = 0;
    int fim;
    int tamanhoOrigem;
    int posicaoDestino = 0;

    if (origem == NULL) {
        snprintf(
            destino,
            RANKING_NOME_MAX + 1,
            "%s",
            "PLAYER"
        );

        return;
    }

    tamanhoOrigem =
        (int)strlen(origem);

    /*
     * Ignora os espaços do começo.
     */
    while (
        inicio < tamanhoOrigem &&
        isspace((unsigned char)origem[inicio])
    ) {
        inicio++;
    }

    /*
     * Ignora os espaços do final.
     */
    fim =
        tamanhoOrigem - 1;

    while (
        fim >= inicio &&
        isspace((unsigned char)origem[fim])
    ) {
        fim--;
    }

    for (
        int i = inicio;
        i <= fim &&
        posicaoDestino < RANKING_NOME_MAX;
        i++
    ) {
        unsigned char caractere =
            (unsigned char)origem[i];

        /*
         * O caractere "|" não pode aparecer no nome porque é
         * utilizado como separador dentro de ranking.txt.
         */
        if (caractere == '|') {
            caractere = ' ';
        }

        /*
         * Mantém apenas caracteres simples para garantir que o
         * arquivo possa ser lido em Windows, Linux e macOS.
         */
        if (
            isalnum(caractere) ||
            caractere == ' ' ||
            caractere == '-' ||
            caractere == '_'
        ) {
            /*
             * Evita vários espaços consecutivos.
             */
            if (
                caractere == ' ' &&
                (
                    posicaoDestino == 0 ||
                    destino[posicaoDestino - 1] == ' '
                )
            ) {
                continue;
            }

            destino[posicaoDestino++] =
                (char)caractere;
        }
    }

    /*
     * Remove um possível espaço no fim depois da normalização.
     */
    while (
        posicaoDestino > 0 &&
        destino[posicaoDestino - 1] == ' '
    ) {
        posicaoDestino--;
    }

    destino[posicaoDestino] =
        '\0';

    if (posicaoDestino == 0) {
        snprintf(
            destino,
            RANKING_NOME_MAX + 1,
            "%s",
            "PLAYER"
        );
    }
}

/*
 * Compara dois registros.
 *
 * Retorna:
 * - valor negativo quando A deve vir antes de B;
 * - valor positivo quando B deve vir antes de A;
 * - zero quando possuem o mesmo desempenho.
 */
static int comparaRegistros(
    const RegistroRanking *a,
    const RegistroRanking *b
) {
    if (a->pontos != b->pontos) {
        return
            a->pontos > b->pontos
                ? -1
                : 1;
    }

    if (a->portoes != b->portoes) {
        return
            a->portoes > b->portoes
                ? -1
                : 1;
    }

    if (a->abatidos != b->abatidos) {
        return
            a->abatidos > b->abatidos
                ? -1
                : 1;
    }

    return 0;
}

/*
 * Ordenação estável por inserção.
 *
 * O ranking possui no máximo dez elementos, portanto uma
 * ordenação simples é suficiente e evita dependências extras.
 */
static void ordenaRanking(Ranking *ranking) {
    for (
        int i = 1;
        i < ranking->quantidade;
        i++
    ) {
        RegistroRanking atual =
            ranking->registros[i];

        int j =
            i - 1;

        while (
            j >= 0 &&
            comparaRegistros(
                &atual,
                &ranking->registros[j]
            ) < 0
        ) {
            ranking->registros[j + 1] =
                ranking->registros[j];

            j--;
        }

        ranking->registros[j + 1] =
            atual;
    }
}

/*
 * Verifica se os valores de uma linha do arquivo são válidos.
 */
static bool registroValido(
    const char *nome,
    int pontos,
    int portoes,
    int abatidos
) {
    if (
        nome == NULL ||
        nome[0] == '\0'
    ) {
        return false;
    }

    if (
        pontos < 0 ||
        portoes < 0 ||
        abatidos < 0
    ) {
        return false;
    }

    return true;
}

/*
 * Descarta o restante de uma linha do arquivo.
 *
 * Isso evita que uma linha muito longa interfira na leitura
 * do próximo registro.
 */
static void descartaAteFimDaLinha(FILE *arquivo) {
    int caractere;

    do {
        caractere =
            fgetc(arquivo);
    } while (
        caractere != '\n' &&
        caractere != EOF
    );
}

/* ============================================================
 * API pública
 * ============================================================ */

Ranking ranking_vazio(void) {
    Ranking ranking;

    memset(
        &ranking,
        0,
        sizeof ranking
    );

    return ranking;
}

bool ranking_pontuacao_entra(
    const Ranking *ranking,
    int pontos
) {
    if (ranking == NULL) {
        return false;
    }

    if (pontos < 0) {
        return false;
    }

    /*
     * Enquanto existirem vagas, qualquer pontuação válida entra.
     */
    if (
        ranking->quantidade <
        RANKING_MAX_REGISTROS
    ) {
        return true;
    }

    /*
     * Com dez registros, a pontuação precisa superar a última
     * colocada.
     */
    return
        pontos >
        ranking
            ->registros[
                RANKING_MAX_REGISTROS - 1
            ]
            .pontos;
}

bool ranking_adiciona(
    Ranking *ranking,
    const char *nome,
    int pontos,
    int portoes,
    int abatidos
) {
    if (ranking == NULL) {
        return false;
    }

    if (
        pontos < 0 ||
        portoes < 0 ||
        abatidos < 0
    ) {
        return false;
    }

    if (
        !ranking_pontuacao_entra(
            ranking,
            pontos
        )
    ) {
        return false;
    }

    RegistroRanking novoRegistro;

    memset(
        &novoRegistro,
        0,
        sizeof novoRegistro
    );

    normalizaNome(
        nome,
        novoRegistro.nome
    );

    novoRegistro.pontos =
        pontos;

    novoRegistro.portoes =
        portoes;

    novoRegistro.abatidos =
        abatidos;

    if (
        ranking->quantidade <
        RANKING_MAX_REGISTROS
    ) {
        ranking
            ->registros[
                ranking->quantidade
            ] =
            novoRegistro;

        ranking->quantidade++;
    } else {
        /*
         * A lista está cheia, mas a pontuação já foi aprovada.
         * Substituímos temporariamente a última colocação e
         * depois ordenamos novamente.
         */
        ranking
            ->registros[
                RANKING_MAX_REGISTROS - 1
            ] =
            novoRegistro;
    }

    ordenaRanking(ranking);

    return true;
}

Ranking ranking_carrega(void) {
    Ranking ranking =
        ranking_vazio();

    FILE *arquivo =
        fopen(
            RANKING_ARQUIVO,
            "r"
        );

    /*
     * O ranking começa vazio quando o arquivo ainda não existe.
     * Isso é normal na primeira execução do jogo.
     */
    if (arquivo == NULL) {
        return ranking;
    }

    char nomeLido[128];
    int pontos;
    int portoes;
    int abatidos;

    while (!feof(arquivo)) {
        memset(
            nomeLido,
            0,
            sizeof nomeLido
        );

        pontos = 0;
        portoes = 0;
        abatidos = 0;

        int camposLidos =
            fscanf(
                arquivo,
                " %127[^|]|%d|%d|%d",
                nomeLido,
                &pontos,
                &portoes,
                &abatidos
            );

        if (camposLidos == EOF) {
            break;
        }

        descartaAteFimDaLinha(arquivo);

        if (camposLidos != 4) {
            continue;
        }

        if (
            !registroValido(
                nomeLido,
                pontos,
                portoes,
                abatidos
            )
        ) {
            continue;
        }

        /*
         * Durante o carregamento, inserir pelo mesmo método
         * garante ordenação e limite de dez registros.
         */
        ranking_adiciona(
            &ranking,
            nomeLido,
            pontos,
            portoes,
            abatidos
        );
    }

    fclose(arquivo);

    ordenaRanking(&ranking);

    return ranking;
}

bool ranking_salva(const Ranking *ranking) {
    if (ranking == NULL) {
        return false;
    }

    FILE *arquivo =
        fopen(
            RANKING_ARQUIVO,
            "w"
        );

    if (arquivo == NULL) {
        return false;
    }

    int quantidade =
        ranking->quantidade;

    if (quantidade < 0) {
        quantidade = 0;
    }

    if (
        quantidade >
        RANKING_MAX_REGISTROS
    ) {
        quantidade =
            RANKING_MAX_REGISTROS;
    }

    for (
        int i = 0;
        i < quantidade;
        i++
    ) {
        const RegistroRanking *registro =
            &ranking->registros[i];

        if (
            fprintf(
                arquivo,
                "%s|%d|%d|%d\n",
                registro->nome,
                registro->pontos,
                registro->portoes,
                registro->abatidos
            ) < 0
        ) {
            fclose(arquivo);
            return false;
        }
    }

    if (fclose(arquivo) != 0) {
        return false;
    }

    return true;
}