/* PRQA S 0292 EOF */ /* M3CM 1.1: Allow '$','@','`' in comments */
/* PRQA S 3200 EOF */ /* M3CM 17.7: Allow unused function return values */
/* $Revision: 1.21 $    $Date: 2008-11-11 14:27:33 $
 * Copyright (c) 2001-2002 Altia Inc.
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
/***************************************************************************/
/* FILE     altiaAPICbk.c
 *
 * AUTHOR       : Altia, Incorporated
 *                Copyright 1991-2001, Altia, Incorporated
 *                All rights reserved.
 *
 *
 * ONELINER     : Altia API Callback functions for DeepScreen standalone API
 *
 * DESCRIPTION  : This file contains versions of the Altia API Callback
 *                functions that work with DeepScreen generated code in a
 *                standalone environment (i.e., no connection to an
 *                existing Altia session is established to receive events).
 *
 ***************************************************************************/

/****************************************************************************
 * DEFINES
 *
 * To enable debug messages, define ALTIAAPIDEBUG. For a Windows console
 * program our UNIX style application, messages will go to stderr.  For
 * a true Windows (WinMain) application, messages will go to cerror.log
 * in the current working directory.
 */
/* #define ALTIAAPIDEBUG */

/*
 * Defining ALTIAINTAPI compiles this API with an "int" interface.  That
 * is, all values sent or received are integers.  This is compatible
 * with applications that are linking with the "lib" version of the API
 * library on a host (for example, c:\usr\altia\lib\ms32\libdde.lib on
 * Windows).  By default, this API is compiled with a "float" interface
 * where all values sent or received are doubles.
 *
 * This API will also compile with an "int" interface if ALTIAFLOAT is
 * undefined.
 */
/* #define ALTIAINTAPI */

/* To compile the API with a UNICODE 16-bit wide character interface,
 * (i.e., all event names and text are sent and received as wide character
 * strings), define ALTIAUNICODEAPI.  The standard DeepScreen make file
 * will compile it as an 8-bit character interface.  On UNICODE targets
 * such as Windows CE, 8-bit strings will be translated to 16-bit strings
 * and vice-a-versa when interfacing with the rest of the DeepScreen code.
 * Only define ALTIAUNICODEAPI for a UNICODE target!
 */
/* #define ALTIAUNICODEAPI */



/****************************************************************************
 * BEGIN CODE...
 ****************************************************************************/

/* If we are not on a UNICODE target, don't let ALTIAUNICODEAPI get defined!
 * MUST BE DONE BEFORE altia.h GETS INCLUDED!
 */
#if !defined(UNICODE) && defined(ALTIAUNICODEAPI)
#undef ALTIAUNICODEAPI
#endif


/* DeepScreen code may have a different representation for AltiaEventType
 * than API so temporarily define AltiaEventType as LocalEventType during
 * our include of DeepScreen headers.  We can use the "local" version when
 * needed for interfacing to DeepScreen functions.
 */
#define AltiaEventType LocalEventType
#define LocalEventTypeDefined

#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif
#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#undef AltiaEventType

#include <string.h>
#include <stdio.h>

#if !defined(WIN32) && defined(NO_STIMULUS_TIMERS)
/* Should only need errno.h if this is not WIN32 and
 * NO_STIMULUS_TIMERS is defined (which it is not defined
 * by default).  If it is defined and not on Win32, code
 * uses select(2) and uses errno to check if select()
 * gets interrupted by a signal.  Otherwise, errno is not
 * used and can cause problems on simple targets.
 */
#include <errno.h>
#endif

/* VXWORKS need stdlib.h for malloc and selectLib.h for timeval struct. */
#if defined(VXWORKS)
#include <stdlib.h>
#include <selectLib.h>
#endif

#if defined(USE_POSIX_SELECT_AND_TIME)
#include <stdlib.h>
#include <sys/select.h>
#include <pthread.h>

#elif defined(sun) || defined(SGI) || defined(MICROWIN) || defined(LINUX) || defined(UNIX)
/* UNIX variants need stdlib.h for malloc and time.h for timeval struct. */
#include <stdlib.h>
#include <sys/time.h>

#elif (defined(ALTIAGL) && !defined(WIN16) && !defined(WIN32)) || defined(GREEN_HILLS) || defined(QNX)
/* For non-Windows AltiaGL targets, use driver OS wrapper header file. It
 * must provide a definition for os_clock_gettime(OS_CLOCK_REALTIME, &ticks)
 * and struct timeval { long tv_sec; long tv_usec; };
 */
#include "os_Wrapper.h"

#elif !defined(WIN16) && !defined(WIN32)
#include <time.h>
#endif /* USE_POSIX_SELECT_AND_TIME */

/* Windows variants need windows.h and then winsock.h for timeval struct. */
#if defined(WIN16) || defined(WIN32)
#ifdef MINSHORT
#undef MINSHORT
#undef MAXSHORT
#endif
#include <windows.h>
#include <winsock.h>
#endif

/* Force function prototyping on 32-bit Windows */
#ifdef WIN32
#define AltiaFunctionPrototyping 1
#endif

#include <altia.h>

/* If we are compiling as C++, some Altia API function parameters are const */
#if defined(__cplusplus) || defined(c_plusplus)
#undef CONST
#define CONST const
#ifdef sun
#define realloc_type malloc_t
#else
#define realloc_type void* 
#endif

#else  /* NOT C++ */
#undef CONST
#define CONST
#define realloc_type void* 

#endif /* C++ */


/************************************************************************/
/* Establish type for API_STRCHR character argument if not already done */
/************************************************************************/
#ifndef API_STRCHR_TYPE
#ifdef ALTIAUNICODEAPI
#define API_STRCHR_TYPE AltiaCharType
#else
#define API_STRCHR_TYPE int
#endif
#endif

/************************************************/
/* Establish value for NULL if not already done */
/************************************************/
#ifndef NULL
#define NULL 0
#endif


/*****************************************************************************
 * For debug in the API functions.
 *****************************************************************************/
#if defined(WIN16) || defined(WIN32)
#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif
    /* PRQA S 3447 1 */ /* M3CM 8.5: Allow extern from altiaAPI.c */
    extern FILE *_altiaOutfile;
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
#endif /* WIN16 || WIN32 */

#ifdef ALTIAAPIDEBUG

#if defined(_WINDLL) || defined(WIN16) || defined(UNDER_CE)
#define PRINTERR1(a) (_altiaOutfile?(fprintf(_altiaOutfile,a),fflush(_altiaOutfile)):0)
#define PRINTERR2(a,b) (_altiaOutfile?(fprintf(_altiaOutfile,a,b),fflush(_altiaOutfile)):0)
#define PRINTERR3(a,b,c) (_altiaOutfile?(fprintf(_altiaOutfile,a,b,c),fflush(_altiaOutfile)):0)
#define PRINTERR4(a,b,c,d) (_altiaOutfile?(fprintf(_altiaOutfile,a,b,c,d),fflush(_altiaOutfile)):0)
#define PRINTERR5(a,b,c,d,e) (_altiaOutfile?(fprintf(_altiaOutfile,a,b,c,d,e),fflush(_altiaOutfile)):0)
#define PRINTERR6(a,b,c,d,e,f) (_altiaOutfile?(fprintf(_altiaOutfile,a,b,c,d,e,f),fflush(_altiaOutfile)):0)

#elif defined(WIN32)
#define PRINTERR1(a) (_altiaOutfile?(fprintf(_altiaOutfile,a),fflush(_altiaOutfile)):printf(a))
#define PRINTERR2(a,b) (_altiaOutfile?(fprintf(_altiaOutfile,a,b),fflush(_altiaOutfile)):printf(a,b))
#define PRINTERR3(a,b,c) (_altiaOutfile?(fprintf(_altiaOutfile,a,b,c),fflush(_altiaOutfile)):printf(a,b,c))
#define PRINTERR4(a,b,c,d) (_altiaOutfile?(fprintf(_altiaOutfile,a,b,c,d),fflush(_altiaOutfile)):printf(a,b,c,d))
#define PRINTERR5(a,b,c,d,e) (_altiaOutfile?(fprintf(_altiaOutfile,a,b,c,d,e),fflush(_altiaOutfile)):printf(a,b,c,d,e))
#define PRINTERR6(a,b,c,d,e,f) (_altiaOutfile?(fprintf(_altiaOutfile,a,b,c,d,e,f),fflush(_altiaOutfile)):printf(a,b,c,d,e,f))

#else
#define PRINTERR1(a) fprintf(stderr,a)
#define PRINTERR2(a,b) fprintf(stderr,a,b)
#define PRINTERR3(a,b,c) fprintf(stderr,a,b,c)
#define PRINTERR4(a,b,c,d) fprintf(stderr,a,b,c,d)
#define PRINTERR5(a,b,c,d,e) fprintf(stderr,a,b,c,d,e)
#define PRINTERR6(a,b,c,d,e,f) fprintf(stderr,a,b,c,d,e,f)

#endif /* WINDOWS VARIATIONS ELSE NOT WINDOWS */
#endif /* ALTIAAPIDEBUG */


/*************************************/
/* Allow us to get along with malloc */
/*************************************/
#ifdef API_NOMALLOC
#ifndef NAMESIZE
#define NAMESIZE 60U
#endif
#ifndef CALLBACKSIZE
#define CALLBACKSIZE 200U
#endif
#ifndef INPUTSIZE
#define INPUTSIZE 1U
#endif
#ifndef TIMEOUTSIZE
#define TIMEOUTSIZE 1U
#endif
#ifndef CALLBACKSTRINGS
#define CALLBACKSTRINGS 20U
#endif

#else   /* OK to MALLOC */
#ifndef NAMESIZE
#define NAMESIZE 256U
#endif
#ifndef CALLBACKSIZE
#define CALLBACKSIZE 50U
#endif
#ifndef INPUTSIZE
#define INPUTSIZE 10U
#endif
#ifndef TIMEOUTSIZE
#define TIMEOUTSIZE 10U
#endif

#endif  /* API_NOMALLOC */

#ifndef STRINGCALLBACKSIZE
#define STRINGCALLBACKSIZE 80U
#endif


/***************************************************************************/
/* Structure for storing Altia callback entries - normal, string, or clone */
/***************************************************************************/
#define AtNoneCb 1
#define AtNormalCb 2
#define AtTextCb 4
#define AtCloneCb 8
#define AtAnyCb 15
typedef void (*AtFPointer)(void);

typedef struct loc_AtCallbackStruct
{
#ifdef API_NOMALLOC
    AltiaCharType name[NAMESIZE];
#else
    CharPtr name;
#endif
    AtConnectId cnct;
    AtFPointer proc;
    AtPointer data;
    int type;
    ALTIA_BOOLEAN newCBFlag; /* 4/18/94: tjw - need to keep track of new adds */
    AtPointer typeData;
} AtCallbackStruct;


/*****************************************************/
/* Structure for strings for Text callback entries */
/*****************************************************/
typedef struct loc_AtTextStruct
{
#ifdef API_NOMALLOC
    AltiaCharType string[STRINGCALLBACKSIZE];
#else
    CharPtr string;
#endif
    int size;
    int offset;
    int cb;
    int clone;         /* 12-5-06: dc - Added to handle clone text callbacks */
} AtTextStruct;


/***************************************************/
/* Structure for storing file I/O callback entries */
/***************************************************/
typedef struct loc_AtInputStruct
{
    int source;
    AtInputType inputType;
    AtInputProc proc;
    AtPointer data;
} AtInputStruct;


/************************************************/
/* Structure for storing timer callback entries */
/************************************************/
typedef struct loc_AtTimerStruct
{
    unsigned long interval;
    long remaining;
    AtTimerProc proc;
    AtPointer data;
} AtTimerStruct;

/************************************************/
/* Keep track if any timers are active.         */
/************************************************/
static int loc_timerCount = -1;


/************************************************/
/* Allowing us to work without malloc           */
/************************************************/
#ifdef API_NOMALLOC
static AtCallbackStruct loc_callbackList[CALLBACKSIZE];
static AtTimerStruct loc_timerList[TIMEOUTSIZE];
static AtTextStruct loc_stringList[CALLBACKSTRINGS];
static unsigned int loc_callbackSize = 0;
static unsigned int loc_timerSize = 0;
static unsigned int loc_stringSize = 0;
static ALTIA_BOOLEAN loc_newCBFlag = ALTIA_FALSE; /* 4/18/94: tjw - track new adds */

#if !defined(WIN32) && defined(NO_STIMULUS_TIMERS)
static AtInputStruct loc_inputList[INPUTSIZE];
static unsigned int loc_inputSize = 0;
#endif

#else  /* OK to MALLOC */
static AtCallbackStruct *loc_callbackList;
static AtTimerStruct *loc_timerList;
static unsigned int loc_callbackSize = 0;
static unsigned int loc_timerSize = 0;
static ALTIA_BOOLEAN loc_newCBFlag = ALTIA_FALSE; /* 4/18/94: tjw - track new adds */

#if !defined(WIN32) && defined(NO_STIMULUS_TIMERS)
static AtInputStruct *loc_inputList;
static unsigned int loc_inputSize = 0;
#endif

#endif  /* API_NOMALLOC */

/****************************************************************************
 * External variables from other API sources.
 ****************************************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif
    /* PRQA S 3447 1 */ /* M3CM 8.5: Allow extern from altiaAPI.c */
    extern int _AtConnected;
    /* PRQA S 3447 1 */ /* M3CM 8.5: Allow extern from altiaAPI.c */
    extern int _AtDebugLevel;
    /* PRQA S 3447 1 */ /* M3CM 8.5: Allow extern from altiaAPI.c */
    extern int _AtNoConnectErrors;

    /* PRQA S 3447 1 */ /* M3CM 8.5: Allow extern from altiaAPI.c */
    extern void _AtDestroyConnection(
        AtConnectId id,
        int level
    );
    /* PRQA S 3447 1 */ /* M3CM 8.5: Allow extern from altiaAPI.c */
    extern char *_localStr2ErrorStr(
        CONST ALTIA_CHAR *string
    );

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


/*****************************************************************************
 * Some forward declarations for local functions.
 *****************************************************************************/
static int loc_getCallbackEntry(
    AtConnectId connection,
    CONST AltiaCharType *name,
    int type,
    AtFPointer proc,
    AtPointer data
);
static void loc_handleNormalCb(
    AtCallbackStruct *entry,
    AltiaEventType value
);
static void loc_handleTextCb(
    AtCallbackStruct *entry,
    AltiaEventType value
);
static void loc_handleCloneCb(
    AtCallbackStruct *entry,
    AltiaEventType value
);
static void loc_cleanupNormalCb(
    AtCallbackStruct *entry
);
static void loc_cleanupTextCb(
    AtCallbackStruct *entry
);
static void loc_cleanupCloneCb(
    AtCallbackStruct *entry
);
static int loc_getTimerEntry(
    AtTimerProc proc
);
static unsigned long loc_getElapsedTime(
    struct timeval* now
);

#if !defined(WIN32) && defined(NO_STIMULUS_TIMERS)
static int loc_getInputEntry(
    AtInputProc proc
);
#endif


/*****************************************************************************/
/* The next function, AtDispatchEvent(), doesn't have a counterpart in the   */
/* base library; however, it would be used in conjunction with the           */
/* AtNextEvent() function in most cases.  If AtNextEvent() fetches an event  */
/* that its caller doesn't know how to handle, AtDispatchEvent() could be    */
/* called to dispatch the event to any handlers that are registered for it.  */
/*****************************************************************************/

/*****************************************************************************/
/*  NOTE:  It seems like this function would not need protection with
 *         semaphores because it doesn't make any significant API calls
 *         itself.  However, it does need protection because of the
 *         data it accesses and functions it calls.
 */
#if AltiaFunctionPrototyping
    void AtDispatchEvent(AtConnectId connection,
                              CONST AltiaCharType *eventName, 
                              AltiaEventType eventValue)
#else
    void AtDispatchEvent(connection, eventName, eventValue)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AltiaEventType eventValue;
#endif
{
    unsigned int i, originalSize;
    AtCallbackStruct *callList;

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
#if !defined(ALTIAINTAPI) && defined(ALTIAFLOAT)
        PRINTERR4( "AtDispatchEvent(id = %d, event = %s, value = %g)\n",
                   connection, _localStr2ErrorStr(eventName), eventValue);
#else
        PRINTERR4( "AtDispatchEvent(id = %d, event = %s, value = %d)\n",
                   connection, _localStr2ErrorStr(eventName), eventValue);
#endif
    }
#endif /* ALTIAAPIDEBUG */

    if ((connection != 0) || (0 == _AtConnected))
    {
        return;
    }

    /* Protect us from other toolkit functions. */
    TargetAltiaSemGet(1);

    /* 4/18/94: tjw - If new callbacks flag is set, then one or more callback
     *                entries also have the flag set.  Clear the flag for
     *                all entries so all entries appear old.
     */
    if (loc_newCBFlag)
    {
        callList = loc_callbackList;
        for (i = 0; i < loc_callbackSize; i++, callList++)
        {
            callList->newCBFlag = ALTIA_FALSE;
        }
        loc_newCBFlag = ALTIA_FALSE;
    }

    /* Must be carefull in this for loop with callback list. A callback
     * we invoke can very easily add more callbacks to list and change
     * address of callback list because of a realloc.
     */
    originalSize = loc_callbackSize;
    for (i = 0; i < originalSize; i++)
    {
        /* Get pointer to next entry every time through for loop */
        callList = &(loc_callbackList[i]);

        /* PRQA S 3415 4 */ /* M3CM 13.5: OK no side effects of evaluations */
        if ((callList->name != NULL)
            && (!(callList->newCBFlag)) /* 4/18/94: tjw - don't call new CBs */
            && (callList->cnct == connection)
            && (API_STRCMP(callList->name, eventName) == 0))
        {
            /* Note: handlers that invoke the callback will do
             * the necessary releasing and retaking of the tookit
             * semaphore.
             */

            switch (callList->type)
            {
                case AtTextCb:
                    loc_handleTextCb(callList, eventValue);
                    break;
                case AtCloneCb:
                    loc_handleCloneCb(callList, eventValue);
                    break;
                case AtNormalCb:
                    loc_handleNormalCb(callList, eventValue);
                    break;
                default:
                    break;
            }

            if (0 == _AtConnected)
            {
                break;
            }
        }
    }

    TargetAltiaSemRelease(1);
    return;
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    static void loc_addCallback(AtConnectId connection,
                                CONST AltiaCharType *eventName,
                                AtFPointer proc, AtPointer clientData,
                                int cbType, int clone)
#else
    static void loc_addCallback(connection, eventName, proc, clientData, cbType, clone)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AtFPointer proc;
    AtPointer clientData;
    int cbType;
    int clone;
#endif
{
    int entry;

    unsigned int nameSize = API_STRLEN(eventName);

    /* Protect us from other toolkit functions. */
    TargetAltiaSemGet(1);

    /* Is there a callback entry that matches users entry exactly */
    entry = loc_getCallbackEntry(connection, eventName, cbType, proc, clientData);
    if (entry >= 0)
    {
        /* Select to receive this Altia event */
        /* DC - 1-23-07 - Changed for clones to use already allocated
         * name string in Callback entry since AddCloneCallback
         * creates Event string on stack which is lost upon return.
         */
        if (cbType == AtCloneCb)
        {
            altiaSelectEvent(loc_callbackList[entry].name);
        }
        else
        {
            altiaSelectEvent(eventName);
        }

#ifdef ALTIAAPIDEBUG
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
            PRINTERR1( "\tcallback already exists\n");
        }
#endif /* ALTIAAPIDEBUG */

        TargetAltiaSemRelease(1);
        return;
    }

    /* Is there a new callback entry available */
    entry = loc_getCallbackEntry(-1, NULL, AtNoneCb, NULL, NULL);
    if (entry >= 0)
    {
#ifndef API_NOMALLOC
        loc_callbackList[entry].name = (AltiaCharType *) ALTIA_MALLOC((nameSize + 1U)
                                                     * sizeof(AltiaCharType));
#endif
        if (cbType == AtCloneCb)
        {
            /* PRQA S 0306 1 */ /* M3CM 11.4,11.6: OK cast int to AtPointer */
            loc_callbackList[entry].typeData = (AtPointer) clone;
        }
        API_STRCPY(loc_callbackList[entry].name, eventName);
        loc_callbackList[entry].cnct = connection;
        loc_callbackList[entry].proc = proc;
        loc_callbackList[entry].data = clientData;
        loc_callbackList[entry].type = cbType;
        loc_callbackList[entry].newCBFlag = ALTIA_TRUE; /* 4/18/94: tjw - flag new entry */
        loc_newCBFlag = ALTIA_TRUE; /* 4/18/94: tjw - and globally flag new entry */

        /* Select to receive this Altia event */
        /* DC - 1-23-07 - Changed for clones to use already allocated
         * name string in Callback entry since AddCloneCallback
         * creates Event string on stack which is lost upon return.
         */
        if (cbType == AtCloneCb)
        {
            altiaSelectEvent(loc_callbackList[entry].name);
        }
        else
        {
            altiaSelectEvent(eventName);
        }

#ifdef ALTIAAPIDEBUG
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
            PRINTERR1( "\tsucceeded\n");
        }
#endif /* ALTIAAPIDEBUG */

    }
#ifdef ALTIAAPIDEBUG
    else
    {
        if (0 == _AtNoConnectErrors)
        {
            PRINTERR1( "loc_addCallback:  out of memory\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtAddCallback(AtConnectId connection, CONST AltiaCharType *eventName,
                       AtCallbackProc proc, AtPointer clientData)
#else
    void AtAddCallback(connection, eventName, proc, clientData)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AtCallbackProc proc;
    AtPointer clientData;
#endif
{

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR5(
            "AtAddCallback(id = %d, event = %s, proc = 0x%lx, data = 0x%lx)\n",
            connection, _localStr2ErrorStr(eventName), proc, clientData);
    }
#endif /* ALTIAAPIDEBUG */

    if ((connection != 0) || (0 == _AtConnected))
    {
        return;
    }

    /* Is event name illegal or procedure illegal */
    if ((eventName == NULL) || (proc == NULL))
    {
        return;
    }

    if (API_STRLEN(eventName) >= NAMESIZE)
    {

#ifdef ALTIAAPIDEBUG
        if (0 == _AtNoConnectErrors)
        {
            PRINTERR2( "AtAddCallback: event name %s too big - ignoring\n",
                      _localStr2ErrorStr(eventName));
        }
#endif /* ALTIAAPIDEBUG */

        return;
    }

    /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtFPointer */
    loc_addCallback(connection, eventName, (AtFPointer)proc, clientData,
                    AtNormalCb, -1);
    return;
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtAddCloneCallback(AtConnectId connection, int cloneNumber,
                            CONST AltiaCharType *eventName,
                            AtCloneProc proc, AtPointer clientData)
#else
    void AtAddCloneCallback(connection,cloneNumber,eventName,proc,clientData)
    AtConnectId connection;
    int cloneNumber;
    CONST AltiaCharType *eventName;
    AtCloneProc proc;
    AtPointer clientData;
#endif
{
    AltiaCharType cloneName[NAMESIZE];
    API_SPRINTF(cloneName, API_TEXT("%d"), cloneNumber);

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR6(
            "AtAddCloneCallback(id = %d, clone = %d, event = %s, proc = 0x%lx, data = 0x%lx)\n",
            connection, cloneNumber, _localStr2ErrorStr(eventName), proc, clientData);
    }
#endif /* ALTIAAPIDEBUG */

    if ((connection != 0) || (0 == _AtConnected))
    {
        return;
    }

    /* Is event name illegal or procedure illegal */
    if ((eventName == NULL) || (proc == NULL))
    {
        return;
    }

    if ((API_STRLEN(eventName) + API_STRLEN(cloneName)) >= NAMESIZE)
    {

#ifdef ALTIAAPIDEBUG
        if (0 == _AtNoConnectErrors)
        {
            PRINTERR2( "AtAddCloneCallback: event name %s too big - ignoring\n",
                      _localStr2ErrorStr(eventName));
        }
#endif /* ALTIAAPIDEBUG */

        return;
    }

    API_SPRINTF(cloneName, API_TEXT("%d:%s"), cloneNumber, eventName);

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR2(
            "AtAddCloneCallback() adding callback for: \"%s\"\n", cloneName);
    }
#endif /* ALTIAAPIDEBUG */

    /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtFPointer */
    loc_addCallback(connection, (CONST AltiaCharType *)cloneName,
                    (AtFPointer)proc, clientData, AtCloneCb, cloneNumber);
    return;
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    static void loc_removeCallback(AtConnectId connection,
                          CONST AltiaCharType *eventName, AtFPointer proc,
                          AtPointer clientData, int cbType)
#else
    static void loc_removeCallback(connection, eventName, proc, clientData, cbType)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AtFPointer proc;
    AtPointer clientData;
    int cbType;
#endif
{
    int entry;

    /* Protect us from other toolkit functions. */
    TargetAltiaSemGet(1);

    /* Is there a callback entry that matches users entry exactly */
    entry = loc_getCallbackEntry(connection, eventName, cbType, proc, clientData);
    if (entry >= 0)
    {
        if (cbType == AtCloneCb)
        {
            loc_cleanupCloneCb(&(loc_callbackList[entry]));
        }
        else
        {
            loc_cleanupNormalCb(&(loc_callbackList[entry]));
        }

#ifdef ALTIAAPIDEBUG
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
            PRINTERR1( "\tsucceeded\n");
        }
#endif /* ALTIAAPIDEBUG */

        /* If this is last callback for this event, Unselect to receive this
         * Altia event.
         */
        if (loc_getCallbackEntry(connection, eventName,
                                 (int) ((unsigned int) AtNormalCb 
                                        | (unsigned int) AtTextCb
                                        | (unsigned int) AtCloneCb),
                                 NULL, NULL) < 0)
        {
            if (_AtDebugLevel < 3)
            {
                altiaUnselectEvent(eventName);
            }
        }
    }
#ifdef ALTIAAPIDEBUG
    else
    {
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
            PRINTERR1( "\tcallback not found\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
void AtRemoveCallback(AtConnectId connection,
                      CONST AltiaCharType *eventName, AtCallbackProc proc,
                      AtPointer clientData)
#else
void AtRemoveCallback(connection, eventName, proc, clientData)
AtConnectId connection;
CONST AltiaCharType *eventName;
AtCallbackProc proc;
AtPointer clientData;
#endif
{

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR5(
            "AtRemoveCallback(id = %d, event = %s, proc = 0x%lx, data = 0x%lx)\n",
            connection, _localStr2ErrorStr(eventName), proc, clientData);
    }
#endif /* ALTIAAPIDEBUG */

    if ((connection != 0) || (0 == _AtConnected))
    {
        return;
    }

    /* Are event name and callback parameters valid */
    if ((eventName == NULL) || (proc == NULL))
    {
        return;
    }

    /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtFPointer */
    loc_removeCallback(connection, eventName, (AtFPointer)proc, clientData,
                       AtNormalCb);
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtRemoveCloneCallback(AtConnectId connection, int cloneNumber,
                          CONST AltiaCharType *eventName, AtCloneProc proc,
                          AtPointer clientData)
#else
    void AtRemoveCloneCallback(connection, cloneNumber, eventName, proc, clientData)
    AtConnectId connection;
    int cloneNumber;
    CONST AltiaCharType *eventName;
    AtCloneProc proc;
    AtPointer clientData;
#endif
{
    AltiaCharType cloneName[NAMESIZE];
    API_SPRINTF(cloneName, API_TEXT("%d"), cloneNumber);

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR6(
            "AtRemoveCloneCallback(id = %d, clone = %d, event = %s, proc = 0x%lx, data = 0x%lx)\n",
            connection, cloneNumber, _localStr2ErrorStr(eventName), proc, clientData);
    }
#endif /* ALTIAAPIDEBUG */

    if ((connection != 0) || (0 == _AtConnected))
    {
        return;
    }

    /* Are event name and callback parameters valid */
    if ((eventName == NULL) || (proc == NULL))
    {
        return;
    }

    if ((API_STRLEN(eventName) + API_STRLEN(cloneName)) >= NAMESIZE)
    {

#ifdef ALTIAAPIDEBUG
        if (0 == _AtNoConnectErrors)
        {
            PRINTERR2( "AtRemoveCloneCallback: event name %s too big - ignoring\n",
                      _localStr2ErrorStr(eventName));
        }
#endif /* ALTIAAPIDEBUG */

        return;
    }

    API_SPRINTF(cloneName, API_TEXT("%d:%s"), cloneNumber, eventName);

    /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtFPointer */
    loc_removeCallback(connection, (CONST AltiaCharType *)cloneName,
                       (AtFPointer)proc, clientData, AtCloneCb);
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    static void loc_addTextCallback(AtConnectId connection,
                            CONST AltiaCharType *eventName, AtFPointer proc,
                            AtPointer clientData, int type, int clone)
#else
    static void loc_addTextCallback(connection,eventName,proc,clientData,type,clone)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AtFPointer proc;
    AtPointer clientData;
    int type;
    int clone;
#endif
{
#ifdef API_NOMALLOC
    unsigned int i;
#else
    AtTextStruct *info;
#endif
    int entry;
    unsigned int nameSize = API_STRLEN(eventName);
    AtCallbackStruct *entryPtr;

    /* Protect us from other toolkit functions. */
    TargetAltiaSemGet(1);

    /* Is there a callback entry that matches users entry exactly */
    entry = loc_getCallbackEntry(connection, eventName, AtTextCb, proc, clientData);
    if (entry >= 0)
    {
        altiaSelectEvent(eventName);

#ifdef ALTIAAPIDEBUG
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
            PRINTERR1( "\tcallback already exists\n");
        }
#endif /* ALTIAAPIDEBUG */

        TargetAltiaSemRelease(1);
        return;
    }

    /* Is there a new callback entry available */
    entry = loc_getCallbackEntry(-1, NULL, AtNoneCb, NULL, NULL);
    if (entry >= 0)
    {
        entryPtr = &(loc_callbackList[entry]);
#ifndef API_NOMALLOC
        entryPtr->name = (AltiaCharType *) ALTIA_MALLOC((nameSize + 1U)
                                           * sizeof(AltiaCharType));
        info = (AtTextStruct *) ALTIA_MALLOC(sizeof(AtTextStruct));
        info->string = (AltiaCharType *) ALTIA_MALLOC(STRINGCALLBACKSIZE 
                                        * sizeof(AltiaCharType));
        info->size = (int) STRINGCALLBACKSIZE;
        info->offset = 0;
        info->string[0] = (AltiaCharType) ('\0');
        info->clone = -1;
        if (type == AtCloneCb)
        {
            info->clone = clone;
        }
        entryPtr->typeData = (AtPointer) info;
#else
        for (i = 0; i < loc_stringSize; i++)
        {
            if (loc_stringList[i].cb == -1)
            {
                break;
            }
        }
        if (i >= loc_stringSize)
        {

#ifdef ALTIAAPIDEBUG
            if (0 == _AtNoConnectErrors)
            {
                PRINTERR1( "AtAddTextCallback:  out of string buffers\n");
            }
#endif /* ALTIAAPIDEBUG */

            TargetAltiaSemRelease(1);
            return;
        }
        loc_stringList[i].cb = entry;
        loc_stringList[i].offset = 0;
        loc_stringList[i].string[0] = (AltiaCharType) ('\0');
        loc_stringList[i].clone = -1;
        if (type == AtCloneCb)
        {
            loc_stringList[i].clone = clone;
        }
        entryPtr->typeData = (AtPointer) (&(loc_stringList[i]));
#endif
        API_STRCPY(entryPtr->name, eventName);
        entryPtr->cnct = connection;
        entryPtr->proc = proc;
        entryPtr->data = clientData;
        entryPtr->type = AtTextCb;
        entryPtr->newCBFlag = ALTIA_TRUE; /* 4/18/94: tjw - flag new entry */
        loc_newCBFlag = ALTIA_TRUE; /* 4/18/94: tjw - and globally flag new entry */

        /* Select to receive this Altia event */
        altiaSelectEvent(eventName);

#ifdef ALTIAAPIDEBUG
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
            PRINTERR1( "\tsucceeded\n");
        }
#endif /* ALTIAAPIDEBUG */

    }
#ifdef ALTIAAPIDEBUG
    else
    {
        if (0 == _AtNoConnectErrors)
        {
            PRINTERR1( "AtAddTextCallback:  out of memory\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return;
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtAddTextCallback(AtConnectId connection,
                           CONST AltiaCharType *eventName,
                           AtTextProc proc, AtPointer clientData)
#else
    void AtAddTextCallback(connection, eventName, proc, clientData)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AtTextProc proc;
    AtPointer clientData;
#endif
{

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR5(
            "AtAddTextCallback(id = %d, event = %s, proc = 0x%lx, data = 0x%lx)\n",
            connection, _localStr2ErrorStr(eventName), proc, clientData);
    }
#endif /* ALTIAAPIDEBUG */

    if ((connection != 0) || (0 == _AtConnected))
    {
        return;
    }

    /* Is event name illegal or procedure illegal */
    if ((eventName == NULL) || (proc == NULL))
    {
        return;
    }

    if (API_STRLEN(eventName) >= NAMESIZE)
    {

#ifdef ALTIAAPIDEBUG
        if (0 == _AtNoConnectErrors)
        {
            PRINTERR2("AtAddTextCallback: event name %s too big - ignoring\n",
                      _localStr2ErrorStr(eventName));
        }
#endif /* ALTIAAPIDEBUG */

        return;
    }

    /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtFPointer */
    loc_addTextCallback(connection, eventName, (AtFPointer)proc, clientData,
                        AtNormalCb, -1);
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtAddCloneTextCallback(AtConnectId connection, int cloneNumber,
                           CONST AltiaCharType *eventName,
                           AtCloneTextProc proc, AtPointer clientData)
#else
    void AtAddCloneTextCallback(connection, cloneNumber, eventName, proc,
                                clientData)
    AtConnectId connection;
    int cloneNumber;
    CONST AltiaCharType *eventName;
    AtCloneTextProc proc;
    AtPointer clientData;
#endif
{
    AltiaCharType cloneName[NAMESIZE];
    API_SPRINTF(cloneName, API_TEXT("%d"), cloneNumber);

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR6(
            "AtAddCloneTextCallback(id = %d, clone = %d, event = %s, proc = 0x%lx, data = 0x%lx)\n",
            connection, cloneNumber, _localStr2ErrorStr(eventName), proc, clientData);
    }
#endif /* ALTIAAPIDEBUG */

    if ((connection != 0) || (0 == _AtConnected))
    {
        return;
    }

    /* Is event name illegal or procedure illegal */
    if ((eventName == NULL) || (proc == NULL))
    {
        return;
    }

    if ((API_STRLEN(eventName) + API_STRLEN(cloneName)) >= NAMESIZE)
    {

#ifdef ALTIAAPIDEBUG
        if (0 == _AtNoConnectErrors)
        {
            PRINTERR2("AtAddCloneTextCallback: event name %s too big - ignoring\n",
                      _localStr2ErrorStr(eventName));
        }
#endif /* ALTIAAPIDEBUG */

        return;
    }
    API_SPRINTF(cloneName, API_TEXT("%d:%s"), cloneNumber, eventName);

    /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtFPointer */
    loc_addTextCallback(connection, (CONST AltiaCharType *)cloneName,
                        (AtFPointer)proc, clientData, AtCloneCb, cloneNumber);
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    static void loc_removeTextCallback(AtConnectId connection,
                              CONST AltiaCharType *eventName, AtFPointer proc,
                              AtPointer clientData)
#else
    static void loc_removeTextCallback(connection, eventName, proc, clientData)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AtFPointer proc;
    AtPointer clientData;
#endif
{
    int entry;

    /* Protect us from other toolkit functions. */
    TargetAltiaSemGet(1);

    /* Is there a callback entry that matches users entry exactly */
    entry = loc_getCallbackEntry(connection, eventName, AtTextCb, proc, clientData);
    if (entry >= 0)
    {
        loc_cleanupTextCb(&(loc_callbackList[entry]));

#ifdef ALTIAAPIDEBUG
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
            PRINTERR1( "\tsucceeded\n");
        }
#endif /* ALTIAAPIDEBUG */

        /* If this is last callback for this event, Unselect to receive this
         * Altia event.
         */
        if (loc_getCallbackEntry(connection, eventName,
                                 (int) ((unsigned int) AtNormalCb 
                                        | (unsigned int) AtTextCb
                                        | (unsigned int) AtCloneCb),
                                 NULL, NULL) < 0)
        {
            if (_AtDebugLevel < 3)
            {
                altiaUnselectEvent(eventName);
            }
        }
    }
#ifdef ALTIAAPIDEBUG
    else
    {
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
            PRINTERR1( "\tcallback not found\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtRemoveTextCallback(AtConnectId connection,
                              CONST AltiaCharType *eventName, AtTextProc proc,
                              AtPointer clientData)
#else
    void AtRemoveTextCallback(connection, eventName, proc, clientData)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AtTextProc proc;
    AtPointer clientData;
#endif
{

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR5(
            "AtRemoveTextCallback(id = %d, event = %s, proc = 0x%lx, data = 0x%lx)\n",
            connection, _localStr2ErrorStr(eventName), proc, clientData);
    }
#endif /* ALTIAAPIDEBUG */

    if ((connection != 0) || (0 == _AtConnected))
    {
        return;
    }

    /* Are event name and callback parameters valid */
    if ((eventName == NULL) || (proc == NULL))
    {
        return;
    }

    /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtFPointer */
    loc_removeTextCallback(connection, eventName, (AtFPointer)proc, clientData);
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtRemoveCloneTextCallback(AtConnectId connection, int cloneNumber,
                              CONST AltiaCharType *eventName,
                              AtCloneTextProc proc, AtPointer clientData)
#else
    void AtRemoveCloneTextCallback(connection,cloneNumber,eventName,proc,clientData)
    AtConnectId connection;
    int cloneNumber;
    CONST AltiaCharType *eventName;
    AtCloneTextProc proc;
    AtPointer clientData;
#endif
{
    AltiaCharType cloneName[NAMESIZE];
    API_SPRINTF(cloneName, API_TEXT("%d"), cloneNumber);

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR6(
            "AtRemoveCloneTextCallback(id = %d, clone = %d, event = %s, proc = 0x%lx, data = 0x%lx)\n",
            connection, cloneNumber, _localStr2ErrorStr(eventName), proc, clientData);
    }
#endif /* ALTIAAPIDEBUG */

    if ((connection != 0) || (0 == _AtConnected))
    {
        return;
    }

    /* Are event name and callback parameters valid */
    if ((eventName == NULL) || (proc == NULL))
    {
        return;
    }

    if ((API_STRLEN(eventName) + API_STRLEN(cloneName)) >= NAMESIZE)
    {

#ifdef ALTIAAPIDEBUG
        if (0 == _AtNoConnectErrors)
        {
            PRINTERR2("AtAddCloneTextCallback: event name %s too big - ignoring\n",
                      _localStr2ErrorStr(eventName));
        }
#endif /* ALTIAAPIDEBUG */

        return;
    }
    API_SPRINTF(cloneName, API_TEXT("%d:%s"), cloneNumber, eventName);

    /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtFPointer */
    loc_removeTextCallback(connection, (CONST AltiaCharType *)cloneName,
                           (AtFPointer)proc, clientData);
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtRemoveAllCallbacks(AtConnectId connection,
                              CONST AltiaCharType *eventName)
#else
    void AtRemoveAllCallbacks(connection, eventName)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
#endif
{
    int entry;
    AtCallbackStruct *entryPtr;

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR3(
            "AtRemoveAllCallbacks(id = %d, event %s)\n", connection,
            _localStr2ErrorStr(eventName));
    }
#endif

    if ((connection != 0) || (0 == _AtConnected))
    {
        return;
    }

    /* Is event name valid */
    if (eventName == NULL)
    {
        return;
    }

    /* Protect us from other toolkit functions. */
    TargetAltiaSemGet(1);

    /* While we find callback entries for this event */
    /* PRQA S 3326 2 */ /* M3CM 13.4: OK to use assignment in while */
    while ((entry = loc_getCallbackEntry(connection, eventName, AtAnyCb,
                                         NULL, NULL)) >= 0)
    {
        entryPtr = &(loc_callbackList[entry]);

        switch (entryPtr->type)
        {
            case AtTextCb:

#ifdef ALTIAAPIDEBUG
                /* DEBUG PRINT-OUT */
                if (0 != _AtDebugLevel)
                {
                    PRINTERR2("\tremoving Text callback %s\n",
                              _localStr2ErrorStr(entryPtr->name));
                }
#endif /* ALTIAAPIDEBUG */

                loc_cleanupTextCb(entryPtr);
                break;
            case AtCloneCb:
                /* Name in actual entry has "#:" prepended to it so we need
                 * to do an explicit deselect because the name is different
                 * from eventName.
                 */
                if (_AtDebugLevel < 3)
                {
                    altiaUnselectEvent(entryPtr->name);
                }

#ifdef ALTIAAPIDEBUG
                /* DEBUG PRINT-OUT */
                if (0 != _AtDebugLevel)
                {
                    PRINTERR2("\tremoving Clone callback %s\n",
                              _localStr2ErrorStr(entryPtr->name));
                }
#endif /* ALTIAAPIDEBUG */

                loc_cleanupCloneCb(entryPtr);
                break;
            case AtNormalCb:

#ifdef ALTIAAPIDEBUG
                /* DEBUG PRINT-OUT */
                if (0 != _AtDebugLevel)
                {
                    PRINTERR2("\tremoving Normal callback %s\n",
                              _localStr2ErrorStr(entryPtr->name));
                }
#endif /* ALTIAAPIDEBUG */

                loc_cleanupNormalCb(entryPtr);
                break;
            default:
                break;
        }
    }

    /* No more callbacks for this event, let's not receive it anymore */
    if (_AtDebugLevel < 3)
    {
        altiaUnselectEvent(eventName);
    }

    TargetAltiaSemRelease(1);
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    AtTimerId AtAddTimer(unsigned long interval, AtTimerProc proc,
                              AtPointer clientData)
#else
    AtTimerId AtAddTimer(interval, proc, clientData)
    unsigned long interval;
    AtTimerProc proc;
    AtPointer clientData;
#endif
{
    AtTimerId returnId;

    if (proc == NULL)
    {

#ifdef ALTIAAPIDEBUG
        if (0 == _AtNoConnectErrors)
        {
            PRINTERR1(
              "AtAddTimer: callback proc out of range - ignoring request\n");
        }
#endif /* ALTIAAPIDEBUG */

        return -1;
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR4(
            "AtAddTimer(interval = %ld, proc = 0x%lx, data = 0x%lx)",
            interval, proc, clientData);
    }
#endif /* ALTIAAPIDEBUG */

    /* Protect us from other toolkit functions. */
    TargetAltiaSemGet(1);

    returnId = loc_getTimerEntry(NULL);
    if (returnId >= 0)
    {
        loc_timerList[returnId].interval = interval;
        loc_timerList[returnId].remaining = (long) interval + (long) loc_getElapsedTime(NULL);
        loc_timerList[returnId].proc = proc;
        loc_timerList[returnId].data = clientData;
        loc_timerCount++;  /* Should be done after loc_getElapsedTime() call */

#ifdef ALTIAAPIDEBUG
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
            PRINTERR1( " succeeded\n");
        }
#endif /* ALTIAAPIDEBUG */

    }
    else
    {

#ifdef ALTIAAPIDEBUG
        PRINTERR1( "AtAddTimer: out of memory - ignoring request\n");
#endif /* ALTIAAPIDEBUG */

        returnId = -1;
    }

    TargetAltiaSemRelease(1);
    return returnId;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtRemoveTimer(AtTimerId id)
#else
    void AtRemoveTimer(id)
    AtTimerId id;
#endif
{

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR2( "AtRemoveTimer(id = %ld)\n", id);
    }
#endif /* ALTIAAPIDEBUG */

    /* Protect us from other tookit functions. */
    TargetAltiaSemGet(1);

    if ((id >= 0) && (((unsigned int) id) < loc_timerSize))
    {
        loc_timerList[(unsigned int) id].proc = NULL;
    }

    TargetAltiaSemRelease(1);
    return;
}


/*****************************************************************************/
/* Forward references */
static int loc_checkAltia(
    AtConnectId connection
);
static void loc_checkTimeouts(
    void
);
static struct timeval *loc_getWakeupTime(
    void
);

#if !defined(WIN32) && defined(NO_STIMULUS_TIMERS)
static void loc_checkInputs(
    AtInputType inputType,
    fd_set *mask
);
static int loc_getInputMask(
    AtInputType inputType,
    fd_set *mask,
    int lastMaxFD
);
#endif


/****************************************************************************
 * Wait for input from Altia file descriptors, or timeout events
 * Note:  If we possibly have stimulus timers, then we have to always
 *        process things using the WIN32 approach.  This is a wait for
 *        the next Altia event which also waits for other system events
 *        for us like mouse and timer expirations.  By default, we assume
 *        that we may have stimulus timers (i.e., NO_STIMULUS_TIMERS is not
 *        defined)
 ****************************************************************************/
void AtMainLoop(void)
{
    struct timeval *interval;
    /* PRQA S 2981 1 */   /* M3CM 2.2: OK next init to 0 not always redundant */
    int retval = 0;

#if !defined(WIN32) && defined(NO_STIMULUS_TIMERS)
    int maxFD, altiaFD = 0;
    fd_set readfds, writefds, exceptfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
#endif

    do
    {
        /* If we have some inputs from sources other than altia descriptor,
         * process them.
         * 1/6/97: tjw - Only check if altiaFD descriptor is set if it
         *               is a valid descriptor number.
         */
#if !defined(WIN32) && defined(NO_STIMULUS_TIMERS)
        if (((retval > 0) && ((altiaFD < 0) || (!FD_ISSET(altiaFD, &readfds))))
            || (retval > 1))
        {
            loc_checkInputs(AtInputRead, &readfds);
            loc_checkInputs(AtInputWrite, &writefds);
            loc_checkInputs(AtInputExcept, &exceptfds);
        }
#endif  /* NOT WIN32 && NO STIMULUS TIMERS */

        /* Take toolkit semaphore while we do base level calls and
         * callbacks, but make sure we release it when invoking any
         * callbacks and then retake it when callbacks are done.
         */
        TargetAltiaSemGet(1);

        /* Check for timeouts.
         * Note that toolkit semaphore doesn't need to be
         * released before checking.  This check function
         * assumes it is taken.  It releases it and retakes
         * it if necessary.
         */
        loc_checkTimeouts();

        if (0 != _AtConnected)
        {
            /* PRQA S 3326 1 */ /* M3CM 13.4: OK to use assignment in while */
            while ((retval = altiaPending()) > 0)
            {
                /* Handle any available frontpanel events.
                 * Note that toolkit semaphore doesn't need to be
                 * released before handling events.  The check functions
                 * used here assume it is taken.  They release it
                 * and retake it if necessary.
                 */
                retval = loc_checkAltia(0);
                if (retval != 0)
                {
                    loc_checkTimeouts();
                    break;
                }

                /* Check for timeouts again. checkAltia() could take some 
                 * time and we want to catch anything that timed out and
                 * provide for an accurate interval time for select(2).
                 */
                loc_checkTimeouts();
            }

            /* If we've lost contact, give it up for DeepScreen. */
            if (retval < 0)
            {
                TargetAltiaSemRelease(1);
                return;
            }
        }

#if !defined(WIN32) && defined(NO_STIMULUS_TIMERS)
        /* Reset masks */
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);

        /* Clear max fd holder */
        maxFD = -1;

        /* Set flags for select(2) - must be done each time! */
        /* If we still have a good descriptor */
        if ((altiaFD = altiaInputNumber()) >= 0)
        {
            altiaFlushOutput();
            FD_SET(altiaFD, &readfds);
            if (altiaFD > maxFD)
            {
                maxFD = altiaFD;
            }
        }
        else
        {
            AltiaEventType tempValue;

#ifdef ALTIAAPIDEBUG
            /* DEBUG PRINT-OUT */
            if (0 != _AtDebugLevel)
            {
                PRINTERR1( "AtMainLoop: connection went down\n");
            }
#endif /* ALTIAAPIDEBUG */

            /* For retry counts of 1, force exit. Insures we
             * get through a race condition with interface shutdown.
             */
            altiaPollEvent(API_TEXT("None"), &tempValue);
            altiaPollEvent(API_TEXT("None"), &tempValue);

            /* Note this is a level 2 destroy because pid is ours */
            _AtDestroyConnection(0, 2);
        }

        maxFD = loc_getInputMask(AtInputRead, &readfds, maxFD);
        maxFD = loc_getInputMask(AtInputWrite, &writefds, maxFD);
        maxFD = loc_getInputMask(AtInputExcept, &exceptfds, maxFD);

#endif  /* NOT WIN32 && NO STIMULUS TIMERS */

        /* If there is a timer event waiting, use it as the timeout for the
         * select.
         */
        interval = loc_getWakeupTime();

#if defined(WIN32) || !defined(NO_STIMULUS_TIMERS)
        /* Wimping out a bit here for now.  If we have timer callbacks,
         * block for some amount of time.  Otherwise, block forever waiting
         * for an Altia event.
         */
        if (interval != NULL)
        {
            unsigned long msecs = ((unsigned long) (interval->tv_sec) * 1000U)
                                + ((unsigned long) (interval->tv_usec) / 1000U);

            /* Don't hold toolkit semaphore while we wait.
             * Semaphore will be retaken when we go back to
             * the beginning of the outer do...while loop.
             */
            TargetAltiaSemRelease(1);

            if ((msecs > 0U) && (msecs < 50U))
            {
                altiaSleep(msecs);
            }
            else
            {
                if (msecs > 0U)
                {
                    altiaSleep(50L);
                }
            }
        }
        else if (0 != _AtConnected)
        {
            /* Wait forever for a new Altia event.
             * If we lose contact, give it up for DeepScreen.
             * Note that toolkit semaphore doesn't need to be
             * released before the check.  The check function
             * used here assumes it is taken.  It releases it
             * and retakes it if necessary.
             */
            if (loc_checkAltia(0) != 0)
            {
                TargetAltiaSemRelease(1);
                return;
            }

            /* Now we need to release the toolkit semaphore
             * because it is retaken at the beginning
             * of the outer do...while loop.
             */
            TargetAltiaSemRelease(1);
        }
        else
        {
            /* If no timeouts and no connection, we are locked in an
             * infinite loop.  Just return now.
             */
            TargetAltiaSemRelease(1);
            return;
        }
      
    } while (1);

#else /* NOT WIN32  && NO STIMULUS TIMERS */

        /* Don't hold toolkit semaphore while we wait.
         * Semaphore will be retaken when we go back to
         * the beginning of the outer do...while loop.
         */
        TargetAltiaSemRelease(1);

        /* If no descriptors and no timeouts, we are locked in an infinite
         * loop.  Just return now.
         */
        if ((maxFD == -1) && (interval == NULL))
        {
            return;
        }

#if AltiaFunctionPrototyping

#if defined(sun) || defined(PMAX) || defined(VXWORKS) || defined(IBM) || defined(MICROWIN)
        /* PRQA S 3326 3 */ /* M3CM 13.4: OK to use assignment in while */
        while ((retval = select(maxFD + 1, (fd_set *) &readfds,
                                (fd_set *) &writefds, (fd_set *) &exceptfds,
                                interval)) < 0)
#else
        /* PRQA S 3326 3 */ /* M3CM 13.4: OK to use assignment in while */
        while ((retval = select(maxFD + 1, (int *) &readfds,
                                (int *) &writefds, (int *) &exceptfds,
                                interval)) < 0)
#endif /* sun */
        {
            /* 8/22/95: tjw - Handle interruptions from signals */
            if (0 != _AtDebugLevel)
            {
                perror("Evaluating select failure:");
            }

            /* 8/22/95: tjw - Handle interruptions from signals */
            if (errno != EINTR)
            {
                break;
            }
        }
    } while (retval >= 0);

#else /* NOT AltiaFunctionPrototyping */

        /* PRQA S 3326 3 */ /* M3CM 13.4: OK to use assignment in while */
        while ((retval = select(maxFD + 1, &readfds, &writefds, &exceptfds,
                                interval)) < 0)
        {
            if (0 != _AtDebugLevel)
            {
                perror("Evaluating select failure:");
            }

            /* 8/22/95: tjw - Handle interruptions from signals */
            if (errno != EINTR)
            {
                break;
            }
        }
    } while (retval >= 0);

#endif /* AltiaFunctionPrototyping */
#endif /* WIN32 || STIMULUS TIMERS */

    /* Have a select failure - not good! */
}


/****************************************************************************
 * This function only checks to see if new events are pending and
 * gets one if they are.  It returns 0 if everything went ok and -1
 * if there was a disconnect detected during the check.
 * This routine should be called before every select(2) call if
 * any altiaPollEvent() calls are used.  It should be called after select(2)
 * if select returned because the altia descriptor had data.
 *
 *  NOTE:  This function assumes toolkit semaphore was taken by caller.
 *         The semaphore will be released temporarily to invoke callbacks
 *         and then retaken.
 *
 ****************************************************************************/
#if AltiaFunctionPrototyping
    static int loc_checkAltia(AtConnectId connection)
#else
    static int loc_checkAltia(connection)
    AtConnectId connection;
#endif
{
    unsigned int i;
    unsigned int originalSize;
    AltiaEventType value;
    CharPtr name;
    AtCallbackStruct *callList;

    if (altiaNextEvent(&name, &value) != 0)
    {

#ifdef ALTIAAPIDEBUG
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
            PRINTERR2("AtMainLoop: received connect %d disconnect\n",
                      connection);
        }
#endif /* ALTIAAPIDEBUG */

        /* Wasn't an event, but a disconnect */
        return -1;
    }
    else
    {
        /* 4/18/94: tjw - If new callbacks flag is set, then one or more
         *                callback entries also have the flag set.  Clear the
         *                flag for all entries so all entries appear old.
         */
        if (loc_newCBFlag)
        {
            callList = loc_callbackList;
            for (i = 0; i < loc_callbackSize; i++, callList++)
            {
                callList->newCBFlag = ALTIA_FALSE;
            }
            loc_newCBFlag = ALTIA_FALSE;
        }

        /* Must be carefull in this for loop with callback list. A callback
         * we invoke can very easily add more callbacks to list and change
         * address of callback list because of a realloc.
         */
        originalSize = loc_callbackSize;
        for (i = 0; i < originalSize; i++)
        {
            /* Get pointer to next entry every time through for loop */
            callList = &(loc_callbackList[i]);

            /* PRQA S 3415 4 */ /* M3CM 13.5: OK no side effects of evaluations */
            if ((callList->name != NULL)
                && (!(callList->newCBFlag)) /* 4/18/94: tjw - no call new CBs */
                && (callList->cnct == connection)
                && (API_STRCMP(callList->name, name) == 0))
            {
                /* Note: handlers that invoke the callback will do
                 * the necessary releasing and retaking of the tookit
                 * semaphore.
                 */

                switch (callList->type)
                {
                    case AtTextCb:
                        loc_handleTextCb(callList, value);
                        break;
                    case AtCloneCb:
                        loc_handleCloneCb(callList, value);
                        break;
                    case AtNormalCb:
                        loc_handleNormalCb(callList, value);
                        break;
                    default:
                        break;
                }

                if (0 == _AtConnected)
                {
                    break;
                }
            }
        }
    }

    return 0;
}


/*****************************************************************************/
/* Keep track of last time we checked timeout intervals */
/*****************************************************************************/
static struct timeval loc_lastTime;

#if AltiaFunctionPrototyping
    static unsigned long loc_getElapsedTime(struct timeval* now)
#else
    static unsigned long loc_getElapsedTime(now)
    struct timeval* now;
#endif
{
    struct timeval local_now;
#ifdef WIN32
    unsigned long ticks;
#elif defined(VXWORKS) || defined(USE_POSIX_SELECT_AND_TIME) || defined(GREEN_HILLS)
    struct timespec ticks;
#elif defined(PMAX)
#elif defined(LINUX) || defined(UNIX)
#elif defined(ALTIAGL)
    OS_timespec ticks;
#else
    struct timezone zone;
#endif

    if (now == NULL)
    {
        now = &local_now;
    }

    /* What's the time now */
#ifdef WIN32
    ticks = GetTickCount();
    now->tv_sec = (long) ticks / 1000L;
    now->tv_usec = ((long) ticks % 1000L) * 1000L;
#elif defined(VXWORKS) || defined(USE_POSIX_SELECT_AND_TIME) || defined(GREEN_HILLS)
    clock_gettime(CLOCK_REALTIME, &ticks);
    now->tv_sec = ticks.tv_sec;
    now->tv_usec = ticks.tv_nsec / 1000L;
#elif defined(PMAX)
    gettimeofday(now);
#elif defined(LINUX) || defined(UNIX)
    gettimeofday(now, NULL);
#elif defined(ALTIAGL)
    os_clock_gettime(OS_CLOCK_REALTIME, &ticks);
    now->tv_sec = ticks.tv_sec;
    now->tv_usec = ticks.tv_nsec / 1000L;
#else
    gettimeofday(now, &zone);
#endif

    /* Initialize last time if not already done */
    if (loc_timerCount <= 0)
    {
        loc_lastTime.tv_sec = now->tv_sec;
        loc_lastTime.tv_usec = now->tv_usec;
        loc_timerCount = 0;
        return 0;
    }

    /* How many milliseconds have elapsed since last time check */
    return (((unsigned long) (now->tv_sec) * 1000U) + ((unsigned long) (now->tv_usec) / 1000U))
           - (((unsigned long) (loc_lastTime.tv_sec) * 1000U) + ((unsigned long) (loc_lastTime.tv_usec) / 1000U));
}


/****************************************************************************
 * Check if anyone needs to be awoken since last check.
 * Updates static loc_lastTime variable so it is set to currentTime on exit.
 *
 *  NOTE:  This function assumes toolkit semaphore was taken by caller.
 *         The semaphore will be released temporarily to invoke callbacks
 *         and then retaken.
 *
 ****************************************************************************/
static void loc_checkTimeouts(void)
{
    unsigned int i;
    struct timeval now;
    long elapsed;
    AtTimerStruct *timerList;

    /* If no timers active, do nothing */
    if (loc_timerCount <= 0)
    {
        return;
    }

    /* How many milliseconds have elapsed since last time check */
    elapsed = (long) loc_getElapsedTime(&now);

    /* Update last time.  It is now the current time */
    loc_lastTime.tv_sec = now.tv_sec;
    loc_lastTime.tv_usec = now.tv_usec;

    /* Check for timeouts that need to be processed */
    /* tjw, 8/29/96:  Changed this for loop so we set the temp timerList
     * on each loop because it can change on us if the list gets reallocated.
     * as a result of a call to a timer procedure.
     */
    for (i = 0; i < loc_timerSize; i++)
    {
        timerList = &(loc_timerList[i]);
        if (timerList->proc != NULL)
        {
            /* Call everyone that has timed out */
            if (timerList->remaining <= elapsed)
            {
                AtTimerProc proc = timerList->proc;
                timerList->proc = NULL;

                /* It is assumed toolkit semaphore is taken at this
                 * moment.  It must be released for invoking callbacks.
                 */
                TargetAltiaSemRelease(1);

                (*(proc))(timerList->data, timerList->interval, (AtTimerId) i);

                /* Get back toolkit semaphore */
                TargetAltiaSemGet(1);
            }
            else
            {
                /* Haven't timed out.  Update time remaining. */
                timerList->remaining -= elapsed;
            }
        }
    }

    /* Now how many milliseconds have elapsed since last time check */
    elapsed = (long) loc_getElapsedTime(&now);

    /* Update last time.  It is now the current time */
    loc_lastTime.tv_sec = now.tv_sec;
    loc_lastTime.tv_usec = now.tv_usec;

    /* Update remaining times for all timers */
    loc_timerCount = 0;
    for (timerList = loc_timerList, i = 0; i < loc_timerSize; timerList++, i++)
    {
        if (timerList->proc != NULL)
        {
            /* Haven't timed out.  Update time remaining. */
            timerList->remaining -= elapsed;
            loc_timerCount++;
        }
    }
}


/****************************************************************************
 * Get time until next wakeup. Returns NULL if no timeouts are enabled.
 *
 *  NOTE:  This function assumes toolkit semaphore was taken by caller.
 *
 ****************************************************************************/
static struct timeval *loc_getWakeupTime(void)
{
    static struct timeval loc_wakeupTime;

    long shortest = 0x7fffffff;
    unsigned int i;
    AtTimerStruct *timerList;

    /* If no timers turned on, no wakeup interval necessary */
    if (loc_timerCount <= 0)
    {
        return NULL;
    }

    for (timerList = loc_timerList, i = 0; i < loc_timerSize; timerList++, i++)
    {
        if ((timerList->proc != NULL) && (timerList->remaining < shortest))
        {
            shortest = timerList->remaining;
        }
    }

    if (shortest == 0x7fffffff)
    {
        /* Flag so others no that timers are not active */
        loc_timerCount = 0;
        return NULL;
    }
    else
    {
        if (shortest < 0)
        {
            loc_wakeupTime.tv_sec = 0;
            loc_wakeupTime.tv_usec = 1;
            return &loc_wakeupTime;
        }
    }

    loc_wakeupTime.tv_sec = shortest/1000;
    loc_wakeupTime.tv_usec = (shortest%1000) * 1000;
    return &loc_wakeupTime;
}


#if !defined(WIN32) && defined(NO_STIMULUS_TIMERS)
/****************************************************************************
 * Check if sources need to be handled.
 *
 *  NOTE:  This function assumes toolkit semaphore was taken by caller.
 *
 ****************************************************************************/
#if AltiaFunctionPrototyping
    static void loc_checkInputs(AtInputType inputType, fd_set *mask)
#else
    static void loc_checkInputs(inputType, mask)
    AtInputType inputType;
    fd_set *mask;
#endif
{
    unsigned int i;
    AtInputStruct *inputList;

    /* tjw, 8/29/96:  Changed this for loop so we set the temp inputList
     * on each loop because it can change on us if the list gets reallocated.
     * as a result of a call to an input procedure.
     */
    for (i = 0; i < loc_inputSize; i++)
    {
        inputList = &(loc_inputList[i]);
        if ((inputList->proc != NULL) && (inputList->inputType == inputType))
        {
            if (FD_ISSET(inputList->source, mask))
            {
                (*(inputList->proc))(inputList->data, inputList->source, i);
            }
        }
    }
}
#endif  /* NOT WIN32 && NO STIMULUS TIMERS */


#if !defined(WIN32) && defined(NO_STIMULUS_TIMERS)
/****************************************************************************
 * Get build mask for given input type.
 *
 *  NOTE:  This function assumes toolkit semaphore was taken by caller.
 *
 ****************************************************************************/
#if AltiaFunctionPrototyping
    static int loc_getInputMask(AtInputType inputType, fd_set *mask,
                             int lastMaxFD)
#else
    static int loc_getInputMask(inputType, mask, lastMaxFD)
    AtInputType inputType;
    fd_set *mask;
    int lastMaxFD;
#endif
{
    unsigned int i;
    AtInputStruct *inputList;

    for (inputList = loc_inputList, i = 0; i < loc_inputSize; inputList++, i++)
    {
        if ((inputList->proc != NULL) && (inputList->inputType == inputType))
        {
            if (inputList->source > lastMaxFD)
            {
                lastMaxFD = inputList->source;
            }
            FD_SET(inputList->source, mask);
        }
    }

    return lastMaxFD;
}
#endif  /* NOT WIN32 && NO STIMULUS TIMERS */


/*****************************************************************************/
/*
 *  NOTE:  This function assumes toolkit semaphore was taken by caller.
 *         The semaphore will be released temporarily to invoke
 *         a callback and then retaken.
 */
#if AltiaFunctionPrototyping
    static void loc_handleNormalCb(AtCallbackStruct *entry, AltiaEventType value)
#else
    static void loc_handleNormalCb(entry, value)
    AtCallbackStruct *entry;
    AltiaEventType value;
#endif
{

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
#ifdef ALTIAFLOAT
        PRINTERR5(
            "AtMainLoop: calling proc 0x%lx for cnct %d event %s(%g)\n",
            entry->proc, entry->cnct,
            _localStr2ErrorStr(entry->name), value);
#else
        PRINTERR5(
            "AtMainLoop: calling proc 0x%lx for cnct %d event %s(%d)\n",
            entry->proc, entry->cnct,
            _localStr2ErrorStr(entry->name), value);
#endif
    }
#endif /* ALTIAAPIDEBUG */

    /* Release toolkit semaphore before invoking callback. */
    TargetAltiaSemRelease(1);

    /* Call suggested function. */
    /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtCallbackProc */
    (*((AtCallbackProc)(entry->proc)))
             (entry->cnct, entry->name, value, entry->data);

    /* Retake toolkit semaphore after invoking callback. */
    TargetAltiaSemGet(1);
}

#define STRSIZE 512

/*****************************************************************************/
/*
 *  NOTE:  This function assumes toolkit semaphore was taken by caller.
 *         The semaphore will be released temporarily to invoke
 *         a callback and then retaken.
 */
#if AltiaFunctionPrototyping
    static void loc_handleTextCb(AtCallbackStruct *entry, AltiaEventType value)
#else
    static void loc_handleTextCb(entry, value)
    AtCallbackStruct *entry;
    AltiaEventType value;
#endif
{
    AtTextStruct *info = (AtTextStruct *)(entry->typeData);
    AltiaCharType tmpStr[STRSIZE];
    CharPtr tmpPtr;
    CharPtr name;    /* 11-5-06: DC - Added to handle clone text callbacks */
    int len;

    /* Call suggested function. */
    if (value == (AltiaEventType) 0)
    {

#ifdef ALTIAAPIDEBUG
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
            char temp[512];
            strcpy(temp, _localStr2ErrorStr(entry->name));
            PRINTERR5(
                "AtMainLoop: calling proc 0x%lx for cnct %d text event %s(\"%s\")\n",
                entry->proc, entry->cnct, temp, _localStr2ErrorStr(info->string));
        }
#endif /* ALTIAAPIDEBUG */

        /* Since info could get deleted with the callback call
           We have to clear the data now.  This means making a copy
           of the string and passing the copy to the callback */
        info->offset = 0;
        len = (int) API_STRLEN(info->string);
        if (len < (STRSIZE - 1))
        {
            /* make a copy of the string incase it gets deleted */
            tmpPtr = tmpStr;
        }
        else
        {
#ifndef API_NOMALLOC
            tmpPtr
                = (AltiaCharType *) ALTIA_MALLOC(((unsigned int) len + 1U) * sizeof(AltiaCharType));
#else
            return;
#endif
        }
        API_STRCPY(tmpPtr, info->string);
        info->string[0] = (AltiaCharType) ('\0');

        /* Release toolkit semaphore before invoking callback. */
        TargetAltiaSemRelease(1);

        /* 11-5-06: DC - Added to handle clone text callbacks */
        /* PRQA S 3326 2 */ /* M3CM 13.4: OK to use assignment in evaluation */
        /* PRQA S 3415 2 */ /* M3CM 13.5: OK no side effects of evaluations */
        if ((info->clone != -1) && ((name = API_STRCHR(entry->name, (API_STRCHR_TYPE) (':'))) != NULL))
        {
            ++name;
            /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtCloneTextProc */
            (*((AtCloneTextProc)(entry->proc)))
                 (entry->cnct, info->clone, name, tmpPtr, entry->data);
        }
        else
        {
            /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtTextProc */
            (*((AtTextProc)(entry->proc)))
                 (entry->cnct, entry->name, tmpPtr, entry->data);
        }
        /* Retake toolkit semaphore after invoking callback. */
        TargetAltiaSemGet(1);

#ifndef API_NOMALLOC
        if (len >= (STRSIZE - 1))
        {
            ALTIA_FREE(tmpPtr);
        }
#endif


    }
    else
    {
        if (value != (AltiaEventType) -1) /* 9/7/95, tjw: With new textio object, ignore -1 */
        {

#ifdef ALTIAAPIDEBUG
            /* DEBUG PRINT-OUT */
            if (_AtDebugLevel > 1)
            {
                PRINTERR4(
                    "AtMainLoop: buffering cnct %d text event %s(\'%c\')\n",
                    entry->cnct, _localStr2ErrorStr(entry->name),
                    (char) value);
            }
#endif /* ALTIAAPIDEBUG */

            if (info->offset == (info->size - 1))
            {
#ifndef API_NOMALLOC
                info->string
                    = (AltiaCharType *) ALTIA_REALLOC((realloc_type) info->string,
                                                      (info->size + STRINGCALLBACKSIZE)
                                                      * sizeof(AltiaCharType));
                info->size += (int) STRINGCALLBACKSIZE;
#else
                return;
#endif
            }
            info->string[info->offset++] = (AltiaCharType) ((int) value);
            info->string[info->offset] = (AltiaCharType) ('\0');
        }
    }
}


/*****************************************************************************/
/*
 *  NOTE:  This function assumes toolkit semaphore was taken by caller.
 *         The semaphore will be released temporarily to invoke
 *         a callback and then retaken.
 */
#if AltiaFunctionPrototyping
    static void loc_handleCloneCb(AtCallbackStruct *entry, AltiaEventType value)
#else
    static void loc_handleCloneCb(entry, value)
    AtCallbackStruct *entry;
    AltiaEventType value;
#endif
{
    /* PRQA S 0306,0309 1 */ /* M3CM 11.4,11.6,1.3: OK cast AtPointer to int */
    int clone = (int)(entry->typeData);
    AltiaCharType *baseName;

    /* Call suggested function. */
    baseName = API_STRCHR(entry->name, (API_STRCHR_TYPE) (':'));
    if (baseName != NULL)
    {
        baseName++;

#ifdef ALTIAAPIDEBUG
        /* DEBUG PRINT-OUT */
        if (0 != _AtDebugLevel)
        {
#ifdef ALTIAFLOAT
            PRINTERR6(
                "AtMainLoop: calling proc 0x%lx for cnct %d clone %d event %s(%g)\n",
                entry->proc, entry->cnct, clone,
                _localStr2ErrorStr(baseName), value);
#else
            PRINTERR6(
                "AtMainLoop: calling proc 0x%lx for cnct %d clone %d event %s(%d)\n",
                entry->proc, entry->cnct, clone,
                _localStr2ErrorStr(baseName), value);
#endif
        }
#endif /* ALTIAAPIDEBUG */

        /* Release toolkit semaphore before invoking callback. */
        TargetAltiaSemRelease(1);

        /* PRQA S 0313 2 */ /* M3CM 11.1: OK cast proc to AtCloneProc */
        (*((AtCloneProc)(entry->proc)))
                     (entry->cnct, clone, baseName,
                      value, entry->data);

        /* Retake toolkit semaphore after invoking callback. */
        TargetAltiaSemGet(1);
    }
}


/*****************************************************************************/
/*
 *  NOTE:  This function assumes caller gets the appropriate semaphore
 *         to protect access to volatile data.
 */
#if AltiaFunctionPrototyping
    static void loc_cleanupNormalCb(AtCallbackStruct *entry)
#else
    static void loc_cleanupNormalCb(entry)
    AtCallbackStruct *entry;
#endif
{
#ifndef API_NOMALLOC
    ALTIA_FREE(entry->name);
    entry->name = NULL;
#else
    entry->name[0] = (AltiaCharType) ('\0');
#endif
    entry->proc = NULL;
    entry->cnct = -1;
    entry->type = AtNoneCb;
}


/*****************************************************************************/
/*
 *  NOTE:  This function assumes caller gets the appropriate semaphore
 *         to protect access to volatile data.
 */
#if AltiaFunctionPrototyping
    static void loc_cleanupTextCb(AtCallbackStruct *entry)
#else
    static void loc_cleanupTextCb(entry)
    AtCallbackStruct *entry;
#endif
{
    AtTextStruct *stringList = (AtTextStruct *)(entry->typeData);

#ifndef API_NOMALLOC
    ALTIA_FREE(stringList->string);
    ALTIA_FREE(stringList);
#else
    stringList->cb = -1;
    stringList->offset = 0;
    stringList->string[0] = (AltiaCharType) ('\0');
    stringList->clone = -1;
#endif

    entry->typeData = NULL;

#ifndef API_NOMALLOC
    ALTIA_FREE(entry->name);
    entry->name = NULL;
#else
    entry->name[0] = (AltiaCharType) ('\0');
#endif
    entry->proc = NULL;
    entry->cnct = -1;
    entry->type = AtNoneCb;
}


/*****************************************************************************/
/*
 *  NOTE:  This function assumes caller gets the appropriate semaphore
 *         to protect access to volatile data.
 */
#if AltiaFunctionPrototyping
    static void loc_cleanupCloneCb(AtCallbackStruct *entry)
#else
    static void loc_cleanupCloneCb(entry)
    AtCallbackStruct *entry;
#endif
{
    entry->type = AtNoneCb;

#ifndef API_NOMALLOC
    ALTIA_FREE(entry->name);
    entry->name = NULL;
#else
    entry->name[0] = (AltiaCharType) ('\0');
#endif
    entry->proc = NULL;
    entry->cnct = -1;
}


/*****************************************************************************/
/*
 *  NOTE:  This function assumes caller gets the appropriate semaphore
 *         to protect access to volatile data.
 */
#if AltiaFunctionPrototyping
    static int loc_getCallbackEntry(AtConnectId connection,
                                 CONST AltiaCharType *name, int type,
                                 AtFPointer proc, AtPointer data)
#else
    static int loc_getCallbackEntry(connection, name, type, proc, data)
    AtConnectId connection;
    CONST AltiaCharType *name;
    int type;
    AtFPointer proc;
    AtPointer data;
#endif
{
    unsigned int i;
    AtCallbackStruct *callList;

    for (callList = loc_callbackList, i = 0; i < loc_callbackSize; callList++, i++)
    {
        if (name != NULL)
        {
            /* PRQA S 3415 9 */ /* M3CM 13.5: OK no side effects of eval */
            if ((callList->name != NULL)
                && (callList->cnct == connection)
                && ((callList->proc == proc) || (proc == NULL))
                && ((callList->data == data) || (data == NULL))
                && (0U != ((unsigned int)(callList->type) & (unsigned int) type))
                && ((API_STRCMP(name, callList->name) == 0)
                    || ((type == AtAnyCb) && (callList->type == AtCloneCb)
                        && (API_STRCMP(name, API_STRCHR(callList->name, (API_STRCHR_TYPE) (':'))
                                             + sizeof(AltiaCharType)) == 0))))
            {
                return (int) i;
            }
        }
        else
        {
            if ((callList->name == NULL) || (callList->name[0] == (AltiaCharType) ('\0')))
            {
                return (int) i;
            }
        }
    }

    if (name != NULL)
    {
        return -1;
    }

#ifdef API_NOMALLOC
    if (loc_callbackSize == 0U)
    {
        AtTextStruct *stringList;

        for (callList = loc_callbackList, i = 0; i < CALLBACKSIZE; callList++, i++)
        {
            callList->cnct = -1;
            callList->proc = NULL;
            callList->name[0] = (AltiaCharType) ('\0');
            callList->type = AtNoneCb;
        }
        loc_callbackSize = CALLBACKSIZE;

        for (stringList = loc_stringList, i = 0;
             i < CALLBACKSTRINGS; stringList++, i++)
        {
            stringList->size = (int) STRINGCALLBACKSIZE;
            stringList->cb = -1;
            stringList->offset = 0;
            stringList->string[0] = (AltiaCharType) ('\0');
        }
        loc_stringSize = CALLBACKSTRINGS;

        return 0;
    }
    return -1;

#else
    if (loc_callbackSize == 0U)
    {
        loc_callbackList =
           (AtCallbackStruct *) ALTIA_MALLOC(sizeof(AtCallbackStruct) * CALLBACKSIZE);
    }
    else
    {
        loc_callbackList =
            (AtCallbackStruct *) ALTIA_REALLOC((realloc_type) loc_callbackList,
                 (loc_callbackSize + CALLBACKSIZE) * sizeof(AtCallbackStruct));
    }

    if (loc_callbackList == NULL)
    {
        return -1;
    }

    callList = loc_callbackList + loc_callbackSize;

    for (i = 0; i < CALLBACKSIZE; i++, callList++)
    {
        callList->cnct = -1;
        callList->proc = NULL;
        callList->name = NULL;
        callList->type = AtNoneCb;
    }

    i = loc_callbackSize;
    loc_callbackSize += CALLBACKSIZE;
    return (int) i;
#endif
}


#if !defined(WIN32) && defined(NO_STIMULUS_TIMERS)
/*****************************************************************************/
/*
 *  NOTE:  This function assumes caller gets the appropriate semaphore
 *         to protect access to volatile data.
 */
#if AltiaFunctionPrototyping
    static int loc_getInputEntry(AtInputProc proc)
#else
    static int loc_getInputEntry(proc)
    AtInputProc proc;
#endif
{
    unsigned int i;
    AtInputStruct *inputList;

    for (inputList = loc_inputList, i = 0; i < loc_inputSize; inputList++, i++)
    {
        if (inputList->proc == proc)
        {
            return (int) i;
        }
    }

    if (proc != NULL)
    {
        return -1;
    }

#ifdef API_NOMALLOC
    if (loc_inputSize == 0U)
    {
        for (inputList = loc_inputList, i = 0; i < INPUTSIZE; inputList++, i++)
        {
            inputList->proc = NULL;
        }

        loc_inputSize = INPUTSIZE;
        return 0;
    }
    return -1;

#else
    if (loc_inputSize == 0U)
    {
        loc_inputList =
           (AtInputStruct *) ALTIA_MALLOC(sizeof(AtInputStruct) * INPUTSIZE);
    }
    else
    {
        loc_inputList =
            (AtInputStruct *) ALTIA_REALLOC((realloc_type) loc_inputList,
                 (loc_inputSize + INPUTSIZE) * sizeof(AtInputStruct));
    }

    if (loc_inputList == NULL)
    {
        return -1;
    }

    inputList = loc_inputList + loc_inputSize;

    for (i = 0; i < INPUTSIZE; i++, inputList++)
    {
        inputList->proc = NULL;
    }

    i = loc_inputSize;
    loc_inputSize += INPUTSIZE;
    return (int) i;
#endif
}
#endif  /* NOT WIN32 && NO STIMULUS TIMERS */


/*****************************************************************************/
/*
 *  NOTE:  This function assumes caller gets the appropriate semaphore
 *         to protect access to volatile data.
 */
#if AltiaFunctionPrototyping
    static int loc_getTimerEntry(AtTimerProc proc)
#else
    static int loc_getTimerEntry(proc)
    AtTimerProc proc;
#endif
{
    unsigned int i;
    AtTimerStruct *timerList;

    for (timerList = loc_timerList, i = 0; i < loc_timerSize; timerList++, i++)
    {
        if (timerList->proc == proc)
        {
            return (int) i;
        }
    }

    if (proc != NULL)
    {
        return -1;
    }

#ifdef API_NOMALLOC
    if (loc_timerSize == 0U)
    {
        for (timerList = loc_timerList, i = 0; i < TIMEOUTSIZE; timerList++, i++)
        {
            timerList->proc = NULL;
        }

        loc_timerSize = TIMEOUTSIZE;
        return 0;
    }
    return -1;

#else
    if (loc_timerSize == 0U)
    {
        loc_timerList =
           (AtTimerStruct *) ALTIA_MALLOC(sizeof(AtTimerStruct) * TIMEOUTSIZE);
    }
    else
    {
        loc_timerList =
            (AtTimerStruct *) ALTIA_REALLOC((realloc_type) loc_timerList,
                 (TIMEOUTSIZE + loc_timerSize) * sizeof(AtTimerStruct));
    }

    if (loc_timerList == NULL)
    {
        return -1;
    }

    timerList = loc_timerList + loc_timerSize;

    for (i = 0; i < TIMEOUTSIZE; i++, timerList++)
    {
        timerList->proc = NULL;
    }

    i = loc_timerSize;
    loc_timerSize += TIMEOUTSIZE;
    return (int) i;
#endif
}
