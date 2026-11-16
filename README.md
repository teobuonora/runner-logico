# Runner Lógico

Jogo 2D inspirado no dinossauro do Chrome, desenvolvido em **C com Raylib**, no qual a **Lógica Proposicional faz parte da mecânica principal**.

O jogador controla um dinossauro que corre automaticamente, pula obstáculos, enfrenta inimigos, coleta power-ups e configura os valores de **P**, **Q** e **R** para tornar verdadeiras as fórmulas dos portões.

Projeto integrador de **Programação Imperativa e Funcional** da CESAR School e trabalho da AV2 de **Lógica para Computação**.

---

## Objetivo

Abra **8 portões lógicos** antes de perder todas as vidas.

O jogador começa com **3 vidas** e pode acumular até 5.

- Portão correto: **+100 pontos**
- Portão incorreto: **-1 vida**
- 8 portões abertos: vitória
- 0 vidas: derrota

---

## Proposições e poderes

Três proposições controlam as habilidades do jogador:

| Tecla | Proposição | Poder quando verdadeira |
|---|---|---|
| `1` | **P** | Escudo: destrói obstáculos e inimigos no contato |
| `2` | **Q** | Arma: permite disparar |
| `3` | **R** | Turbo: +35% de velocidade e pontos de distância em dobro |

Cada proposição verdadeira consome energia.

Se a energia chegar a zero, ocorre uma **sobrecarga**: P, Q e R voltam para falso e ficam bloqueadas por 1,5 segundo.

A energia é recuperada quando todas as proposições estão falsas.

---

## Desafio dos portões

Quando o jogador se aproxima de um portão, a corrida é congelada e começa um desafio de **5 segundos**.

Durante o desafio:

1. A fórmula aparece em destaque.
2. O jogador altera P, Q e R usando `1`, `2` e `3`.
3. `Enter` confirma a resposta antes do fim do tempo.
4. Se o tempo acabar, a resposta atual é confirmada automaticamente.
5. A fórmula é avaliada somente uma vez.

Enquanto o desafio estiver ativo, cenário, inimigos, tiros, spawn, pontuação por distância e energia ficam congelados.

Exemplo:

```text
Fórmula: P & ~Q

P = V
Q = F
R = F
```

Nesse caso, `P & ~Q` é verdadeira e o portão abre.

A tecla `T` mostra ou esconde a tabela-verdade do próximo portão durante a corrida.

---

## Controles

| Ação | Teclas |
|---|---|
| Iniciar ou reiniciar | `Enter` |
| Pular | `Espaço`, `↑` ou `W` |
| Abaixar ou cair rápido | `↓` ou `S` |
| Atirar | `X`, `Z` ou `Ctrl esquerdo` |
| Alternar P, Q e R | `1`, `2` e `3` |
| Confirmar resposta do portão | `Enter` |
| Mostrar ou esconder tabela-verdade | `T` |
| Pausar ou continuar | `P` |
| Voltar ao menu | `Esc` |

> Para atirar, primeiro pressione `2` e deixe **Q = V**. Depois use `X`, `Z` ou `Ctrl esquerdo`.

---

## Símbolos das fórmulas

| Símbolo | Significado |
|---|---|
| `~A` | Não A, negação |
| `A & B` | A e B, conjunção |
| `A \| B` | A ou B, disjunção |
| `A -> B` | Se A, então B, condicional |
| `A <-> B` | A se e somente se B, bicondicional |

Os símbolos em ASCII garantem compatibilidade com a fonte padrão da Raylib.

---

## Inimigos e power-ups

### Inimigos

| Elemento | Comportamento |
|---|---|
| **Cacto** | Pule por cima ou destrua com o escudo |
| **Andarilho roxo** | Possui 2 vidas e vale 30 pontos |
| **Voador laranja** | Pode ser evitado ao pular, abaixar ou atirar |

### Power-ups

| Power-up | Efeito |
|---|---|
| **E amarelo** | Recupera 50 de energia |
| **+ vermelho** | Recupera 1 vida |
| **T roxo** | Abre automaticamente o próximo portão |

A tautologia vale somente para um portão e é consumida após o uso.

---

## Interface

O visual é construído diretamente com primitivas gráficas da Raylib, sem sprites externos.

A interface inclui:

- céu em degradê, estrelas, sol e nuvens;
- montanhas em paralaxe;
- chão animado;
- dinossauro e inimigos redesenhados;
- efeitos visuais de escudo, arma e turbo;
- portões tecnológicos;
- HUD de vidas, energia, pontos e progresso;
- cartões para P, Q e R;
- tabela-verdade;
- menu, pausa, vitória e derrota;
- painel do desafio lógico de 5 segundos.

Toda a renderização fica em `src/render.c` e não altera o estado do jogo.

---

## Como executar

É necessário ter um compilador C11, GNU Make ou `mingw32-make` e a biblioteca **Raylib 5.x**.

### Windows

Com Raylib e MinGW configurados, entre na pasta do projeto e execute:

```bash
mingw32-make run
```

Para executar os testes:

```bash
mingw32-make test
```

Também é possível utilizar o terminal **MSYS2 UCRT64**. Nesse caso, instale:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
    mingw-w64-ucrt-x86_64-make \
    mingw-w64-ucrt-x86_64-raylib \
    mingw-w64-ucrt-x86_64-pkgconf
```

Depois, entre na pasta do projeto e execute:

```bash
mingw32-make run
```

### Linux: Ubuntu ou Debian

Instale as dependências:

```bash
sudo apt update

sudo apt install build-essential git \
    libasound2-dev libx11-dev libxrandr-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev \
    libxcursor-dev libxinerama-dev
```

Baixe e instale a Raylib:

```bash
git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git

cd raylib/src

make PLATFORM=PLATFORM_DESKTOP

sudo make install
```

Depois, na pasta do projeto:

```bash
make run
```

Caso a Raylib não esteja instalada globalmente:

```bash
make run RAYLIB_SRC=$HOME/raylib/src
```

### macOS

```bash
brew install raylib
make run
```

---

## Comandos do Makefile

| Comando | Função |
|---|---|
| `make` | Compila o jogo |
| `make run` | Compila e executa |
| `make test` | Executa todos os testes |
| `make test-logica` | Testa a lógica e os padrões funcionais |
| `make test-mundo` | Testa as regras do jogo |
| `make clean` | Remove os arquivos compilados |

No Windows, substitua `make` por `mingw32-make`.

---

## Estrutura do projeto

```text
runner-logico/
├── Makefile
├── README.md
├── docs/
│   └── LOGICA.md
├── src/
│   ├── main.c
│   ├── mundo.c
│   ├── mundo.h
│   ├── logica.c
│   ├── logica.h
│   ├── funcional.c
│   ├── funcional.h
│   ├── render.c
│   ├── render.h
│   └── config.h
└── tests/
    ├── test_logica.c
    └── test_mundo.c
```

| Arquivo | Responsabilidade |
|---|---|
| `main.c` | Game loop, teclado e troca de telas |
| `mundo.c/.h` | Estado, física, colisões, spawn e portões |
| `logica.c/.h` | Fórmulas, avaliação e tabela-verdade |
| `funcional.c/.h` | `map`, `filter`, `fold` e contagem |
| `render.c/.h` | Cenário, entidades, HUD e telas |
| `config.h` | Constantes do jogo |
| `tests/` | Testes da lógica e das regras |
| `docs/LOGICA.md` | Explicação da lógica e roteiro de apresentação |

---

## Arquitetura

O projeto separa entrada, atualização e desenho:

```text
main.c
  │
  ├── lê o teclado
  │
  ├── mundo_atualiza(Mundo, Entrada, dt)
  │       ├── aplica as regras
  │       ├── avalia as fórmulas
  │       └── devolve um novo Mundo
  │
  └── render_mundo(&Mundo)
          └── apenas desenha
```

A atualização principal recebe o estado antigo por valor e devolve um novo estado:

```c
Mundo mundo_atualiza(Mundo m, Entrada in, float dt);
```

Com a mesma semente e as mesmas entradas, o jogo produz a mesma simulação.

---

## Programação funcional

O projeto implementa funções de alta ordem genéricas em `src/funcional.c`.

### Map

- `moveEntidade`: move cada entidade;
- `moveTiro`: move cada tiro;
- `tiroContraInimigos`: aplica colisões dos tiros;
- `jogadorContraEntidade`: aplica colisões do jogador.

### Filter

- `entidadeAtiva`: remove entidades destruídas ou fora da tela;
- `tiroAtivo`: remove tiros inativos;
- `sorteiaFormulaId`: seleciona fórmulas do nível atual.

### Fold

- `conta(... ehPortaoVivo ...)`: conta portões ativos;
- `contaModelos`: conta as valorações verdadeiras de uma fórmula.

Também são utilizadas funções puras, como:

```c
avalia
regra_energia
regra_velocidade_base
regra_conta_ativas
regra_pontos_inimigo
regra_nivel_formula
```

---

## Testes

Os testes são executados sem abrir a janela do jogo:

```bash
mingw32-make test
```

ou:

```bash
make test
```

Os testes verificam:

- conectivos e fórmulas;
- tautologias, contradições e equivalências;
- tabelas-verdade;
- `map`, `filter` e `fold`;
- pulo, gravidade e colisões;
- energia e sobrecarga;
- tiros, inimigos e power-ups;
- desafio lógico de 5 segundos;
- confirmação manual e automática;
- tautologia, vitória e derrota;
- robustez e determinismo.

A implementação atual passa nos testes de lógica e de mundo com **0 falhas**.

---

## Estado atual

### Implementado

- [x] Movimento, pulo, abaixar e queda rápida
- [x] Vidas, energia e sobrecarga
- [x] Escudo, arma e turbo
- [x] Inimigos, obstáculos e power-ups
- [x] Fórmulas e tabela-verdade
- [x] Portões com desafio de 5 segundos
- [x] Vitória, derrota, pontuação e recorde
- [x] Menu, pausa e tela final
- [x] Cenário em paralaxe e renovação visual
- [x] `map`, `filter`, `fold` e funções puras
- [x] Testes automatizados e determinismo

### Próximas melhorias

- [ ] Sons e música
- [ ] Fonte com `∧`, `∨`, `¬`, `→` e `↔`
- [ ] Partículas e efeitos adicionais
- [ ] Persistência do recorde em arquivo
- [ ] Fase de chefe com fórmulas maiores
- [ ] Publicação na itch.io

---

## Créditos

Projeto acadêmico desenvolvido para as disciplinas de **Programação Imperativa e Funcional** e **Lógica para Computação** da **CESAR School**.

Desenvolvido em **C com Raylib**, utilizando atualização de estado, funções puras, funções de alta ordem e testes automatizados.

Todos os elementos visuais atuais são desenhados diretamente com primitivas gráficas da Raylib.
