#ifndef RANKING_H
#define RANKING_H

#include <stdbool.h>

/*
 * Quantidade máxima de jogadores exibidos no ranking.
 */
#define RANKING_MAX_REGISTROS 10

/*
 * Limite do nome sem contar o caractere nulo.
 */
#define RANKING_NOME_MAX 15

/*
 * Arquivo local usado para persistir o ranking.
 *
 * O arquivo será criado automaticamente na pasta em que
 * o executável for iniciado.
 */
#define RANKING_ARQUIVO "ranking.txt"

/*
 * Um registro do ranking.
 */
typedef struct {
    char nome[RANKING_NOME_MAX + 1];

    int pontos;
    int portoes;
    int abatidos;
} RegistroRanking;

/*
 * Coleção com os dez melhores resultados.
 */
typedef struct {
    RegistroRanking registros[RANKING_MAX_REGISTROS];
    int quantidade;
} Ranking;

/*
 * Cria um ranking vazio.
 */
Ranking ranking_vazio(void);

/*
 * Carrega o ranking salvo no arquivo ranking.txt.
 *
 * Se o arquivo não existir, estiver vazio ou possuir linhas
 * inválidas, os registros válidos são preservados e o restante
 * é ignorado.
 */
Ranking ranking_carrega(void);

/*
 * Salva o ranking no arquivo ranking.txt.
 *
 * Retorna true quando o arquivo foi gravado com sucesso.
 */
bool ranking_salva(const Ranking *ranking);

/*
 * Verifica se uma pontuação pode entrar no Top 10.
 *
 * Qualquer pontuação entra quando ainda existem menos de
 * dez registros. Quando a lista está cheia, a pontuação
 * precisa ser maior que a última colocada.
 */
bool ranking_pontuacao_entra(
    const Ranking *ranking,
    int pontos
);

/*
 * Insere um novo resultado, ordena o ranking da maior para a
 * menor pontuação e mantém somente os dez melhores.
 *
 * Em caso de empate:
 * 1. mais portões fica na frente;
 * 2. mais inimigos abatidos fica na frente;
 * 3. o registro mais antigo permanece na frente.
 *
 * Retorna true quando o registro entrou no ranking.
 */
bool ranking_adiciona(
    Ranking *ranking,
    const char *nome,
    int pontos,
    int portoes,
    int abatidos
);

#endif