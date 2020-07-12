/****************************************************************************
    keyboard.c
****************************************************************************/

#include <allegro/keyboard.h> // use Allegro library for keyboard hardware access
#include "keyboard.h"


int kybrd_key;                      // kybrd key pressed

static bool _numlock, _scrolllock, _capslock;
static bool _shift, _alt, _ctrl;    // shift, alt, and ctrl keys current states
static bool kybrd_disabled = true;  // assumed disabled until when installed


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
}


void kybrd_update()                 // update keyboard input
{
    if(kybrd_disabled)
        return;
    if(keypressed())
        kybrd_key = readkey();      // call Allegro function
    else kybrd_key = ASCII_NULL;
    _alt = key[KEY_ALT] || key[KEY_ALTGR];
    _ctrl = key[KEY_LCONTROL] || key[KEY_RCONTROL];
    _shift = key[KEY_LSHIFT] || key[KEY_RSHIFT];
    _numlock = key[KEY_NUMLOCK];
    _capslock = key[KEY_CAPSLOCK];
    _scrolllock = key[KEY_SCRLOCK];
}

void kybrd_install ()               // install keyboard hardware driver
{
    install_keyboard();             // install Allegro keyboard
    kybrd_disable(false);
}

void kybrd_disable(bool bl)         //  enable/disable keyboard hardware
{
    kybrd_disabled=bl;
    clearkey();                     // initialise values
    _shift = _alt = _ctrl = false;
    _numlock = _scrolllock = _capslock = false;
}

