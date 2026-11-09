# Lógica Proposicional no Runner Lógico

Este documento explica **onde e como** a Lógica Proposicional aparece no jogo
(critério "Lógica proposicional explícita") e sugere um roteiro de apresentação.

## 1. A ideia central

O jogador não responde perguntas (não é quiz). Ele **controla o valor-verdade de três
proposições** e precisa **escolher a valoração certa na hora certa**:

| Proposição | Significado no jogo | Efeito quando **V** |
|:----------:|---------------------|---------------------|
| **P** | "O escudo está ligado" | destrói o que encostar |
| **Q** | "A arma está ligada"   | permite atirar |
| **R** | "O turbo está ligado"  | +velocidade, pontos x2 |

A **valoração** é o trio (P, Q, R) atual, por exemplo `P=V, Q=F, R=V`.

## 2. O portão lógico = avaliar uma fórmula

Ao cruzar um portão, o jogo avalia a fórmula com a valoração atual
(`avalia()` em `src/logica.c`):

* fórmula **V** → portão abre, +100 pontos;
* fórmula **F** → perde 1 vida.

Exemplo: portão `P & ~Q`.

| P | Q | R | `P & ~Q` |
|:-:|:-:|:-:|:--------:|
| V | F | V | **V** ✔ |
| V | V | F | F ✘ |
| F | F | F | F ✘ |

O jogador **vê a tabela-verdade do portão** (tecla `T`) com a linha da valoração atual destacada,
então aprende lendo a tabela, não decorando.

## 3. Conectivos usados

| Símbolo no jogo | Conectivo | Regra |
|:---------------:|-----------|-------|
| `~A` | negação | V quando A é F |
| `A & B` | conjunção | V só se os dois são V |
| `A \| B` | disjunção | F só se os dois são F |
| `A -> B` | condicional | F **só** quando A=V e B=F |
| `A <-> B` | bicondicional | V quando A e B têm o mesmo valor |

## 4. Níveis de dificuldade (catálogo de 21 fórmulas)

| Nível | Exemplos | Conceito |
|:-----:|----------|----------|
| 0 | `P`, `~Q`, `P & Q`, `P \| Q` | variáveis e conectivos básicos |
| 1 | `P & ~Q`, `P -> Q`, `~P & ~R`, `~P \| Q` | negação, condicional |
| 2 | `(P -> Q) & R`, `P <-> Q`, `~(P & Q)`, `~P \| ~Q` | fórmulas compostas, bicondicional |
| 3 | `(P & Q) -> R`, `~P -> (Q & R)`, `~(P <-> R) & Q` | fórmulas aninhadas |

Os portões ficam mais difíceis conforme o jogador avança (a cada 2 portões sobe um nível).

## 5. Onde o jogo ensina equivalências

O catálogo contém pares **equivalentes de propósito**, e os testes (`tests/test_logica.c`) provam isso:

* `P -> Q` ≡ `~P | Q` (condicional como disjunção);
* `~(P & Q)` ≡ `~P | ~Q` (**Lei de De Morgan**).

Dois portões com fórmulas diferentes podem ser resolvidos com a mesma valoração, e as tabelas-verdade
mostram o porquê.

## 6. Outras mecânicas com lógica

* **Power-up Tautologia (T)**: uma tautologia é verdadeira para toda valoração; o power-up faz o
  próximo portão se comportar como tal.
* **Energia**: cada proposição **V** custa energia. Ter tudo verdadeiro o tempo todo não funciona;
  o jogador precisa decidir *quais* proposições manter V em cada momento.
* **Condição de uso da arma**: "atirar só é possível se Q é V" — uma implicação em ação (`atira -> Q`).
* **Combinações**: escudo (P) + turbo (R) ao mesmo tempo é uma conjunção de estados, com custo de energia.

## 7. Por que não é um quiz

* O jogador **age em tempo real** (pular, atirar, desviar) enquanto decide a valoração.
* Existe **tomada de decisão**: custo de energia, risco, timing.
* Não há perguntas com alternativas: a lógica **é** a regra que abre ou fecha o caminho.

## 8. Mapeamento com os critérios da AV2

| Critério | Peso | Onde mostrar |
|----------|:----:|--------------|
| Jogabilidade | 30% | jogo roda do início ao fim (vitória ou derrota), regras no menu e no HUD |
| Lógica proposicional explícita | 30% | portões, tabela-verdade na tela, conectivos, tautologia, equivalências |
| Apresentação | 20% | roteiro abaixo |
| Originalidade | 20% | corrida + arma + proposições como "poderes" + portões lógicos |

## 9. Roteiro de apresentação (≈ 5 min)

1. **O que é** (30 s): "um runner onde a lógica abre o caminho".
2. **Proposições e valoração** (1 min): mostre P, Q e R no painel inferior e as teclas 1, 2 e 3.
3. **Demonstração ao vivo** (2 min): jogue até um portão, mostre a tabela-verdade com a linha atual
   destacada; erre de propósito um portão e acerte o seguinte.
4. **Conectivos e equivalências** (1 min): compare `~(P & Q)` e `~P | ~Q` (De Morgan) usando as tabelas.
5. **Tautologia e energia** (30 s): pegue o power-up **T** e explique por que ele sempre abre.
6. **Fechamento**: o código tem testes que verificam as tabelas-verdade (`make test`).
