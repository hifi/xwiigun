all: xwiigun-mouse xwiigun-test libxwiigun.so

xwiigun-mouse: mouse.c xwiigun.c
	gcc -std=c99 -g3 -pedantic -Wall -o xwiigun-mouse mouse.c xwiigun.c -lxwiimote -lm -lX11 -lXtst -lXext -lXrandr

xwiigun-test: test.c xwiigun.c
	gcc -std=c99 -g3 -pedantic -Wall -o xwiigun-test test.c xwiigun.c -lxwiimote `sdl2-config --cflags --libs` -lSDL2_gfx -lm

libxwiigun.so: pcsxr/pad.c xwiigun.c
	gcc -fPIC -shared -std=c99 -g3 -pedantic -Wall -I. -o libxwiigun.so pcsxr/pad.c xwiigun.c -lxwiimote -lm

clean:
	rm -f xwiigun-test xwiigun-mouse libxwiigun.so
