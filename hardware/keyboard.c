/****************************************************************************
    keyboard.c

    Variable types are rather confusing here.
    They alternate between char, int and enum KEYCODE.
    To be fixed later.
****************************************************************************/

#include <ctype.h>
#include <_string.h>
#include <keyboard.h>
#include "..\hal\hal.h"


// keyboard encoder ------------------------------------------

enum KYBRD_ENCODER_IO
{
    KYBRD_ENC_INPUT_BUF = 0x60,
    KYBRD_ENC_CMD_REG   = 0x60
};

enum KYBRD_ENC_CMDS
{
    KYBRD_ENC_CMD_SET_LED               =    0xED,
    KYBRD_ENC_CMD_ECHO                  =    0xEE,
    KYBRD_ENC_CMD_SCAN_CODE_SET         =    0xF0,
    KYBRD_ENC_CMD_ID                    =    0xF2,
    KYBRD_ENC_CMD_AUTODELAY             =    0xF3,
    KYBRD_ENC_CMD_ENABLE                =    0xF4,
    KYBRD_ENC_CMD_RESETWAIT             =    0xF5,
    KYBRD_ENC_CMD_RESETSCAN             =    0xF6,
    KYBRD_ENC_CMD_ALL_AUTO              =    0xF7,
    KYBRD_ENC_CMD_ALL_MAKEBREAK         =    0xF8,
    KYBRD_ENC_CMD_ALL_MAKEONLY          =    0xF9,
    KYBRD_ENC_CMD_ALL_MAKEBREAK_AUTO    =    0xFA,
    KYBRD_ENC_CMD_SINGLE_AUTOREPEAT     =    0xFB,
    KYBRD_ENC_CMD_SINGLE_MAKEBREAK      =    0xFC,
    KYBRD_ENC_CMD_SINGLE_BREAKONLY      =    0xFD,
    KYBRD_ENC_CMD_RESEND                =    0xFE,
    KYBRD_ENC_CMD_RESET                 =    0xFF
};


// keyboard controller ---------------------------------------

enum KYBRD_CTRL_IO
{
    KYBRD_CTRL_STATS_REG =  0x64,
    KYBRD_CTRL_CMD_REG   =  0x64
};

enum KYBRD_CTRL_STATS_MASK
{
    KYBRD_CTRL_STATS_MASK_OUT_BUF   =   1,      //00000001
    KYBRD_CTRL_STATS_MASK_IN_BUF    =   2,      //00000010
    KYBRD_CTRL_STATS_MASK_SYSTEM    =   4,      //00000100
    KYBRD_CTRL_STATS_MASK_CMD_DATA  =   8,      //00001000
    KYBRD_CTRL_STATS_MASK_LOCKED    =   0x10,   //00010000
    KYBRD_CTRL_STATS_MASK_AUX_BUF   =   0x20,   //00100000
    KYBRD_CTRL_STATS_MASK_TIMEOUT   =   0x40,   //01000000
    KYBRD_CTRL_STATS_MASK_PARITY    =   0x80    //10000000
};

enum KYBRD_CTRL_CMDS
{
    KYBRD_CTRL_CMD_READ             =    0x20,
    KYBRD_CTRL_CMD_WRITE            =    0x60,
    KYBRD_CTRL_CMD_SELF_TEST        =    0xAA,
    KYBRD_CTRL_CMD_INTERFACE_TEST   =    0xAB,
    KYBRD_CTRL_CMD_DISABLE          =    0xAD,
    KYBRD_CTRL_CMD_ENABLE           =    0xAE,
    KYBRD_CTRL_CMD_READ_IN_PORT     =    0xC0,
    KYBRD_CTRL_CMD_READ_OUT_PORT    =    0xD0,
    KYBRD_CTRL_CMD_WRITE_OUT_PORT   =    0xD1,
    KYBRD_CTRL_CMD_READ_TEST_INPUTS =    0xE0,
    KYBRD_CTRL_CMD_SYSTEM_RESET     =    0xFE,
    KYBRD_CTRL_CMD_MOUSE_DISABLE    =    0xA7,
    KYBRD_CTRL_CMD_MOUSE_ENABLE     =    0xA8,
    KYBRD_CTRL_CMD_MOUSE_PORT_TEST  =    0xA9,
    KYBRD_CTRL_CMD_MOUSE_WRITE      =    0xD4
};

// scan error codes ------------------------------------------
enum KYBRD_ERROR
{
    KYBRD_ERR_BUF_OVERRUN           =    0,
    KYBRD_ERR_ID_RET                =    0x83AB,
    KYBRD_ERR_BAT                   =    0xAA,    //note: can also be L. shift key make code
    KYBRD_ERR_ECHO_RET              =    0xEE,
    KYBRD_ERR_ACK                   =    0xFA,
    KYBRD_ERR_BAT_FAILED            =    0xFC,
    KYBRD_ERR_DIAG_FAILED           =    0xFD,
    KYBRD_ERR_RESEND_CMD            =    0xFE,
    KYBRD_ERR_KEY                   =    0xFF
};



static int kybrd_key;               // kybrd key pressed
static bool _numlock, _scrolllock, _capslock;  // lock keys
static bool _shift, _alt, _ctrl;    // shift, alt, and ctrl keys current states
static bool kybrd_disabled = true;  // assumed disabled until when installed

// current scan code
static char _scancode;

// set if keyboard error
static int _kybrd_error = 0;

// set if the Basic Assurance Test (BAT) failed
static bool _kybrd_bat_res = false;

// set if diagnostics failed
static bool _kybrd_diag_res = false;

// set if system should resend last command
static bool _kybrd_resend_res = false;

// wait for key stroke
static bool _new_key_stroke = false;



// original xt scan code set. Array index == make code
// change what keys the scan code corrospond to if your scan code set is different
static int _kybrd_scancode_std [] =
{
    // key            scancode
    KEY_UNKNOWN,    //0
    KEY_ESCAPE,       //1
    KEY_1,            //2
    KEY_2,            //3
    KEY_3,            //4
    KEY_4,            //5
    KEY_5,            //6
    KEY_6,            //7
    KEY_7,            //8
    KEY_8,            //9
    KEY_9,            //0xa
    KEY_0,            //0xb
    KEY_MINUS,        //0xc
    KEY_EQUAL,        //0xd
    KEY_BACKSPACE,    //0xe
    KEY_TAB,        //0xf
    KEY_Q,            //0x10
    KEY_W,            //0x11
    KEY_E,            //0x12
    KEY_R,            //0x13
    KEY_T,            //0x14
    KEY_Y,            //0x15
    KEY_U,            //0x16
    KEY_I,            //0x17
    KEY_O,            //0x18
    KEY_P,            //0x19
    KEY_LEFTBRACKET,//0x1a
    KEY_RIGHTBRACKET,//0x1b
    KEY_RETURN,        //0x1c
    KEY_LCTRL,        //0x1d
    KEY_A,            //0x1e
    KEY_S,            //0x1f
    KEY_D,            //0x20
    KEY_F,            //0x21
    KEY_G,            //0x22
    KEY_H,            //0x23
    KEY_J,            //0x24
    KEY_K,            //0x25
    KEY_L,            //0x26
    KEY_SEMICOLON,    //0x27
    KEY_QUOTE,        //0x28
    KEY_GRAVE,        //0x29
    KEY_LSHIFT,        //0x2a
    KEY_BACKSLASH,    //0x2b
    KEY_Z,            //0x2c
    KEY_X,            //0x2d
    KEY_C,            //0x2e
    KEY_V,            //0x2f
    KEY_B,            //0x30
    KEY_N,            //0x31
    KEY_M,            //0x32
    KEY_COMMA,        //0x33
    KEY_DOT,        //0x34
    KEY_SLASH,        //0x35
    KEY_RSHIFT,        //0x36
    KEY_KP_ASTERISK,//0x37
    KEY_RALT,        //0x38
    KEY_SPACE,        //0x39
    KEY_CAPSLOCK,    //0x3a
    KEY_F1,            //0x3b
    KEY_F2,            //0x3c
    KEY_F3,            //0x3d
    KEY_F4,            //0x3e
    KEY_F5,            //0x3f
    KEY_F6,            //0x40
    KEY_F7,            //0x41
    KEY_F8,            //0x42
    KEY_F9,            //0x43
    KEY_F10,        //0x44
    KEY_KP_NUMLOCK,    //0x45
    KEY_SCROLLLOCK,    //0x46
    KEY_HOME,        //0x47
    KEY_KP_8,        //0x48    //keypad up arrow
    KEY_PAGEUP,        //0x49
    KEY_KP_2,        //0x50    //keypad down arrow
    KEY_KP_3,        //0x51    //keypad page down
    KEY_KP_0,        //0x52    //keypad insert key
    KEY_KP_DECIMAL,    //0x53    //keypad delete key
    KEY_UNKNOWN,    //0x54
    KEY_UNKNOWN,    //0x55
    KEY_UNKNOWN,    //0x56
    KEY_F11,        //0x57
    KEY_F12            //0x58
};



uint8b  kybrd_ctrl_read_status ();
void    kybrd_ctrl_send_cmd (uint8b);
uint8b  kybrd_enc_read_buf ();
void    kybrd_enc_send_cmd (uint8b);
void    i86_kybrd_i_handler ();



// read status from keyboard controller
uint8b kybrd_ctrl_read_status ()
{
    return inportb (KYBRD_CTRL_STATS_REG);
}

// send command byte to keyboard controller
void kybrd_ctrl_send_cmd (uint8b cmd)
{
    // wait for keyboard controller input buffer to be clear
    while (1)
    {   delayABit();
        if ( (kybrd_ctrl_read_status () & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
            break;
    }
    outportb (KYBRD_CTRL_CMD_REG, cmd);
}

// read keyboard encoder buffer
uint8b kybrd_enc_read_buf ()
{
    int i;
    // wait for keyboard controller output buffer to be full
    for(i=0; i<1000; i++)
    {   delayABit();
        if (kybrd_ctrl_read_status () & KYBRD_CTRL_STATS_MASK_OUT_BUF)
            break;
    }
    return inportb (KYBRD_ENC_INPUT_BUF);
}

// send command byte to keyboard encoder
void kybrd_enc_send_cmd (uint8b cmd)
{
    // wait for keyboard controller input buffer to be clear
    while (1)
    {   delayABit();
        if ( (kybrd_ctrl_read_status () & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
            break;
    }
    // send command byte to kybrd encoder
    outportb (KYBRD_ENC_CMD_REG, cmd);
}




// keyboard interrupt handler
void i86_kybrd_i_handler ()
{
    int key;
    static bool _extended;
    int code;
    ASM_START_I_HANDLER

    _new_key_stroke = true;

    _extended = false;      // == scancode+scancode+..., but not yet handled!


    // read the scan code
    code = kybrd_enc_read_buf ();

    // is this an extended code? If so, set it and return
    if (code == 0xE0 || code == 0xE1)
        _extended = true;
    else
    {
        // either the second byte of an extended scan code or a single byte scan code
        _extended = false;

        // test if this is a break code (Original XT Scan Code Set specific)
        if (code & 0x80)    //test bit 7
        {
            // covert the break code into its make code equivelant
            code -= 0x80;

            // grab the key
            key = _kybrd_scancode_std [code];

            // store key in higher bits and ascii version in lower 8 bits
            kybrd_key = key<<8 | ((uint8b)kybrd_key_to_ascii(key));

            // test if a special key has been released and clear it
            switch (key)
            {
                case KEY_LCTRL:
                case KEY_RCTRL:
                    _ctrl = false;
                    break;

                case KEY_LSHIFT:
                case KEY_RSHIFT:
                    _shift = false;
                    break;

                case KEY_LALT:
                case KEY_RALT:
                    _alt = false;
                    break;
            }
        }
        else
        {
            // this is a make code - set the scan code
            _scancode = code;

            // grab the key
            key = _kybrd_scancode_std [code];

            // store key in higher bits and ascii version in lower 8 bits
            kybrd_key = key<<8 | ((uint8b)kybrd_key_to_ascii(key));

            // test if user is holding down any special keys and set it
            switch (key)
            {
                case KEY_LCTRL:
                case KEY_RCTRL:
                    _ctrl = true;
                    break;

                case KEY_LSHIFT:
                case KEY_RSHIFT:
                    _shift = true;
                    break;

                case KEY_LALT:
                case KEY_RALT:
                    _alt = true;
                    break;

                case KEY_CAPSLOCK:
                    _capslock = (_capslock) ? false : true; // alternate
                    kybrd_set_leds (_numlock, _capslock, _scrolllock);
                    break;

                case KEY_KP_NUMLOCK:
                    _numlock = (_numlock) ? false : true;   // alternate
                    kybrd_set_leds (_numlock, _capslock, _scrolllock);
                    break;

                case KEY_SCROLLLOCK:
                    _scrolllock = (_scrolllock) ? false : true; // alternate
                    kybrd_set_leds (_numlock, _capslock, _scrolllock);
                    break;
            }
        }
    }

    // watch for errors
    switch (code)
    {
        case KYBRD_ERR_BAT_FAILED:
            _kybrd_bat_res = false;
            break;

        case KYBRD_ERR_DIAG_FAILED:
            _kybrd_diag_res = false;
            break;

        case KYBRD_ERR_RESEND_CMD:
            _kybrd_resend_res = true;
            break;
    }

    interruptdone(1);       // tell hal we are done
    ASM_STOP_I_HANDLER      // return from interrupt handler
}



void kybrd_ignore_resend ()         // tells driver to ignore last resend request
{ _kybrd_resend_res = false; }

bool kybrd_check_resend ()          // return if system should redo last commands
{ return _kybrd_resend_res; }

bool kybrd_get_diagnostic_res ()    // return diagnostics test result
{ return _kybrd_diag_res; }

bool kybrd_get_bat_res ()           // return BAT test result
{ return _kybrd_bat_res; }

uint8b kybrd_get_last_scan ()       // return last scan code
{ return _scancode; }

// sets leds
void kybrd_set_leds (bool num, bool caps, bool scroll)
{
    uint8b data = 0;

    // set or clear the bit
    data = (scroll) ? (data | 1) : (data & 1);
    data = (num) ? (num | 2) : (num & 2);
    data = (caps) ? (num | 4) : (num & 4);

    // send the command -- update keyboard Light Emetting Diods (LEDs)
    kybrd_enc_send_cmd (KYBRD_ENC_CMD_SET_LED);
    kybrd_enc_send_cmd (data);
}



// convert key to an ascii character
char kybrd_key_to_ascii (uint16b code)
{
    uint8b key = code;

    // check if key is not an ascii character
    if (! isascii (key))
        return 0;

    // if shift key is down or caps lock is on, make the key uppercase
    if (_shift || _capslock)
        if (key >= 'a' && key <= 'z')
            key -= 32;

    if (_shift && !_capslock)
        if (key >= '0' && key <= '9')
            switch (key)
            {
                case '0':
                    key = KEY_RIGHTPARENTHESIS;
                    break;
                case '1':
                    key = KEY_EXCLAMATION;
                    break;
                case '2':
                    key = KEY_AT;
                    break;
                case '3':
                    key = KEY_EXCLAMATION;
                    break;
                case '4':
                    key = KEY_HASH;
                    break;
                case '5':
                    key = KEY_PERCENT;
                    break;
                case '6':
                    key = KEY_CARRET;
                    break;
                case '7':
                    key = KEY_AMPERSAND;
                    break;
                case '8':
                    key = KEY_ASTERISK;
                    break;
                case '9':
                    key = KEY_LEFTPARENTHESIS;
                    break;
            }
        else
        {
            switch (key)
            {   case KEY_COMMA:
                    key = KEY_LESS;
                    break;

                case KEY_DOT:
                    key = KEY_GREATER;
                    break;

                case KEY_SLASH:
                    key = KEY_QUESTION;
                    break;

                case KEY_SEMICOLON:
                    key = KEY_COLON;
                    break;

                case KEY_QUOTE:
                    key = KEY_QUOTEDOUBLE;
                    break;

                case KEY_LEFTBRACKET :
                    key = KEY_LEFTCURL;
                    break;

                case KEY_RIGHTBRACKET :
                    key = KEY_RIGHTCURL;
                    break;

                case KEY_GRAVE:
                    key = KEY_TILDE;
                    break;

                case KEY_MINUS:
                    key = KEY_UNDERSCORE;
                    break;

                case KEY_PLUS:
                    key = KEY_EQUAL;
                    break;

                case KEY_BACKSLASH:
                    key = KEY_BAR;
                    break;
            }
        }
    return key;
}



// reset the system
void kybrd_reset_system ()
{
    // writes 11111110 to the output port (sets reset system line low)
    kybrd_ctrl_send_cmd (KYBRD_CTRL_CMD_WRITE_OUT_PORT);
    kybrd_enc_send_cmd (0xfe);
}

// run self test
bool kybrd_self_test ()
{
    // send command
    kybrd_ctrl_send_cmd (KYBRD_CTRL_CMD_SELF_TEST);

    // wait for output buffer to be full
    while (1)
        if (kybrd_ctrl_read_status () & KYBRD_CTRL_STATS_MASK_OUT_BUF)
            break;

    // if output buffer == 0x55, test passed
    return (kybrd_enc_read_buf () == 0x55) ? true : false;
}





bool kybrd_get_scrolllock ()        // returns scroll lock state
{ return _scrolllock; }

bool kybrd_get_numlock ()           // returns num lock state
{ return _numlock; }

bool kybrd_get_capslock ()          // returns caps lock state
{ return _capslock; }

bool kybrd_get_ctrl ()              // returns status of control key
{ return _ctrl; }

bool kybrd_get_alt ()               // returns status of alt key
{ return _alt; }

bool kybrd_get_shift ()             // returns status of shift key
{ return _shift; }



int getkey()                        // get latest kybrd key pressed
{ return kybrd_key; }

void setkey(int key)                // set latest kybrd key pressed
{ kybrd_key = key; }

void clearkey()                     // set kybrd key to unknown
{ kybrd_key = ASCII_NULL; }

void wait_key_stroke()              // wait for a new key stroke, not stored
{
    _new_key_stroke = false;
    while(_new_key_stroke == false);
}



void kybrd_update()                 // update keyboard input
{ // not needed here since kybrd is updated through its interrupt handler
}



// install keyboard hardware driver
void kybrd_install ()
{
    // Install the interrupt handler (irq 1 uses interrupt 0x20+1)
    install_i_handler (0x21, i86_kybrd_i_handler);

    // assume BAT test is good. If there is a problem, the IRQ handler where catch the error
    _kybrd_bat_res = true;
    _scancode = 0;

    // set lock keys and led lights
    _numlock = _scrolllock = _capslock = false;
    kybrd_set_leds (false, false, false);

    // shift, ctrl, and alt keys
    _shift = _alt = _ctrl = false;

    clearkey();

    // keyboard uses Interrupt Request (IRQ) 1
    enable_irq(1);
}



void kybrd_disable(bool bl)         //  enable or disable keyboard hardware
{
    if(bl) kybrd_ctrl_send_cmd (KYBRD_CTRL_CMD_DISABLE);
    else kybrd_ctrl_send_cmd (KYBRD_CTRL_CMD_ENABLE);
    kybrd_disabled=bl;
    clearkey();                     // initialise values
    _shift = _alt = _ctrl = false;
    _numlock = _scrolllock = _capslock = false;
}
