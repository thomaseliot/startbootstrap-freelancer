/* $Revision: 1.14 $    $Date: 2009-07-20 21:19:19 $
 * Copyright (c) 2001 Altia Inc.
 *
 * Permission to use, copy, and modify this software for any purpose is
 * hereby granted, provided that this copyright notice appears in all
 * copies and that the entity using, copying, or modifying this software
 * has purchased an Altia Deep Screen license. Permission to sell or
 * distribute this source code is denied. Altia makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALTIA INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL ALTIA INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/** FILE:  altiaInput.c ****************************************************
 **
 ** If code is generated for any Altia objects with stimulus,
 ** this file contains the routines to process input events.
 ** If any Altia objects have timers, this file also contains
 ** routines to process timers.
 ***************************************************************************/

#include <string.h>
#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "../altiaData.h"
#include "altiaLibDraw.h"
#include "egl_Wrapper.h"


/****************************************************************************
 * External Interfaces
 ***************************************************************************/

extern int TargetCheckEvent(int *status);


extern ALTIA_UINT32 TargetGetCurTime(void);



/****************************************************************************
 * Definitions
 ***************************************************************************/

#ifdef ALTIAINPUTDEBUG
    #define DBG_INPUT1(a)           printf(a);fflush(stdout)
    #define DBG_INPUT2(a,b)         printf(a,b);fflush(stdout)
    #define DBG_INPUT3(a,b,c)       printf(a,b,c);fflush(stdout)
    #define DBG_INPUT4(a,b,c,d)     printf(a,b,c,d);fflush(stdout)
    #define DBG_INPUT5(a,b,c,d,e)   printf(a,b,c,d,e);fflush(stdout)
    #define DBG_INPUT6(a,b,c,d,e,f) printf(a,b,c,d,e,f);fflush(stdout)
#elif defined(WIN32) || defined(_WINDLL) || defined(WIN16) || defined(UNDER_CE) || defined(NO_VA_ARGS)
    #define DBG_INPUT1(a)
    #define DBG_INPUT2(a,b)
    #define DBG_INPUT3(a,b,c)
    #define DBG_INPUT4(a,b,c,d)
    #define DBG_INPUT5(a,b,c,d,e)
    #define DBG_INPUT6(a,b,c,d,e,f)
#else
    #define DBG_INPUT1(...)
    #define DBG_INPUT2(...)
    #define DBG_INPUT3(...)
    #define DBG_INPUT4(...)
    #define DBG_INPUT5(...)
    #define DBG_INPUT6(...)
#endif /* ALTIAINPUTDEBUG */


/****************************************************************************
 * Types
 ***************************************************************************/


/****************************************************************************
 * Constants
 ***************************************************************************/


/****************************************************************************
 * Globals
 ***************************************************************************/


/* To avoid checking for events between control blocks
 * immediately following stimulus input events,  a
 * counter is used to control how often _altiaCheckSystem()
 * actually calls TargetGetEvent().  This counter is
 * manipulated by _altiaCheckSystem() and also by
 * altiaCallExpiredTimers() and altiaHandleWinEvent().
 */
static int localCheckSystemCnt = 0;



/* If generated code includes timers, we need to keep track
 * of the timers that are running so that they can be checked
 * and processed with other input events in TargetGetEvent()
 * and TargetCheckEvent().
 */
static ALTIA_INDEX localArmIdx = -1;
static ALTIA_INDEX localRunCnt = 0;


/****************************************************************************
 * Functions
 ***************************************************************************/

/*--- localStartNextTimer() -----------------------------------------------
 *
 * Local utility to find the timer that is expiring next.
 *-------------------------------------------------------------------------*/
static void localStartNextTimer(void)
{
    int i;
    AltiaTimerTime_t *runningPtr = &test_localTimers[0];
    ALTIA_UINT32 minTime = 0xffffffff;
    ALTIA_UINT32 nextTime;

    localArmIdx = -1;

    for (i = 0; i < localRunCnt; i++, runningPtr++)
    {



            nextTime = ALTIA_TIMER_MSECS(runningPtr->index) + runningPtr->startTime;

        if (nextTime < minTime)
        {
            localArmIdx = i;
            minTime = nextTime;
        }
    }
}


/*--- altiaStartTimer() ---------------------------------------------------
 *
 * Function called by other Altia code to add a timer to the list
 * of running timers (if any).
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaStartTimer(ALTIA_INDEX index, ALTIA_UINT32 msecs)
{
    ALTIA_UINT32 curTime = TargetGetCurTime();
    AltiaTimerTime_t *runningPtr = &test_localTimers[localRunCnt];

    runningPtr->index = index;
    runningPtr->startTime = curTime;

    localRunCnt++;

    if (localRunCnt == 1)
    {
        localArmIdx = 0;
    }
    else if (localArmIdx >= 0)
    {
        ALTIA_UINT32 tmsecs;
        runningPtr = &test_localTimers[localArmIdx];

        /* If this new timer is going to expire before the timer
         * that currently has the shortest wait, make this
         * new timer the next timer.
         */



        {
            tmsecs = ALTIA_TIMER_MSECS(runningPtr->index);
        }

        if ((curTime + msecs) < (runningPtr->startTime + tmsecs))
        {
            localArmIdx = localRunCnt - 1;
        }
    }
    else
    {
        /* Seem we don't know which timer is expiring next
         * so figure it out.
         */
        localStartNextTimer();
    }
}


/*--- altiaStopTimer() ----------------------------------------------------
 *
 * Function called by other Altia code to remove a specific timer
 * from the list of running timers.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaStopTimer(ALTIA_INDEX index)
{
    int i;
    AltiaTimerTime_t *runningPtr = &test_localTimers[0];
    for (i = 0; i < localRunCnt; i++, runningPtr++)
    {
        if (runningPtr->index == index

            )
        {
            /* Found the running timer so remove it.  If
             * there are other timers in the list after it,
             * move them forward.
             */
            int cnt = localRunCnt - i - 1;

            if (cnt > 0)
            {
                memmove(&test_localTimers[i], &test_localTimers[i+1],
                        sizeof(AltiaTimerTime_t) * cnt);
            }

            localRunCnt--;

            /* Figure out which timer (if any) is next to expire if
             * it was previously this timer or one after this timer.
             */
            if (localArmIdx >= i)
                localStartNextTimer();

            break;
        }
    }
}




/*--- altiaGetTimerWait() -------------------------------------------------
 *
 * The next function is called from other Altia target specific
 * code to get an msec wait timer whenever we wait for events if we
 * have any timers waiting to expire.  This function must always be
 * present, but it does nothing if code is not generated for timers.
 *-------------------------------------------------------------------------*/
ALTIA_BOOLEAN altiaGetTimerWait(ALTIA_UINT32 *waitTime)
{



    if (localRunCnt > 0 && localArmIdx >= 0)
    {
        ALTIA_UINT32 curTime = TargetGetCurTime();
        ALTIA_UINT32 timerTime;
        ALTIA_UINT32 nextTime;



            timerTime = ALTIA_TIMER_MSECS(test_localTimers[localArmIdx].index);

        nextTime = test_localTimers[localArmIdx].startTime
                      + timerTime;

        if (nextTime <= curTime)
            *waitTime = 0;
        else
            *waitTime = nextTime - curTime;

        return ALTIA_TRUE;
    }



    return ALTIA_FALSE;
}


/*--- altiaCallExpiredTimers() --------------------------------------------
 *
 * The next function is called from other Altia target specific
 * code whenever we check for events.  Its job is to handle
 * expired timers.
 *
 * This function must always be present, but it
 * does nothing if code is not generated for timers.
 *
 * WARNING:  This function may get called recursively if there
 *           is Altia control that uses a LOOP statement.
 *-------------------------------------------------------------------------*/
ALTIA_BOOLEAN altiaCallExpiredTimers()
{
    ALTIA_BOOLEAN res = ALTIA_FALSE;

    ALTIA_BOOLEAN done = ALTIA_FALSE;

    while (!done && localRunCnt > 0)
    {
        int i;
        int expiredIndex = -1;
        AltiaTimerTime_t *runningPtr = &test_localTimers[0];
        ALTIA_UINT32 curTime = TargetGetCurTime();

        for (i = 0; i < localRunCnt; i++, runningPtr++)
        {
            ALTIA_UINT32 msecs;



                msecs = ALTIA_TIMER_MSECS(runningPtr->index);

            if (msecs == 0)
                msecs = 1;

            if ((runningPtr->startTime + msecs) <= curTime)
            {
                expiredIndex = i;
                break;
            }
            else if (runningPtr->startTime > curTime)
            {
                /* OS timer has rolled over */
                ALTIA_UINT32 deltaTime = curTime + (((ALTIA_UINT32)-1) - runningPtr->startTime);
                if (deltaTime >= msecs)
                {
                    expiredIndex = i;
                    break;
                }
            }
                
        }

        /* We need to be out of the loop before we call altiaHandleInputEvent
        * since that call can change the number of running timers.
        */
        if (expiredIndex == -1)
            done = ALTIA_TRUE;
        else
        {



            {
                Altia_InputEvent_type event;
                event.eventType = ALTIA_TIMER_EVENT_TYPE;
                event.button = runningPtr->index;

                /* Avoid checking for events between control blocks
                 * immediately following stimulus input events.  We
                 * can infer that we are not stuck in a control loop
                 * because we are currently processing an input event.
                 */
                localCheckSystemCnt = 0;

                /* Ticket 1034:  Reset start time prior to execution */
                runningPtr->startTime = curTime;

                res |= altiaHandleInputEvent(&event);
            }

        }
    }

    /* Figure out which timer expires next (if any are left). */
    localStartNextTimer();

    return res;
}


/*--- _altiaCheckSystem() -------------------------------------------------
 *
 * The next function is called from other Altia code when
 * control is doing nested execution and we still want to get system
 * input.  For this target, it's easy.  We just call the
 * routine to check for events.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void _altiaCheckSystem()
{
    int status;

    /* If we check too often on this target, we can end up getting
     * input events before a set of atomic input stimulus definitions
     * (that may trigger nested control blocks) can completely execute.
     * Avoid this by checking less often.
     */
    if (localCheckSystemCnt++ >= 10)
    {
        DBG_INPUT1("DOING EVENT CHECK FROM _altiaCheckSystem()!\n");

        localCheckSystemCnt = 0;
        TargetCheckEvent(&status);
    }
}


/*--- altiaHandleWinEvent() -----------------------------------------------
 *
 * The next function is called from other Altia target specific
 * code to do all the work of translating a target specific event into
 * a generic Altia event.  A pointer to the target specific event
 * arrives in the "data" argument.  If code is not generated for
 * stimulus, this function does nothing.
 *
 * WARNING:  This function may get called recursively if there
 *           is Altia control that uses a LOOP statement.
 *-------------------------------------------------------------------------*/
ALTIA_BOOLEAN altiaHandleWinEvent(ALTIA_WINDOW win, void *data)
{

    ALTIA_BOOLEAN valid = ALTIA_FALSE;
    Altia_InputEvent_type event;


    /* If timers come in as events, this function may have to do
     * something to handle them.  For this target, they do not
     * come in as events and must be handled elsewhere.
     *
     * event.eventType = ALTIA_TIMER_EVENT_TYPE;
     * event.button = wParam - 1;
     * valid = ALTIA_TRUE;
     */

    if (valid)
    {

        /* Avoid checking for events between control blocks
         * immediately following stimulus input events.  We
         * can infer that we are not stuck in a control loop
         * because we are currently processing an input event.
         */
        localCheckSystemCnt = 0;

        return altiaHandleInputEvent(&event);
    }


    return ALTIA_FALSE;
}

