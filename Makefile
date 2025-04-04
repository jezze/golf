BIN:=golf
OBJ:=ball.o camera.o game.o gfx_sdl.o main.o map.o

CFLAGS=-O2 -std=c89 -Wall -pedantic `pkg-config --cflags sdl2`
LIBS=-lm `pkg-config --libs sdl2` -lSDL2_image

all: ${BIN}

%.o: %.c
	gcc -c -o $@ ${CFLAGS} $<

${BIN}: ${OBJ}
	gcc -o $@ $^ ${LIBS}

clean:
	rm -f ${BIN} ${OBJ}
