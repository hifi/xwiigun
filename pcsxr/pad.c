#include "pad.h"
#include "xwiigun.h"

#include <string.h>
#include <stdio.h>

// accurate GunCon data format from http://problemkaputt.de/psx-spx.htm#controllerslightgunsnamcoguncon

static struct xwiigun gun;

char *PSEgetLibName(void)
{
    return "xwiigun G-Con 45 Input";
}

uint32_t PSEgetLibType(void)
{
    return PSE_LT_PAD;
}

uint32_t PSEgetLibVersion(void)
{
    return 1;
}

long PADinit(long flags)
{
    printf("PADinit(flags=%08X)\n", (unsigned int)flags);
    return PSE_ERR_SUCCESS;
}

long PADshutdown(void)
{
    printf("PADshutdown()\n");
    return PSE_ERR_SUCCESS;
}

long PADopen(unsigned long *Disp)
{
    printf("PADopen(Disp=%p)\n", (void *)Disp);

    if (xwiigun_open(&gun)) {
        printf("No gun :(\n");
        return PSE_PAD_ERR;
    }

    return PSE_ERR_SUCCESS;
}

long PADclose(void)
{
    printf("PADclose()\n");

    xwiigun_close(&gun);

    return PSE_ERR_SUCCESS;
}

long PADquery(void)
{
    printf("PADquery()\n");
    return PSE_PAD_USE_PORT1;
}

static union {
    uint8_t u8[8];
    uint16_t u16[4];
} data;

static uint8_t data_pos;

unsigned char PADstartPoll(int pad)
{
    data_pos = 0;
    memset(data.u8, 0, sizeof(data));

    xwiigun_poll(&gun);

    return -1;
}

unsigned char PADpoll(unsigned char cmd)
{
    static bool off_button = false;
    static bool off_enabled = false;

    if (data_pos == 0) {
        switch (cmd) {
            case CMD_CONFIG_MODE:
                printf("CMD_CONFIG_MODE\n");
                return 0;

            case CMD_SET_MODE_AND_LOCK:
                printf("CMD_SET_MODE_AND_LOCK\n");
                return 0;

            case CMD_QUERY_MODEL_AND_MODE:
                printf("CMD_QUERY_MODEL_AND_MODE\n");
                return 0;

            case CMD_QUERY_ACT:
                printf("CMD_QUERY_ACT\n");
                return 0;

            case CMD_QUERY_COMB:
                printf("CMD_QUERY_COMB\n");
                return 0;

            case CMD_QUERY_MODE:
                printf("CMD_QUERY_MODE\n");
                return 0;

            case CMD_VIBRATION_TOGGLE:
                printf("CMD_VIBRATION_TOGGLE\n");
                return 0;

            case CMD_READ_DATA_AND_VIBRATE:
                data.u16[0] = 0x5A63; // GunCon id
                data.u16[1] = 0xFFFF; // button bits, all high for released

                if (gun.keys[XWII_KEY_B])
                    data.u16[1] &= ~(1 << 13); // A/O

                if (gun.keys[XWII_KEY_A])
                    data.u16[1] &= ~(1 << 14); // B/X

                if (gun.keys[XWII_KEY_ONE] && !off_button) {
                    off_enabled = !off_enabled;
                    printf("toggling offscreen hack: %s\n", off_enabled ? "yes" : "no");
                    off_button = true;
                } else if (!gun.keys[XWII_KEY_ONE] && off_button) {
                    off_button = false;
                }

                if (gun.keys[XWII_KEY_TWO])
                    data.u16[1] &= ~(1 << 3); // Start

                if (off_enabled && !gun.offscreen)
                    data.u16[1] &= ~(1 << 3); // Start

                if (gun.offscreen || !gun.calibrated) {
                    data.u16[2] = 0x1; //
                    data.u16[3] = 0xA; // no light
                } else {
                    data.u16[2] = 0x4 + ((0x1CD - 0x4) * gun.hpos);
                    data.u16[3] = 0x20 + ((0x127 - 0x20) * gun.vpos);
                    //data.u16[3] = 0x19 + ((0xF8 - 0x19) * gun.vpos);
                }
                break;

            default:
                printf("Unknown PADpoll command: 0x%02X\n", cmd);
                return 0;
        }
    }

    if (data_pos >= sizeof(data))
        return 0;

    return data.u8[data_pos++];
}

long PADreadPort1(PadDataS *pad)
{
    printf("PADreadPort1(pad=%p)\n", (void *)pad);
    return PSE_ERR_SUCCESS;
}

long PADreadPort2(PadDataS *pad)
{
    printf("PADreadPort2(pad=%p)\n", (void *)pad);
    return PSE_ERR_SUCCESS;
}

long PADconfigure(void)
{
    printf("PADconfigure()\n");
    return 0;
}

void PADabout(void)
{
    printf("PADabout()\n");
}

long PADtest(void)
{
    printf("PADtest()\n");
    return 0;
}
