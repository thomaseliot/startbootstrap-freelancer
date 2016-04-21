/* $Revision: 1.32 $    $Date: 2010-10-10 17:57:29 $
 * Copyright (c) 2001-2003 Altia Inc.
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

/** FILE:  altiaUtils.c ****************************************************
 **
 ** This is where it all really starts for initializing and controlling
 ** Altia DeepScreen generated code!
 **
 ** This file contains target specific functions for initializing and
 ** controlling a DeepScreen application running on a target.  Functions
 ** of most interest are:
 **
 **   void TargetAltiaInitialize();
 **   int TargetAltiaAnimate(ALTIA_CHAR *funcName, LocalEventType value);
 **   void TargetAltiaUpdate();
 **   int TargetCheckEvent(int *status);
 **   ALTIA_BOOLEAN TargetGetEvent(int *status);
 **   int TargetAltiaLoop(ALTIA_CHAR *chosenPort);
 **   void TargetAltiaClose();
 **   void TargetSleep(unsigned long millisecs);
 **   void TargetAltiaSetParentWindow(unsigned long windowId);
 **
 ** The DeepScreen version of the Altia API uses these functions 
 ** exclusively to interact with the DeepScreen code.
 **
 ** These functions typically require extensive customizations from one
 ** target to another.
 **
 ** By default, the generic code file altiaSimpleMain.c provides a
 ** main() with calls to initialize and run the event processing loop.
 ** To have a custom main loop that the DeepScreen make file will automically
 ** compile and link with the DeepScreen code, create a file <DESIGN>.c
 ** (where <DESIGN> is the base name of the .dsn design file for which code
 ** is being generated) and put the function:
 **
 **     main(int argc, AltiaNativChar *argv[])
 **
 ** in it.  This is the classic C main entry point.  The main() function
 ** should do a return when it finishes.
 **
 ** Altia API functions are used if ALTIALIB is defined.  The API functions
 ** can come from a full version of the API for connecting to a host based
 ** Altia session or they can come from a standalone version of the API
 ** for running completely standalone (usually with custom user code
 ** developed using the API).  The version of the API to be used is
 ** determined by which API code gets linked into the final executable.
 ***************************************************************************/

/* DeepScreen code may have a different representation for AltiaEventType
 * than API so temporarily define AltiaEventType as LocalEventType during
 * our include of DeepScreen headers.  We can use the "local" version when
 * needed for interfacing to DeepScreen functions.
 *
 * This should be at the top of this file before all #include statements
 * to protect for any include file that may also include "altiaBase.h".
 * In this case we're protecting for "os_Wrapper.h" or "egl_Wrapper.h"
 * including "altiaBase.h".
 */
#define AltiaEventType LocalEventType

#include "os_Wrapper.h"
#include "egl_Wrapper.h"

/* Drawing with double buffering is the default mode of operation
 * to minimize display flicker.  To draw without double buffering,
 * ALTIA_DOUBLE_BUFFERING must be defined as 0.  This can be done
 * from a compiler directive because ALTIA_DOUBLE_BUFFERING is not
 * defined here unless it is currently undefined.  Alternatively,
 * the definition here can be changed directly from 1 to 0 if
 * this is a custom copy of altiaUtils.c.
 */
#ifndef ALTIA_DOUBLE_BUFFERING
#define ALTIA_DOUBLE_BUFFERING 1
#endif

#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "../altiaData.h"
#include "altiaLibDraw.h"


#undef AltiaEventType

#ifdef KANJI
#include <imm.h>
#ifndef UNDER_CE
#include <mbctype.h>
#include <locale.h>
#endif /* !UNDER_CE */
#endif /* KANJI */

/* For debug output */
#ifdef DEBUG_WINDOW
#include <stdio.h>
#endif

/* If using the Altia API (either the full version to connect to a
 * host based Altia session or the standalone version), then some
 * Altia API definitions are needed.
 */
#ifdef ALTIALIB
/* If we are not on a UNICODE target, don't let ALTIAUNICODEAPI get defined!
 * MUST BE DONE BEFORE altia.h GETS INCLUDED!
 */
#ifndef UNICODE
#undef ALTIAUNICODEAPI
#endif

#include "altia.h"

/* All flavors of Altia API provide an _altiaBreakLoop global as of 4/16/01.
 * It allows the API to be informed of a window close while its looping
 * for events from Altia.
 */
extern int _altiaBreakLoop;
#endif /* ALTIALIB */

/* Flag that updates should not be performed.  Part of
 * builtin animation altiaCacheOutput.
 */
extern ALTIA_BOOLEAN _altiaOverrideUpdate;

/* The host computer to connect to using the Altia API
 * on a remote target.  This is taken from the Environment
 * variable TARGETHOST if it is set at code generation
 * time.  Else, the code generator uses the name for the
 * machine that generated the code.
 */
#define HOSTNAME ALT_TEXT("desktop-tk8pejh")
#define PORT ALT_TEXT("5100")

/* If altia.h did not define ALTIA_NO_USER_MAIN, define it */
#ifndef ALTIA_NO_USER_MAIN
#define ALTIA_NO_USER_MAIN -12345
#endif

/* Forward declarations for global functions in this file that can be
 * used externally.
 */
#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif
    int TargetAltiaInitialize(void);
    int TargetAltiaAnimate(ALTIA_CHAR *funcName, LocalEventType value);
    int TargetAltiaAnimateId(int nameId, LocalEventType value);
    void TargetAltiaUpdate(void);
    int TargetAltiaLoop(ALTIA_CHAR *chosenPort);
    void TargetAltiaClose(void);
    void TargetAltiaSetParentWindow(unsigned long windowId);
    #ifdef ALTIA_TASKING
    int TargetAltiaTaskRun(ALTIA_UINT32 milliSeconds);
    void TargetAltiaTaskStatus(AltiaQueueStatusType * status);
    void TargetAltiaObjectStatus(AltiaQueueStatusType * status);
    #endif /* ALTIA_TASKING */
    int TargetGetTextWidth(ALTIA_CONST ALTIA_CHAR *name, ALTIA_CONST ALTIA_CHAR *text);
    int TargetGetTextWidthId(int nameId, ALTIA_CONST ALTIA_CHAR *text);
    int TargetGetFont(ALTIA_CONST ALTIA_CHAR *name);
    int TargetGetFontId(int nameId);
    int TargetSetFont(ALTIA_CONST ALTIA_CHAR *name, int fontId);
    int TargetSetFontId(int nameId, int fontId);
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/* Special external functions in other DeepScreen sources */
extern EGL_COLOR altiaLibGetColor(ALTIA_WINDOW w, ALTIA_COLOR pixel);
extern ALTIA_BOOLEAN altiaHandleWinEvent(ALTIA_WINDOW w, void *event);
extern ALTIA_BOOLEAN altiaGetTimerWait(ALTIA_UINT32 *waitTime);
extern ALTIA_BOOLEAN altiaCallExpiredTimers(void);
extern ALTIA_BOOLEAN _altiaConnectionToFloat(ALTIA_CHAR *funcName);
extern ALTIA_WINDOW driverGetMainWindowID(void);
extern ALTIA_WINDOW altiaGetViewWindowByRealWin(ALTIA_WINDOW);

#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif
    extern void TargetAltiaSemGet(int index);
    extern void TargetAltiaSemRelease(int index);
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif



#ifdef DEBUG_WINDOW
extern void egl_OpenDebugWindow(void);
#endif

/****************************************************************************
 * Local Variable and Function Declarations/Definitions.
 ****************************************************************************/
ALTIA_BOOLEAN localQuitDetected = 0;
ALTIA_BOOLEAN noDisplayDraw = 0;

/* Used to prevent recursion into batch draw updates */
static volatile int localInBatch = 0;

/* for localInitialized 0 - not init, 1 - init ok, 2 - init error */
static ALTIA_BOOLEAN localInitialized = 0;

/* Some forward declarations for local functions called by
 * TargetAltiaInitialize() or TargetAltiaClose().
 */
#ifdef DEBUG_WINDOW
void SampleDebugOutputOpen(void);
#endif
void SampleReportAnimation(ALTIA_CHAR *name, LocalEventType value);
void SampleRedraw(AltiaDevInfo *devInfoPtr, ALTIA_BOOLEAN doubleBufferOn,
    Altia_Extent_type* extent);
void SampleCursorCreate(AltiaDevInfo *devInfoPtr);
void SampleCursorDestroy(AltiaDevInfo *devInfoPtr);
void SampleResize(AltiaDevInfo *devInfoPtr, EGL_EVENT *eventPtr,
    ALTIA_BOOLEAN doubleBufferOn);
void SampleExpose(AltiaDevInfo *devInfoPtr, EGL_EVENT *eventPtr,
    ALTIA_BOOLEAN doubleBufferOn);

#ifdef ALTIA_TASKING

/* Update states */
#define ALTIA_UPDATE_START      0
#define ALTIA_UPDATE_INIT       1
#define ALTIA_UPDATE_BUILD      2
#define ALTIA_UPDATE_DRAW       3
#define ALTIA_UPDATE_END        4
#define ALTIA_UPDATE_COUNT      5

static int localWriteTaskQ(void);

ALTIA_BOOLEAN AltiaTaskRunning = ALTIA_TRUE;

static volatile int _taskWrite;
static volatile int _taskRead;

static AltiaQueueStatusType _taskStatus;

static int _objectCount;
static int _objectCurrent;
static int _objectState;
static int _objectExtentIdx;
static Altia_Extent_type _objectExtent;

static AltiaQueueStatusType _objectStatus;
#ifndef ALTIA_TASK_NO_FLUSH_OPTIMIZATION
static volatile int _taskFlush;
#endif

#endif /* ALTIA_TASKING */


/*--- altiaLibGetDevInfo() ------------------------------------------------
 *
 * Target specific utility to get the device ids for the current session.
 * Needed by the font loading routines since the device information isn't
 * passed into the functions when they are called by target independent
 * code.
 *-------------------------------------------------------------------------*/
AltiaDevInfo *altiaLibGetDevInfo(void)
{

    return test_layerDevInfo;


}


/* The window we pass to the draw functions */
ALTIA_WINDOW altiaGetMainWin(void)
{
    return (ALTIA_WINDOW)altiaLibGetDevInfo();
}

/* The actual window id. */
ALTIA_WINDOW altiaGetMainWinID(void)
{
    return (ALTIA_WINDOW)driverGetMainWindowID();
}




/*--- TargetAltiaUpdate() -------------------------------------------------
 *
 * This function is called to perform an update of the Altia graphics.
 * It is global so that other code outside of this file can call it.
 * It uses local, target specific variables to call the actual AltiaUpdate()
 * function.  By having it here, other code that wants to do an update
 * can be target independent.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from the Altia API code.
 *-------------------------------------------------------------------------*/
void TargetAltiaUpdate(void)
{
#ifdef ALTIA_TASKING
    int idx;

    if (localInitialized != 1)
        return;

    idx = localWriteTaskQ();
    if (idx >= 0)
    {
        /* Write data into queue element */
        altiaTaskQueue[idx].type = ALTIA_TASK_FLUSH;

#ifndef ALTIA_TASK_NO_FLUSH_OPTIMIZATION
        /* Save position of most recent flush in task queue.
        ** MUST occur before _taskWrite assignment to be
        ** thread safe.
        */
        _taskFlush = idx;
#endif
        /* Commit the data in the queue */
        _taskWrite = idx;
    }

    return;
#else
    ALTIA_INT doubleBufferOn;
    EGL_BITMAP_ID drawArea;
    AltiaDevInfo * devinfo = NULL;
    int startBatch = 0;
#ifdef WIN32
    int saveExtent;
#endif


    if (localInitialized != 1)
        return;

    /* No draw if override is enabled */
    if (_altiaOverrideUpdate)
        return;

    /* No draw if extent list is empty */
    if (0 == test_extents_count)
        return;


        /* Perform a batch of graphics rendering operations. Starting a
         * batch operation on this target locks shared display resources
         * to the calling thread until the batch is ended.  If the
         * resources are already locked by another thread, we will block.
         */
        devinfo = altiaLibGetDevInfo();
#ifdef WIN32
        saveExtent = test_extents_count;
        TargetAltiaSemGet(2);
        test_extents_count = saveExtent;
#endif
        if (localInBatch == 0)
        {
            startBatch = 1;
            if (egl_BatchStart(devinfo->gc) == EGL_STATUS_ERROR)
            {
#ifdef WIN32
                TargetAltiaSemRelease(2);
#endif
                return;
            }
            localInBatch = 1;
            /* NOTE: Do not return without clearing localInBatch! */
        }
#if ALTIA_DOUBLE_BUFFERING
        /* If an offscreen bitmap was successfully allocated at
         * initialization time, we can do double buffering.
         * Otherwise, we are stuck drawing directly to the display.
         */
        drawArea = devinfo->memBitmap;
        devinfo->drawable = drawArea;
        doubleBufferOn = (drawArea != EGL_DISPLAY_ID) ? 1 : 0;
#else
        /* No double buffering means we draw directly to display. */
        drawArea = EGL_DISPLAY_ID;
        doubleBufferOn = 0;
#endif
        /* Let Altia do its drawing.  This generic function will
         * call the target specific altiaLibCopy() function to copy
         * from offscreen memory to the display if doubleBufferOn is 1.
         */
        AltiaUpdate((ALTIA_WINDOW) devinfo,
            (ALTIA_UINT32) devinfo->gc, doubleBufferOn);

        /* The batch of graphics rendering operations is complete */
        if (startBatch == 1)
        {
            egl_BatchEnd(devinfo->gc);
            localInBatch = 0;
        }
#ifdef WIN32
        TargetAltiaSemRelease(2);
#endif
#if ALTIA_DOUBLE_BUFFERING
        /* Restore the drawable back to the screen */
        if (!noDisplayDraw)
            devinfo->drawable = devinfo->display;
#endif

#endif /* ALTIA_TASKING */
}

/*--- TargetAltiaAnimate() ------------------------------------------------
 *
 * This function is called to perform an Animate of the Altia graphics.
 * It is global so that other code outside of this file can call it.
 * If necessary, it can contain target specific logic as part of calling
 * the AltiaAnimate() function.  By having it here, other code that wants
 * to do an animate can be completely target independent.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from the Altia API code.
 *-------------------------------------------------------------------------*/
int TargetAltiaAnimate(ALTIA_CHAR *funcName, LocalEventType value)
{
    int retVal;

    if (localInitialized != 1)
        return 0;

    retVal = AltiaAnimate(funcName, value);

    return retVal;
}

/*--- TargetAltiaAnimateId() ------------------------------------------------
 *
 * This function is called to perform an Animate of the Altia graphics.
 * It is global so that other code outside of this file can call it.
 * If necessary, it can contain target specific logic as part of calling
 * the AltiaAnimate() function.  By having it here, other code that wants
 * to do an animate can be completely target independent.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from the Altia API code.
 *-------------------------------------------------------------------------*/
int TargetAltiaAnimateId(int nameId, LocalEventType value)
{
    int retVal;

    if (localInitialized != 1)
        return 0;

    retVal = AltiaAnimateId(nameId, value);

    return retVal;
}


/*--- TargetGetEvent() ----------------------------------------------------
 *
 * This function is called to perform a system event get/dispatch (such as
 * a screen refesh, mouse or keyboard event).  This function is intended
 * to block until an event is available.  Use TargetCheckEvent() for a
 * non-blocking request.  The content of this function is target specific.
 * By having it here, other code that is target independent can call
 * it to do target specific work.  For example, this function is used by
 * the standalone version of the Altia API AtNextEvent() function.
 * Returns 1 on success, 0 on failure and status parameter is set to point
 * to a suggested return value by the caller on failure.
 *
 * For this target, this function may get called recursively.  If there is
 * Altia control that uses a LOOP statement, it is possible that a call
 * to altiaHandleWinEvent() or altiaCallExpiredTimers() will trigger
 * additional calls to TargetGetEvent().  This function,
 * altiaHandleWinEvent(), and altiaCallExpiredTimers() must be written
 * to accept recursion.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from Altia API code.
 *-------------------------------------------------------------------------*/
ALTIA_BOOLEAN TargetGetEvent(int *status)
{
    ALTIA_BOOLEAN retval = 0;
    ALTIA_UINT32 waitTime = (ALTIA_UINT32) EGL_WAIT_FOREVER;
    EGL_EVENT event;
    AltiaDevInfo * devInfo = altiaLibGetDevInfo();

    *status = 0;

    /* If we are not initialized, don't try anything.
     * Also, if some other function in this file (like TargetSleep())
     * detected a reason to quit (for example, if we are windowed
     * and the user closed our window), act on it now.
     */
    if (localInitialized != 1 || localQuitDetected)
        return 0;

    /* Check if we have any running timers in case we need to set our
     * wait time to something finite in order to handle expiring
     * timers.  If the wait time is 0 (one or more timers are ready now),
     * set the wait time to the graphics library's "no wait" value.
     */
    if (altiaGetTimerWait(&waitTime) && waitTime == 0)
        waitTime = (ALTIA_UINT32) EGL_NO_WAIT;

    if (devInfo->qId != NULL)
    {

        int res = egl_EventGet(devInfo->qId, &event, sizeof(event),
            (EGL_TIMEOUT) waitTime);
        if (res == EGL_STATUS_Q_DATA)
        {
#if EGL_HAS_WINDOWCREATE
            /* If the target graphics library is using windowing,
             * we need to first handle a resize, expose, or close
             * of the window.
             */
            if (event.header.category == EGL_EVENT_CATEGORY_INPUT
                && event.header.type == EGL_EVENT_TYPE_RESIZE)
            {
                SampleResize(devInfo, &event, ALTIA_DOUBLE_BUFFERING);
            }
            else if (event.header.category == EGL_EVENT_CATEGORY_INPUT
                && event.header.type == EGL_EVENT_TYPE_EXPOSE)
            {
                SampleExpose(devInfo, &event, ALTIA_DOUBLE_BUFFERING);
            }
            else if (event.header.category == EGL_EVENT_CATEGORY_INPUT
                && event.header.type == EGL_EVENT_TYPE_CLOSE)
            {

                    TargetAltiaClose();

                    /* Return because DeepScreen code is now de-initialized. */
                    return 0;

            }
            else
#endif /* EGL_HAS_WINDOWCREATE */
            if (altiaHandleWinEvent((ALTIA_WINDOW)devInfo,
                (void *) &event))
            {
                TargetAltiaUpdate();
            }
        }else if (res == EGL_STATUS_QUIT)
        {
            return 0;
        }
#ifdef ALTIALIB
        /* All flavors of Altia API provide an _altiaBreakLoop global as of
         * 4/16/01.  It allows the API to be informed of a window close while
         * its looping for events from Altia.
         */
        if (_altiaBreakLoop)
            return 0;
#endif
        retval = 1;
    }
    else
    {
        /* Allow execution without any input queue (which implies there
         * is no input device).  This may be common on very simple
         * targets.
         */
        retval = 1;
    }

    /* If we used a finite wait time, call any expired timers and
     * update the graphics if necessary.
     */
    if (waitTime != (ALTIA_UINT32) EGL_WAIT_FOREVER
        && altiaCallExpiredTimers())
    {
        TargetAltiaUpdate();
        retval = 1;
    }

    return retval;
}


/*--- TargetCheckEvent() --------------------------------------------------
 *
 * This function is called to check for a new system event (such as
 * a screen refesh, mouse or keyboard event).  If one is available, it
 * is processed and then the function returns immediately.  For a
 * blocking version, see TargetGetEvent().  The content of this function
 * is target specific.  By having it here, other code that is target
 * independent can call it to do the target specific work.  This function
 * is used by the standalone version of the Altia API AtPending() function.
 * Returns 1 if an event was processed, 0 if no event was processed, and -1
 * if a failure occurred and the status parameter is set to point to
 * a suggested return value by the caller on failure.
 *
 * For this target, this function may get called recursively.  If there is
 * Altia control that uses a LOOP statement, it is possible that a call
 * to altiaHandleWinEvent() or altiaCallExpiredTimers() will trigger
 * additional calls to TargetGetEvent().  This function,
 * altiaHandleWinEvent(), and altiaCallExpiredTimers() must be written
 * to accept recursion.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from Altia API code.
 *-------------------------------------------------------------------------*/
int TargetCheckEvent(int *status)
{
    int retval = 0;
    ALTIA_UINT32 waitTime;
    EGL_EVENT event;
    AltiaDevInfo * devInfo = altiaLibGetDevInfo();

    *status = 0;

    /* If we are not initialized, don't try anything.
     * Also, if some other function in this file (like TargetSleep())
     * detected a reason to quit (for example, if we are windowed
     * and the user closed our window), act on it now.
     */
    if (localInitialized != 1 || localQuitDetected)
        return -1;

    if (devInfo->qId != NULL)
    {
        if (egl_EventGet(devInfo->qId, &event, sizeof(event), EGL_NO_WAIT)
            != EGL_STATUS_Q_EMPTY)
        {
            retval = 1;

#if EGL_HAS_WINDOWCREATE
            /* If the target graphics library is using windowing,
             * we need to first handle a resize, expose, or close
             * of the window.
             */
            if (event.header.category == EGL_EVENT_CATEGORY_INPUT
                && event.header.type == EGL_EVENT_TYPE_RESIZE)
            {
                SampleResize(devInfo, &event, ALTIA_DOUBLE_BUFFERING);
            }
            else if (event.header.category == EGL_EVENT_CATEGORY_INPUT
                && event.header.type == EGL_EVENT_TYPE_EXPOSE)
            {
                SampleExpose(devInfo, &event, ALTIA_DOUBLE_BUFFERING);
            }
            else if (event.header.category == EGL_EVENT_CATEGORY_INPUT
                && event.header.type == EGL_EVENT_TYPE_CLOSE)
            {

                    TargetAltiaClose();

                    /* Return because DeepScreen code is now de-initialized. */
                    return 0;

            }
            else
#endif /* EGL_HAS_WINDOWCREATE */
            if (altiaHandleWinEvent((ALTIA_WINDOW)devInfo, (void *)&event))
            {
                TargetAltiaUpdate();
            }
        }
#ifdef ALTIALIB
        /* All flavors of Altia API provide an _altiaBreakLoop global as of
         * 4/16/01.  It allows the API to be informed of a window close while
         * its looping for events from Altia.
         */
        if (_altiaBreakLoop)
            return -1;
#endif
    }

    /* Check if we have any running timers in case some have expired.
    */
    if (altiaGetTimerWait(&waitTime) && waitTime == 0)
    {
        retval = 1;
        if (altiaCallExpiredTimers())
            TargetAltiaUpdate();
    }

    return retval;
}


/*--- TargetGetCurTime() --------------------------------------------------
 *
 * This function gets the current time in milliseconds.  How to do this
 * is specific to a given target.  By having it here, other code that is
 * target independent can call it to do the target specific work.  For
 * example, this function is used when code is generated with timer
 * stimulus to determine when a timer has expired.
 *
 * WARNING:  The name for this function should not change because this
 *           function may get called from Altia API code for this target.
 *-------------------------------------------------------------------------*/
ALTIA_UINT32 TargetGetCurTime(void)
{
    ALTIA_UINT32 ret;
    OS_timespec curTime;

    if (os_clock_gettime(OS_CLOCK_REALTIME, &curTime) == 0)
    {
        ret = ((ALTIA_UINT32)(curTime.tv_sec) & 0x3fffff) * 1000;
        ret += curTime.tv_nsec / 1000000;
    }
    else
        ret = 0;

    return ret;
}


/*--- TargetSleep() -------------------------------------------------------
 *
 * This function implements a millisecond sleep.  How the sleep is performed
 * is specific to a given target.  By having it here, other code that is
 * target independent can call it to do the target specific work.  For
 * example, this function is used by the standalone version of the Altia API
 * altiaSleep() function.
 *
 * This function must allow processing of input events (including timer
 * stimulus expirations) for long sleep periods.  Otherwise, mouse,
 * keyboard, and timer stimulus will seem to stop working during the
 * sleep.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from Altia API code.
 *
 * WARNING:  This function will take semaphore "0" when it starts, release
 *           it while it sleeps, take it again if it needs to do any
 *           Altia graphics, and release it when it finally returns.
 *           It is assumed that the DeepScreen version of the Altia API
 *           altiaSleep() function does not use the same semaphore
 *           thereby leaving it up to this target specific code to
 *           determine if the semaphore needs to be used based on the
 *           type of operations required.
 *-------------------------------------------------------------------------*/
void TargetSleep(unsigned long millisecs)
{
    /* ALTIA_UINT32 clockRate = (ALTIA_UINT32) OS_CLOCKS_PER_SEC; */
    ALTIA_UINT32 partialSleep;
    ALTIA_BOOLEAN haveTimers;
    OS_timeval tv;
    ALTIA_UINT32 startTime = TargetGetCurTime();

    /* Get fundamental semaphore while we make calls into the graphics. */
    TargetAltiaSemGet(0);

    /* While we have timer stimulus that may expire during the sleep
     * period or if the sleep period exceeds 100 milliseconds, do
     * incremental sleeps in order to check for input events (including
     * timer stimulus expirations).
     */
    while ((haveTimers = altiaGetTimerWait(&partialSleep)) || millisecs > 100)
    {
        /* Never sleep for more than 100 milliseconds so we can catch
         * incoming events with some decent level of response.
         */
        if (!haveTimers || partialSleep > 100)
            partialSleep = 100;

        /* If the partial sleep time is less than the remaining sleep time,
         * than sleep for the partial sleep time, process any pending input
         * events (including timer stimulus expirations), and subtract the
         * amount of time used from our remaining sleep time.
         *
         * Otherwise, the remaining sleep time is short enough that we
         * should just sleep that amount and return.
         */
        if (partialSleep <= millisecs)
        {
            ALTIA_UINT32 newTime, elapsedTime;
            int status;

            /* If partial sleep time is not too short or it
             * is short, but we potentially have a lot of sleeping
             * to do, then we want to actually sleep so that we
             * give some CPU cycles to other tasks.  On the other
             * hand, if the partial sleep time is short and there
             * isn't much of it to do, don't bother sleeping.
             */
            if (partialSleep > 50
                || (partialSleep > 0 && (partialSleep * 5) < millisecs))
            {
                if (partialSleep > 50)
                    partialSleep -= 50;

                /* int ticks = (partialSleep * clockRate)/1000;
                * os_taskDelay(ticks);
                */

                tv.tv_sec = partialSleep/1000;
                tv.tv_usec = (partialSleep%1000) * 1000;

                /* Release semaphore just while we sleep. */
                TargetAltiaSemRelease(0);
                os_select(0,0,0,0,&tv);
                TargetAltiaSemGet(0);
            }

            /* Check for input events (this includes a check for
             * timer stimulus expirations).
             */
            TargetCheckEvent(&status);

            /* Compute exactly how much time has elapsed. */
            newTime = TargetGetCurTime();
            
            /* Check if the OS time rolled over */
            if (newTime < startTime)
            {
                elapsedTime = ((ALTIA_UINT32)(-1) - startTime) + newTime;
            }
            else
            {
                elapsedTime = newTime - startTime;
            }
            startTime = newTime;

            if (elapsedTime >= millisecs)
            {
                /* If the time it took to sleep and process input
                 * events was more than what is left of our sleep
                 * time, we are done!
                 */
                millisecs = 0;
                break;
            }
            else
            {
                /* We still have more time to sleep */
                millisecs -= elapsedTime;
            }
        }
        else
            break;
    }

    /* If we have any sleep time left after doing incremental sleeps
     * to process possible input events, go to sleep for the time
     * remaining.
     */
    if (millisecs)
    {
        ALTIA_UINT32 elapsedTime;

        /* Release semaphore while we sleep one last time. */
        TargetAltiaSemRelease(0);

        /* See how much time we actually have left for sleeping.
         * Always sleep for at least 1 msec to potentially give
         * up the CPU to other tasks.
         */
        elapsedTime = TargetGetCurTime() - startTime;
        if (elapsedTime >= millisecs)
            millisecs = 1;
        else
            millisecs -= elapsedTime;

        tv.tv_sec = millisecs/1000;
        tv.tv_usec = (millisecs%1000) * 1000;

        os_select(0,0,0,0,&tv);
        return;
    }

    /* Release semaphore and return. */
    TargetAltiaSemRelease(0);
}


/*--- TargetAltiaSetParentWindow() ----------------------------------------
 *
 * Function for external code to set parent window id to use for creating
 * Altia window.  It allows for placing the Altia window into an existing
 * window hierarchy.
 *-------------------------------------------------------------------------*/
#if EGL_HAS_WINDOWCREATE
EGL_WINDOW_ID parentWindowId = (EGL_WINDOW_ID) 0;
#endif
void TargetAltiaSetParentWindow(unsigned long windowId)
{
#if EGL_HAS_WINDOWCREATE
    parentWindowId = (EGL_WINDOW_ID) windowId;
#endif
}


/*--- TargetAltiaInitialize() ---------------------------------------------
 *
 * This function does the work of creating a Window for the current platform
 * and initializing the DeepScreen code.  It determines whether the Window
 * has already been initialized and does nothing in such a case.  This
 * allows it to be called more than once by unrelated sub-systems that
 * may want to insure that everything is initialized.
 * This function returns 0 if initialization failed.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from Altia API code.
 *-------------------------------------------------------------------------*/
int TargetAltiaInitialize(void)
{
    int fullScreen = 0;
    int width;
    int height;
    EGL_UINT32 driverId;
    int i;

#if EGL_HAS_WINDOWCREATE
    EGL_SIZE winWidth, winHeight;
#endif

    /* If we are already initialized, don't do anything. */
    if (localInitialized == 1)
        return 1;
    else if (localInitialized == 2)
        return 0; /* We had a previous error so return an error.*/

    /* Set initialized flag immediately so that we don't
     * attempt to initialize again even while we are
     * in the middle of initializing.
     */
    localInitialized = 1;

#ifdef ALTIALIB
    /* _altiaLogOutput("cerror.log"); */
#endif

#ifdef DEBUG_WINDOW
    SampleDebugOutputOpen();
#endif

#ifdef ALTIA_TASKING
    /* Initialize Tasking states */
    _taskWrite = 0;
    _taskRead = 0;
    _objectCount = 0;
    _objectCurrent = 0;
    _objectState = ALTIA_UPDATE_START;
#ifndef ALTIA_TASK_NO_FLUSH_OPTIMIZATION
    _taskFlush = -1;
#endif

    /* Status data */
    _taskStatus.size = altiaTaskQueue_count;
    _taskStatus.peak = 0;
    _taskStatus.error = 0;
    _objectStatus.size = altiaObjectQueue_count;
    _objectStatus.peak = 0;
    _objectStatus.error = 0;
    _objectStatus.time = 0;
    _objectStatus.id = 0;
#endif /* ALTIA_TASKING */

    /*******************************/
    /* INITIALIZE GRAPHICS DEVICE: */
    /*******************************/
#if EGL_HAS_DEVICEOPEN
    /* The target graphics library has a single function for
     * opening and initializing the display device and we don't
     * need an event service ID or font driver ID or event queue
     * ID.  This makes initialization much simpler.
     */
    if ((driverId = (EGL_UINT32) egl_DeviceOpen(NULL)) == 0)
    {
        _altiaErrorMessage(ALT_TEXT("GL display device open failed!"));

        /* If we don't have a display device, no sense in continuing */
        localInitialized = 2;
        return 0;
    }

    /* Initialize the device IDs for all layers to be just the one display device ID */
    for (i = 0; i < test_layers_count; i++)
    {
        test_layerDevInfo[i].devId = (EGL_DEVICE_ID) driverId;
        test_layerDevInfo[i].eventServiceId = (EGL_EVENT_SERVICE_ID) driverId;
        test_layerDevInfo[i].fontDrvId = (EGL_FONT_DRIVER_ID) driverId;
        test_layerDevInfo[i].qId = (EGL_EVENT_Q_ID) driverId;
    }
#else /* NOT EGL_HAS_DEVICEOPEN */

    /* The target graphics library needs to initialize itself and
     * then we need a display driver ID, event service ID, font
     * driver ID and event queue ID.
     */
    if (egl_Initialize() != EGL_STATUS_OK)
    {
        _altiaErrorMessage(ALT_TEXT("GL Initialize failed!"));

        /* If we don't have a graphics library, no sense in continuing */
        localInitialized = 2;
        return 0;
    }

    /* Obtain the first instance of the display device identifier */
    if (egl_DriverFind(EGL_DISPLAY_TYPE, 0, &driverId) != EGL_STATUS_OK)
    {
        driverId = 0;
        _altiaErrorMessage(ALT_TEXT("EGL_DISPLAY_TYPE Id not found!"));
    }

    for (i = 0; i < test_layers_count; i++)
        test_layerDevInfo[i].devId = (EGL_DEVICE_ID) driverId;

    /* Obtain the first instance of the event service identifier */
    if (egl_DriverFind(EGL_EVENT_SERVICE_TYPE, 0, &driverId) != EGL_STATUS_OK)
    {
        driverId = 0;
        _altiaErrorMessage(ALT_TEXT("GL has no input drivers"));
    }

    for (i = 0; i < test_layers_count; i++)
        test_layerDevInfo[i].eventServiceId = (EGL_DEVICE_ID) driverId;

    /* Obtain the first instance of the font driver identifier */
    if (egl_DriverFind(EGL_FONT_ENGINE_TYPE, 0, &driverId) != EGL_STATUS_OK)
    {
        driverId = 0;
        _altiaErrorMessage(ALT_TEXT("GL has no font drivers"));
    }

    for (i = 0; i < test_layers_count; i++)
        test_layerDevInfo[i].fontDrvId = (EGL_DEVICE_ID) driverId;
#endif /* EGL_HAS_DEVICEOPEN */

    /*****************************************************/
    /* CREATE A GRAPHICS CONTEXT AND SET ITS ATTRIBUTES: */
    /*                                                   */
    /* When creating a graphics context, suitable        */
    /* default values for most attributes are set during */
    /* the creation.                                     */
    /*****************************************************/
    for (i = 0; i < test_layers_count; i++)
    {

        test_layerDevInfo[i].colorDepth = egl_GetDepth(test_layerDevInfo[i].devId);
        test_layerDevInfo[i].gc = egl_GcCreate(test_layerDevInfo[i].devId, test_layerDevInfo[i].colorDepth);
        if (test_layerDevInfo[i].gc == NULL)
        {
            _altiaErrorMessage(ALT_TEXT("GL GC created seems bad (NULL)!"));

            /* If we don't have a gc, no sense in continuing */
#if EGL_HAS_DEVICEOPEN
            egl_DeviceClose(test_layerDevInfo[i].devId);
#else
            egl_Deinitialize();
#endif
            localInitialized = 2;
            return 0;
        }
    }

    /* Drawing operations on the gc are copies by default (versus
     * ORing, XORing, or ANDing).  Any routine that changes the mode
     * should set it back to copy when their operations are complete.
     */
    for (i = 0; i < test_layers_count; i++)
    {
        if (egl_RasterModeSet(test_layerDevInfo[i].gc, EGL_RASTER_OP_COPY) != EGL_STATUS_OK)
            _altiaErrorMessage(ALT_TEXT("Cannot set raster mode in GL GC!"));
    }

    /* Get the window size at code generation time.  If full screen
     * code generation was chosen, width and height will be -1.
     */
    _altiaGetWindowSize(&width, &height);

    /*******************************************************/
    /* OPTIONALLY CREATE A WINDOW OR USE DISPLAY DIRECTLY: */
    /*******************************************************/
#if EGL_HAS_WINDOWCREATE
    /* If the target graphics library is using windowing, ask it to create
     * the window and tell it the size of the window that we want or if we
     * want full screen mode.  It will return to us the actual window size
     * that it was able to create based on the arguments given as well as
     * any hardware constraints.
     */
    if ((width == -1 && height == -1))
        fullScreen = 1;

    winWidth = width;
    winHeight = height;

    test_layerDevInfo->drawable = egl_WindowCreate(test_layerDevInfo->devId,
                                                      parentWindowId,
                                                      &winWidth, &winHeight,
                                                      fullScreen, 
                                                      -1, 0, 0);
    if ((test_layerDevInfo->drawable == (EGL_WINDOW_ID) 0)
    {
        _altiaErrorMessage(ALT_TEXT("GL window creation failed (NULL)!"));

        /* If we don't have a window, no sense in continuing */
        egl_GcDestroy(test_layerDevInfo->gc);
#if EGL_HAS_DEVICEOPEN
        egl_DeviceClose(test_layerDevInfo->devId);
#else
        egl_Deinitialize();
#endif
        test_layerDevInfo->gc = NULL;
        localInitialized = 2;
        return 0;
    }
    else
    {
        Altia_Extent_type winExtent;

        test_layerDevInfo->winId = test_layerDevInfo->drawable;
        egl_Info(test_layerDevInfo->devId, EGL_MODE_INFO_REQ, (void *)test_layerDevInfo, 0);

        width = winWidth;
        height = winHeight;

        winExtent.x0 = (ALTIA_COORD)0;
        winExtent.y0 = (ALTIA_COORD)0;

        if (width <= 0)
            winExtent.x1 = (ALTIA_COORD)0;
        else
            winExtent.x1 = (ALTIA_COORD)(width - 1);

        if (height <= 0)
            winExtent.y1 = (ALTIA_COORD)0;
        else
            winExtent.y1 = (ALTIA_COORD)(height - 1);

        /* Record the window extent for others to get.
         * It is important to note that we adjusted y1 to
         * (height - 1) because we will use y1 extensively to
         * convert Altia y coordinates (which have their origin
         * at the bottom of the window) to target y coordinates
         * (which have their origin at the top of the window).
         * If winExtent.y1 is (height - 1), then an Altia y
         * coordinate converts to a target window coordinate
         * with (winExtent.y1 - y).
         */
        altiaLibSetWindowExtent((ALTIA_WINDOW)test_layerDevInfo, &winExtent);

        /* With a windowing system, record the window extent also as the
         * full extent since we cannot draw outside of the window.
         */
        altiaLibSetFullWindowExtent((ALTIA_WINDOW)test_layerDevInfo, &winExtent);
    }
#else /* NOT EGL_HAS_WINDOWCREATE */
    /* Without windowing, we are implicitly in full screen mode.
     * However, we still will not draw into the areas of the display
     * that are not occupied by Altia graphics so there is a difference
     * between the size of the screen and the size of our area of the
     * screen if width and height are both -1.
     */
    fullScreen = 1;

    if (fullScreen || (width == -1 && height == -1))
    {
        Altia_Extent_type fullExtent;

        fullExtent.x0 = (ALTIA_COORD)0;
        fullExtent.x1 = (ALTIA_COORD)0;
        fullExtent.y0 = (ALTIA_COORD)0;
        fullExtent.y1 = (ALTIA_COORD)0;

        /* We have a device id and gc at this point so there should
         * be no problem getting the full size of the display.
         * It is needed for bounding the suggested width and height
         * or getting a width and height if there are no suggestions.
         * Also, we may need this information for drawing to the display.
         */
        for (i = 0; i < test_layers_count; i++)
        {
            test_layerDevInfo[i].colorDepth = 8;

            if (egl_Info(test_layerDevInfo[i].devId, EGL_MODE_INFO_REQ, 
                         (void *)&test_layerDevInfo[i], i) == EGL_STATUS_OK)
            {
                /* Capture largest drawable dimensions so all layers can be appropriately drawn */
                if (fullExtent.x1 < (ALTIA_COORD)(test_layerDevInfo[i].drawable->width - 1))
                    fullExtent.x1 = (ALTIA_COORD)(test_layerDevInfo[i].drawable->width - 1);
                if (fullExtent.y1 < (ALTIA_COORD)(test_layerDevInfo[i].drawable->height - 1))
                    fullExtent.y1 = (ALTIA_COORD)(test_layerDevInfo[i].drawable->height - 1);
            }
        }

        /* Record the full display extent for others to get. */
        altiaLibSetFullWindowExtent((ALTIA_WINDOW)test_layerDevInfo, &fullExtent);

        /* For the purposes of getting the extent of the window to
         * determine what needs to be drawn, the window size at code
         * generation time is used if it is valid.  If it is not
         * valid, use the full display size as the extent of the
         * window.
         */
        if (width != -1 || height != -1)
        {
            Altia_Extent_type winExtent;

            winExtent.x0 = 0;
            winExtent.y0 = 0;

            if (width <= 0)
                winExtent.x1 = (ALTIA_COORD)0;
            else if (width >= fullExtent.x1)
                winExtent.x1 = (ALTIA_COORD)fullExtent.x1;
            else
                winExtent.x1 = (ALTIA_COORD)(width - 1);

            if (height <= 0)
                winExtent.y1 = (ALTIA_COORD)0;
            else if (height >= fullExtent.y1)
                winExtent.y1 = (ALTIA_COORD)fullExtent.y1;
            else
                winExtent.y1 = (ALTIA_COORD)(height - 1);

            /* Record the window extent for others to get.
             * It is important to note that we adjusted y1 to
             * (height - 1) because we will use y1 extensively to
             * convert Altia y coordinates (which have their origin
             * at the bottom of the window) to target y coordinates
             * (which have their origin at the top of the window).
             * If winExtent.y1 is (height - 1), then an Altia y
             * coordinate converts to a target window coordinate
             * with (winExtent.y1 - y).
             */
            altiaLibSetWindowExtent((ALTIA_WINDOW)test_layerDevInfo, &winExtent);
        }
        else
        {
            /* Record the full display as the window extent. */
            altiaLibSetWindowExtent((ALTIA_WINDOW)test_layerDevInfo, &fullExtent);
        }

        /* The width and height variables will be used to create an
         * offscreen bitmap for double buffering if double buffering
         * is enabled.  In full screen mode, we want the offscreen
         * bitmap to be the size of the full display.
         */
        width = fullExtent.x1 + 1;
        height = fullExtent.y1 + 1;

        fullScreen = 1;
    }
#endif /* EGL_HAS_WINDOWCREATE */

    /**********************************************************************/
    /* PREPARE FOR DIRECT DRAWING AND OPTIONALLY SET UP DOUBLE BUFFERING: */
    /**********************************************************************/

    /* By default, set up display as the drawing area.
     * Set up double buffering next, if it is desired.
     */
    for (i = 0; i < test_layers_count; i++)
        test_layerDevInfo[i].memBitmap = EGL_DISPLAY_ID;

    /* Ready to set up double buffering if it is desired.
     * Double buffering provides smooth updating of changing display
     * areas without flashing or flicker.  It is desirable in almost
     * all applications.  For a small display, it may be possible to
     * draw directly to it and not see any flashing or flicker.  By
     * default, ALTIA_DOUBLE_BUFFERING is set to 1 earlier in this
     * file to enable double buffering.
     */
#if ALTIA_DOUBLE_BUFFERING
    {
        /* Initialize a device independent bitmap (DIB) for creating a
         * a device dependent bitmap (DDB) that we can use as our offscreen
         * drawing area.  Note that we really only have to set the DIB
         * width and height.  The other elements are not used because we
         * are just going to initialize the entire DDB with zeros (0).
         */
        EGL_DIB dib;
        dib.width = width;
        dib.height = height;
        dib.imageFormat = 0;
        dib.colorFormat = 0;
        dib.clutSize = 0;
        dib.pClut = NULL;
        dib.pImage = NULL;

        /* Try to create the bitmap in the device's default memory pool.
         * If that doesn't work, then we will draw directly to the display
         * (i.e., we won't have double buffering).
         */
        for (i = 0; i < test_layers_count; i++)
        {

            test_layerDevInfo[i].memBitmap = egl_BitmapCreate(test_layerDevInfo[i].devId, &dib,
                                                                 EGL_DIB_INIT_VALUE,
                                                                 (ALTIA_UINT32) 0, NULL);
            if (test_layerDevInfo[i].memBitmap == NULL)
            {
                test_layerDevInfo[i].memBitmap = EGL_DISPLAY_ID;
                _altiaErrorMessage(ALT_TEXT("Cannot set double buffer in GL GC!"));
            }
            else if (noDisplayDraw)
            {
                /* We are not to draw to the display so set it so
                 * we draw to the double buffer mem bitmap
                 */
                test_layerDevInfo[i].drawable = test_layerDevInfo[i].memBitmap;
            }
        }

    }
#endif /* ALTIA_DOUBLE_BUFFERING == 1 */

#ifdef ALTIALIB
    /******************************
     * Verify that we are using   *
     * the integer API            * 
     ******************************/
    if (EVENTSIZE != 4)
        _altiaErrorMessage(
            ALT_TEXT("Must use Integer API in Fixed Point mode"));
#endif
 
    /* This doesn't actually draw the graphics.  The first rendering
     * of graphics to the display happens a little bit later.
     * The initialization performed here is for items like fonts,
     * patterns, routing of the "altiaInitDesign" event, and other
     * chores.
     */
    if (ALTIA_FALSE == _altiaInit((ALTIA_WINDOW)test_layerDevInfo))
    {
        _altiaErrorMessage(ALT_TEXT("Failed to initialize Altia"));
        return 0;
    }

    /* SET UP ALTIA EVENT FUNCTION POINTER IF DESIRED:
     * If you want to receive Altia events when they occur.  Set the
     * AltiaReportFuncPtr value to the procedure you want to be called.
     * Note that if the Altia API is used, it will set this function
     * pointer to one of its routines so that it can receive Altia Events.
     * The application can get these events from altiaNextEvent/AtNextEvent.
     *
     * AltiaReportFuncPtr = SampleReportAnimation;
     */

    /* Tell the driver that initialization is complete.  This
     * allows for any final things to happen. For multiple layers
     * we use the device ID of the first, this is ok as they are
     * all the same.
     */
    egl_FinishInit(test_layerDevInfo->devId);

    /*********************************************************/
    /* DRAW ALTIA GRAPHICS TO THE SCREEN FOR THE FIRST TIME: */
    /*********************************************************/
#if EGL_USE_INIT_REFRESH
    /* Don't have to do the first graphics drawing since the refresh
     * event will cause it to happen.  
     */
#else
    {
        /* We use the SampleRedraw() routine to do this.  For this target,
         * SampleRedraw() knows how to draw all objects in the entire
         * extent of the design or screen.
         */
        Altia_Extent_type extent;
        altiaLibGetFullWindowExtent((ALTIA_WINDOW)test_layerDevInfo, &extent);

        /* This is a full extent draw so there is no reason to handle
         * any partial extents that were added as a result of routing
         * the "altiaInitDesign" event.  It can be a problem for some
         * targets if any of these extents are offscreen so just ignore
         * all partial extents that were added to this point.
         */
        test_extents_count = 0;
        SampleRedraw(test_layerDevInfo, ALTIA_DOUBLE_BUFFERING, &extent);
    }
#endif /* EGL_USE_INIT_REFRESH */

    /***********************************************************************/
    /* OPTIONALLY FINISH DEVICE INITIALIZATION BY CREATING AN EVENT QUEUE: */
    /***********************************************************************/
#if !defined(EGL_HAS_DEVICEOPEN) || (EGL_HAS_DEVICEOPEN < 1)
    /* If the target graphics library does not have a single
     * function for opening and initializing the display device,
     * we assume we must explicitly set up (and later destroy)
     * an event queue for handling input (mouse and keyboard)
     * events.
     */
    for (i = 0; i < test_layers_count; i++)
    {
        if (test_layerDevInfo[i].eventServiceId != (EGL_EVENT_SERVICE_ID) 0)
        {
            test_layerDevInfo[i].qId = egl_EventQCreate(test_layerDevInfo[i].eventServiceId, 100);
            if (test_layerDevInfo[i].qId == NULL)
                _altiaErrorMessage(ALT_TEXT("Cannot create GL input queue!"));
        }
        else
            test_layerDevInfo[i].qId = NULL;
    }
#endif /* NOT EGL_HAS_DEVICEOPEN || EGL_HAS_DEVICEOPEN < 1 */

    /***********************************************************************/
    /* CREATE AND SETUP THE SCREEN CURSOR (THIS MAY ACTUALLY DO NOTHING)   */
    /***********************************************************************/

    /* Don't do anything with a cursor if there is no event queue.
     * Cursor positioning information comes from mouse motion events,
     * but there are no mouse motion events without an event queue.
     */
    for (i = 0; i < test_layers_count; i++)
    {
        if (test_layerDevInfo[i].qId != NULL)
            SampleCursorCreate(&test_layerDevInfo[i]);
    }

#ifdef ALTIA_TASKING
    /* Force animations to queue */
    AltiaTaskRunning = ALTIA_FALSE;
#endif
    return 1;
}


/*--- TargetAltiaClose() --------------------------------------------------
 *
 * This function "closes" the DeepScreen code and cleans up the Window for
 * the current platform.  It determines whether the Window has already
 * been closed and does nothing in such a case.  This allows it to be
 * called more than once by unrelated sub-systems that may want to insure
 * that everything is cleaned up.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from Altia API code.
 *-------------------------------------------------------------------------*/
void TargetAltiaClose(void)
{
    /* If there is something to close, then close it. */
    if (localInitialized == 1)
    {
        int i;
#ifdef ALTIA_TASKING
        /* Finish processing the queue until it's empty */
        TargetAltiaTaskRun(0);

        /* Disable aniamtion queuing */
        AltiaTaskRunning = ALTIA_TRUE;
#endif

#ifdef WIN32
        /* Get secondary semaphore while we make calls into the graphics. */
        TargetAltiaSemGet(2);
#endif
        /* VERY IMPORTANT TO RESET INITIALIZED FLAG IMMEDIATELY!
         * It minimizes the chance that another task attempts a
         * close at the same time.
         */
        localInitialized = 0;

        /* Independent of how main processing is done, we should
         * end up here where we can close the Altia window, do 
         * any platform specific closes, and return a value.
         */
        _altiaClose();

        for (i = 0; i < test_layers_count; i++)
        {

            if (test_layerDevInfo[i].qId != NULL)
            {
                /* Close and destroy the cursor.  This may actually do nothing
                 * if cursor handling is not under the control of the graphics
                 * library.
                 */
                SampleCursorDestroy(&test_layerDevInfo[i]);

#if !defined(EGL_HAS_DEVICEOPEN) || (EGL_HAS_DEVICEOPEN < 1)
                /* If the target graphics library does not have a
                 * single function for opening and initializing the
                 * display device, we assume we must explicitly 
                 * destroy an event queue that we previously created
                 * for handling input (mouse and keyboard) events.
                 */
                egl_EventQDestroy(test_layerDevInfo[i].eventServiceId, test_layerDevInfo[i].qId);
#endif
                test_layerDevInfo[i].qId = NULL;
            }

#if EGL_HAS_WINDOWCREATE
            /* The target graphics library is using windowing,
             * ask it to destroy the window previously creating.
             */
            if (test_layerDevInfo[i].winId != (EGL_WINDOW_ID) 0)
            {
                egl_WindowDestroy(test_layerDevInfo[i].devId, test_layerDevInfo[i].winId);
                test_layerDevInfo[i].winId = (EGL_WINDOW_ID) 0;
            }
#endif /* EGL_HAS_WINDOWCREATE */

            if (test_layerDevInfo[i].gc != NULL)
            {
                egl_GcDestroy(test_layerDevInfo[i].gc);
                test_layerDevInfo[i].gc = NULL;
            }

            if (test_layerDevInfo[i].memBitmap != EGL_DISPLAY_ID)
            {
                egl_BitmapDestroy(test_layerDevInfo[i].devId, test_layerDevInfo[i].memBitmap, 0);
                test_layerDevInfo[i].memBitmap = EGL_DISPLAY_ID;
            }
        }

        /* If the target graphics library has a single function
         * for opening and initializing the display device,
         * assume it has a complimentary function for closing
         * the display device.  Otherwise, use the alternative
         * function to deinitialize the graphics library.
         */
#if EGL_HAS_DEVICEOPEN
        /* Use the device ID from the first layer -- they are all the same */
        egl_DeviceClose(test_layerDevInfo->devId);
#else
        egl_Deinitialize();
#endif
        for (i = 0; i < test_layers_count; i++)
        {
            test_layerDevInfo[i].devId = (EGL_DEVICE_ID) 0;
            test_layerDevInfo[i].eventServiceId = (EGL_EVENT_SERVICE_ID) 0;
            test_layerDevInfo[i].fontDrvId = (EGL_FONT_DRIVER_ID) 0;
        }

        /* Clear our quit detection flag since we are all closed. */
        localQuitDetected = 0;
#ifdef WIN32
        TargetAltiaSemRelease(2);
#endif
    }
    else
        localInitialized = 0;
}


/*--- TargetAltiaLoop() ---------------------------------------------------
 *
 * This function provides a simple event processing loop that can
 * be used if there is no user supplied main() that has substance to it.
 * It returns a suggested program return value (but never ALTIA_NO_USER_MAIN
 * because this loop has substance to it!).
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
int TargetAltiaLoop(ALTIA_CHAR *chosenPort)
{
    int retval = 0;

#ifdef ALTIALIB
    AltiaEventType value;
    LocalEventType localValue;
    ALTIA_BOOLEAN need_update;
    int event_count;
#if defined(UNICODE) && !defined(ALTIAUNICODEAPI)
    AltiaCharType *eventPtr;
#endif

#if defined(UNICODE) && !defined(ALTIAUNICODEAPI)
    ALTIA_CHAR localEvent[256];
#else
    ALTIA_CHAR *localEvent;

#endif /* UNICODE && !ALTIAUNICODEAPI */

#ifdef UNDER_CE
    ALTIA_CHAR defaultPort[64];
#endif /* UNDER_CE */
#endif /* ALTIALIB */

    /* Initialize Altia window. */
    if (TargetAltiaInitialize() == 0)
        return 1;

#ifndef ALTIALIB
    while (TargetGetEvent(&retval))
    {
        /* Just keep getting messages while it is working. */
    #ifdef ALTIA_TASKING
        /* Pump the task */
        TargetAltiaTaskRun(0);
    #endif
        /* If we don't have any input driver, TargetGetEvent() is
         * just going to return immediately.  In this case, sleep a
         * bit so that we don't use up all of the system CPU cycles.
         * Check the Queue ID for just the first layer.
         */
        if (test_layerDevInfo->qId == NULL)
            TargetSleep((unsigned long) 50);
    }
#else
    /* If we are using the Altia API, then do the
     * connect and process Altia events (which indirectly
     * gets and dispatches Windows messages for us).
     */
    if (chosenPort == NULL || chosenPort[0] == '\0')
    {
    #ifndef UNDER_CE
        chosenPort = NULL;
    #else
        /* To connect from a target to an Altia running on a host,
         * comment out setting cmdPtr to NULL and set cmdPtr to
         * reference socket 5100 on the host.  An example for
         * a host named "host" is provided.
         *
         * IF YOU CHANGE THIS SETTING, you will need to copy your
         * new version of this altiaUtils.c to the master version
         * found in the Altia usercode\windows directory.  Otherwise,
         * your change will get overwritten the next time code is
         * generated.
         */
        ALT_STRCPY(defaultPort, HOSTNAME);
        ALT_STRCAT(defaultPort, ALT_TEXT(":"));
        ALT_STRCAT(defaultPort, PORT);
        chosenPort = defaultPort;
    #endif /* UNDER_CE */
    }

    /* If we are on a UNICODE target, but API is not UNICODE, then
     * we need to convert chosen port name from a UNICODE string to
     * a normal 8-bit string that the API expects.
     */
    #if defined(UNICODE) && !defined(ALTIAUNICODEAPI)
    {
        AltiaCharType convertedPort[64];
        wcstombs(convertedPort, chosenPort,
            sizeof(convertedPort)/sizeof(AltiaCharType));
        if (altiaConnect(convertedPort) == -1)
            return 0;
    }
    #else
    if (altiaConnect(chosenPort) == -1)
        return 0;
    #endif /* UNICODE && !ALTIAUNICODEAPI */

    altiaSelectAllEvents();

    /* Must have 2 versions of while instead of just assigning
     * eventPtr to localEvent if we don't need mbstowcs().  Otherwise,
     * MS C 4.2 compiler crashes!
     */
    need_update = 0;
    event_count = 0;
    do
    {
    #if defined(UNICODE) && !defined(ALTIAUNICODEAPI)
        if (altiaNextEvent(&eventPtr, &value) != -1)
    #else
            if (altiaNextEvent(&localEvent, &value) != -1)
    #endif
            {
                /* If we are on a UNICODE target, but API is not UNICODE,
                 * must convert API event name string to UNICODE string.
                 */
    #if defined(UNICODE) && !defined(ALTIAUNICODEAPI)
                mbstowcs(localEvent, eventPtr,
                    sizeof(localEvent)/sizeof(LocalEventType));
    #endif
                /* If we are using an integer API and value is going to a
                 * float, value from API should be a float unioned into an int
                 * so squeeze it back out of the int into a local type.
                 */
    #if defined(ALTIAINTAPI) || !defined(ALTIAFLOAT)
                if (_altiaConnectionToFloat(localEvent))
                {
                    union
                    {
                        int ival;
                        float fval;
                    } tempValue;
                    tempValue.ival = value;
                    localValue = (LocalEventType) tempValue.fval;
                }
                else
                    localValue = value;
    #else
                localValue = value;
    #endif
                if (TargetAltiaAnimate(localEvent, localValue))
                {
                    /* TargetAltiaUpdate(); */
                    need_update = 1;
                }
            }
            else
                break;

        if (--event_count <= 0)
        {
            if (need_update)
                TargetAltiaUpdate();
            need_update = 0;
            event_count = altiaPending();
    #ifdef ALTIA_TASKING
            /* Pump the task */
            TargetAltiaTaskRun(0);
    #endif
        }
    } while (event_count >= 0);

    retval = 0;
    altiaDisconnect();
#endif /* ALTIALIB */

    /* Close Altia window. */
    TargetAltiaClose();

    /* Never return ALTIA_NO_USER_MAIN because this loop has
     * completed all of the event processing.
     */
    if (retval == ALTIA_NO_USER_MAIN)
        return 0;

    return retval;
}


/*--- SampleDebugOutputOpen() ---------------------------------------------
 *
 * Function to create a console window for stdout/stderr debug if needed.
 *-------------------------------------------------------------------------*/
#ifdef DEBUG_WINDOW
void SampleDebugOutputOpen(void)
{
    egl_OpenDebugWindow();
}
#endif


/*--- SampleReportAnimation() ---------------------------------------------
 *
 * If TargetAltiaInitialize() sets the AltiaReportFuncPtr function pointer
 * to reference this function, it will get called by AltiaAnimate to inform
 * the user that an altia Animation has occured.  Please note that if
 * the Altia API is used, it will set the AltiaReportFuncPtr function
 * pointer to one of its routines so that it can receive Altia Events.
 *-------------------------------------------------------------------------*/
void SampleReportAnimation(ALTIA_CHAR *name, LocalEventType value)
{
#ifdef DEBUG_WINDOW
    /* printf("%s %g\n", name, (double) value); */
#endif
}


/*--- SampleRedraw() ------------------------------------------------------
 *
 * This function performs a total redraw of the screen. This is useful
 * at initialization time.
 *-------------------------------------------------------------------------*/
void SampleRedraw(AltiaDevInfo *devInfoPtr, ALTIA_BOOLEAN doubleBufferOn,
                  Altia_Extent_type *extent)
{
    int changedDrawable = 0;
    int startBatch = 0;



#ifdef WIN32

    int saveExtent;

#endif

    /* No draw if override is enabled */
    if (_altiaOverrideUpdate)
        return;

    if (devInfoPtr == NULL)
        devInfoPtr = altiaLibGetDevInfo();


        /* Perform a batch of graphics rendering operations. Starting a
         * batch operation on this target locks shared display resources
         * to the calling thread until the batch is ended.  If the
         * resources are already locked by another thread, we will block.
         */
        devInfoPtr = altiaLibGetDevInfo();


#ifdef WIN32
        saveExtent = test_extents_count;
        TargetAltiaSemGet(2);
        test_extents_count = saveExtent;
#endif

        if (localInBatch == 0)
        {
            startBatch = 1;
            if (egl_BatchStart(devInfoPtr->gc) == EGL_STATUS_ERROR)
            {
#ifdef WIN32
                TargetAltiaSemRelease(2);
#endif
                return;
            }
            localInBatch = 1;
            /* NOTE: Do not return without clearing localInBatch! */
        }

        if (noDisplayDraw && devInfoPtr->drawable == devInfoPtr->display)
        {
            /* We are not to draw to the display but the display was passed
             * in as what we are to draw to.  So change drawable to 
             * the memory bitmap;
             */
            devInfoPtr->drawable = devInfoPtr->memBitmap;
            changedDrawable = 1;
        }
        /* Note that we are sending Altia coords in the extent.
         * They have y0 = 0 on the bottom.
         */

        /* If we are NOT suppose to use double buffering, we are going
         * directly to the display.
         */

        _altiaDrawExtent((ALTIA_WINDOW)devInfoPtr, (ALTIA_UINT32)(devInfoPtr->gc), extent);

        /* If double buffering is on, copy buffer to the display. */
        if (doubleBufferOn)
            altiaLibCopy((ALTIA_WINDOW)devInfoPtr, (ALTIA_UINT32)(devInfoPtr->gc), extent);

        /* The batch of graphics rendering operations is complete */
        if (startBatch == 1)
        {
            egl_BatchEnd(devInfoPtr->gc);
            localInBatch = 0;
        }
#ifdef WIN32
        TargetAltiaSemRelease(2);
#endif
        if (changedDrawable)
        {
            /* We are not to draw to the display but the display was passed
             * in as what we are to draw to and since we changed it we
             * need to change it back.
             */
            devInfoPtr->drawable = devInfoPtr->display;
            changedDrawable = 0;
        }

}


/*--- SampleCursorCreate() ------------------------------------------------
 *
 * This function can handle creation and setup of the cursor if it is not
 * handled automatically by the graphics library.
 *-------------------------------------------------------------------------*/
#if EGL_USE_CURSOR_HANDLING
static EGL_CDDB_ID pointerImage = NULL;
#endif
void SampleCursorCreate(AltiaDevInfo *devInfoPtr)
{
#if EGL_USE_CURSOR_HANDLING
    /* color of cursor */
    static EGL_ARGB cursorClut[] =
    {
        EGL_MAKE_ARGB (0xff,0x00, 0x00, 0x00),
        EGL_MAKE_ARGB (0xff,0xFF, 0xFF, 0xFF)
    };

    /* bitmap of cursor image */
    static ALTIA_UBYTE pointerData[] =
    {
#define T EGL_CURSOR_COLOR_TRANSPARENT,
#define B 0,
#define W 1,
        B T T T T T T T T T T
        B B T T T T T T T T T
        B W B T T T T T T T T
        B W W B T T T T T T T
        B W W W B T T T T T T
        B W W W W B T T T T T
        B W W W W W B T T T T
        B W W W W W W B T T T
        B W W W W W W W B T T
        B W W W W W W W W B T
        B W W W W W B B B B B
        B W W B W W B T T T T
        B W B T B W W B T T T
        B B T T B W W B T T T
        B T T T T B W W B T T
        T T T T T B W W B T T
        T T T T T T B W W B T
        T T T T T T B W W B T
        T T T T T T T B B T T
#undef T
#undef B
#undef W
    };

    EGL_CDIB pointerDib;
    EGL_POS cursorX;
    EGL_POS cursorY;
    Altia_Extent_type extent;

    /* Put the cursor in the middle of the display to start with */
    altiaLibGetFullWindowExtent((ALTIA_WINDOW) devInfoPtr, &extent);
    cursorX = (extent.x1 - extent.x0 + 1) / 2;
    cursorY = (extent.y1 - extent.y0 + 1) / 2;

    if (egl_CursorInit(devInfoPtr->devId, 20, 20,
        cursorX, cursorY) != EGL_STATUS_OK)
    {
        _altiaErrorMessage(ALT_TEXT("GL cursor initialization failed!"));
        return;
    }

    pointerDib.width = 11;
    pointerDib.stride = 11;
    pointerDib.height = 19;
    pointerDib.hotSpot.x = 0;
    pointerDib.hotSpot.y = 0;
    pointerDib.pClut = cursorClut;
    pointerDib.clutSize = 2;
    pointerDib.pImage = pointerData;

    if ((pointerImage = egl_CursorBitmapCreate(devInfoPtr->devId,
        &pointerDib)) == NULL)
    {
        _altiaErrorMessage(ALT_TEXT("GL cursor bitmap creation failed!"));
        return;
    }

    if (egl_CursorImageSet(devInfoPtr->devId, pointerImage) != EGL_STATUS_OK)
    {
        _altiaErrorMessage(ALT_TEXT("GL cursor bitmap set failed!"));
        return;
    }

    if (egl_CursorOn(devInfoPtr->devId) != EGL_STATUS_OK)
        _altiaErrorMessage(ALT_TEXT("GL cursor show failed!"));
#endif /* EGL_USE_CURSOR_HANDLING */
}


/*--- SampleCursorDestroy() -----------------------------------------------
 *
 * This function can handle destroying the cursor if it is not
 * handled automatically by the graphics library.
 *-------------------------------------------------------------------------*/
void SampleCursorDestroy(AltiaDevInfo *devInfoPtr)
{
#if EGL_USE_CURSOR_HANDLING
    egl_CursorDeinit(devInfoPtr->devId);
    if (pointerImage != NULL)
        egl_CursorBitmapDestroy(devInfoPtr->devId, pointerImage);
    pointerImage = NULL;
#endif /* EGL_USE_CURSOR_HANDLING */
}


/*--- SampleResize() ------------------------------------------------------
 *
 * This function can handle a resize of the window in a windowing system
 * environment. 
 *-------------------------------------------------------------------------*/
void SampleResize(AltiaDevInfo *devInfoPtr, EGL_EVENT *eventPtr,
    ALTIA_BOOLEAN doubleBufferOn)
{
#if EGL_HAS_WINDOWCREATE
    AltiaDevInfo *devInfo = devInfoPtr;
    int startBatch = 0;

    /* If the target graphics library is using windowing, process a
     * resize event if that is what we have.
     */
    if (eventPtr->header.category != EGL_EVENT_CATEGORY_INPUT
        || eventPtr->header.type != EGL_EVENT_TYPE_RESIZE)
    {
        return;
    }
    else
    {
        EGL_INPUT_EVENT *inputEventPtr = (EGL_INPUT_EVENT *) eventPtr;
#ifdef WIN32
        int saveExtent;
#endif

        /* Only handle an expose for our window */
        if (inputEventPtr->window == (EGL_UINT32) devInfoPtr->winId

            )
        {
            Altia_Extent_type extent;
            int newWidth = inputEventPtr->type.resize.width;
            int newHeight = inputEventPtr->type.resize.height;
            int oldWidth;
            int oldHeight;

            /* By definition, if we get a resize event, we must be
             * in a windowing system.  If the window size (which is
             * also the full window size when we are in a windowing
             * environment) is actually different, fix it and fix
             * the offscreen bitmap size.
             */

            {
                altiaLibGetFullWindowExtent((ALTIA_WINDOW) devInfo, &extent);
                oldWidth =  extent.x1 - extent.x0 + 1;
                oldHeight =  extent.y1 - extent.y0 + 1;
            }

            if ((oldWidth) != newWidth || (oldHeight) != newHeight)
            {
#ifdef DEBUG_WINDOW
                printf("Resize for win 0x%x, from %dx%d to %dx%d\n",
                    (unsigned long) inputEventPtr->window,
                    extent.x1 - extent.x0 + 1, extent.y1 - extent.y0 + 1,
                    newWidth, newHeight);
#endif


                    extent.x0 = (ALTIA_COORD)0;
                    extent.y0 = (ALTIA_COORD)0;
                    extent.x1 = (ALTIA_COORD)(newWidth - 1);
                    extent.y1 = (ALTIA_COORD)(newHeight - 1);

                    altiaLibSetWindowExtent((ALTIA_WINDOW) devInfo,
                        &extent);
                    altiaLibSetFullWindowExtent((ALTIA_WINDOW) devInfo,
                        &extent);

#ifdef WIN32
                saveExtent = test_extents_count;
                TargetAltiaSemGet(2);
                test_extents_count = saveExtent;
#endif
                if (localInBatch == 0)
                {
                    startBatch = 1;
                    if (egl_BatchStart(devInfoPtr->gc) == EGL_STATUS_ERROR)
                    {
#ifdef WIN32
                        TargetAltiaSemRelease(2);
#endif
                        return;
                    }
                    localInBatch = 1;
                    /* NOTE: Do not return without clearing localInBatch! */
                }

                /* If double buffering is on, create a new double buffer. */
                if (doubleBufferOn)
                {
                    if (devInfo->memBitmap != EGL_DISPLAY_ID)
                    {
                        EGL_DIB dib;
                        int setDrawable = 0;

                        if (devInfo->drawable == devInfo->memBitmap)
                            setDrawable = 1;
                        egl_BitmapDestroy(devInfo->devId,
                            (EGL_DDB_ID) devInfo->memBitmap, 
                            0);

                        dib.width = newWidth;
                        dib.height = newHeight;
                        dib.imageFormat = 0;
                        dib.colorFormat = 0;
                        dib.clutSize = 0;
                        dib.pClut = NULL;
                        dib.pImage = NULL;

                        /* Try to create the bitmap in the device's default
                         * memory pool.  If that doesn't work, then we will
                         * draw directly to the display (i.e., we won't have
                         * double buffering).
                         */
                        if ((devInfo->memBitmap
                            = egl_BitmapCreate(devInfo->devId, &dib,
                            EGL_DIB_INIT_VALUE,
                            (ALTIA_UINT32) 0, NULL)) == NULL)
                        {
                            devInfo->memBitmap = EGL_DISPLAY_ID;
                            _altiaErrorMessage(
                                ALT_TEXT("Cannot set double buffer in GL GC!"));
                        }
                        if (setDrawable)
                            devInfo->drawable = devInfo->memBitmap;
                    }
                }

                /* Redraw the window now. */
                _altiaDrawExtent((ALTIA_WINDOW) devInfo,
                (ALTIA_UINT32) (devInfo->gc), &extent);

                /* If double buffering is on, copy buffer to the display. */
                if (doubleBufferOn)
                    altiaLibCopy((ALTIA_WINDOW) devInfo,
                        (ALTIA_UINT32) (devInfo->gc), &extent);

                if (startBatch == 1)
                {
                    egl_BatchEnd(devInfo->gc);
                    localInBatch = 0;
                }
#ifdef WIN32
                TargetAltiaSemRelease(2);
#endif
            }
#ifdef DEBUG_WINDOW
            else
                printf("Window resize doesn't change win 0x%x, wxh=%dx%d\n",
                    (unsigned long) inputEventPtr->window,
                    (unsigned int) newWidth, (unsigned int) newHeight);
#endif

        }
#ifdef DEBUG_WINDOW
        else
            printf("Window resize for UNRECOGNIZED win 0x%x, wxh=%dx%d\n",
                (unsigned long) inputEventPtr->window,
                inputEventPtr->type.resize.width,
                inputEventPtr->type.resize.height);
#endif
    }
#endif /* EGL_HAS_WINDOWCREATE */
}


/*--- SampleExpose() ------------------------------------------------------
 *
 * This function can handle an expose of the window in a windowing system
 * environment.
 *-------------------------------------------------------------------------*/
void SampleExpose(AltiaDevInfo *devInfoPtr, EGL_EVENT *eventPtr,
    ALTIA_BOOLEAN doubleBufferOn)
{
#if EGL_HAS_WINDOWCREATE
    AltiaDevInfo *devInfo = devInfoPtr;
    int startBatch = 0;

    /* If the target graphics library is using windowing, process a
     * window expose event if that is what we have.
     */
    if (eventPtr->header.category != EGL_EVENT_CATEGORY_INPUT
        || eventPtr->header.type != EGL_EVENT_TYPE_EXPOSE)
    {
        return;
    }
    else
    {
#ifdef WIN32
        int saveExtent;
#endif
        EGL_INPUT_EVENT *inputEventPtr = (EGL_INPUT_EVENT *) eventPtr;

        if (inputEventPtr->window == (EGL_UINT32) devInfo->winId

            )
        {
            Altia_Extent_type extent;
            int winMaxY;
            int count = inputEventPtr->type.expose.count;

                altiaLibGetFullWindowExtent((ALTIA_WINDOW) devInfo,
                    &extent);
            winMaxY = extent.y1;
#ifdef WIN32
            saveExtent = test_extents_count;
            TargetAltiaSemGet(2);
            test_extents_count = saveExtent;
#endif
            if (localInBatch == 0)
            {
                startBatch = 1;
                if (egl_BatchStart(devInfo->gc) == EGL_STATUS_ERROR)
                {
#ifdef WIN32
                    TargetAltiaSemRelease(2);
#endif
                    return;
                }
                localInBatch = 1;
                /* NOTE: Do not return without clearing localInBatch! */
            }

            /* Multiple exposure events can arrive at a single "moment".
             * We want to process all of them.
             */
            do
            {
#ifdef DEBUG_WINDOW
                printf("Expose for window 0x%x, x=%d,y=%d,w=%d,h=%d,count=%d\n",
                    (unsigned long) inputEventPtr->window,
                    inputEventPtr->x, inputEventPtr->y,
                    inputEventPtr->type.expose.width,
                    inputEventPtr->type.expose.height,
                    inputEventPtr->type.expose.count);
#endif
                extent.x0 = inputEventPtr->x;
                extent.y0 = winMaxY
                    - (inputEventPtr->y
                    + inputEventPtr->type.expose.height - 1);
                /* LINUX seems to need the extra pixel */
                extent.x1 = extent.x0 + inputEventPtr->type.expose.width;
                extent.y1 = extent.y0 + inputEventPtr->type.expose.height;
#ifdef DEBUG_WINDOW
                printf("Doing _altiaDrawExtent(x0,y0 = %d,%d, x1,y1 = %d,%d\n",
                    extent.x0, extent.y0, extent.x1, extent.y1);
#endif

                /* Redraw the extent that was just exposed. */
                _altiaDrawExtent((ALTIA_WINDOW) devInfo,
                    (ALTIA_UINT32) (devInfo->gc), &extent);

                /* If double buffering is on, copy buffer to the display. */
                if (doubleBufferOn)
                    altiaLibCopy((ALTIA_WINDOW) devInfo,
                        (ALTIA_UINT32) (devInfo->gc), &extent);

                /* Get next exposure event if there is one. */
                if (count-- > 0)
                    egl_EventGetNextExpose(devInfo->qId, eventPtr,
                        sizeof(EGL_EVENT));
            } while (count >= 0);

            if (startBatch == 1)
            {
                egl_BatchEnd(devInfo->gc);
                localInBatch = 0;
            }
#ifdef WIN32
            TargetAltiaSemRelease(2);
#endif
        }
    }
#endif /* EGL_HAS_WINDOWCREATE */
}


/*--- SampleMessageHandler() ----------------------------------------------
 *
 * On some targets, mouse/keyboard/timer events come in through a callback
 * function.  This is not the case for this target.  If it were the case for
 * the current target, a function like the following could be defined to
 * handle events.  Probably in TargetAltiaInitialize(), this function would
 * be assigned as the handler using some target API call.
 *-------------------------------------------------------------------------*/
#ifdef MESSAGE_HANDLER_ENABLE
void SampleMessageHandler (void *message)
{
    Altia_Extent_type extent;
    AltiaDevInfo * devInfoPtr = altiaLibGetDevInfo();
#ifdef DEBUG_WINDOW
    /* printf("message %x\n", message); */
#endif

    /* Give Altia code a chance to interpret the message. */
    if (altiaHandleWinEvent((ALTIA_WINDOW)devInfoPtr, message))
        TargetAltiaUpdate();

    /* Process other possible messages */
    switch (message)
    {
    case (void *) 0:
        /* Maybe this is a refresh message. */
        altiaLibGetFullWindowExtent((ALTIA_WINDOW)devInfoPtr, &extent);
        SampleRedraw(devInfoPtr, ALTIA_DOUBLE_BUFFERING, &extent);
        break;

    case (void *) 1:
        /* Maybe this is a resize message in which case the
         * offscreen bitmap used for double buffering might
         * need to be resized.
         */
        if (devInfoPtr->memBitmap != EGL_DISPLAY_ID)
        {
            /* Get new window width and height, delete old
             * offscreen bitmap if it is wrong size and create
             * a new one.
             */
        }
        break;

    case (void *) 2:
        /* Maybe this is a message to tell us that our window
         * has been closed.  If so, we should do what is
         * necessary to close.  Maybe we can only set a flag
         * here in the callback and wait until another function
         * running in a regular thread can detect the quit.
         */
        localQuitDetected = 1;
#ifdef ALTIALIB
        /* All flavors of Altia API provide an _altiaBreakLoop
         * global as of 4/16/01.  It allows the API to be
         * informed of a window close while its looping for
         * events from Altia.
         */
        _altiaBreakLoop = 1;
#endif        
        break;

    default:
        /* Maybe there is some default system callback to invoke
         * if we cannot handle the message directly.
         */
        break;
    }
}
#endif /* MESSAGE_HANDLER_ENABLE */

#ifdef ALTIA_TASKING
/***************************************************************************/
ALTIA_BOOLEAN TargetAltiaTaskQueue(AltiaTaskElementType * task)
{
    ALTIA_BOOLEAN retval = ALTIA_FALSE;

    if (task)
    {
        int idx;

        idx = localWriteTaskQ();
        if (idx >= 0)
        {
            /* Write data into queue element */
            altiaTaskQueue[idx] = *task;

            /* Commit the data in the queue */
            _taskWrite = idx;

            retval = ALTIA_TRUE;
        }
    }

    return retval;
}

/***************************************************************************/
ALTIA_BOOLEAN TargetAltiaTaskUpdate(ALTIA_UINT32 milliSeconds, ALTIA_UINT32 start)
{
    AltiaDevInfo * devinfo = NULL;

    /* No draw if override is enabled */
    if (_altiaOverrideUpdate)
        return ALTIA_TRUE;

    /* No draw if extent list is empty */
    if (0 == test_extents_count)
        return ALTIA_TRUE;

    /* Initialize start of draw operation */
    if (ALTIA_UPDATE_START == _objectState)
    {
        /* If we're in a batch operation we cannot start */
        if (localInBatch)
            return ALTIA_FALSE;

        /* No draw if the hardware is blocked */
        if (egl_UpdateBlocked())
            return ALTIA_FALSE;
#ifdef WIN32
        /* Lock down operations until the draw is complete in Windows */
        TargetAltiaSemGet(2);
#endif
        /* Begin batch operation */
        localInBatch = 1;

        _objectState = ALTIA_UPDATE_INIT;
    }

    /* Exit Check */
    if (milliSeconds)
    {
        ALTIA_UINT32 tick = TargetGetCurTime();
        if ((tick - start) > milliSeconds)
            return ALTIA_FALSE;
    }

        /* Get the device information */
        devinfo = altiaLibGetDevInfo();

        /* Initialize the draw */
        if (ALTIA_UPDATE_INIT == _objectState)
        {
            /* Perform start of batch of graphics rendering operations. 
             * Starting a batch operation on this target locks shared display 
             * resources to the calling thread until the batch is ended.  If the
             * resources are already locked by another thread, we will block.
             */
            if (egl_BatchStart(devinfo->gc) == EGL_STATUS_ERROR)
                return ALTIA_TRUE;

#if ALTIA_DOUBLE_BUFFERING
            /* If an offscreen bitmap was successfully allocated at
             * initialization time, we can do double buffering.
             * Otherwise, we are stuck drawing directly to the display.
             */
            devinfo->drawable = devinfo->memBitmap;
#endif
            _objectExtentIdx = 0;
            _objectState = ALTIA_UPDATE_BUILD;
        }

        /* Iterate on each extent that must be drawn */
        while (_objectExtentIdx < test_extents_count)
        {
            /* Build the object list for this extent */
            if (ALTIA_UPDATE_BUILD == _objectState)
            {

                /* Create "displayable" extent */
                if (!AltiaPrepareBuild((ALTIA_WINDOW)devinfo, _objectExtentIdx, &_objectExtent))
                {
                    /* Skip this extent if nothing to build */
                    _objectExtentIdx++;
                    continue;
                }

                /* Assemble object list */
                _objectCount = AltiaUpdateBuild((ALTIA_WINDOW)devinfo, 
                                                (ALTIA_UINT32)devinfo->gc,
                                                &_objectExtent);

                /* Update status */
                if (_objectCount >= altiaObjectQueue_count)
                    _objectStatus.error = 1;
                if (_objectCount > _objectStatus.peak)
                    _objectStatus.peak = _objectCount;

                /* Ready for Draw */
                _objectState = ALTIA_UPDATE_DRAW;
                _objectCurrent = 0;

                /* Exit Check */
                if (milliSeconds)
                {
                    ALTIA_UINT32 tick = TargetGetCurTime();
                    if ((tick - start) > milliSeconds)
                        return ALTIA_FALSE;
                }
            }

            /* Draw all objects in the object list for this extent */
            if (ALTIA_UPDATE_DRAW == _objectState)
            {
#if ALTIA_DOUBLE_BUFFERING
                ALTIA_INT doubleBufferOn = (devinfo->drawable != EGL_DISPLAY_ID) ? 1 : 0;
#else
                ALTIA_INT doubleBufferOn = 0;
#endif
                /* Prepare the extent if this is the first draw op */
                if (0 == _objectCurrent)
                    AltiaPrepareDraw((ALTIA_WINDOW)devinfo, 
                                     (ALTIA_UINT32)devinfo->gc,
                                     &_objectExtent);

                /* continue drawing the object list */
                while (_objectCurrent < _objectCount)
                {
                    /* Let Altia do its drawing.  This generic function will
                     * call the target specific altiaLibCopy() function to copy
                     * from offscreen memory to the display if doubleBufferOn is 1.
                     */
                    AltiaUpdateDraw((ALTIA_WINDOW)devinfo, 
                                    (ALTIA_UINT32)devinfo->gc,
                                    _objectCurrent);

                    /* Draw complete for this object, ready for next object */
                    _objectCurrent++;

                    /* Exit Check */
                    if (milliSeconds)
                    {
                        ALTIA_UINT32 delta = TargetGetCurTime() - start;
                        if (delta > milliSeconds)
                        {
                            /* Check metrics */
                            if (delta > (ALTIA_UINT32)_objectStatus.time)
                            {
                                ALTIA_INT idx;

                                /* Save new max time data */
                                _objectStatus.time = (ALTIA_INT32)delta;

                                /* Save object id for max time data */
                                idx = _objectCurrent - 1;
                                if (AltiaDynamicObject == altiaObjectQueue[idx].type)
                                {

                                    idx = (ALTIA_INT)altiaObjectQueue[idx].idx;
                                    _objectStatus.id = (ALTIA_INT32)ALTIA_DOBJ_ID(idx);

                                }
                                else
                                {

                                    idx = (ALTIA_INT)altiaObjectQueue[idx].idx;
                                    _objectStatus.id = (ALTIA_INT32)ALTIA_SOBJ_ID(idx);

                                }
                            }

                            return ALTIA_FALSE;
                        }
                    }
                }

                /* Finish drawing this extent */
                AltiaFinishDraw((ALTIA_WINDOW)devinfo, 
                                (ALTIA_UINT32)devinfo->gc,
                                &_objectExtent,
                                doubleBufferOn);

                /* Ready to build the next extent */
                _objectState = ALTIA_UPDATE_BUILD;

                /* Exit Check */
                if (milliSeconds)
                {
                    ALTIA_UINT32 tick = TargetGetCurTime();
                    if ((tick - start) > milliSeconds)
                    {
                        /* Current extent completed, but out of time to work
                         * on the next extent.  On next call to this function,
                         * begin processing the next extent.
                         */
                        _objectExtentIdx++;
                        return ALTIA_FALSE;
                    }
                }
            }

            /* Current extent completed. Ready for next extent (if any) */
            _objectExtentIdx++;
        }

        /* Terminate the draw */
        egl_BatchEnd(devinfo->gc);
#if ALTIA_DOUBLE_BUFFERING
        /* Restore the drawable back to the screen */
        if (!noDisplayDraw)
            devinfo->drawable = devinfo->display;
#endif


    /* We are finally done with the extent list -- clear the count */
    test_extents_count = 0;



    /* Finished */
    return ALTIA_TRUE;
}

/***************************************************************************/
int TargetAltiaTaskRun(ALTIA_UINT32 milliSeconds)
{
    int start, end, total;
    ALTIA_UINT32 tick0 = 0;

    /* _taskRead and _taskWrite are volatile.  Store them locally before
    ** processing to make function thread safe.
    */
    start = _taskRead;
    end = _taskWrite;

    /* Update metrics for Task Queue */
    if (end < start)
        total = (altiaTaskQueue_count - (start - end));
    else
        total = (end - start);
    if (total > _taskStatus.peak)
        _taskStatus.peak = total;

    /* Store flush index into 'total' */
#ifndef ALTIA_TASK_NO_FLUSH_OPTIMIZATION
    total = _taskFlush;
#endif

    if (milliSeconds)
        tick0 = TargetGetCurTime();

    /* Run until queue is empty */
    while (start != end)
    {
        /* next element in queue */
        start = (start + 1) % altiaTaskQueue_count;

        if (ALTIA_TASK_FLUSH == altiaTaskQueue[start].type)
        {
#ifndef ALTIA_TASK_NO_FLUSH_OPTIMIZATION 
            if (total == start || ALTIA_UPDATE_START != _objectState)
#endif
            {
                /* Perform state machine based draw operation */
                if (!TargetAltiaTaskUpdate(milliSeconds, tick0))
                {
                    /* Not finished, come back next time */
                    return 0;
                }

                /* Clear the recursive batch draw flag */
                localInBatch = 0;
#ifdef WIN32
                /* Allow other operations after draw is complete in Windows */
                TargetAltiaSemRelease(2);
#endif
                /* Finished with this flush */
                _objectState = ALTIA_UPDATE_START;
            }
        }
        else if (ALTIA_TASK_EVENT == altiaTaskQueue[start].type)
        {
            /* Set running state to TRUE so any animations generated as
            ** a result of this queued animation are immediately processed.
            ** This ensures animations are executed in the same sequential
            ** order as without tasking.
            */
            AltiaTaskRunning = ALTIA_TRUE;
            AltiaAnimateId((int)altiaTaskQueue[start].anim, 
                           altiaTaskQueue[start].event);
            AltiaTaskRunning = ALTIA_FALSE;
        }

        /* Update read pointer */
        _taskRead = start;

        /* Check for exit */
        if (milliSeconds)
        {
            ALTIA_UINT32 tick1 = TargetGetCurTime();
            if ((tick1 - tick0) > milliSeconds)
                break;
        }
    }

    return 0;
}

/***************************************************************************/
void TargetAltiaTaskStatus(AltiaQueueStatusType * status)
{
    if (status)
    {
        int start, end;

        /* _taskRead and _taskWrite are volatile.  Store them locally before
        ** processing to make function thread safe.
        */
        start = _taskRead;
        end = _taskWrite;

        /* Task Queue */
        if (end < start)
            _taskStatus.current = (altiaTaskQueue_count - (start - end));
        else
            _taskStatus.current = (end - start);

        /* Load status with current data */
        *status = _taskStatus;

        /* Clear persistent status */
        _taskStatus.error = 0;
        _taskStatus.peak = 0;
    }
}

/***************************************************************************/
void TargetAltiaObjectStatus(AltiaQueueStatusType * status)
{
    if (status)
    {
        /* Object Queue */
        _objectStatus.current = _objectCount;

        /* Load status with current data */
        *status = _objectStatus;

        /* Clear persistent status */
        _objectStatus.error = 0;
        _objectStatus.peak = 0;
        _objectStatus.time = 0;
        _objectStatus.id = 0;
    }
}

/***************************************************************************/
static int localWriteTaskQ(void)
{
    /* _taskRead and _taskWrite are volatile.  Store them locally before
    ** processing to make function thread safe.
    */
    int over = _taskRead;
    int idx = _taskWrite;
    idx = (idx + 1) % altiaTaskQueue_count;
    if (idx == over)
    {
        /* Overflow */
        _taskStatus.error = 1;
        return -1;
    }

    return idx;
}

#endif /* ALTIA_TASKING */
/***************************************************************************/
static ALTIA_INDEX localFindLabel(int nameId)
{




    ALTIA_INDEX count;

    /* Check if valid animation (excluding builtins) */
    if (nameId < 0 || nameId >= 66)
        return -1;

    /* Find the first text object using this Animation Id */
    count = test_funcIndex[nameId].count;
    if (count > 0)
    {
        ALTIA_INDEX i;
        ALTIA_INDEX first = test_funcIndex[nameId].firstIndex;

        /* Process all the functions associated with this animation */
        for (i = 0; i < count; i++)
        {
            /* Look for Animation type */
            if (AltiaAnimationFunc == test_funcFIndexs[first + i].funcType)
            {
                ALTIA_INDEX findex = (ALTIA_INDEX)test_funcFIndexs[first + i].index;
                if (AltiaCustomSAnimation == ALTIA_FNAME_ANIMATETYPE(findex))
                {
                    ALTIA_INDEX obj = ALTIA_FNAME_OBJECT(findex);
                    ALTIA_INDEX didx = ALTIA_DOBJ_DRAWINDEX(obj);

                    if (AltiaTextioDraw == ALTIA_DOBJ_DRAWTYPE(obj))
                        return ALTIA_TEXT_LABELINDEX(didx);


                }
            }
        }
    }




    /* No text object found for this animation Id */
    return -1;
}

/***************************************************************************/
int TargetGetTextWidth(ALTIA_CONST ALTIA_CHAR *name, ALTIA_CONST ALTIA_CHAR *text)
{
    int nameId = AltiaAnimationNameToId((ALTIA_CHAR *)name);
    if (nameId < 0)
        return -1;

    return TargetGetTextWidthId(nameId, text);
}

/***************************************************************************/
int TargetGetTextWidthId(int nameId, ALTIA_CONST ALTIA_CHAR *text)
{
    int retval = -1;

    if (text)
    {
        int fontId = TargetGetFontId(nameId);
        if (fontId >= 0)
        {
            int height, offset;

            retval = 0;
            altiaLibFontExtent(&test_fonts[fontId], 
                               (ALTIA_CHAR *)text, 
                               ALT_STRLEN((ALTIA_CHAR *)text),
                               &retval, &height, &offset);
        }
    }

    return retval;
}

/***************************************************************************/
int TargetGetFont(ALTIA_CONST ALTIA_CHAR *name)
{
    int nameId = AltiaAnimationNameToId((ALTIA_CHAR *)name);
    if (nameId < 0)
        return -1;

    return TargetGetFontId(nameId);
}

/***************************************************************************/
int TargetGetFontId(int nameId)
{
    int retval = -1;

    ALTIA_INDEX label = localFindLabel(nameId);
    if (label >= 0)
        retval = (int)test_labels[label].font;

    return retval;
}

/***************************************************************************/
int TargetSetFont(ALTIA_CONST ALTIA_CHAR *name, int fontId)
{
    int nameId = AltiaAnimationNameToId((ALTIA_CHAR *)name);
    if (nameId < 0)
        return -1;

    return TargetSetFontId(nameId, fontId);
}

/***************************************************************************/
int TargetSetFontId(int nameId, int fontId)
{
    int retval = -1;





    if (fontId >=0 && fontId < test_fonts_count)
    {
        ALTIA_INDEX count;

        /* Check if valid animation (excluding builtins) */
        if (nameId < 0 || nameId >= 66)
            return -1;

        /* Find the first text object using this Animation Id */
        count = test_funcIndex[nameId].count;
        if (count > 0)
        {
            ALTIA_INDEX i;
            ALTIA_INDEX first = test_funcIndex[nameId].firstIndex;

            /* Process all the functions associated with this animation */
            for (i = 0; i < count; i++)
            {
                /* Look for Animation type */
                if (AltiaAnimationFunc == test_funcFIndexs[first + i].funcType)
                {
                    ALTIA_INDEX findex = (ALTIA_INDEX)test_funcFIndexs[first + i].index;
                    if (AltiaCustomSAnimation == ALTIA_FNAME_ANIMATETYPE(findex))
                    {
                        ALTIA_INDEX obj = ALTIA_FNAME_OBJECT(findex);

                        if (builtinSetFont(fontId, AltiaDynamicObject, obj))
                            retval = 0;
                    }
                }
            }
        }
    }





    return retval;
}
