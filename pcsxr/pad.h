#include <stdint.h>

/*         PAD PlugIn          */

#define PSE_LT_PAD					8

// DLL function return codes
#define PSE_ERR_SUCCESS				0	// every function in DLL if completed sucessfully should return this value
#define PSE_ERR_FATAL				-1	// undefined error but fatal one, that kills all functionality

/*

  functions that must be exported from PAD Plugin

  long	PADinit(long flags);	// called only once when PSEmu Starts
  void	PADshutdown(void);		// called when PSEmu exits
  long	PADopen(PadInitS *);	// called when PSEmu is running program
  long	PADclose(void);
  long	PADconfigure(void);
  void  PADabout(void);
  long  PADtest(void);			// called from Configure Dialog and after PADopen();
  long	PADquery(void);

  unsigned char PADstartPoll(int);
  unsigned char PADpoll(unsigned char);

*/

// PADquery responses (notice - values ORed)
// PSEmu will use them also in PADinit to tell Plugin which Ports will use
// notice that PSEmu will call PADinit and PADopen only once when they are from
// same plugin

// might be used in port 1
#define PSE_PAD_USE_PORT1			1
// might be used in port 2
#define PSE_PAD_USE_PORT2			2


// MOUSE SCPH-1030
#define PSE_PAD_TYPE_MOUSE			1
// NEGCON - 16 button analog controller SLPH-00001
#define PSE_PAD_TYPE_NEGCON			2
// GUN CONTROLLER - gun controller SLPH-00014 from Konami
#define PSE_PAD_TYPE_GUN			3
// STANDARD PAD SCPH-1080, SCPH-1150
#define PSE_PAD_TYPE_STANDARD		4
// ANALOG JOYSTICK SCPH-1110
#define PSE_PAD_TYPE_ANALOGJOY		5
// GUNCON - gun controller SLPH-00034 from Namco
#define PSE_PAD_TYPE_GUNCON			6
// ANALOG CONTROLLER SCPH-1150
#define PSE_PAD_TYPE_ANALOGPAD		7


// sucess, everything configured, and went OK.
#define PSE_PAD_ERR_SUCCESS			0
// general plugin failure (undefined error)
#define PSE_PAD_ERR_FAILURE			-1


// ERRORS
// this error might be returned as critical error but none of below
#define PSE_PAD_ERR					-80
// this driver is not configured
#define PSE_PAD_ERR_NOTCONFIGURED	PSE_PAD_ERR - 1
// this driver failed Init
#define PSE_PAD_ERR_INIT			PSE_PAD_ERR - 2


// WARNINGS
// this warning might be returned as undefined warning but allowing driver to continue
#define PSE_PAD_WARN				80

typedef struct
{
    // controler type - fill it withe predefined values above
    unsigned char controllerType;

    // status of buttons - every controller fills this field
    unsigned short buttonStatus;

    // for analog pad fill those next 4 bytes
    // values are analog in range 0-255 where 127 is center position
    unsigned char rightJoyX, rightJoyY, leftJoyX, leftJoyY;

    // for mouse fill those next 2 bytes
    // values are in range -128 - 127
    unsigned char moveX, moveY;

    unsigned char reserved[91];
} PadDataS;

enum {
    CMD_READ_DATA_AND_VIBRATE = 0x42,
    CMD_CONFIG_MODE = 0x43,
    CMD_SET_MODE_AND_LOCK = 0x44,
    CMD_QUERY_MODEL_AND_MODE = 0x45,
    CMD_QUERY_ACT = 0x46, // ??
    CMD_QUERY_COMB = 0x47, // ??
    CMD_QUERY_MODE = 0x4C, // QUERY_MODE ??
    CMD_VIBRATION_TOGGLE = 0x4D
};

enum {
    GUNCON_RELOAD,
    GUNCON_TRIGGER,
    GUNCON_A,
    GUNCON_B
};

char *PSEgetLibName(void);
uint32_t PSEgetLibType(void);
uint32_t PSEgetLibVersion(void);
long PADinit(long flags);
long PADshutdown(void);
long PADopen(unsigned long *Disp);
long PADclose(void);
long PADquery(void);
unsigned char PADstartPoll(int pad);
unsigned char PADpoll(unsigned char value);
long PADreadPort1(PadDataS *pad);
long PADreadPort2(PadDataS *pad);
long PADkeypressed(void);
long PADconfigure(void);
void PADabout(void);
long PADtest(void);
