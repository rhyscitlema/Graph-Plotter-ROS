/**************************************************
    mouse.c
**************************************************/

#include <_malloc.h>
#include <userinterface.h>
#include <mouse.h>
#include "..\hal\hal.h"

Mouse *headMouse;



// clear mouse dx, dy and dz values to zeros
void mouse_clear_motion (Mouse *mouse)
{
    mouse->motion[0] = mouse->motion[1] = mouse->motion[2] = 0;
}

// clear all mouse buttons to FALSE
void mouse_clear_buttons (Mouse *mouse)
{
    mouse->leftBefore = mouse->middleBefore = mouse->rightBefore = FALSE;
    mouse->leftPressed = mouse->middlePressed = mouse->rightPressed = FALSE;
    mouse->ctrlPressed = FALSE;
}

// clear all mouse pointer variables
void mouse_clear_pointers (Mouse *mouse)
{
    mouse->clickedObject = mouse->pointedObject = NULL;
    mouse->clickedCamera = mouse->pointedCamera = NULL;
    mouse->leftClickedCamera = NULL;
}

// enable or disable mouse
void mouse_enable (Mouse *mouse, bool bl)
{
    mouse->enabled = bl;
    mouse_clear_motion (mouse);
    mouse_clear_buttons (mouse);
    mouse_clear_pointers (mouse);
    //mouse->display_features = 0;
}





Mouse *mouse_new (int id)
{
    Mouse *mouse = (Mouse*) _malloc (sizeof(Mouse));
    _memset(mouse, 0, sizeof(Mouse));
    mouse->ID = id;
    mouse->prev = NULL;
    mouse->next = NULL;
    return mouse;
}

void mouse_remove (Mouse *mouse)
{
    if(mouse == NULL) return;
    if(mouse == headMouse)
        headMouse = mouse->next;
    if(mouse->prev != NULL)
        mouse->prev->next = mouse->next;
    if(mouse->next != NULL)
        mouse->next->prev = mouse->prev;
    _memset(mouse, 0, sizeof(Mouse));
    _free(mouse);
}



// process mouse input
static void mouse_process (Mouse *m)
{
    int pixel;

    if(m->leftPressed || m->rightPressed || m->ctrlPressed) m->allow = TRUE;
    if(m->middlePressed) m->allow = FALSE;
    if(!m->allow) { mouse_clear_motion(m); return; }

    if(m->rightPressed)
    {
        if(!m->rightBefore)
        {
            m->rightBefore = TRUE;
            m->clickedCamera = m->pointedCamera;

            if(m->clickedCamera != NULL)
                 m->showSignature = 2;      // '2' for showing camera signature
            else m->showSignature = 3;      // '3' for showing null signature
        }
        m->clickedObject = NULL;            // enforce clickedObject to be NULL
    }
    else m->rightBefore = FALSE;

    //m->pointedObject = camera_get_object (m->pointedCamera, m->currentPosition[0], m->currentPosition[1]);
    if(m->pointedCamera == NULL) return;
    pixel = m->currentPosition[1] * m->pointedCamera->XSize + m->currentPosition[0];
    if(pixel>=0 && pixel < m->pointedCamera->XSize * m->pointedCamera->YSize)
        m->pointedObject = m->pointedCamera->closestObject [pixel];

    if(m->leftPressed)
    {
        if(!m->leftBefore)
        {
            m->leftBefore = TRUE;
            m->clickedObject = m->pointedObject;
            m->leftClickedCamera = m->pointedCamera;

            if(m->clickedObject != NULL)
            {
                // this line is because graphplotter_update() is not called when onPause
                m->clickedObject->isClicked = FALSE;

                m->showSignature = 1;       // '1' for showing object signature
            }
            else m->showSignature = 3;      // '3' for showing null signature
        }
        m->clickedCamera = NULL;            // enforce clickedCamera to be NULL
    }
    else m->leftBefore = FALSE;

    if(m->ctrlPressed)
         m->ctrlBefore = TRUE;
    else m->ctrlBefore = FALSE;
}


void mouse_process_all()
{ mouse_process(headMouse); }



void mouse_update()             // update mouse input
{ // not needed here since mouse is updated through its interrupt handler
}



/* mouse interrupt handler */

static bool Zmotion_enabled = false;
static bool extra_buttons_enabled = false;
static uint8b mouse_cycle = 0;
static uint8b mouse_flag = 0;

static void i86_mouse_i_handler()
{
    signed char value;
    Mouse* mouse;
    ASM_START_I_HANDLER

    mouse = headMouse;

    if(!mouse->enabled)                 // note: mouse_cycle=value=0 is used
    { mouse_cycle = value = 0; }        // because it is an invalid combination

    else value = inportb(0x60);         // KYBRD_ENC_INPUT_BUF = 0x60

    switch(mouse_cycle)
    {
        case 0:
            if(!(value&0x08)) break;    // check bit 3 so to check if the byte is
            mouse_flag   = value;       // the flag (or first) byte of the packet.
            mouse->leftPressed   = (value & 1) ? true:false;
            mouse->rightPressed  = (value & 2) ? true:false;
            mouse->middlePressed = (value & 4) ? true:false;
            mouse_cycle++;
            break;

        case 1:
            mouse->motion[0] += value;  // get x motion
            mouse_cycle++;
            break;

        case 2:
            mouse->motion[1] += value;  // get y motion
            if(Zmotion_enabled)
                mouse_cycle++;
            else mouse_cycle=0;
            break;

        case 3:
            if(extra_buttons_enabled)
            {
                mouse->fourthPressed = (value & 0x10) ? true:false;
                mouse->fifthPressed  = (value & 0x20) ? true:false;
                mouse->motion[2]    += (value & 0x0F);  // get z motion
            }
            else mouse->motion[2] -= value;  // get z motion
            mouse_cycle=0;
            break;
    }

    interruptdone(12);      // tell hal we are done
    ASM_STOP_I_HANDLER
}



extern void kybrd_ctrl_send_cmd (uint8b cmd);
extern void kybrd_enc_send_cmd (uint8b cmd);
extern uint8b kybrd_enc_read_buf ();

static void mouse_ps2_send (uint8b value)
{
    kybrd_ctrl_send_cmd (0xD4);     // prepare to send cmd or data byte
    kybrd_enc_send_cmd (value);     // send cmd or data byte
    kybrd_enc_read_buf ();          // acknowledge
}

static void mouse_ps2_enable()
{
    uint8b cmd; //, ID;
    disable_ints();

    kybrd_ctrl_send_cmd (0xA8);     // enable mouse device
    kybrd_enc_read_buf ();          // acknowledge

    kybrd_ctrl_send_cmd (0x20);     // prepare to get command byte
    cmd = kybrd_enc_read_buf ();    // get command byte
    cmd = kybrd_enc_read_buf ();    // ...not sure myself why this is a must!

    kybrd_ctrl_send_cmd (0x60);     // prepare to write modified command byte
    kybrd_enc_send_cmd (cmd|3);     // bit 1 set = enable mouse interrupt (IRQ12)
    kybrd_enc_read_buf ();          // acknowledge

    mouse_ps2_send (0xF6);          // set default values (100 packets/s, 4 pixels/mm)

    mouse_ps2_send (0xF2);

    // magic sequence to enable mouse scrolling (Zmotion)
    mouse_ps2_send (0xF3);          // set sample rate
    mouse_ps2_send (200);           // to 200 packets/s
    mouse_ps2_send (0xF3);          // set sample rate
    mouse_ps2_send (100);           // to 100 packets/s
    mouse_ps2_send (0xF3);          // set sample rate
    mouse_ps2_send (80);            // to 80 packets/s

    //Zmotion_enabled = true;         // assume mouse has Z motion

    mouse_ps2_send (0xF4);          // enable automatic streaming

    enable_ints();
}



// install mouse hardware driver
void mouse_install()
{
    // Install the interrupt handler (irq 12 uses interrupt 0x20+0x0C)
    install_i_handler (0x2C, i86_mouse_i_handler);

    // mouse uses Interrupt Request (IRQ) 12
    enable_irq(12);
    enable_irq(2); // Cascade interrupt for IRQs 8-15.
    mouse_ps2_enable();

    headMouse = mouse_new (1);
    mouse_enable (headMouse, TRUE);
}
