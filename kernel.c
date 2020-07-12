/*******************************
   kernel.c
*******************************/

#include "userinterface/user_main_gui.h"
#include <_malloc.h>
#include <hardware.h>
#include <timer.h>
#include <mouse.h>
#include <keyboard.h>
#include "hardware/screen.h"


#define KERNEL_LOCATION  0x00100000 /* 0x00100000 <= kernel space  <= kernel file size */
#define STACK_LOCATION   0x00800000 /* 0x00100000 <= stack space   <= 0x00800000 (stack goes downward) */
#define SCREEN_BUFFER    0x00800000 /* 0x00800000 <= screen buffer <= 0x01000000 */
#define FREE_MEMORY      0x01000000 /* 0x01000000 <= FreeMemory    <= RAM total size */
#define FREE_MEMORY_SIZE 0x08000000


/* kernel entry point is called by the kernel loader
 * important: both must agree on the KERNEL_LOCATION.
 * kernel.c and screen.c must agree on value of SCREEN_BUFFER */


void kernelentry ()
{
    _asm
    {
        cli                     ; make sure interrupts are cleared
        mov     ax, 0x10        ; refresh all segment registers
        mov     ds, ax          ; to the data descriptor.
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
        mov     ss, ax
        mov     esp, STACK_LOCATION
        mov     ebp, esp        ; For fail-safeness, store the current stack frame pointer,
        push    ebp             ; this will insure that any routines we call has a place to return to.
    }

    /* install or initialise screen device */
    screen_install();

    useFreeMemory ((void*)FREE_MEMORY, FREE_MEMORY_SIZE);

    /* install or initialise hardware environment */
	hardware_initialise();

    /* install or initialise keyboard device */
    kybrd_install();

    /* install or initialise mouse device */
    mouse_install();

    /* install or initialise timer device */
    timer_install(0);

    /* start software environment */
    userinterface_start();

    /* terminate hardware environment */
    hardware_shutdown();
}


/* just for compiler sake */
int main() {return 0;}
