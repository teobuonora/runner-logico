# Runner Lógico — Makefile (Linux, macOS e Windows/MSYS2)
#
#   make          compila o jogo
#   make run      compila e executa
#   make test     roda os testes (lógica pura + regras do mundo)
#   make clean    apaga os binários
#
# Se o raylib estiver numa pasta própria (ex.: instalador do Windows):
#   make RAYLIB_SRC=C:/raylib/raylib/src

CC      ?= gcc
CFLAGS  += -std=c11 -Wall -Wextra -O2 -Isrc
TARGET   = runner-logico
EXE      =

SRC_LOGICA = src/logica.c src/funcional.c
SRC_JOGO   = src/main.c src/render.c src/mundo.c $(SRC_LOGICA)
SRC_MUNDO  = src/mundo.c $(SRC_LOGICA)

ifdef RAYLIB_SRC
  CFLAGS += -I$(RAYLIB_SRC)
  LDFLAGS += -L$(RAYLIB_SRC)
endif

PKG_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
PKG_LIBS   := $(shell pkg-config --libs raylib 2>/dev/null)
CFLAGS += $(PKG_CFLAGS)

ifeq ($(OS),Windows_NT)
  EXE = .exe
  LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -lm
else
  UNAME := $(shell uname -s)
  ifeq ($(UNAME),Darwin)
    LDLIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lm
  else
    LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
  endif
endif
ifneq ($(PKG_LIBS),)
  LDLIBS = $(PKG_LIBS) -lm
endif

all: $(TARGET)$(EXE)

$(TARGET)$(EXE): $(SRC_JOGO) src/*.h
	$(CC) $(CFLAGS) $(SRC_JOGO) -o $@ $(LDFLAGS) $(LDLIBS)

run: all
	./$(TARGET)$(EXE)

test-logica: tests/test_logica.c $(SRC_LOGICA) src/*.h
	$(CC) $(CFLAGS) tests/test_logica.c $(SRC_LOGICA) -o tests/test_logica$(EXE) -lm
	./tests/test_logica$(EXE)

test-mundo: tests/test_mundo.c $(SRC_MUNDO) src/*.h
	$(CC) $(CFLAGS) tests/test_mundo.c $(SRC_MUNDO) -o tests/test_mundo$(EXE) $(LDFLAGS) $(LDLIBS)
	./tests/test_mundo$(EXE)

test: test-logica test-mundo

clean:
	rm -f $(TARGET) $(TARGET).exe tests/test_logica tests/test_logica.exe tests/test_mundo tests/test_mundo.exe

.PHONY: all run test test-logica test-mundo clean
