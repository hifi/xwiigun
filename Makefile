all: xwiigun-test libxwiigun.so

xwiigun-test: test.c xwiigun.c
	gcc -std=c99 -g3 -pedantic -Wall -o xwiigun-test test.c xwiigun.c -lxwiimote `sdl2-config --cflags --libs` -lSDL2_gfx -lm -lX11 -lXtst -lXext

libxwiigun.so: pcsxr/pad.c xwiigun.c
	gcc -shared -std=c99 -g3 -pedantic -Wall -I. -o libxwiigun.so pcsxr/pad.c xwiigun.c -lxwiimote -lm

clean:
	rm -f xwiigun-test libxwiigun.so
