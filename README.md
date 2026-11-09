# Runner Lógico

Jogo 2D estilo "dinossauro do Chrome" feito em **C + Raylib**, onde a **Lógica Proposicional**
faz parte da mecânica: você corre, pula, atira em inimigos, pega power-ups e precisa
**tornar verdadeiras as fórmulas dos portões** para passar sem perder vida.

> Projeto integrador de *Programação Imperativa e Funcional* (CESAR School) e trabalho da AV2
> de *Lógica Proposicional*.

## Como jogar

Você controla um dinossauro que corre sozinho. Três **proposições** controlam seus poderes:

| Tecla | Proposição | Quando é **V** (verdadeira)            |
|:-----:|:----------:|----------------------------------------|
| `1`   | **P**      | **Escudo**: destrói o que encostar em você |
| `2`   | **Q**      | **Arma**: você consegue atirar          |
| `3`   | **R**      | **Turbo**: +35% de velocidade e pontos x2 |

Cada proposição verdadeira **gasta energia**. Se a energia acaba, ocorre uma **sobrecarga**:
tudo volta a ser falso e você fica 1,5 s sem poder mexer nas proposições.

De tempos em tempos aparece um **portão lógico** com uma fórmula, por exemplo `P & ~Q`.
Quando você cruza o portão, a fórmula é avaliada com os valores atuais de P, Q e R:

* **Verdadeira** → o portão abre (+100 pontos).
* **Falsa** → você perde 1 vida.

Abra **8 portões** para vencer. Você perde se as vidas (3) acabarem.

### Controles

| Ação | Teclas |
|------|--------|
| Pular | `Espaço`, `↑` ou `W` |
| Abaixar (no ar: cair rápido) | `↓` ou `S` |
| Atirar (só com **Q = V**) | `X`, `Z` ou `Ctrl` |
| Alternar P, Q, R | `1`, `2`, `3` |
| Mostrar/esconder tabela-verdade do portão | `T` |
| Pausar | `P` |
| Menu | `Esc` |

### Símbolos das fórmulas

| Símbolo | Significa |
|:-------:|-----------|
| `~A`    | não A (negação) |
| `A & B` | A e B (conjunção) |
| `A \| B` | A ou B (disjunção) |
| `A -> B`| se A então B (condicional) |
| `A <-> B`| A se e somente se B (bicondicional) |

### Inimigos e power-ups

* **Cacto**: fixo. Pule por cima (ou use o escudo).
* **Andarilho** (roxo): tem 2 de vida. Atire duas vezes. Vale 30 pontos.
* **Voador** (laranja): vem baixo ou alto. Abaixe, pule ou atire. Vale 40 pontos.
* **E** amarelo: +50 de energia. **+** vermelho: +1 vida.
* **T** roxo (**Tautologia**): o próximo portão abre sempre, não importa a fórmula.

---

## Como rodar

Você precisa de um compilador C e da biblioteca **raylib** (versão 5.x).

### Linux (Ubuntu/Debian)

```bash
# 1) dependências
sudo apt update
sudo apt install build-essential git libasound2-dev libx11-dev libxrandr-dev \
     libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev

# 2) baixar e compilar a raylib (uma vez só)
git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install          # instala em /usr/local
cd ../..

# 3) compilar e rodar o jogo
git clone <URL-DO-SEU-REPOSITORIO>
cd runner-logico
make run
```

Se não quiser usar `sudo make install`, deixe a raylib numa pasta e aponte para ela:

```bash
make run RAYLIB_SRC=$HOME/raylib/src
```

### macOS

```bash
brew install raylib
make run
```

### Windows (recomendado: MSYS2)

1. Instale o [MSYS2](https://www.msys2.org/) e abra o terminal **MSYS2 UCRT64**.
2. Instale as ferramentas:
   ```bash
   pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make \
         mingw-w64-ucrt-x86_64-raylib mingw-w64-ucrt-x86_64-pkgconf
   ```
3. Entre na pasta do projeto (o disco `C:` vira `/c`) e rode:
   ```bash
   cd /c/Users/SEU_USUARIO/caminho/runner-logico
   mingw32-make run
   ```

Alternativa no Windows 11: usar o **WSL2** com Ubuntu e seguir os passos de Linux.

### Comandos do Makefile

| Comando | O que faz |
|---------|-----------|
| `make` | compila o jogo (`runner-logico`) |
| `make run` | compila e abre o jogo |
| `make test` | roda os testes (lógica pura + regras do jogo) |
| `make clean` | apaga os binários |

---

## Estrutura do projeto

```
runner-logico/
├── Makefile
├── README.md
├── docs/LOGICA.md        <- explicação da Lógica Proposicional no jogo + roteiro de apresentação
├── src/
│   ├── main.c            <- GAME LOOP (imperativo): entrada -> atualizar -> desenhar
│   ├── mundo.c / .h      <- estado do jogo + regras (sem Raylib de desenho, sem teclado)
│   ├── logica.c / .h     <- Lógica Proposicional pura: fórmulas, avaliação, tabela-verdade
│   ├── funcional.c / .h  <- mapa / filtra / dobra (map / filter / fold) em C
│   ├── render.c / .h     <- só desenha; nunca altera o estado
│   └── config.h          <- constantes (velocidade, gravidade, custo de energia...)
└── tests/
    ├── test_logica.c     <- testes da lógica e do map/filter/fold (sem janela)
    └── test_mundo.c      <- testes das regras do jogo (sem janela)
```

### Arquitetura (da Aula 19)

```
 main.c  (game loop imperativo)
   │  entrada ──► mundo_atualiza(Mundo, Entrada, dt) ──► novo Mundo ──► render_mundo(&Mundo)
   │                      │
   │                      └── usa logica.c (avalia fórmulas) e funcional.c (mapa/filtra/dobra)
```

`mundo_atualiza` recebe o estado antigo **por valor** e **devolve um novo estado**. Mesma semente
aleatória + mesmas entradas = exatamente o mesmo jogo (os testes conferem isso).

### Onde estão os padrões funcionais (map / filter / fold)

| Padrão | Onde | Para quê |
|--------|------|----------|
| **map** | `moveEntidade`, `moveTiro` (`mundo.c`) | move cada inimigo e cada tiro |
| **map** | `tiroContraInimigos`, `jogadorContraEntidade`, `avaliaPortao` | aplica as regras de colisão e dos portões a cada objeto |
| **filter** | `entidadeAtiva`, `tiroAtivo` | remove mortos e o que saiu da tela |
| **filter** | `sorteiaFormulaId` | escolhe só as fórmulas do nível atual |
| **fold** | `conta(... ehPortaoVivo ...)` | conta os portões na tela (controla o spawn) |
| **fold** | `contaModelos` (`logica.c`) | conta as linhas verdadeiras da tabela-verdade |
| **funções puras** | `avalia`, `regra_energia`, `regra_velocidade_base`, `regra_pontos_inimigo`... | regras testáveis sem abrir a janela |

`mapa`, `filtra`, `dobra` e `conta` (em `funcional.c`) funcionam com qualquer vetor de structs,
recebendo um **ponteiro de função**.

### Requisitos da Aula 19

| Requisito | Atendido por |
|-----------|--------------|
| Janela e game loop a 60 FPS com `GetFrameTime()` | `main.c` (`SetTargetFPS(60)`, `dt` repassado a todo movimento) |
| Controle do jogador | pulo, abaixar, atirar, alternar P/Q/R |
| Coleção de objetos (vetor) | `ents[]` e `tiros[]` em `Mundo` |
| Colisões com funções da Raylib | `CheckCollisionRecs` em `mundo.c` |
| Pontuação, vidas, telas de início/fim | HUD, menu, pausa, tela de fim com recorde |
| Vitória / derrota | abrir 8 portões / perder as 3 vidas |
| Separação update/render | `mundo.c` não desenha; `render.c` não altera estado |
| Robustez | vetor cheio, lista vazia, objetos fora da tela, quadro muito longo |
| Makefile + README + commits | este repositório |

---

## Próximos passos (visual e conteúdo)

- [ ] Sprites (Kenney / OpenGameArt) e animações — lembre de creditar as fontes
- [ ] Sons (pulo, tiro, portão abrindo/errando) — o `Mundo` já devolve `eventos` por quadro
- [ ] Fonte com os símbolos `∧ ∨ ¬ → ↔` (hoje usamos `& | ~ -> <->` por limitação da fonte padrão)
- [ ] Fase de chefe com fórmulas maiores
- [ ] Salvar o recorde em arquivo
- [ ] Publicar na itch.io
