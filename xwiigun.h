#include <stdint.h>
#include <time.h>
#include <xwiimote.h>
#include <poll.h>

enum {
    SIDE_INVALID   = -1,
    SIDE_TOP       = 0,
    SIDE_RIGHT     = 1,
    SIDE_BOTTOM    = 2,
    SIDE_LEFT      = 3,
};

struct xwiigun
{
    // if polling should block or not, defaults to false
    bool blocking;

    // rumble when trigger is pressed
    bool trigger_rumble;

    // relative cursor position [0.0f - 1.0f]
    double hpos, vpos;

    // calculated screen aspect ratio
    double ar;

    // if gun is pointed outside the screen
    bool offscreen;

    // pressed down key data
    uint8_t keys[XWII_KEY_NUM];

    // time when calibration was last successful
    time_t calibrated;

    // adjustable IR camera center
    struct xwii_event_abs center;

    // IR state
    struct {
        // previous visible IR points for tracking
        struct xwii_event_abs prev[4];

        // current visible IR points
        struct xwii_event_abs now[4];

        // calibration "diamond", updated when all IR points are visible
        struct xwii_event_abs cal[4];

        // adjusted IR points from now and cal
        struct xwii_event_abs adj[4];
    } ir;

    // xwiimote state
    struct {
        struct pollfd fds[2];
        struct xwii_monitor *monitor;
        struct xwii_iface *iface;
    } xwii;
};

int xwiigun_open(struct xwiigun *gun);
int xwiigun_poll(struct xwiigun *gun);
int xwiigun_close(struct xwiigun *gun);
