#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <errno.h>

#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XTest.h>

#include "xwiigun.h"

static struct xwiigun gun;

int main(int argc, char **argv)
{
    int ret = 1;
    Display *dpy = NULL;
    int monitor = 0, nmonitors = 0;
    XRRMonitorInfo *monitors = NULL;

    if (argc > 1) {
        monitor = atoi(argv[1]);
        printf("Using monitor %d\n", monitor);
    }

    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Failed to open X11 display :(\n");
        goto error;
    }

    monitors = XRRGetMonitors(dpy, DefaultRootWindow(dpy), 1, &nmonitors);
    if (monitors == NULL || nmonitors == 0) {
        fprintf(stderr, "No monitors :(\n");
        goto error;
    }

    printf("Number of monitors: %d\n", nmonitors);
    for (int i = 0; i < nmonitors; i++) {
        printf("  %d: %dx%d at %dx%d\n", i, monitors[i].width, monitors[i].height, monitors[i].x, monitors[i].y);
    }

    if (monitor > nmonitors - 1) {
        fprintf(stderr, "Monitor %d does not exist :(\n", monitor);
        goto error;
    }

    if (xwiigun_open(&gun)) {
        fprintf(stderr, "xwiigun open failed :(\n");
        goto error;
    }

    // we don't have our own event loop
    gun.blocking = true;

    uint8_t buttons[2] = { 0, 0 };

    while (!xwiigun_poll(&gun))
    {
        XTestFakeMotionEvent(
            dpy,
            0,
            monitors[monitor].x + (monitors[monitor].width * gun.hpos),
            monitors[monitor].y + (monitors[monitor].height * gun.vpos),
            CurrentTime);


        if (gun.keys[XWII_KEY_A] != buttons[0]) {
            buttons[0] = !buttons[0];
            XTestFakeButtonEvent(dpy, 3, buttons[0], CurrentTime);
        }

        if (gun.keys[XWII_KEY_B] != buttons[1]) {
            buttons[1] = !buttons[1];
            XTestFakeButtonEvent(dpy, 1, buttons[1], CurrentTime);
        }

        XSync(dpy, 0);
    }

    ret = 0;
error:
    xwiigun_close(&gun);

    if (monitors)
        XRRFreeMonitors(monitors);

    if (dpy)
        XCloseDisplay(dpy);

    return ret;
}
