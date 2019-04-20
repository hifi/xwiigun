#include <string.h>
#include <stdio.h>
#include <poll.h>
#include <errno.h>

#include "xwiigun.h"

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#define WIN_WIDTH 1280
#define WIN_HEIGHT 1024

static struct xwiigun gun;
static SDL_Window *win;
static SDL_Renderer *renderer;
static SDL_Surface *screen;

static void draw_box(SDL_Surface *s, struct xwii_event_abs *p, const char *name, bool adjust, int r, int g, int b, int a)
{
    SDL_Rect box;
    box.w = 4;
    box.h = 4;
    box.x = p->x - 2;
    box.y = p->y - 2;

    if (!xwii_event_ir_is_valid(p))
        return;

    if (adjust) {
        box.x += (WIN_WIDTH - 1024) / 2;
        box.y += (WIN_HEIGHT - 768) / 2;
    }

    SDL_FillRect(screen, &box, SDL_MapRGBA(screen->format, r, g, b, a));

    if (name != NULL)
        stringRGBA(renderer, box.x - (8*(strlen(name) / 2)), box.y + 10, name, r, g, b, a);
}

static void draw_ir(struct xwii_event_abs ir[4], int r, int g, int b, int a)
{
    SDL_Surface *screen = SDL_GetWindowSurface(win);

    draw_box(screen, &ir[SIDE_TOP], "top", true, r, g, b, a);
    draw_box(screen, &ir[SIDE_RIGHT], "right", true, r, g, b, a);
    draw_box(screen, &ir[SIDE_BOTTOM], "bottom", true, r, g, b, a);
    draw_box(screen, &ir[SIDE_LEFT], "left", true, r, g, b, a);

    int x = (WIN_WIDTH - 1024) / 2;
    int y = (WIN_HEIGHT - 768) / 2;

    if (xwii_event_ir_is_valid(&ir[SIDE_TOP]) && xwii_event_ir_is_valid(&ir[SIDE_BOTTOM]))
        lineRGBA(renderer, ir[SIDE_TOP].x + x, ir[SIDE_TOP].y + y, ir[SIDE_BOTTOM].x + x, ir[SIDE_BOTTOM].y + y, r, g, b, a);

    if (xwii_event_ir_is_valid(&ir[SIDE_LEFT]) && xwii_event_ir_is_valid(&ir[SIDE_RIGHT]))
        lineRGBA(renderer, ir[SIDE_LEFT].x + x, ir[SIDE_LEFT].y + y, ir[SIDE_RIGHT].x + x, ir[SIDE_RIGHT].y + y, r, g, b, a);

    if (xwii_event_ir_is_valid(&ir[SIDE_TOP]) && xwii_event_ir_is_valid(&ir[SIDE_RIGHT]))
        lineRGBA(renderer, ir[SIDE_TOP].x + x, ir[SIDE_TOP].y + y, ir[SIDE_RIGHT].x + x, ir[SIDE_RIGHT].y + y, r, g, b, a);

    if (xwii_event_ir_is_valid(&ir[SIDE_RIGHT]) && xwii_event_ir_is_valid(&ir[SIDE_BOTTOM]))
        lineRGBA(renderer, ir[SIDE_RIGHT].x + x, ir[SIDE_RIGHT].y + y, ir[SIDE_BOTTOM].x + x, ir[SIDE_BOTTOM].y + y, r, g, b, a);

    if (xwii_event_ir_is_valid(&ir[SIDE_LEFT]) && xwii_event_ir_is_valid(&ir[SIDE_BOTTOM]))
        lineRGBA(renderer, ir[SIDE_LEFT].x + x, ir[SIDE_LEFT].y + y, ir[SIDE_BOTTOM].x + x, ir[SIDE_BOTTOM].y + y, r, g, b, a);

    if (xwii_event_ir_is_valid(&ir[SIDE_TOP]) && xwii_event_ir_is_valid(&ir[SIDE_LEFT]))
        lineRGBA(renderer, ir[SIDE_TOP].x + x, ir[SIDE_TOP].y + y, ir[SIDE_LEFT].x + x, ir[SIDE_LEFT].y + y, r, g, b, a);
}

static void render()
{
    SDL_Surface *screen = SDL_GetWindowSurface(win);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 255));

    // background for visible IR area
    SDL_Rect b;
    b.w = 1024;
    b.h = 768;
    b.x = (WIN_WIDTH - 1024) / 2;
    b.y = (WIN_HEIGHT - 768) / 2;
    SDL_FillRect(screen, &b, SDL_MapRGB(screen->format, 80, 80, 255));

    // draw grid on visible IR area
    lineRGBA(renderer, 0, WIN_HEIGHT / 2, WIN_WIDTH, WIN_HEIGHT / 2, 255, 255, 255, 80);
    lineRGBA(renderer, WIN_WIDTH / 2, 0, WIN_WIDTH / 2, WIN_HEIGHT, 255, 255, 255, 80);

    draw_ir(gun.ir.now, 255, 255, 255, 255);

    if (gun.calibrated)
        draw_ir(gun.ir.adj, 255, 80, 80, 128);

    SDL_Rect center;
    center.w = 3;
    center.h = 3;
    center.x = WIN_WIDTH / 2 - 1;
    center.y = WIN_HEIGHT / 2 - 1;
    SDL_FillRect(screen, &center, SDL_MapRGB(screen->format, 255, 0, 0));


    char calibrated_str[255] = "CALIBRATION DATA MISSING - POINT AT THE CENTER OF THE SCREEN";

    if (gun.calibrated) {
        struct xwii_event_abs c;

        c.x = WIN_WIDTH * gun.hpos;
        c.y = WIN_HEIGHT * gun.vpos;

        draw_box(screen, &c, "cursor", false, 255, 255, 255, 255);

        char calibrated_time_str[255];
        struct tm *tm;
        tm = localtime(&gun.calibrated);
        strftime(calibrated_time_str, sizeof(calibrated_time_str), "%F %H:%M:%S", tm);
        snprintf(calibrated_str, sizeof(calibrated_str), "Last calibration: %s", gun.calibrated ? calibrated_time_str : "N/A");
        stringRGBA(renderer, 0, WIN_HEIGHT - 8, calibrated_str, 255, 255, 255, 255);

        char cursor_str[255];
        snprintf(cursor_str, sizeof(cursor_str), "[ %1.4f %1.4f ]", gun.hpos, gun.vpos);
        stringRGBA(renderer, WIN_WIDTH - (strlen(cursor_str) * 8), WIN_HEIGHT - 8, cursor_str, 255, 255, 255, 255);
    } else {
        stringRGBA(renderer, 0, WIN_HEIGHT - 8, calibrated_str, 255, 0, 0, 255);
    }

    SDL_UpdateWindowSurface(win);
}

int main(int argc, char **argv)
{
    struct pollfd fds[1];

    memset(&fds, 0, sizeof(fds));

    if (xwiigun_open(&gun)) {
        printf("No gun :(\n");
        return 1;
    }

    fds[0].fd = xwii_iface_get_fd(gun.xwii.iface);
    fds[0].events = POLLIN;

    SDL_Init(SDL_INIT_VIDEO);

    win = SDL_CreateWindow("xwiigun calibration test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
    screen = SDL_GetWindowSurface(win);
    renderer = SDL_CreateSoftwareRenderer(SDL_GetWindowSurface(win));

    SDL_SetWindowBordered(win, false);

    while (!xwiigun_poll(&gun)) {
        int ret;

        //printf("cal: %s, off: %s, hpos: %1.3f, vpos: %1.3f, ar: %1.3f\n", gun.calibrated ? "ok" : "??", gun.offscreen ? "yes" : "no ", gun.hpos, gun.vpos, gun.ar);
        render(&gun);

        if ((ret = poll(fds, 1, -1)) < 0) {
            if (ret == -EAGAIN)
                return 0;

            perror("poll() failed");
            return 1;
        }
    }

    xwiigun_close(&gun);

    return 0;
}
