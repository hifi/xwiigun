#include "xwiigun.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <unistd.h>
#include <linux/uinput.h>

#include <signal.h>
#include <time.h>

static int fd;
static struct xwiigun gun;

/* Signal Handler for SIGINT */
void sigint_handler(int sig_num)
{
    printf("\n User provided signal handler for Ctrl+C \n");

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);

    xwiigun_close(&gun);

    exit(0);
}

void emit(int fd, int type, int code, int val)
{
    struct input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = val;
    /* timestamp values below are ignored */
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;

    write(fd, &ie, sizeof(ie));
}

int main(int argc, char **argv)
{
    int ret = 1;
    struct uinput_user_dev dev;
    int width = 65535,
        height = 65535;

    if (argc > 1 && argc < 3) {
        fprintf(stderr, "usage: %s [width height]\n", argv[0]);
        return 1;
    } else if (argc == 3) {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }

    if ((fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) < 0) {
        perror("open");
        return 1;
    }

    printf("xwiigun-mouse: Emulating an absolute touch device %dx%d\n", width, height);

    signal(SIGINT, sigint_handler);

    /* enable mouse button left and relative events */
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);

    ioctl(fd, UI_SET_EVBIT, EV_ABS);
    ioctl(fd, UI_SET_ABSBIT, ABS_X);
    ioctl(fd, UI_SET_ABSBIT, ABS_Y);

    memset(&dev, 0, sizeof(dev));
    dev.id.bustype = BUS_USB;
    dev.id.vendor = 0x1234; /* sample vendor */
    dev.id.product = 0x5678; /* sample product */
    strcpy(dev.name, "xwiigun");

    dev.absmax[ABS_X] = width;
    dev.absmax[ABS_Y] = height;
    dev.absmin[ABS_X] = 0;
    dev.absmin[ABS_Y] = 0;
    write(fd, &dev, sizeof(dev));

    ioctl(fd, UI_DEV_CREATE);

    /*
     * On UI_DEV_CREATE the kernel will create the device node for this
     * device. We are inserting a pause here so that userspace has time
     * to detect, initialize the new device, and can start listening to
     * the event, otherwise it will not notice the event we are about
     * to send. This pause is only needed in our example code!
     */
    sleep(1);

    if (xwiigun_open(&gun)) {
        fprintf(stderr, "xwiigun-mouse: xwiigun_open() failed :(\n");
        goto error;
    }

    // we don't have our own event loop
    gun.blocking = true;

    uint8_t buttons[2] = { 0, 0 };

    printf("xwiigun-mouse: Running\n");

    while (!xwiigun_poll(&gun))
    {
        if (gun.hpos < 0) gun.hpos = 0;
        if (gun.hpos > 1) gun.hpos = 1;
        if (gun.vpos < 0) gun.vpos = 0;
        if (gun.vpos > 1) gun.vpos = 1;

#if 0
        printf("xwiigun-mouse: %d x %d\n", (int)(gun.hpos * width), (int)(gun.vpos * height));
#endif

        emit(fd, EV_ABS, ABS_X, gun.hpos * width);
        emit(fd, EV_ABS, ABS_Y, gun.vpos * height);

        if (gun.keys[XWII_KEY_A] != buttons[0]) {
            buttons[0] = !buttons[0];
            emit(fd, EV_KEY, BTN_RIGHT, buttons[0]);
        }

        if (gun.keys[XWII_KEY_B] != buttons[1]) {
            buttons[1] = !buttons[1];
            emit(fd, EV_KEY, BTN_LEFT, buttons[1]);
        }

        emit(fd, EV_SYN, SYN_REPORT, 0);
    }

    printf("xwiigun-mouse: Not Running\n");

    ret = 0;

    /*
    * Give userspace some time to read the events before we destroy the
    * device with UI_DEV_DESTOY.
    */
    sleep(1);

error:

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);

    xwiigun_close(&gun);

    return ret;
}
