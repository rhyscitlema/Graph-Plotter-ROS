/*
    timer.c
*/

#include <graphplotter.h>
#include <userinterface.h>
#include <timer.h>


static number global_milli_time;

#define TIMER_PERIOD 50

static number incremented_time = TIMER_PERIOD;

static bool timer_enabled = FALSE;

static void* timer_window;

#define IDT_TIMER 0


static void MyTimerProc()
{
    //if(message !=  WM_TIMER) return; // well, it cannot be true!
    //if(idEvent != IDT_TIMER) return; // well, it cannot be true!
    global_milli_time += incremented_time;
    graphplotter_update();
}


void timer_install (void* window)
{
    timer_window = window;
    timer_enabled = TRUE;
}


static void timer_uninstall (void* window)
{
    timer_enabled = FALSE;
}


int timer_isRunning()
{ return timer_enabled; }

void timer_pause (int pause)
{
    if(pause) timer_uninstall (timer_window);
    else timer_install (timer_window);
}


number timer_get_normal_increment ()
{ return TIMER_PERIOD; }

void timer_set_increment (number increment)
{ incremented_time = increment; }

number timer_get_increment ()
{ return incremented_time; }


void timer_set_time (number new_time)
{ global_milli_time = new_time*1000; }

number timer_get_time ()
{ return global_milli_time/1000; }

