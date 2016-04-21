/* PRQA S 0292 EOF */ /* M3CM 1.1: Allow '$','@','`' in comments */
/* PRQA S 3200 EOF */ /* M3CM 17.7: Allow unused function return values */
/* $Revision: 1.43 $    $Date: 2009-02-05 15:49:11 $
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
/* FILE     altiaAPI.c
 *
 * AUTHOR       : Altia, Incorporated
 *                Copyright 1991-2001, Altia, Incorporated
 *                All rights reserved.
 *
 *
 * ONELINER     : Altia API functions for DeepScreen standalone application
 *
 * DESCRIPTION  : This file contains versions of the Altia API functions
 *                that work with DeepScreen generated code in a
 *                standalone environment (i.e., no connection to an
 *                existing Altia session is established to receive events).
 *                In such an environment, events come from Altia Stimulus
 *                or they can be injected from custom code by calling the
 *                altiaLocalAddEvent() function supplied in this file.
 *
 *                The altiaLocalAddEvent() function is unique to this
 *                standalone version of the Altia API.  The function
 *                prototype for altiaLocalAddEvent() is:
 *
 *     #ifndef UNICODE
 *     int altiaLocalAddEvent(char *eventName, LocalEventType value)
 *     #else
 *     int altiaLocalAddEvent(wchar_t *eventName, LocalEventType value)
 *     #endif
 *
 *                NOTE THAT THIS FUNCTION TAKES AN 8-BIT STRING ON NON-UNICODE
 *                SYSTEMS OR A 16-BIT STRING ON UNICODE.  THIS IS INDEPENDENT
 *                OF THE REST OF THE API FUNCTIONS.  AND VALUE MUST BE THE
 *                VALUE TYPE EXPECTED BY DEEPSCREEN CODE (USUALLY DOUBLE) AND
 *                NOT JUST AN INT IF THIS IS AN INTEGER API.
 *
 *                In any case, all events injected with altiaLocalAddEvent()
 *                must first be selected with AtSelectEvent() or
 *                altiaSelectEvent().  And then they can be received with
 *                AtNextEvent(), altiaNextEvent(), AtCheckEvent() or
 *                altiaCheckEvent().  See Section II of the Altia API manual
 *                for complete details regarding these standard Altia API
 *                functions and others.
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

/* UNIX variants need stdlib.h for exit definition. */
#if defined(sun) || defined(SGI) || defined(MICROWIN) || defined(VXWORKS) || defined(LINUX) || defined(UNIX)
#include <stdlib.h>
#endif

#if defined(WIN16) || defined(WIN32)
#ifdef MINSHORT
#undef MINSHORT
#undef MAXSHORT
#endif
#include <windows.h>
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
#else
#undef CONST
#define CONST
#endif /* C++ */


/***********************************/
/* Establish some string functions */
/***********************************/
#ifdef UNICODE
#ifndef ALT_SSCANF
#define ALT_SSCANF swscanf
#endif
#ifndef ALT_API_STRCMP
#ifdef ALTIAUNICODEAPI
#define ALT_API_STRCMP wcscmp
#define ALT_API_STRCPY wcscpy
#define ALT_API_STRLEN wcslen
#else
#define ALT_API_STRCMP strcmp
#define ALT_API_STRCPY strcpy
#define ALT_API_STRLEN strlen
#endif
#endif

#else
#ifndef ALT_SSCANF
#define ALT_SSCANF sscanf
#endif
#ifndef ALT_API_STRCMP
#define ALT_API_STRCMP strcmp
#define ALT_API_STRCPY strcpy
#define ALT_API_STRLEN strlen
#endif

#endif /* UNICODE */

/************************************************/
/* Establish value for NULL if not already done */
/************************************************/
#ifndef NULL
#define NULL 0
#endif

/*************************************************************************
 * For event error, must define in this file!
 *************************************************************************/
/* PRQA S 0602 1 */   /* M3CM 1.3,21.2: OK reserved name for back compat */
int _altiaBreakLoop = 0;

/*************************************************************************
 * For debug in the API functions.
 *************************************************************************/
#if defined(WIN16) || defined(WIN32)
#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif
     /* PRQA S 3447 1 */   /* M3CM 8.5: OK extern for back compat */
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
#define PRINTERR1(a) (fprintf(stderr,a))
#define PRINTERR2(a,b) (fprintf(stderr,a,b))
#define PRINTERR3(a,b,c) (fprintf(stderr,a,b,c))
#define PRINTERR4(a,b,c,d) (fprintf(stderr,a,b,c,d))
#define PRINTERR5(a,b,c,d,e) (fprintf(stderr,a,b,c,d,e))
#define PRINTERR6(a,b,c,d,e,f) (fprintf(stderr,a,b,c,d,e,f))

#endif /* WINDOWS VARIATIONS ELSE NOT WINDOWS */
#endif /* ALTIAAPIDEBUG */


/*****************************************************************************
 * Some forward declarations for local functions.
 *****************************************************************************/
static void localReportAnimation(
    ALTIA_CHAR *localName,
    LocalEventType localValue
);
static int localAltiaAnimate(
    CONST ALTIA_CHAR *localName,
    LocalEventType localValue
);
static ALTIA_BOOLEAN localNextEvent(
    CONST ALTIA_CHAR **localName,
    LocalEventType *localValue
);
static void localFixQueue(
    void
);
static ALTIA_BOOLEAN localPollEvent(
    CONST ALTIA_CHAR *localName,
    LocalEventType *localValue
);
static ALTIA_BOOLEAN localCheckEvent(
    CONST ALTIA_CHAR *localName,
    LocalEventType *localValue
);
static int localGetEventCount(
    void
);
static ALTIA_BOOLEAN localSelectEvent(
    CONST AltiaCharType *name
);
static ALTIA_BOOLEAN localEventSelected(
    CONST AltiaCharType *name
);
static ALTIA_BOOLEAN localUnselectEvent(
    CONST AltiaCharType *name
);
static void localSetIgnoreEvent(
    CONST ALTIA_CHAR *localName,
    LocalEventType localValue
);
static ALTIA_BOOLEAN localCompareIgnoreEvent(
    ALTIA_CHAR *localName,
    LocalEventType localValue
);
static int localAltiaAnimateId(
    int localNameId,
    LocalEventType localValue
);
static void localSetIgnoreEventId(
    int localNameId,
    LocalEventType localValue
);


/*****************************************************************************
 * Local event queue declarations.  For now, event queue size is fixed
 * unless ALTIA_APIQ_SIZE is already defined.  For Windows, UNIX and LINUX,
 * default size is larger.  If ALTIA_APIQ_MALLOC is defined, events will go
 * onto the queue as allocated strings instead of just pointers to strings
 * provided by callers of altiaLocalAddEvent().  As an example, this is
 * necessary if code for clones is enabled.  The clone event name usually
 * comes from a temporary string that will not be around after the call to
 * altiaLocalAddEvent().
 *****************************************************************************/
#ifdef ALTIAAPITEST
#define LOCALQUEUESIZE 10
#elif defined(ALTIA_APIQ_SIZE) && (ALTIA_APIQ_SIZE > 0)
#define LOCALQUEUESIZE ALTIA_APIQ_SIZE
#elif defined(WIN32) || defined(sun) || defined(UNIX) || defined(LINUX)
#define LOCALQUEUESIZE 300
#else
#define LOCALQUEUESIZE 100
#endif
#define LOCALMAXEVENT (LOCALQUEUESIZE - 1)
static int localFirstEvent = -1;
static int localLastEvent = -1;

#ifdef ALTIA_APIQ_MALLOC
static ALTIA_BOOLEAN localEventQueueInited = ALTIA_FALSE;
#endif
static struct localEventStruct
{
    CONST ALTIA_CHAR *name;
    LocalEventType value;
#ifdef ALTIA_APIQ_MALLOC
    ALTIA_CHAR *buffer;
    unsigned int bufferSize;
#endif
} localEventQueue[LOCALQUEUESIZE];

/******************************************
 * FUNCTION: localEventQueueInit()
 * Initialize event queue if it needs it.
 ******************************************/
#if AltiaFunctionPrototyping
static void localEventQueueInit(void)
#else
static void localEventQueueInit()
#endif
{
    /* Only take action if event queue elements are dynamically allocated
     * and queue is not already initialized.
     */
#ifdef ALTIA_APIQ_MALLOC
    if (!localEventQueueInited)
    {
        struct localEventStruct *ptr = localEventQueue;
        int i;
        for (i = 0; i < LOCALQUEUESIZE; i++, ptr++)
        {
            ptr->bufferSize = 0;
        }
        localEventQueueInited = ALTIA_TRUE;
    }
#endif
}

/******************************************
 * FUNCTION: localEventQueueAdd()
 * Adding an event to the queue only takes
 * extra effort if the queue entry element
 * names are dynamically allocated so
 * this function will only exist if the
 * ALTIA_APIQ_MALLOC macro is defined.
 ******************************************/
#ifdef ALTIA_APIQ_MALLOC
#if AltiaFunctionPrototyping
static void localEventQueueAdd(struct localEventStruct *ptr,
                               ALTIA_CHAR *localName, 
                               LocalEventType localValue)
#else
static void localEventQueueAdd(ptr, localName, localValue)
struct localEventStruct *ptr;
ALTIA_CHAR *localName;
LocalEventType localValue;
#endif
{
    unsigned int localSize = ALT_STRLEN(localName) + 1U;
    if (!localEventQueueInited)
    {
        localEventQueueInit();
    }
    /* First make sure there is enough space for the new string. */
    if (ptr->bufferSize < localSize)
    {
        /* Need more space for this new string!  Get a little extra
         * to reduce future memory allocation calls.
         */
        localSize += 16U;
        if ((ptr->bufferSize > 0U) && (ptr->buffer != NULL))
        {
            ALTIA_FREE(ptr->buffer);
        }
        ptr->buffer = (ALTIA_CHAR *) ALTIA_MALLOC(localSize * sizeof(ALTIA_CHAR));
        if (NULL == ptr->buffer)
        {
            ptr->bufferSize = 0;
            ptr->buffer = NULL;
            ptr->name = NULL;
            return;
        }
        ptr->bufferSize = localSize;
    }
    /* ALWAYS update the queue entry's name with the buffer pointer
     * because the name is NULL if this entry has been out of use
     * (which should always be the case).
     */
    ptr->name = ptr->buffer;
    /* Now it is OK to update the queue entry's name and value elements. */
    ALT_STRCPY(ptr->name, localName);
    ptr->value = localValue;
}
#endif

/******************************************
 * FUNCTION: localEventQueueFree()
 * Free event queue elements if needed.
 ******************************************/
#if AltiaFunctionPrototyping
static void localEventQueueFree(void)
#else
static void localEventQueueFree()
#endif
{
    /* Only take action if event queue elements are dynamically allocated
     * and queue was previously initialized (and therefore might have
     * elements that need to be freed).
     */
#ifdef ALTIA_APIQ_MALLOC
    if (localEventQueueInited)
    {
        struct localEventStruct *ptr = localEventQueue;
        int i;
        for (i = 0; i < LOCALQUEUESIZE; i++, ptr++)
        {
            if (ptr->bufferSize > 0U)
            {
                if (ptr->buffer != NULL)
                {
                    ALTIA_FREE(ptr->buffer);
                    ptr->buffer = NULL;
                }
                ptr->bufferSize = 0;
            }
            ptr->name = NULL;
        }
        localEventQueueInited = ALTIA_FALSE;
    }
#endif
}


/*****************************************************************************
 * Local selected event list.  For now, selected event list size is fixed
 * unless ALTIA_APISELECT_SIZE is already defined or LOCALSELECTEDSIZE is
 * already defined.  For Windows, UNIX and LINUX, default size is larger.
 *****************************************************************************/
#ifdef ALTIA_APISELECT_SIZE
#define LOCALSELECTEDSIZE ALTIA_APISELECT_SIZE
#elif !defined(LOCALSELECTEDSIZE)
#if defined(WIN32) || defined(sun) || defined(UNIX) || defined(LINUX)
#define LOCALSELECTEDSIZE 300
#else
#define LOCALSELECTEDSIZE 100
#endif /* WIN32 || sun || UNIX || LINUX */
#endif /* ALTIA_APISELECT_SIZE */

/* If CALLBACKSIZE defined and larger, use it for selected event list size */
#if defined(CALLBACKSIZE) && (CALLBACKSIZE > LOCALSELECTEDSIZE)
#undef LOCALSELECTEDSIZE
#define LOCALSELECTEDSIZE CALLBACKSIZE
#endif /* CALLBACKSIZE */

#define LOCALMAXSELECTED (LOCALSELECTEDSIZE - 1)
static int localLastSelected = -1;
static struct localSelectedStruct
{
    CONST AltiaCharType *name;
} localSelectedList[LOCALSELECTEDSIZE];








/*****************************************************************************
 * Other local variable declarations.
 *****************************************************************************/
static ALTIA_CHAR    *localEmptyString = (ALTIA_CHAR *) (ALT_TEXT(""));

#ifdef ALTIAUNICODEAPI
static AltiaCharType *apiEmptyString = (AltiaCharType *) (ALT_TEXT(""));
static AltiaCharType *apiAllEvents = (AltiaCharType *) (ALT_TEXT("*"));
static AltiaCharType *apiCloseViewPendingString = (AltiaCharType *) (ALT_TEXT("altiaCloseViewPending"));
#else
static AltiaCharType *apiEmptyString = (AltiaCharType *) ("");
static AltiaCharType *apiAllEvents = (AltiaCharType *) ("*");
static AltiaCharType *apiCloseViewPendingString = (AltiaCharType *) ("altiaCloseViewPending");
#endif

static unsigned int  localNoCheckCount = 0;

/* PRQA S 0750 5 */   /* M3CM 19.2: OK union for back compat */
typedef union
{
    int ival;
    float fval;
} IntFloatType;


/*****************************************************************************
 * For running under UNICODE, need some definitions for wide char to char
 * conversions.
 *****************************************************************************/


/*****************************************************************************
 * FUNCTION:  _localStr2ErrorStr
 *
 * On a UNICODE system, it may be necessary to convert a string
 * of wide characters to a string of normal (char) characters for the
 * purpose of printing error messages.  Result is returned in a temporary
 * buffer which will get overwritten next time this function is called.
 ****************************************************************************/
#if defined(ALTIAAPITEST) || defined(ALTIAAPIDEBUG)
#if AltiaFunctionPrototyping
char *_localStr2ErrorStr(CONST ALTIA_CHAR *name)
#else
char *_localStr2ErrorStr(name)
CONST ALTIA_CHAR *name;
#endif
{
#ifdef UNICODE

#define MAX_localStr2ErrorStr_SIZE 256
    static char _tempByte[MAX_localStr2ErrorStr_SIZE];
    int res;

    if (name == NULL)
    {
        return NULL;
    }

    res = wcstombs(_tempByte, name, MAX_localStr2ErrorStr_SIZE); 
    return _tempByte;

#else
    return (char *) name;

#endif

}

#endif /* ALTIAAPITEST || ALTIAAPIDEBUG */


/*****************************************************************************
 * FUNCTION:  apiStr2LocalStr
 *
 * Because of UNICODE system support, it may be necessary to convert an
 * 8-bit character string coming from user's API calls to a wide character
 * string going to DeepScreen code.  Result may be returned in a temporary
 * buffer which will get overwritten next time this function is called.
 ****************************************************************************/
#if AltiaFunctionPrototyping
static CONST ALTIA_CHAR *apiStr2LocalStr(CONST AltiaCharType *str)
#else
static CONST ALTIA_CHAR *apiStr2LocalStr(str)
CONST AltiaCharType *str;
#endif
{

/* We only need to do something if we are on a UNICODE target, but API
 * interface is not UNICODE.  Then, we need to take the 8-bit version of
 * the string and turn it into a wide char version.
 */
#if defined(UNICODE) && !defined(ALTIAUNICODEAPI)

#define MAX_apiStr2LocalStr_SIZE 256
    static ALTIA_CHAR  _resultStr[MAX_apiStr2LocalStr_SIZE];
    int res;

    if (str == NULL)
    {
        return NULL;
    }

    res = mbstowcs(_resultStr, str, MAX_apiStr2LocalStr_SIZE); 
    return _resultStr;

#else
    return str;
#endif

}


/*****************************************************************************
 * FUNCTION:  apiValue2LocalValue
 *
 * If API is integer based, it is necessary to convert an integer event
 * value into a double event value for DeepScreen.
 ****************************************************************************/
#if AltiaFunctionPrototyping
static LocalEventType apiValue2LocalValue(CONST ALTIA_CHAR *localName,
                                          AltiaEventType eventValue)
#else
static LocalEventType apiValue2LocalValue(localName, eventValue)
CONST ALTIA_CHAR *localName;
AltiaEventType eventValue;
#endif
{
    /* We only need to do something if we are implementing an integer version
     * of the API.  Then, we need to take the int value from the API and
     * convert it to a double for DeepScreen.
     */
#if defined(ALTIAINTAPI) || !defined(ALTIAFLOAT)
    LocalEventType retValue;


    {
        retValue = (LocalEventType) eventValue;

#ifdef ALTIAAPIDEBUG
        PRINTERR3("Animation \"%s\" = %g is TO AN INT\n",
                  _localStr2ErrorStr(localName), (double) retValue);
#endif /* ALTIAAPIDEBUG */

    }

    return retValue;
#else
    return (LocalEventType) eventValue;
#endif /* ALTIAINTAPI || !ALTIAFLOAT */
}

#if AltiaFunctionPrototyping
static LocalEventType apiValue2LocalValueId(int nameId,
                                            AltiaEventType eventValue)
#else
static LocalEventType apiValue2LocalValueId(nameId, eventValue)
int nameId;
AltiaEventType eventValue;
#endif
{
    /* We only need to do something if we are implementing an integer version
     * of the API.  Then, we need to take the int value from the API and
     * convert it to a double for DeepScreen.
     */
#if defined(ALTIAINTAPI) || !defined(ALTIAFLOAT)
    LocalEventType retValue;


    {
        retValue = (LocalEventType) eventValue;

#ifdef ALTIAAPIDEBUG
        PRINTERR3("Animation \"%d\" = %g is TO AN INT\n",
                  nameId, (double) retValue);
#endif /* ALTIAAPIDEBUG */

    }

    return retValue;
#else
    return (LocalEventType) eventValue;
#endif /* ALTIAINTAPI || !ALTIAFLOAT */
}


/*****************************************************************************
 * FUNCTION:  localValue2APIValue
 *
 * If API is integer based, it is necessary to convert a DeepScreen double
 * event value to an API integer value.
 ****************************************************************************/
#if AltiaFunctionPrototyping
static AltiaEventType localValue2APIValue(CONST ALTIA_CHAR *localName,
                                          LocalEventType localValue)
#else
static AltiaEventType localValue2APIValue(localName, localValue)
CONST ALTIA_CHAR *localName;
LocalEventType localValue;
#endif
{

/* We only need to do something if we are implementing an integer version
 * of the API.  Then, we need to take a double value coming from DeepScreen
 * and convert it to an int for the API.
 */
#if defined(ALTIAINTAPI) || !defined(ALTIAFLOAT)

    AltiaEventType retValue;

 
    {
        /* 11/19/01:  Round instead of just type casting.  In order to
         * round the largest possible positive number, we want to cast
         * the addition result to unsigned (versus int).  This will
         * ensure, for example, that we can correctly handle X key sym
         * values for the function keys in X11 because they have values
         * like 0xffbe0000.  These come out of stimulus as doubles
         * (not a problem) and then we use the rounding algorithm to make
         * them integers for an integer API.  But, we will end up with
         * 0x7fffffff whenever the double value is 0x80000000 or larger
         * if we do a signed type cast instead of unsigned.  For negative
         * values, unsigned type casting only generates a warning from
         * the compiler to inform us that it isn't doing anything so
         * just do an integer type cast.
         */



        retValue = (LocalEventType) localValue;


#ifdef ALTIAAPIDEBUG
        PRINTERR3("Animation \"%s\" = %g is FROM AN INT\n",
                  _localStr2ErrorStr(localName), (double) retValue);
#endif /* ALTIAAPIDEBUG */

    }

    return retValue;

#else

    return localValue;

#endif /* ALTIAINTAPI || !ALTIAFLOAT */

}




/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*                   BEGIN LOCAL UTILITY FUNCTIONS                           */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/


/*****************************************************************************
 * FUNCTION:  altiaLocalAddEvent
 *
 * Adds an event to the local queue.  The queue can wrap around for
 * efficiency so the first event's index may be greater than the
 * last event's index.  Returns 1 if event was successfully added,
 * 0 if no more room is left.
 *
 * NOTE
 * ----
 * This function is global so that others outside of this file can use
 * it.  For example, an embedded system might need to poll hardware buttons
 * for input and if there is input, it might want to add them to our
 * queue as Altia events so they get processed like all other Altia
 * events from Altia's own stimulus or control.
 *
 * WARNING
 * -------
 * This code assumes that the character string pointed to by the name
 * parameter resides in static memory.  That is to say, we can store
 * just a pointer to it instead of copying the name into our own space.
 * This saves memory big time.
 *
 * NOTE
 * ----
 * String must be wide string on a UNICODE system and not just an 8-bit
 * string.  And value must be the value type expected by the deepscreen code
 * (usually double) and not just an int if this is an integer API.
 *****************************************************************************/
#if AltiaFunctionPrototyping
int altiaLocalAddEvent(AltiaNativeChar *eventName, LocalEventType value)
#else
int altiaLocalAddEvent(eventName, value)
AltiaNativeChar *eventName;
LocalEventType value;
#endif
{
    AltiaCharType *name;

    /* 11/27/01:  If we are on a UNICODE target, but API is not UNICODE,
     * must temporarily convert UNICODE string coming from DeepScreen
     * into an 8-bit string to check if it is an event of interest.
     */
#if defined(UNICODE) && !defined(ALTIAUNICODEAPI)
#define MAX_localAddEventStr2APIStr_SIZE 256
    char apiName[MAX_localAddEventStr2APIStr_SIZE];
    int res;
#endif /* UNICODE && !ALTIAUNICODEAPI */

#ifdef ALTIAAPIDEBUG
    PRINTERR3("\nInto altiaLocalAddEvent(0x%x, %g\n",
              eventName, (double) value);
    PRINTERR3("\tqueue first = %d, last = %d\n",
              localFirstEvent, localLastEvent);
#endif /* ALTIAAPIDEBUG */

    /* If we cannot fit it, return -1 */
    if ((eventName == NULL) || (*eventName == (AltiaCharType) ('\0'))
        || ((localFirstEvent >= 0)
            && ((localLastEvent == (localFirstEvent - 1))
                || ((localFirstEvent == 0) && (localLastEvent == LOCALMAXEVENT)))))
    {

#ifdef ALTIAAPIDEBUG
    PRINTERR1("\tERROR:  name parameter is bad or queue is full\n\n");
#endif /* ALTIAAPIDEBUG */

        return 0;
    }

    /* 11/27/01:  If we are on a UNICODE target, but API is not UNICODE,
     * must temporarily convert UNICODE string coming from DeepScreen
     * into an 8-bit string to check if it is an event of interest.
     */
#if defined(UNICODE) && !defined(ALTIAUNICODEAPI)
    res = wcstombs(apiName, eventName, MAX_localAddEventStr2APIStr_SIZE); 
    name = apiName;
#else
    name = eventName;
#endif /* UNICODE && !ALTIAUNICODEAPI */

    /* Don't accept an event that we are not selected to receive */
    if (!localEventSelected(name))
    {

#ifdef ALTIAAPIDEBUG
    PRINTERR2("\tevent \"%s\" is NOT selected!\n\n",
              _localStr2ErrorStr(eventName));
#endif /* ALTIAAPIDEBUG */

        return 0;
    }

#ifdef ALTIAAPIDEBUG
    PRINTERR2("\tqueueing selected event \"%s\"...\n",
              _localStr2ErrorStr(eventName));
#endif /* ALTIAAPIDEBUG */

    /* If this is our first event, fix our first event index. */
    if (localFirstEvent == -1)
    {
        localFirstEvent = 0;
    }

    /* New event goes after our last event.  Wrap our last event
     * index if necessary.
     */
    if (++localLastEvent > LOCALMAXEVENT)
    {
        localLastEvent = 0;
    }

#ifndef ALTIA_APIQ_MALLOC
    /* Adding an event to an entry in the queue is easy if queue
     * entry names are not dynamically allocated.
     */
    localEventQueue[localLastEvent].name = eventName;
    localEventQueue[localLastEvent].value = value;
#else
    /* Need to do some work for dynamically allocated queue entry names. */
    localEventQueueAdd(&(localEventQueue[localLastEvent]),
                       eventName, value);
#endif

#ifdef ALTIAAPIDEBUG
    PRINTERR4("\tindex %d has new event \"%s\" = %g\n",
              localLastEvent, _localStr2ErrorStr(eventName),
              (double) value);
    PRINTERR3("\ton exit, queue first = %d, last = %d\n\n",
              localFirstEvent, localLastEvent);
#endif /* ALTIAAPIDEBUG */

    return 1;
}

int altiaLocalAddEventId(int nameId, LocalEventType value)
{
    ALTIA_CHAR * localName = AltiaAnimationIdToName(nameId);
    if (NULL != localName)
    {
        return altiaLocalAddEvent(localName, value);
    }

    return 0;
}


/*****************************************************************************
 * localAltiaAnimate
 * This function is called to perform an Animate of the Altia graphics.
 * It keeps track of the animation that we are executing so that we
 * ignore it if it gets picked up by localReportAnimation().
 ****************************************************************************/
#if AltiaFunctionPrototyping
static int localAltiaAnimate(CONST ALTIA_CHAR *localName,
                             LocalEventType localValue)
#else
static int localAltiaAnimate(localName, localValue)
CONST ALTIA_CHAR *localName;
LocalEventType localValue;
#endif
{
    int retVal;

    /* Want to ignore events that we generate in this function so flag
     * the animation (see localReportAnimation() for further details).
     */
    localSetIgnoreEvent(localName, localValue);

    /* If animate really causes something to happen, update
     * graphics if we are not in caching mode.  Else, set flag
     * indicating that an update is necessary at some point.
     */
    retVal = TargetAltiaAnimate((ALTIA_CHAR *) localName, localValue);
    if (0 != retVal)
    {
        if (0 == _altiaAPICaching)
        {
            int checkReturn;

            TargetAltiaUpdate();

            /* Were about to check for events so clear check count. */
            localNoCheckCount = 0;

           /* Do one loop of system event processing. */
           if (TargetCheckEvent(&checkReturn) < 0)
           {
               /* Assume we need to break due to DeepScreen window close */
               _altiaBreakLoop = 1;
               return -1;
           }
        }
        else
        {
            _altiaAPINeedUpdate = 1;
        }

        retVal = 1;
    }

    /* If we haven't checked for events lately, do it! */
    localNoCheckCount++;
    if (localNoCheckCount >= 50U)
    {
        int checkReturn;

        /* Were about to check for events so clear check count. */
        localNoCheckCount = 0;

        /* Do one loop of system event processing. */
        if (TargetCheckEvent(&checkReturn) < 0)
        {
            /* Assume we need to break due to DeepScreen window close */
            _altiaBreakLoop = 1;
            return -1;
        }
    }

    /* OK, we are done with this animation */
    localSetIgnoreEvent(NULL, (LocalEventType) 0);

    return retVal;
}

#if AltiaFunctionPrototyping
static int localAltiaAnimateId(int localNameId,
                               LocalEventType localValue)
#else
static int localAltiaAnimateId(localNameId, localValue)
int localNameId;
LocalEventType localValue;
#endif
{
    int retVal;

    /* Want to ignore events that we generate in this function so flag
     * the animation (see localReportAnimation() for further details).
     */
    localSetIgnoreEventId(localNameId, localValue);

    /* If animate really causes something to happen, update
     * graphics if we are not in caching mode.  Else, set flag
     * indicating that an update is necessary at some point.
     */
    retVal = TargetAltiaAnimateId(localNameId, localValue);
    if (0 != retVal)
    {
        if (0 == _altiaAPICaching)
        {
            int checkReturn;

            TargetAltiaUpdate();

            /* Were about to check for events so clear check count. */
            localNoCheckCount = 0;

           /* Do one loop of system event processing. */
           if (TargetCheckEvent(&checkReturn) < 0)
           {
               /* Assume we need to break due to DeepScreen window close */
               _altiaBreakLoop = 1;
               return -1;
           }
        }
        else
        {
            _altiaAPINeedUpdate = 1;
        }

        retVal = 1;
    }

    /* If we haven't checked for events lately, do it! */
    localNoCheckCount++;
    if (localNoCheckCount >= 50U)
    {
        int checkReturn;

        /* Were about to check for events so clear check count. */
        localNoCheckCount = 0;

        /* Do one loop of system event processing. */
        if (TargetCheckEvent(&checkReturn) < 0)
        {
            /* Assume we need to break due to DeepScreen window close */
            _altiaBreakLoop = 1;
            return -1;
        }
    }

    /* OK, we are done with this animation */
    localSetIgnoreEventId(-1, (LocalEventType) 0);

    return retVal;
}


/*****************************************************************************
 * FUNCTION:  localReportAnimaton 
 *
 * Because our versions of altiaConnect()/AtOpenConnection() and
 * AtStartInterface() set the global AltiaReportFuncPtr to this function
 * (localReportAnimation), this function is called by AltiaAnimate to
 * inform us that an Altia Animation event has occured.  If we generated
 * the event (e.g., because of a call to altiaSendEvent/AtSendEvent), then
 * we want to ignore it.  Otherwise, we need to decide if it should
 * go on our queue.
 ****************************************************************************/
#if AltiaFunctionPrototyping
static void localReportAnimation(ALTIA_CHAR *localName,
                                 LocalEventType localValue)
#else
static void localReportAnimation(localName, localValue)
ALTIA_CHAR     *localName;
LocalEventType localValue;
#endif
{

#ifdef ALTIAAPIDEBUG
    PRINTERR3("\nInto localReportAnimation(0x%x, %g)\n",
              localName, (double) localValue);
    PRINTERR3("\tqueue first = %d, last = %d\n",
              localFirstEvent, localLastEvent);
#endif /* ALTIAAPIDEBUG */

    if (!localCompareIgnoreEvent(localName, localValue))
    {

#ifdef ALTIAAPIDEBUG
    PRINTERR3("\tattempting to add event \"%s\" = %g...\n",
              _localStr2ErrorStr(localName), (double) localValue);
#endif /* ALTIAAPIDEBUG */

        if (0 != altiaLocalAddEvent(localName, localValue))
        {
            /* Event successfully added */
            return;
        }
    }

#ifdef ALTIAAPIDEBUG
    if (localName == NULL)
    {
        PRINTERR3("\tERROR:  Cannot add 0x%x(%g)\n\n", localName,
                  (double) localValue);
    }
    else
    {
        PRINTERR3("\tnot adding ignored event \"%s\" = %g\n\n",
                  _localStr2ErrorStr(localName), (double) localValue);
    }
#endif /* ALTIAAPIDEBUG */

}

/*****************************************************************************
 * FUNCTION:  localNextEvent
 *
 * Goes through the queue looking for the oldest event available.
 * If an event is available, it is passed to the caller and  1 is
 * returned.  Otherwise, 0 is returned.  Please note that a pointer
 * to the name in the local queue is passed to the caller.  As soon
 * as another API function is called, this string may change.
 *****************************************************************************/
#if AltiaFunctionPrototyping
static ALTIA_BOOLEAN localNextEvent(CONST ALTIA_CHAR **localName,
                                    LocalEventType *localValue)
#else
static ALTIA_BOOLEAN localNextEvent(localName, localValue)
CONST ALTIA_CHAR **localName;
LocalEventType *localValue;
#endif
{

#ifdef ALTIAAPITEST
printf("Into localNextEvent(0x%x, 0x%x)\n", localName, localValue);
printf("\tqueue first = %d, last = %d\n", localFirstEvent, localLastEvent);
#endif

    /* Always return something in name and value */
    *localName = localEmptyString;
    *localValue = (LocalEventType) 0;

    /* If we have an event in the queue, get it and keep trying
     * to get it until we succeed (in which case a return is performed)
     * or until we have no events left in the queue.
     */
    while (localFirstEvent != -1)
    {
        struct localEventStruct *ptr = &(localEventQueue[localFirstEvent]);

        /* Don't return empty events. */
        if ((ptr->name == NULL) || (ptr->name[0] == (AltiaCharType) ('\0')))
        {

#ifdef ALTIAAPITEST
printf("\tempty event at index %d\n", localFirstEvent);
#endif

            /* If no more events, break. */
            if (localFirstEvent == localLastEvent)
            {

#ifdef ALTIAAPITEST
printf("\tno more events at index %d\n", localFirstEvent);
#endif

                localFirstEvent = -1;
                localLastEvent = -1;
                break;
            }
            else
            {
                /* Else, increment first event index and wrap it if necessary. */
                if (++localFirstEvent > LOCALMAXEVENT)
                {
                    localFirstEvent = 0;
                }
            }

#ifdef ALTIAAPITEST
printf("\tnext index is %d\n", localFirstEvent);
#endif

        }
        else
        {
            /* Return next event */
            *localName = ptr->name;
            *localValue = ptr->value;

#ifdef ALTIAAPITEST
printf("\treturning index %d event %s(%g)\n", localFirstEvent,
       _localStr2ErrorStr(ptr->name), (double) (ptr->value));
#endif

            /* Flag if queue is now empty. */
            if (localFirstEvent == localLastEvent)
            {
                localFirstEvent = -1;
                localLastEvent = -1;
            }
            else
            {
                /* Else, move index.  Wrap if necessary */
                if (++localFirstEvent > LOCALMAXEVENT)
                {
                    localFirstEvent = 0;
                }
            }

#ifdef ALTIAAPITEST
printf("\ton exit, queue first = %d, last = %d\n\n", localFirstEvent, localLastEvent);
#endif

            return ALTIA_TRUE;
        }
    }

#ifdef ALTIAAPITEST
printf("\tNOTE:  queue is empty, returning \"%s\"(%g)\n\n",
       _localStr2ErrorStr(*name), (double) (*value));
#endif

    return ALTIA_FALSE;
}


/*****************************************************************************
 * FUNCTION:  localFixQueue
 *
 * Checks the queue and moves first and/or last indexes to minimize the
 * queue length.  Called by localPollEvent() and localCheckEvent() after
 * one or more events have been removed (potentially from either end
 * of the queue).
 *****************************************************************************/
static void localFixQueue(void)
{

#ifdef ALTIAAPITEST
printf("Into localFixQueue()\n");
printf("\tqueue first = %d, last = %d\n", localFirstEvent, localLastEvent);
#endif

    if (localFirstEvent != -1)
    {
        struct localEventStruct *ptr = &(localEventQueue[localFirstEvent]);

        /* Drop empty items at the front of the queue */
        while ((ptr->name == NULL) || (ptr->name[0] == (AltiaCharType) ('\0')))
        {
            /* If no more queue is left, we are seriously done! */
            if (localFirstEvent == localLastEvent)
            {

#ifdef ALTIAAPITEST
printf("\tqueue is empty!\n");
#endif

                localFirstEvent = -1;
                localLastEvent = -1;
                break;
            }

            /* Move the head of the queue and wrap if necesary */
            if (++localFirstEvent > LOCALMAXEVENT)
            {
                    localFirstEvent = 0;
            }

#ifdef ALTIAAPITEST
printf("\tmoved queue first to %d\n", localFirstEvent);
#endif

            ptr = &(localEventQueue[localFirstEvent]);

        }

        /* Drop empty items at the end of the queue */
        if (localLastEvent != -1)
        {
            ptr = &(localEventQueue[localLastEvent]);

            while ((ptr->name == NULL) || (ptr->name[0] == (AltiaCharType) ('\0')))
            {
                /* If no more queue is left, we are seriously done! */
                if (localFirstEvent == localLastEvent)
                {

#ifdef ALTIAAPITEST
printf("\tqueue is empty!\n");
#endif

                    localFirstEvent = -1;
                    localLastEvent = -1;
                    break;
                }

                /* Move the tail of the queue and wrap if necesary */
                if (--localLastEvent < 0)
                {
                    localLastEvent = LOCALMAXEVENT;
                }

#ifdef ALTIAAPITEST
printf("\tmoved queue last to %d\n", localLastEvent);
#endif

                ptr = &(localEventQueue[localLastEvent]);
            }
        }
    }

#ifdef ALTIAAPITEST
printf("\tqueue fix complete, first = %d, last = %d\n\n", localFirstEvent, localLastEvent);
#endif

}


/*****************************************************************************
 * FUNCTION:  localPollEvent
 *
 * Goes through the queue looking for all events of the given name,
 * removes them, and passes the most recent value for the event back to
 * the caller.  Returns 1 if successful,  0 if a value for the event
 * cannot be found in the current queue.
 *****************************************************************************/
#if AltiaFunctionPrototyping
static ALTIA_BOOLEAN localPollEvent(CONST ALTIA_CHAR *localName,
                                    LocalEventType *localValue)
#else
static ALTIA_BOOLEAN localPollEvent(localName, localValue)
CONST ALTIA_CHAR *localName;
LocalEventType *localValue;
#endif
{
#ifdef ALTIAAPITEST
printf("Into localPollEvent(%s, 0x%x\n",
       _localStr2ErrorStr(localName), localValue);
printf("\tqueue first = %d, last = %d\n",
       localFirstEvent, localLastEvent);
#endif

    if (localFirstEvent != -1)
    {
        struct localEventStruct *ptr;
        int i = localFirstEvent;
        ALTIA_BOOLEAN items_removed = ALTIA_FALSE;

        /* Search for matching items in event queue */
        do
        {
            ptr = &(localEventQueue[i]);

            /* If event matches the one we are polling, take its value,
             * set the name to NULL to mark that it has been removed,
             * and flag that we removed something.
             */
            /* PRQA S 3415 2 */   /* M3CM 13.5: OK no side effects of evaluations */
            if ((ptr->name != NULL) && (ptr->name[0] != (AltiaCharType) ('\0'))
                && (ALT_STRCMP(localName, ptr->name) == 0))
            {

#ifdef ALTIAAPITEST
printf("\tremoving index %d %s(%g)\n\n",
       i, _localStr2ErrorStr(localName), (double) (ptr->value));
#endif

                *localValue = ptr->value;
                ptr->name = NULL;
                items_removed = ALTIA_TRUE;


            }

            /* Move to next element in queue if possible, wrap if
             * necessary.
             */
            if (i != localLastEvent)
            {
                if (++i > LOCALMAXEVENT)
                {
                    i = 0;
                }
            }
            else
            {
                i = localFirstEvent;
            }

        } while (i != localFirstEvent);

        /* If we found something, see if we can minimize queue
         * and then return true.
         */
        if (items_removed)
        {
            localFixQueue();
            return ALTIA_TRUE;
        }
    }

    /* Nothing found if we get to here */
    return ALTIA_FALSE;
}


/*****************************************************************************
 * FUNCTION:  localCheckEvent
 *
 * Goes through the queue looking for oldest event of the given name,
 * removes it, and passes the value for the event back to the caller.
 * Returns 1 if successful,  0 if a value for the event
 * cannot be found in the current queue.
 *****************************************************************************/
#if AltiaFunctionPrototyping
static ALTIA_BOOLEAN localCheckEvent(CONST ALTIA_CHAR *localName,
                                     LocalEventType *localValue)
#else
static ALTIA_BOOLEAN localCheckEvent(localName, localValue)
CONST ALTIA_CHAR *localName;
LocalEventType *localValue;
#endif
{
    if (localFirstEvent != -1)
    {
        struct localEventStruct *ptr;
        int i = localFirstEvent;
        ALTIA_BOOLEAN item_removed = ALTIA_FALSE;

        /* Search for the first matching item in event queue */
        do
        {
            ptr = &(localEventQueue[i]);

            /* If event matches the one we want to check, take its value,
             * set the name to NULL to mark that it has been removed,
             * flag that we removed something, and break;
             */
            /* PRQA S 3415 2 */   /* M3CM 13.5: OK no side effects of evaluations */
            if ((ptr->name != NULL) && (ptr->name[0] != (AltiaCharType) ('\0'))
                && (ALT_STRCMP(localName, ptr->name) == 0))
            {
                *localValue = ptr->value;
                ptr->name = NULL;
                item_removed = ALTIA_TRUE;
                break;
            }

            /* Move to next element in queue if possible, wrap if
             * necessary.
             */
            if (i != localLastEvent)
            {
                if (++i > LOCALMAXEVENT)
                {
                    i = 0;
                }
            }
            else
            {
                i = localFirstEvent;
            }

        } while (i != localFirstEvent);

        /* If we found something, see if we can minimize queue
         * and then return true.
         */
        if (item_removed)
        {
            localFixQueue();
            return ALTIA_TRUE;
        }
    }

    /* Nothing found if we get to here */
    return ALTIA_FALSE;
}


/*****************************************************************************
 * FUNCTION:  localGetEventCount
 *
 * Returns how many events are in the local queue.
 *****************************************************************************/
static int localGetEventCount(void)
{
    /* If no events in queue, its easy */
    if (localFirstEvent == -1)
    {
        return 0;
    }

    /* If first event comes before last event, it's still pretty easy */
    if (localFirstEvent <= localLastEvent)
    {
        return ((localLastEvent - localFirstEvent) + 1);
    }

    /* Otherwise, events in queue have wrapped so number of events in
     * the queue are the first event to the end of the queue and then
     * the start of the queue to the last event.
     */
    return ((LOCALQUEUESIZE - localFirstEvent) + localLastEvent + 1);
}


/*****************************************************************************
 * FUNCTION:  localSelectEvent
 *
 * Adds a name to the local event selection list if there is room.
 * If event is "*", it means the user wants to select all events.
 * Returns 1 if name was successfully added, 0 if no more room is left.
 *
 * WARNING
 * -------
 * This code assumes that the character string pointed to by the name
 * parameter resides in static memory.  That is to say, we can store
 * just a pointer to it instead of copying the name into our own space.
 * This saves memory big time.
 *****************************************************************************/
#if AltiaFunctionPrototyping
static ALTIA_BOOLEAN localSelectEvent(CONST AltiaCharType *name)
#else
static ALTIA_BOOLEAN localSelectEvent(name)
CONST AltiaCharType *name;
#endif
{
    int i;
    struct localSelectedStruct *empty_ptr = NULL;
    struct localSelectedStruct *ptr = localSelectedList;

    /* If user wants to select all events, OK. */
    if (ALT_API_STRCMP(name, apiAllEvents) == 0)
    {
        /* In this case, list only has one entry. */
        localLastSelected = 0;

        /* 09/29/06:  If ALTIA_APIMALLOC_SELECTS is defined at compile time,
         * make a copy of the string that was passed.  This is necessary if
         * this code is running as a DLL interfaced to something like C#
         * where the name is not a persistent string.  It might also be a
         * desired approach in some other situations as well.
         */
#ifdef ALTIA_APIMALLOC_SELECTS
        ptr->name = (AltiaCharType *) ALTIA_MALLOC(sizeof(AltiaCharType) * (ALT_API_STRLEN(name) + 1U));
        ALT_API_STRCPY(ptr->name, name);
#else
        ptr->name = apiAllEvents;
#endif
        return ALTIA_TRUE;
    }

    /* If user has already selected all events, no need to
     * continue.
     *
     * NOTE:  We are currently missing logic for the case where
     *        user selects all events and then unselects some events.
     */
    /* PRQA S 3415 1 */   /* M3CM 13.5: OK no side effects of evaluations */
    if ((localLastSelected == 0) && (ALT_API_STRCMP(ptr->name, apiAllEvents) == 0))
    {
        return ALTIA_TRUE;
    }

    /* Try to find the name in the existing list of names.
     * While we are doing that, note the first empty entry in
     * the list in case we want to use it.
     */
    for (i = 0; i <= localLastSelected; i++, ptr++)
    {
        if (ptr->name == NULL)
        {
            if (empty_ptr == NULL)
            {
                empty_ptr = ptr;
            }
        }
        else
        {
            if (ALT_API_STRCMP(name, ptr->name) == 0)
            {
                return ALTIA_TRUE;
            }
        }
    }

    /* If we didn't find an empty entry along the way, do we have
     * some unused entries?
     */
    if (empty_ptr == NULL)
    {
        /* If nothing available, return an error */
        if (localLastSelected >= LOCALMAXSELECTED)
        {
            return ALTIA_FALSE;
        }
        else
        {
           localLastSelected++;
           empty_ptr = &(localSelectedList[localLastSelected]);
        }
    }

    /* At this point, empty_ptr should be pointing to an available entry
     * in the list.  Put the name into it.
     * 09/29/06:  If ALTIA_APIMALLOC_SELECTS is defined at compile time,
     * make a copy of the string that was passed.  This is necessary if
     * this code is running as a DLL interfaced to something like C#
     * where the name is not a persistent string.  It might also be a
     * desired approach in some other situations as well.
     */
#ifdef ALTIA_APIMALLOC_SELECTS
    empty_ptr->name = (AltiaCharType *) ALTIA_MALLOC(sizeof(AltiaCharType) * (ALT_API_STRLEN(name) + 1U));
    ALT_API_STRCPY(empty_ptr->name, name);
#else
    empty_ptr->name = name;
#endif

    return ALTIA_TRUE;
}


/*****************************************************************************
 * FUNCTION:  localEventSelected
 *
 * Checks if a name is in our selected list.  Returns 1 if it is, 0 if not.
 *****************************************************************************/
#if AltiaFunctionPrototyping
static ALTIA_BOOLEAN localEventSelected(CONST AltiaCharType *name)
#else
static ALTIA_BOOLEAN localEventSelected(name)
CONST AltiaCharType *name;
#endif
{
    int i;
    struct localSelectedStruct *ptr = localSelectedList;

#ifdef ALTIAAPITEST
#ifdef ALTIAUNICODEAPI
printf("Into localEventSelected(%s)\n", _localStr2ErrorStr(name));
#else
printf("Into localEventSelected(%s)\n", name);
#endif
#endif

    /* If user has already selected all events, no need to
     * continue.
     *
     * NOTE:  We are currently missing logic for the case where
     *        user selects all events and then unselects some events.
     */
    /* PRQA S 3415 1 */   /* M3CM 13.5: OK no side effects of evaluations */
    if ((localLastSelected == 0) && (ALT_API_STRCMP(ptr->name, apiAllEvents) == 0))
    {

#ifdef ALTIAAPITEST
printf("\tall events selected, returning 1\n");
#endif

        return ALTIA_TRUE;
    }

    /* Try to find the name in the existing list of names. */
    for (i = 0; i <= localLastSelected; i++, ptr++)
    {
        /* Skip empty entries (can occur if a name was removed from
         * the selection list
         */
        /* PRQA S 3415 1 */   /* M3CM 13.5: OK no side effects of evaluations */
        if ((NULL != ptr->name) && (0 == ALT_API_STRCMP(name, ptr->name)))
        {

#ifdef ALTIAAPITEST
printf("\tevent is selected (index %d), returning 1\n", i);
#endif

            return ALTIA_TRUE;
        }
    }

#ifdef ALTIAAPITEST
printf("\tevent is not selected, returning 0\n");
#endif

    return ALTIA_FALSE;
}


/*****************************************************************************
 * FUNCTION:  localUnselectEvent()
 *
 * Unselect the requested event if it is selected.  Also does a local poll
 * on the event to remove any occurences of it from our queue.
 * If name is "*", all events are unselected and queue is emptied.
 * Always returns 1.
 *
 * NOTE:  We are currently missing logic for the case where
 *        user selects all events and then unselects some events.
 *****************************************************************************/
#if AltiaFunctionPrototyping
static ALTIA_BOOLEAN localUnselectEvent(CONST AltiaCharType *name)
#else
static ALTIA_BOOLEAN localUnselectEvent(name)
CONST AltiaCharType *name;
#endif
{
    int i;
    struct localSelectedStruct *ptr = localSelectedList;
    ALTIA_BOOLEAN item_removed = ALTIA_FALSE;

#ifdef ALTIAAPITEST
#ifdef ALTIAUNICODEAPI
printf("Into localUnselectEvent(%s)\n", _localStr2ErrorStr(name));
#else
printf("Into localUnselectEvent(%s)\n", name);
#endif
#endif

    /* If user wants to unselect all events, OK. */
    if (ALT_API_STRCMP(name, apiAllEvents) == 0)
    {
        /* 09/29/06:  If ALTIA_APIMALLOC_SELECTS is defined at compile time,
         * then the names in the selected event list were malloced and they
         * need to be freed.
         */
#ifdef ALTIA_APIMALLOC_SELECTS
        for (i = 0; i <= localLastSelected; i++, ptr++)
        {
            if (NULL != ptr->name)
            {
                ALTIA_FREE(ptr->name);
                ptr->name = NULL;
            }
        }
#endif

        /* In this case, empty out selection list and event queue. */
        localLastSelected = -1;
        localFirstEvent = -1;
        localLastEvent = -1;

#ifdef ALTIAAPITEST
printf("\tdone\n\n");
#endif

        return ALTIA_TRUE;
    }
    else
    {
        /* Make sure we remove any occurences of this event from the
         * local event queue.
         */
        LocalEventType localValue;
        localPollEvent(apiStr2LocalStr(name), &localValue);
    }

    /* Try to find the name in the existing list of names. */
    for (i = 0; i <= localLastSelected; i++, ptr++)
    {
        /* Skip empty entries (can occur if a name was removed from
         * the selection list).
         */
        /* PRQA S 3415 1 */   /* M3CM 13.5: OK no side effects of evaluations */
        if ((NULL != ptr->name) && (0 == ALT_API_STRCMP(name, ptr->name)))
        {

#ifdef ALTIAAPITEST
#ifdef ALTIAUNICODEAPI
printf("\tfound %s at index %d\n", _localStr2ErrorStr(name), i);
#else
printf("\tfound %s at index %d\n", name, i);
#endif
#endif
            /* 09/29/06:  If ALTIA_APIMALLOC_SELECTS is defined at compile
             * time, then the name was malloced and it needs to be freed.
             */
#ifdef ALTIA_APIMALLOC_SELECTS
            ALTIA_FREE(ptr->name);
#endif

            ptr->name = NULL;
            item_removed = ALTIA_TRUE;
            break;
        }
    }

    /* If an item was removed from the list, try to clean up the
     * end of the list in case that's where it came from.
     */
    if (item_removed)
    {
        while ((localLastSelected >= 0)
               && (localSelectedList[localLastSelected].name == NULL))
        {
            localLastSelected--;
        }
    }

#ifdef ALTIAAPITEST
printf("\tdone, last selected event index = %d\n\n", localLastSelected);
#endif

    return ALTIA_TRUE;
}


/*****************************************************************************
 * FUNCTION:  localSetIgnoreEvent
 *
 * Flag an event name and value as the one we should be ignoring during
 * an AltiaAnimate cycle (presumably because we generated it).  Note that
 * we only save the pointer to the name.  This requires that the name remain
 * valid throughout the AltiaAnimate cycle which is the case in the current
 * DeepScreen implementation.
 *****************************************************************************/
static CONST ALTIA_CHAR *loc_ignoreLocalName = NULL;
static LocalEventType loc_ignoreLocalValue = (LocalEventType) 0;
#if AltiaFunctionPrototyping
static void localSetIgnoreEvent(CONST ALTIA_CHAR *localName,
                                LocalEventType localValue)
#else
static void localSetIgnoreEvent(name, localValue)
CONST ALTIA_CHAR *localName;
LocalEventType localValue;
#endif
{
    loc_ignoreLocalName = localName;
    loc_ignoreLocalValue = localValue;
}

#if AltiaFunctionPrototyping
static void localSetIgnoreEventId(int localNameId,
                                  LocalEventType localValue)
#else
static void localSetIgnoreEventId(localNameId, localValue)
int localNameId;
LocalEventType localValue;
#endif
{
    ALTIA_CHAR * localName = AltiaAnimationIdToName(localNameId);
    if (NULL != localName)
    {
        localSetIgnoreEvent(localName, localValue);
    }
}

/*****************************************************************************
 * FUNCTION:  localCompareIgnoreEvent
 *
 * Test an event name and value against the one we are currently ignoring.
 * Returns non-zero if they match, 0 otherwise.
 *****************************************************************************/
#if AltiaFunctionPrototyping
static ALTIA_BOOLEAN localCompareIgnoreEvent(ALTIA_CHAR *localName,
                                             LocalEventType localValue)
#else
static ALTIA_BOOLEAN localCompareIgnoreEvent(localName, localValue)
ALTIA_CHAR *localName;
LocalEventType localValue;
#endif
{
    if ((localName == NULL) || (loc_ignoreLocalName == NULL))
    {
        return ALTIA_FALSE;
    }

    if (localValue == loc_ignoreLocalValue)
    {
        return (ALT_STRCMP(localName, loc_ignoreLocalName) == 0);
    }

    return ALTIA_FALSE;
}


/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*              BEGIN ALTIA API BASE LIBRARY FUNCTIONS                       */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/


/*****************************************************************************
 * Some base library globals possibly referenced by other sources.
 *****************************************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif

    /* For limited external use */
    /* PRQA S 3447 3 */   /* M3CM 8.5: OK extern for back compat */
    void altiaSetConnect(
        int index
    );

    /* For limited external use */
    /* PRQA S 3447 3 */   /* M3CM 8.5: OK extern for back compat */
    void altiaSetDebug(
        int level
    );

    /* For limited external use */
    /* PRQA S 3447 3 */   /* M3CM 8.5: OK extern for back compat */
    AltiaCharType *altiaNextEventReturnsName(
        AltiaEventType *eventValueOut
    );

    /* Global for toolkit use */
    /* PRQA S 3447 3 */   /* M3CM 8.5: OK extern for back compat */
    int _altiaTestExit(
        unsigned int currentCount
    );

    /* Global for toolkit use */
    /* PRQA S 3447 3 */   /* M3CM 8.5: OK extern for back compat */
    CONST AltiaCharType *_altiaGetDefaultPortBase(
        void
    );

    /* Global for toolkit use */
    /* PRQA S 3447 3 */   /* M3CM 8.5: OK extern for back compat */
    void altiaShutdown(
        void
    );

    /* Global for toolkit use */
    /* PRQA S 3447 3 */   /* M3CM 8.5: OK extern for back compat */
    int _altiaSync(
        void
    );

    /* Global for toolkit use */
    /* PRQA S 3447 5 */   /* M3CM 8.5: OK extern for back compat */
    int _altiaGetTextInternal(
        CONST AltiaCharType *eventName,
        CONST AltiaCharType *outName,
        AltiaCharType *buffer,
        int bufSize
    );

    /* Next func pointer variable resides in this base library code but is
     * set if AtOpenConnection() is ever called successfully.  Then, if this
     * base library wants to do an exit, it should call the routine to make sure
     * that data structures in the toolkit code get cleaned up properly.  This
     * was added specifically for VXWORKS where one module is accessed by
     * multiple tasks.  It would apply for a DLL as well.
     */
    /* PRQA S 3447 1 */   /* M3CM 8.5: OK extern for back compat */
    extern void (*_AtCleanupFunc)(AtConnectId connectId);
    /* PRQA S 0602 1 */   /* M3CM 1.3,21.2: OK reserved name for back compat */
    void (*_AtCleanupFunc)(AtConnectId connectId) = NULL;

    /* Base library debug flag */
    /* PRQA S 3447 1 */   /* M3CM 8.5: OK extern for back compat */
    extern int _altiaDebug;
    /* PRQA S 0602 1 */   /* M3CM 1.3,21.2: OK reserved name for back compat */
    int _altiaDebug = 0;

    /* For Windows API debug, we have a variable
     * and function to write to a file if a
     * standard output is not available.
     */
#if defined(WIN16) || defined(WIN32)
    /* PRQA S 0602 1 */   /* M3CM 1.3,21.2: OK reserved name for back compat */
    FILE *_altiaOutfile = NULL;
    /* PRQA S 3447 1 */   /* M3CM 8.5: OK extern for back compat */
    int _altiaLogOutput(const char *file);
#endif /* WIN16 || WIN32 */


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

static unsigned int loc_lastConnectFailed = 0;
static ALTIA_BOOLEAN loc_altiaConnect = ALTIA_FALSE;
static ALTIA_BOOLEAN loc_altiaShuttingDown = ALTIA_FALSE;
static unsigned int loc_retryCount = 1U; /* Like windows, default retry is 1 */

/* Exit is suppressed by default because exiting in the middle of execution
 * causes problems on Windows (Desktop and CE) and most embedded platforms.
 * Same thing is true for full LAN and DDE APIs when linked with DeepScreen
 * so at least our behavior is consistent (but not consistent with programs
 * that use API without DeepScreen).  Instead, all API functions will return
 * -1 forever and hopefully someone will notice.
 * To override suppressing of the exit() call, define ALTIA_API_ALLOW_EXIT at
 * compile time and then use AtSuppressExit(id,0) or altiaSuppressExit(0) to
 * override suppressing the exit, but no guarantee the application will exit
 * gracefully on Windows or embedded targets.  These 2 levels of security are
 * put on the call to exit() because it is not considered to be embedded
 * system friendly (See MISRA 2012 Rule 21.8).
 */
static ALTIA_BOOLEAN loc_suppressExit = ALTIA_TRUE;

static AltiaCharType loc_nullName[1];
static ALTIA_BOOLEAN loc_noConnectErrors = ALTIA_FALSE;


/***************************************************************************
 * Function:  For internal use only. So we can exit when retry # is reached.
 ***************************************************************************/
/* PRQA S 0602 1 */   /* M3CM 1.3,21.2: OK reserved name for back compat */
int _altiaTestExit(unsigned int currentCount)
{
    if ((0U != loc_retryCount) && (currentCount >= loc_retryCount))
    {
#ifdef ALTIAAPIDEBUG
        PRINTERR2
        ("Reached maximum retry count of %d for Altia connect.  Exiting...\n",
          loc_retryCount);
#endif /* ALTIAAPIDEBUG */

        /*
         * The next few lines were added for VXWORKS support.
         * Since this module is shared by all other modules in VXWORKS, we
         * should clean up before we exit.  It makes sense in general.
         */
        loc_altiaShuttingDown = ALTIA_TRUE;

        /* If we are suppressing exits (going to stick around), don't
         * clear connection.  User wants us to return -1 continuously
         * until they can detect it.  Clearing connection makes it look
         * like we are ready to connect again!  To clear the connection,
         * this code must be compiled with ALTIA_API_ALLOW_EXIT defined
         * and then enable the use of exit() with a call to
         * AtSuppressExit(id,0) or altiaSuppressExit(0).  These 2 levels of
         * security are put on the call to exit() because it is not considered
         * to be embedded system friendly (See MISRA 2012 Rule 21.8).
         */
#ifdef ALTIA_API_ALLOW_EXIT
        if (!loc_suppressExit)
        {
            altiaClearConnect();
        }
#endif /* ALTIA_API_ALLOW_EXIT */
        if (_AtCleanupFunc != NULL)
        {
            (*_AtCleanupFunc)(0);
        }

        /* Do shutdown last so that driver cleanup is last */
        altiaShutdown();

        loc_altiaShuttingDown = ALTIA_FALSE;

        /* To actually call exit(), compile this code with ALTIA_API_ALLOW_EXIT
         * defined and then enable the use of exit() with a call to
         * AtSuppressExit(id,0) or altiaSuppressExit(0).  These 2 levels of
         * security are put on the call to exit() because it is not considered
         * to be embedded system friendly (See MISRA 2012 Rule 21.8).
         */
#ifdef ALTIA_API_ALLOW_EXIT
        if (!loc_suppressExit)
        {
            /* We know we always get called when semaphore 0 is taken
             * so release it.
             */
            TargetAltiaSemRelease(0);
            exit(0);
        }
        else
#endif /* ALTIA_API_ALLOW_EXIT */
        {
            return -1;
        }
    }
    return 0;
}


/***************************************************************************
 * Function:  For internal use only. So others can get default port name.
 ***************************************************************************/
/* PRQA S 0602 1 */   /* M3CM 1.3,21.2: OK reserved name for back compat */
CONST AltiaCharType *_altiaGetDefaultPortBase(void)
{
    return apiEmptyString;
}


/*****************************************************************************
 * FUNCTION:  altiaNativeStr2AltiaStr
 *
 * Because of UNICODE system support, it may be necessary for user code to
 * convert a wide character string (coming from a main argv list for example)
 * into a string coming into an API function (like altiaConnect()).  The
 * return will be the address of the original string if the native char type
 * is the same as the character type expected by the API.  Otherwise, the
 * original string is converted into the supplied buffer and the buffer's
 * address is returned.
 ****************************************************************************/
#if AltiaFunctionPrototyping
CONST AltiaCharType *altiaNativeStr2AltiaStr(CONST AltiaNativeChar *str,
                                             AltiaCharType *buffer,
                                             int bufferLength)
#else
CONST AltiaCharType *altiaNativeStr2AltiaStr(str, buffer, bufferLength)
CONST AltiaNativeChar *str;
AltiaCharType *buffer;
int bufferLength;
#endif
{

/* We only need to do something if we are on a UNICODE target, but API
 * interface is not UNICODE.  Then, we need to take the wide version of
 * the string and turn it into an 8-bit char version.
 */
#if defined(UNICODE) && !defined(ALTIAUNICODEAPI)

    if (str == NULL)
    {
        return NULL;
    }

    wcstombs(buffer, str, bufferLength); 
    return buffer;

#else
    return str;

#endif /* UNICODE && !ALTIAUNICODEAPI */

}


/*****************************************************************************
 * FUNCTION:  altiaStr2NativeStr
 *
 * Because of UNICODE system support, it may be necessary for user code to
 * convert an 8-bit character string into a wide character string to put
 * into an argv list for a call to AtOpenConnection() or AtStartInterface().
 * The return will be the address of the original string if the native char
 * type is the same as the character type expected by the API.  Otherwise, the
 * original string is converted into the supplied buffer and the buffer's
 * address is returned.
 ****************************************************************************/
#if AltiaFunctionPrototyping
CONST AltiaNativeChar *altiaStr2NativeStr(CONST AltiaCharType *str,
                                          AltiaNativeChar *buffer,
                                          int bufferLength)
#else
CONST AltiaNativeChar *altiaStr2NativeStr(str, buffer, bufferLength)
CONST AltiaCharType *str;
AltiaNativeChar *buffer;
int bufferLength;
#endif
{

/* We only need to do something if we are on a UNICODE target, but API
 * interface is not UNICODE.  Then, we need to take the 8-bit version of
 * the string and turn it into a wide char version.
 */
#if defined(UNICODE) && !defined(ALTIAUNICODEAPI)

    if (str == NULL)
    {
        return NULL;
    }

    mbstowcs(buffer, str, bufferLength); 
    return buffer;

#else
    return str;

#endif /* UNICODE && !ALTIAUNICODEAPI */

}


/***************************************************************************
 * Function:  For limited external use.
 *            Added so we can get debug messages out of base library.
 *
 ***************************************************************************/
#if AltiaFunctionPrototyping
    void altiaSetDebug(int level)
#else
    void altiaSetDebug(level)
    int level;
#endif
{
    _altiaDebug = level;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
void altiaSetConnect(int index)
{
    /* Does nothing because we have only 1 connection into
     * DeepScreen graphics code.
     */
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaGetConnect(void)
#else
    int altiaGetConnect()
    int index;
#endif
{
    /* We have only connection 0 into DeepScreen graphics code. */
    return 0;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaConnect(CONST AltiaCharType *portName)
#else
    int altiaConnect(portName)
    CONST AltiaCharType *portName;
#endif
{
    TargetAltiaSemGet(0);

    /* If already connected, great! */
    if (loc_altiaConnect)
    {
        /* Catch window closing in DeepScreen... */
        if (0 == _altiaBreakLoop)
        {
            TargetAltiaSemRelease(0);
            return 0;
        }

        /* If window closed, this thing is typically over, but don't exit
         * unconditionally because that can cause problems on CE platforms.
         * If exiting is suppressed (default for DeepScreen API), just return
         * -1 forever and hope program notices.  DDE and LAN versions of API
         * do a similar thing for _altiaBreakLoop because exiting causes
         * problems on Windows targets so behavior is consistent within
         * DeepScreen environment.  If exiting isn't suppressed, let
         * _altiaTestExit() call that follows do its thing.
         */
        if (loc_suppressExit)
        {
            TargetAltiaSemRelease(0);
            return -1;
        }
    }

    /* Test if we have tried to connect enough
     * This may exit if we are done
     */
    if(_altiaTestExit((loc_lastConnectFailed)++) != 0)
    {
        TargetAltiaSemRelease(0);
        return -1;
    }

    loc_altiaConnect = ALTIA_TRUE;
    _altiaAPINeedUpdate = 0;

    /* Initialize incoming event queue in case it needs initializing. */
    localEventQueueInit();

    /* Clean everything up in case we left it in a mess */
    localUnselectEvent(apiAllEvents);

    /* To Receive Altia events from DeepScreen code when they occur,
     * set AltiaReportFuncPtr to the procedure we want to be called.
     */
    AltiaReportFuncPtr = localReportAnimation;

    /* Initialize DeepScreen window */
    if (0 == TargetAltiaInitialize())
    {
        return -1;
    }

    TargetAltiaSemRelease(0);
    return 0;
}


/***************************************************************************
 * Function:  For Toolkit usage only.  Closes connection if connection
 *            is not already closed.
 *
 *  NOTE:  This function should not protect itself with a semaphore.
 *         It is the caller's job to do that.
 *
 ***************************************************************************/
#if AltiaFunctionPrototyping
    void altiaShutdown(void)
#else
    void altiaShutdown()
#endif
{
    if (!loc_altiaConnect)
    {
        return;
    }

    loc_altiaConnect = ALTIA_FALSE;
    _altiaAPINeedUpdate = 0;

    /* Close DeepScreen window */
    TargetAltiaClose();
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    void altiaDisconnect(void)
#else
    void altiaDisconnect()
#endif
{
    TargetAltiaSemGet(0);

    /* We'll clear the _altiaBreakLoop that was set on the last DeepScreen
     * window close whenever the user explicitly disconnects.
     */
    _altiaBreakLoop = 0;

    /* If we are holding off on updates, update now if necessary. */
    if (loc_altiaConnect && (0 != _altiaAPICaching) && (0 != _altiaAPINeedUpdate))
    {
        TargetAltiaUpdate();
    }

#ifdef ALTIAAPIDEBUG
    if (_altiaDebug > 1)  /* tjw, 8/22/95:  Conditional debugging support */
    {
        PRINTERR1("altiaDisconnect() explicitly called to disconnect\n");
    }
#endif /* ALTIAAPIDEBUG */

    loc_altiaShuttingDown = ALTIA_TRUE;

    altiaClearConnect();

    /* Do shutdown last so that driver cleanup is last */
    altiaShutdown();

    loc_altiaShuttingDown = ALTIA_FALSE;

    TargetAltiaSemRelease(0);
}


/***************************************************************************
 * Function:  For Toolkit usage only.  Clears out connection information
 *            if connection is indeed closed.
 *
 *  NOTE:  This function should not protect itself with a semaphore.
 *         It is the caller's job to do that.
 *
 ***************************************************************************/
#if AltiaFunctionPrototyping
    void altiaClearConnect(void)
#else
    void altiaClearConnect()
#endif
{
    if (!loc_altiaConnect || loc_altiaShuttingDown)
    {
        /* Not interested in receiving any more events */
        localUnselectEvent(apiAllEvents);

        /* Reset our various options that should be reset from one
         * connect to the next.
         */
#ifndef ALTIA_TASKING
        _altiaAPICaching = 0;
#endif
        loc_lastConnectFailed = 0;
        _altiaAPINeedUpdate = 0;

        /* Free elements of incoming event queue if necessary */
        localEventQueueFree();
    }
}


/***************************************************************************
 * Function:  For Toolkit usage only.  In full version of API, this function
 *            removes domain socket file or pipe files.  For the standalone
 *            version, does nothing.
 *
 *  NOTE:  This function should not protect itself with a semaphore.
 *         It is the caller's job to do that.
 *
 ***************************************************************************/
#if AltiaFunctionPrototyping
    void altiaRemoveConnect(void)
#else
    void altiaRemoveConnect()
#endif
{
    /* For DeepScreen, this Altia API function does nothing, but it must
     * exist for compatability with the socket version of the Altia API.
     */
}


/***************************************************************************
 * Function:  For Toolkit usage only.  Suppress errors from altiaConnect()
 *            calls.
 *
 ***************************************************************************/
#if AltiaFunctionPrototyping
    void altiaSuppressErrors(int yes)
#else
    void altiaSuppressErrors(yes)
    int yes;
#endif
{
    if (0 != yes)
    {
        loc_noConnectErrors = ALTIA_TRUE;
    }
    else
    {
        loc_noConnectErrors = ALTIA_FALSE;
    }
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    void altiaRetryCount(int count)
#else
    void altiaRetryCount(count)
    int count;
#endif
{
    /* Note:  retry count of 0 implies infinity. */
    loc_retryCount = (unsigned int) count;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    void altiaSuppressExit(int yes)
#else
    void altiaSuppressExit(yes)
    int yes;
#endif
{
    if (0 != yes)
    {
        loc_suppressExit = ALTIA_TRUE;
    }
    else
    {
        loc_suppressExit = ALTIA_FALSE;
    }
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaOutputNumber(void)
#else
    int altiaOutputNumber()
#endif
{
    return -1;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaInputNumber(void)
#else
    int altiaInputNumber()
#endif
{
    return -1;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaCacheOutput(int onoff)
#else
    int altiaCacheOutput(onoff)
    int onoff;
#endif
{
    int retVal;

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

#ifdef ALTIA_TASKING
    if (0 == onoff)
    {
#ifdef ALTIAAPITEST
printf("ERROR altiaCacheOutput:  caching is ALWAYS enabled when using ALTIA_TASKING\n\n");
#endif
        retVal = -1;
    }
#else
    _altiaAPICaching = onoff;
#endif /* ALTIA_TASKING */

    /* Update graphics if we are turning caching off and graphics need 
     * updating.
     */
    if ((0 == _altiaAPICaching) && (0 != _altiaAPINeedUpdate))
    {
        TargetAltiaUpdate();
        _altiaAPINeedUpdate = 0;

        /* Were about to check for events so clear check count. */
        localNoCheckCount = 0;

        /* Do one loop of system event processing. */
        if (TargetCheckEvent(&retVal) < 0)
        {
            /* Assume we need to break due to DeepScreen window close */
            _altiaBreakLoop = 1;
            retVal = -1;
        }
    }

    TargetAltiaSemRelease(0);
    return retVal;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaFlushOutput(void)
#else
    int altiaFlushOutput()
#endif
{
    int retVal = 0;
    if (0 != _altiaAPICaching)
    {
        if (0 != _altiaAPINeedUpdate)
        {
            retVal = altiaConnect(NULL);
            if (0 != retVal)
            {
                return retVal;
            }

            TargetAltiaSemGet(0);

            TargetAltiaUpdate();
            _altiaAPINeedUpdate = 0;

            /* Were about to check for events so clear check count. */
            localNoCheckCount = 0;

            /* Do one loop of system event processing. */
            if (TargetCheckEvent(&retVal) < 0)
            {
                /* Assume we need to break due to DeepScreen window close */
                _altiaBreakLoop = 1;
                retVal = -1;
            }

            TargetAltiaSemRelease(0);
        }
    }
    return retVal;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    void altiaSleep(unsigned long millisecs)
#else
    void altiaSleep(millisecs)
    unsigned long millisecs;
#endif
{
    /* Note that we do not protect the TargetSleep() call with any
     * semaphore.  We leave it up to the TargetSleep() function to
     * protect itself if the type of operations it uses require
     * protection.
     */

    TargetSleep(millisecs);
}


/***************************************************************************
 * Function:  For Toolkit usage only.  In full version of Altia API, this
 * routine will wait until altia has finished processing all the events in
 * the event queue and returns a request.  We don't want to mess up
 * the input queue so we can't get any data back.  So we will use a
 * phoney poll event message.  For the standalone version of the API, this
 * function does nothing.
 *
 ***************************************************************************/
/* PRQA S 0602 1 */   /* M3CM 1.3,21.2: OK reserved name for back compat */
int _altiaSync(void)

{
    return 0;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaSendEvent(CONST AltiaCharType *eventName,
                       AltiaEventType eventValue)
#else
    int altiaSendEvent(eventName, eventValue)
    CONST AltiaCharType *eventName;
    AltiaEventType eventValue;
#endif
{
    int retVal;
    CONST ALTIA_CHAR *localName;
    LocalEventType localValue;

    if (eventName == NULL)
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaSendEvent:  event name is bad\n\n");
#endif

        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    localName = apiStr2LocalStr(eventName);
    localValue = apiValue2LocalValue(localName, eventValue);

    if (localAltiaAnimate(localName, localValue) < 0)
    {
        retVal = -1;
    }

    TargetAltiaSemRelease(0);
    return retVal;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaSendText(CONST AltiaCharType *eventName,
                      CONST AltiaCharType *text)
#else
    int altiaSendText(eventName, text)
    CONST AltiaCharType *eventName;
    CONST AltiaCharType *text;
#endif
{
    unsigned int charSize = sizeof(AltiaCharType);
    int ioBuffered = _altiaAPICaching;
    int retVal = 0;
    CONST ALTIA_CHAR *localName;

    if (eventName == NULL)
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaSendText:  text event name is bad\n\n");
#endif

        return -1;
    }

    if (text == NULL)
    {
        return altiaSendEvent(eventName, (AltiaEventType) 0);
    }

    /* Cache events for whole string if caching is off. */
    if (0 == ioBuffered)
    {
        retVal = altiaCacheOutput(1);
    }

    TargetAltiaSemGet(0);

    localName = apiStr2LocalStr(eventName);
    
    /* Send until end of string */
    while ((*text != (AltiaCharType) ('\0')) && (retVal >= 0))
    {
        /* Mask character to avoid sign extension if character
         * has upper bit set.
         */
        /* PRQA S 2995,2996 2 */  /* M3CM 2.2: OK not always true or always false */
        /* PRQA S 2991,2992 1 */  /* M3CM 14.3: OK not always be true or always false */
        if (charSize < 2U)
        {
            /* PRQA S 2985 4 */ /* M3CM 2.2, OK operation not always redundant */
            /* PRQA S 2880 3 */ /* M3CM 2.8: OK this code is reachable */
            /* PRQA S 4394 2 */ /* M3CM 10.8: OK cast unsigned to event type */
            retVal = localAltiaAnimate(localName,
                       (LocalEventType) (((unsigned int) (*text)) & 0x0ffU));
        }
        else
        {
            /* PRQA S 2985 4 */ /* M3CM 2.2, OK operation not always redundant */
            /* PRQA S 2880 3 */ /* M3CM 2.8: OK this code is reachable */
            /* PRQA S 4394 2 */ /* M3CM 10.8: OK cast unsigned to event type */
            retVal = localAltiaAnimate(localName,
                       (LocalEventType) (((unsigned int) (*text)) & 0x0ffffU));
        }
        ++text;
    }

    /* Send a terminator char */
    if (retVal >= 0)
    {
        retVal = localAltiaAnimate(localName, (LocalEventType) 0);
    }

    /* Release semaphore 0 now so that altiaCacheOutput() can use it. */
    TargetAltiaSemRelease(0);

    /* 7/20/93 Addition: Flush and turn off caching if caching was off. */
    if ((0 == ioBuffered) && (retVal >= 0))
    {
        retVal = altiaCacheOutput(0);
    }

    if (retVal < 0)
    {
        return -1;
    }

    return 0;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaPollEvent(CONST AltiaCharType *eventName,
                       AltiaEventType *eventValueOut)
#else
    int altiaPollEvent(eventName, eventValueOut)
    CONST AltiaCharType *eventName;
    AltiaEventType *eventValueOut;
#endif
{
    int retVal;
    CONST ALTIA_CHAR *localName;
    LocalEventType localValue;

    if ((eventName == NULL) || (*eventName == (AltiaCharType) ('\0')) || (eventValueOut == NULL))
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaPollEvent:  event name or value parameter is bad\n\n");
#endif

        return -1;
    }

    /* NOTE:  We don't modify the variable pointed to by eventValueOut unless
     * we really find a value in the local queue.  This is a feature that
     * may be relied upon by the caller!
     */

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    if ((0 != _altiaAPICaching) && (0 != _altiaAPINeedUpdate))
    {
        TargetAltiaUpdate();
        _altiaAPINeedUpdate = 0;
    }

    /* Were about to check for events so clear check count. */
    localNoCheckCount = 0;

    /* Do one loop of system event processing. */
    if (TargetCheckEvent(&retVal) < 0)
    {
        /* Assume we need to break due to DeepScreen window close */
        _altiaBreakLoop = 1;
        retVal = -1;
    }
    else
    {
        /* Now try a poll of the local queue and as a last resort,
         * get the value from the Altia graphics code.
         *
         * Note:  Altia graphics code should be able to return an
         *        error indicating that event has no value, but instead
         *        it will just give us a value of 0.
         */
        localName = apiStr2LocalStr(eventName);
        if (!localPollEvent(localName, &localValue))
        {
            localValue = _altiaFindCurVal((ALTIA_CHAR *) localName);
        }

        *eventValueOut = localValue2APIValue(localName, localValue);
    }

    TargetAltiaSemRelease(0);
    return retVal;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaLocalPollEvent(CONST AltiaCharType *eventName, 
                            AltiaEventType *eventValueOut)
#else
    int altiaLocalPollEvent(eventName, eventValueOut)
    CONST AltiaCharType *eventName;
    AltiaEventType *eventValueOut;
#endif
{
    int retVal;
    CONST ALTIA_CHAR *localName;
    LocalEventType localValue;

    if ((eventName == NULL) || (*eventName == (AltiaCharType) ('\0')) || (eventValueOut == NULL))
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaLocalPollEvent:  event name or value parameter is bad\n\n");
#endif

        return -1;
    }

    /* NOTE:  We don't modify the variable pointed to by eventValueOut unless
     * we really find a value in the local queue.  This is a feature that
     * may be relied upon by the caller!
     */

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    /* If we found one or more events to remove, then eventValueOut will
     * point to the best available value and we should return 0.  Otherwise,
     * the value pointed to by eventValueOut will not change and we will
     * return -1 which is consistent with altiaPollEvent().
     */
    localName = apiStr2LocalStr(eventName);
    if (localPollEvent(localName, &localValue))
    {
        *eventValueOut = localValue2APIValue(localName, localValue);
        retVal = 0;
    }
    else
    {
        retVal = -1;
    }

    TargetAltiaSemRelease(0);
    return retVal;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaCheckEvent(CONST AltiaCharType *eventName,
                        AltiaEventType *eventValueOut)
#else
    int altiaCheckEvent(eventName, eventValueOut)
    CONST AltiaCharType *eventName;
    AltiaEventType *eventValueOut;
#endif
{
    int retVal;
    CONST ALTIA_CHAR *localName;
    LocalEventType localValue;

    if ((eventName == NULL) || (eventValueOut == NULL))
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaCheckEvent:  event name or value parameter is bad\n\n");
#endif

        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    if (*eventName == (AltiaCharType) ('\0'))
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaCheckEvent:  event name is empty\n\n");
#endif

        return 0;
    }

    /* Can only possibly get an event if some are pending.  */
    retVal = altiaPending();
    if (retVal <= 0)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    /* If still nothing of interest in the queue... */
    localName = apiStr2LocalStr(eventName);
    if (!localCheckEvent(localName, &localValue))
    {
        /* Were about to check for events so clear check count. */
        localNoCheckCount = 0;

        /* Do one loop of system event processing. */
        if (TargetCheckEvent(&retVal) < 0)
        {
            /* Assume we need to break due to DeepScreen window close */
            _altiaBreakLoop = 1;

            TargetAltiaSemRelease(0);
            return -1;
        }
        else
        {
            TargetAltiaSemRelease(0);
            return 0;
        }
    }

    /* Caller will get a return of 1 when there is an event */
    *eventValueOut = localValue2APIValue(localName, localValue);

    TargetAltiaSemRelease(0);
    return 1;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaEventSelected(CONST AltiaCharType *eventName)
#else
    int altiaEventSelected(eventName)
    CONST AltiaCharType *eventName;
#endif
{
    int retVal;

    if (eventName == NULL)
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaEventSelected:  event name is bad\n\n");
#endif

        return -1;
    }

    /* If cannot connect, return an error */
    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    if (*eventName == (AltiaCharType) ('\0'))
    {
        return 0;
    }

    TargetAltiaSemGet(0);

    if (localEventSelected(eventName))
    {
        retVal = 1;
    }

    TargetAltiaSemRelease(0);
    return retVal;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaNextEvent(AltiaCharType **eventNameOut,
                       AltiaEventType *eventValueOut)
#else
    int altiaNextEvent(eventNameOut, eventValueOut)
    AltiaCharType **eventNameOut;
    AltiaEventType *eventValueOut;
#endif
{
    int retVal;
    CONST ALTIA_CHAR *localName;
    LocalEventType localValue;

    /* If we are on a UNICODE target, but API is not UNICODE, must
     * convert UNICODE event name string coming from DeepScreen into
     * an 8-bit event name string to return to caller.  8-bit event name
     * string must be static so that it stays around for caller.
     */
#if defined(UNICODE) && !defined(ALTIAUNICODEAPI)
#define MAX_localNextEventStr2APIStr_SIZE 256
    static char _apiName[MAX_localNextEventStr2APIStr_SIZE];
    int res;
#endif /* UNICODE && !ALTIAUNICODEAPI */

    if ((eventNameOut == NULL) || (eventValueOut == NULL))
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaNextEvent:  event name or value parameter is bad\n\n");
#endif

        return -1;
    }

    /* set output name to something so caller has non-nil string */
    loc_nullName[0] = (AltiaCharType) ('\0');
    *eventNameOut = loc_nullName;

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    /* While no local events... */
    while (0 == localGetEventCount())
    {
        /* Before we try some event processing, update graphics
         * if they haven't been updated recently.
         */
        if ((0 != _altiaAPICaching) && (0 != _altiaAPINeedUpdate))
        {
            TargetAltiaUpdate();
            _altiaAPINeedUpdate = 0;
        }

        /* Wait for a system event. */
        if (!TargetGetEvent(&retVal))
        {
            /* Assume we need to break due to DeepScreen window close */
            _altiaBreakLoop = 1;

            TargetAltiaSemRelease(0);
            return -1;
        }
    }

    localNextEvent(&localName, &localValue);

    /* If we are on a UNICODE target, but API is not UNICODE, must
     * convert UNICODE event name string coming from DeepScreen into
     * a static 8-bit event name string to return to caller.
     */
#if defined(UNICODE) && !defined(ALTIAUNICODEAPI)
    res = wcstombs(_apiName, localName, MAX_localNextEventStr2APIStr_SIZE); 
    *eventNameOut = _apiName;
#else
    *eventNameOut = (ALTIA_CHAR *) localName;
#endif /* UNICODE && !ALTIAUNICODEAPI */

    *eventValueOut = localValue2APIValue(localName, localValue);

    TargetAltiaSemRelease(0);
    return 0;
}


/***************************************************************************
 * 09/29/06 Function:  Version of altiaNextEvent() that returns the event
 * name as its return value.  This can be used in C# applications when
 * DeepScreen code is compiled as a DLL.  The regular version of 
 * altiaNextEvent() does not work because the C# code cannot pass a
 * AltiaCharType **.  Note that the function always returns a pointer to
 * a valid string and the string is the empty string if there was a failure.
 ***************************************************************************/
AltiaCharType *altiaNextEventReturnsName(AltiaEventType *eventValueOut)
{
    AltiaCharType *tempPtr;
    if (altiaNextEvent(&tempPtr, eventValueOut) < 0)
    {
        return apiEmptyString;
    }
    return tempPtr;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaPending(void)
#else
    int altiaPending()
#endif
{
    int retVal;

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    /* If no local events, try getting some new ones */
    if (0 == localGetEventCount())
    {
        /* Before we try some event processing, update graphics
         * if they haven't been updated recently.
         */
        if ((0 != _altiaAPICaching) && (0 != _altiaAPINeedUpdate))
        {
            TargetAltiaUpdate();
            _altiaAPINeedUpdate = 0;
        }

        /* Were about to check for events so clear check count. */
        localNoCheckCount = 0;

        /* Do one loop of system event processing. */
        if (TargetCheckEvent(&retVal) < 0)
        {
            /* Assume we need to break due to DeepScreen window close */
            _altiaBreakLoop = 1;

            TargetAltiaSemRelease(0);
            return -1;
        }
    }

    /* Return number of events we know about */
    retVal = localGetEventCount();

    TargetAltiaSemRelease(0);
    return retVal;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaLocalPending(void)
#else
    int altiaLocalPending()
#endif
{
    int retVal;

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    /* Just return number of events we know about locally */
    retVal = localGetEventCount();

    TargetAltiaSemRelease(0);
    return retVal;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaSelectEvent(CONST AltiaCharType *eventName)
#else
    int altiaSelectEvent(eventName)
    CONST AltiaCharType *eventName;
#endif
{
    int retVal;

    if (eventName == NULL)
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaSelectEvent:  event name is bad\n\n");
#endif

        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    if (*eventName == (AltiaCharType) ('\0'))
    {
        return 0;
    }

    /* Note:  Caller can pass an event name of "*" to select all events. */

    TargetAltiaSemGet(0);

    localSelectEvent(eventName);

    TargetAltiaSemRelease(0);
    return 0;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaSelectAllEvents(void)
#else
    int altiaSelectAllEvents()
#endif
{
    int retVal;
    int pending = 0;

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    /* Selecting all events does not mean selecting the closeViewPending one.
     * So check to see if already selected.  This is because selecting
     * closeViewPending will cause the window manager close views to not
     * work and this should only be done if the user explicitly selects
     * this event.
     *
     * NOTE:
     * -----
     * As of 11/29/00, it doesn't work to select all events and then
     * unselect a specific event in the standalone version of the API
     * (see comment header for the localUnselectEvent() function for
     * possibly more up-to-date information).  But, this version also
     * doesn't support selecting of altiaCloseViewPending so there is
     * no harm till someone does want to use altiaCloseViewPending.
     */

    TargetAltiaSemGet(0);

    if (localEventSelected(apiCloseViewPendingString))
    {
        pending = 1;
    }

    localSelectEvent(apiAllEvents);

    /*If not already selected then don't select it with this call*/
    if (pending == 0)
    {
        localUnselectEvent(apiCloseViewPendingString);
    }

    TargetAltiaSemRelease(0);

    return 0;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaUnselectEvent(CONST AltiaCharType *eventName)
#else
    int altiaUnselectEvent(eventName)
    CONST AltiaCharType *eventName;
#endif
{
    int retVal;

    if (eventName == NULL)
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaUnselectEvent:  event name is bad\n\n");
#endif

        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    if (*eventName == (AltiaCharType) ('\0'))
    {
        return 0;
    }

    /* Note:  Caller can pass an event name of "*" to unselect all events. */

    TargetAltiaSemGet(0);

    localUnselectEvent(eventName);

    TargetAltiaSemRelease(0);
    return 0;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaUnselectAllEvents(void)
#else
    int altiaUnselectAllEvents()
#endif
{
    int retVal;

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    /* Not interested in receiving any more events */
    localUnselectEvent(apiAllEvents);

    TargetAltiaSemRelease(0);
    return 0;
}


/***************************************************************************
 * Function:  Internal util that does all of the work of getting text
 *            without touching any semaphores.  It assumes the caller
 *            is doing the necessary semaphore getting and releaseing.
 *            To get text through external connections, caller can pass the
 *            INPUT animation name as eventName and the OUTPUT animation
 *            name as outName.  Otherwise, caller should pass a pointer to
 *            the same string in eventName and outName.
 ***************************************************************************/
/* PRQA S 0602 1 */   /* M3CM 1.3,21.2: OK reserved name for back compat */
int _altiaGetTextInternal(CONST AltiaCharType *eventName,
                          CONST AltiaCharType *outName,
                          AltiaCharType *buffer, int bufSize)
{
#ifdef ALTIA_TASKING

#ifdef ALTIAAPITEST
printf("ERROR altiaGetText:  not supported when using ALTIA_TASKING!\n\n");
#endif

    return -1;
#else
    int i;
    int retVal = 0;
    int eventSelected;
    AltiaEventType nextValue;

    if ((bufSize <= 0) || (eventName == NULL) || (outName == NULL) || (buffer == NULL))
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaGetText:  event name, buffer or buffer size is bad\n\n");
#endif

        return -1;
    }

    /* Initialize buffer as soon as possible */
    buffer[0] = (AltiaCharType) ('\0');

    /* Keep track if event was selected when we came in so we can preserve
     * selection when we leave.  Also wipe out any pending events of the
     * same name if was selected.  Finally, do a new selection.
     */
    eventSelected = altiaEventSelected(eventName);
    /* PRQA S 3415 3 */   /* M3CM 13.5: OK no side effects of evaluations */
    if ((eventSelected < 0)
        || ((eventSelected > 0) && (altiaUnselectEvent(eventName) < 0))
        || (altiaSelectEvent(eventName) < 0))
    {
        return -1;
    }

    /* Tickle the outName to generate text on eventName.  When not
     * getting text through external connections, outName == eventName.
     * We use -1 because it doesn't match any printable character.
     */
    altiaSendEvent(outName, (AltiaEventType) -1);

    /* When getting text through external connections, DeepScreen may
     * send us a bogus -1 as the first character so throw it away if that
     * is the case.  We can be pretty sure that -1 is always bogus unless
     * we are on a 16 or 8 bit CPU.  When this DeepScreen bug is fixed,
     * the code to check for -1 when eventName != outName should be removed.
     */
    i = altiaCheckEvent(eventName, &nextValue);
    if ((i == 1)
        && (eventName != outName) && (nextValue == (AltiaEventType) -1))
    {
        i = altiaCheckEvent(eventName, &nextValue);
    }

    if (i < 1)
    {
        /* If we are not doing inter-process communication,
         * events should be in our queue immediately
         * (i.e., no need to wait for them as in full version of API).
         * altiaSleep(250);
         */

        if (altiaCheckEvent(eventName, &nextValue) < 1)
        {
            /* Replace select state on leaving */
            if (0 == eventSelected)
            {
                altiaUnselectEvent(eventName);
            }

            return -1;
        }
    }

    i = 0;
    do
    {
        buffer[i] = (AltiaCharType) nextValue;

    /* PRQA S 3415 2 */   /* M3CM 13.5: OK no side effects of evaluations */
    } while ((buffer[i] != (AltiaCharType) ('\0')) && (++i < (bufSize - 1))
           && (altiaCheckEvent(eventName, &nextValue) == 1));

    buffer[i] = (AltiaCharType) ('\0');

    /* Wipe out any remaining events of the same name and do a
     * new selection if name was previously selected.
     */
    /* PRQA S 3415 2 */   /* M3CM 13.5: OK no side effects of evaluations */
    if ((altiaUnselectEvent(eventName) < 0)
        || ((eventSelected > 0) && (altiaSelectEvent(eventName) < 0)))
    {
        retVal = -1;
    }

    return retVal;
#endif /* ALTIA_TASKING */
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaGetText(CONST AltiaCharType *textName, AltiaCharType *buffer,
                     int bufSize)
#else
    int altiaGetText(textName, buffer, bufSize)
    CONST AltiaCharType *textName;
    AltiaCharType *buffer;
    int bufSize;
#endif
{
    int retVal;

    /* Note we are using the toolkit semaphore (1) to protect this function
     * because the functions it calls use the base library semaphore (0).
     * Using semaphore 1 allows us to protect altiaGetText() calls from
     * eachother and AtGetText() calls, but not from other base library
     * calls that might try to sneak in on us between our base library calls.
     * But, that shouldn't matter because altiaCheckEvent() is used to check
     * for characters in the queue.  It isn't affected by other types of
     * events coming and going in the queue.
     */
    TargetAltiaSemGet(1);

    retVal = _altiaGetTextInternal(textName, textName, buffer, bufSize);

    TargetAltiaSemRelease(1);
    return retVal;
}


/***************************************************************************
 * Function:  SEE HEADER FILE FOR A DESCRIPTION
 ***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaGetTextExternalConn(CONST AltiaCharType *inName,
                                 CONST AltiaCharType *outName,
                                 AltiaCharType *buffer, int bufSize)
#else
    int altiaGetTextExternalConn(inName, outName, buffer, bufSize)
    CONST AltiaCharType *inName;
    CONST AltiaCharType *outName;
    AltiaCharType *buffer;
    int bufSize;
#endif
{
    int retVal;

    /* Note we are using the toolkit semaphore (1) to protect this function
     * because the functions it calls use the base library semaphore (0).
     * Using the toolkit semaphore allows us to protect altiaGetTextExtConn()
     * calls from eachother and AtGetTextExtConn() calls, but not from other
     * base library calls that might try to sneak in on us between our base
     * library calls.  But, that shouldn't matter because altiaCheckEvent()
     * is used to check for characters in the queue.  It isn't affected by
     * other types of events coming and going in the queue.
     */
    TargetAltiaSemGet(1);

    retVal = _altiaGetTextInternal(inName, outName, buffer, bufSize);

    TargetAltiaSemRelease(1);
    return retVal;
}








/***************************************************************************
 * Function:  For Toolkit and limited external usage on Windows.
 * Opens a file for logging output.
 *
 ***************************************************************************/
#if defined(WIN16) || defined(WIN32)
/* PRQA S 0602 1 */   /* M3CM 1.3,21.2: OK reserved name for back compat */
int _altiaLogOutput(const char *file)
{
    if (_altiaOutfile != NULL)
    {
        fclose(_altiaOutfile);
        return -1;
    }
    if (file == NULL)
    {
        _altiaOutfile = NULL;
        return -1;
    }
    _altiaOutfile = fopen(file,"wtc");
    if (_altiaOutfile != NULL)
    {
#ifndef UNDER_CE
        setvbuf(_altiaOutfile, NULL, _IONBF, 0);
#endif
        return 0;
    }
    else
    {
        return -1;
    }
}
#endif /* WIN16 || WIN32 */



/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*              BEGIN ALTIA API TOOLKIT LIBRARY FUNCTIONS                    */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/


/*****************************************************************************
 * Some Altia API globals possibly referenced by other sources.
 *****************************************************************************/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif
    /* PRQA S 3447 3 */   /* M3CM 8.5: OK externs for back compat */
    extern int _AtConnected;             /* Global for limited external use */
    extern int _AtDebugLevel;            /* Global for limited external use */
    extern int _AtNoConnectErrors;       /* Global for limited external use */

    /* PRQA S 0602 3 */   /* M3CM 1.3,21.2: OK reserved names for back compat */
    int _AtConnected = 0;
    int _AtDebugLevel = 0;
    int _AtNoConnectErrors = 0;

    /* PRQA S 3447 7 */   /* M3CM 8.5: OK externs for back compat */
    void _AtDestroyConnection(           /* For toolkit use */
        AtConnectId connection,
        int level
    );
    void _AtCleanupDataStructs(          /* For toolkit use */
        AtConnectId connection
    );

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


/*****************************************************************************
 * Local util function that does all of the work of opening
 * the connection without touching semaphore 1 that protects
 * toolkit operations.  It assumes the caller is doing the
 * necessary getting and releasing of semaphore 1.
 *****************************************************************************/
#if AltiaFunctionPrototyping
    static AtConnectId localOpenConnection(CONST AltiaCharType *portName,
                                           CONST AltiaCharType *optionName,
                                           int argc,
                                           CONST AltiaNativeChar *argv[])
#else
    static AtConnectId localOpenConnection(portName, optionName, argc, argv)
    CONST AltiaCharType *portName;
    CONST AltiaCharType *optionName;
    int argc;
    CONST AltiaNativeChar *argv[];
#endif
{
    int retVal;
    int i;
    int count = 1;
#if defined(WIN16) || defined(WIN32)
    ALTIA_BOOLEAN alreadyDbg = (_AtDebugLevel != 0);
#endif

    for (i = 0; i < argc; i++)
    {
        /* PRQA S 3415 1 */   /* M3CM 13.5: OK no side effects of evaluations */
        if ((argc > (i+1)) && (ALT_STRCMP(argv[i], ALT_TEXT("-retry")) == 0))
        {
            /* Set retry count as requested by user. */
            /* PRQA S 2469 1 */   /* M3CM 14.2: OK loop control var modified */
            i++;
            if (ALT_SSCANF(argv[i], ALT_TEXT("%d"), &count) != 1)
            {
                count = 1;
            }
        }
        else
        {
            /* PRQA S 3415 1 */   /* M3CM 13.5: OK no side effects of evaluations */
            if ((argc > (i+1)) && (ALT_STRCMP(argv[i], ALT_TEXT("-debug")) == 0))
            {
                _AtDebugLevel = 1; /* set default level to 1 */
                /* Set debug level if user passes a value for it. */
                if (ALT_SSCANF(argv[i+1], ALT_TEXT("%d"), &retVal) == 1)
                {
                    /* PRQA S 2469 1 */   /* M3CM 14.2: OK loop control var modified */
                    i++;
                    _AtDebugLevel = retVal;
                }
            }
        }
    }

#if defined(WIN16) || defined(WIN32)
    if ((0 != _AtDebugLevel) && (!alreadyDbg))
    {
#if !defined(WIN16) && !defined(UNDER_CE)
        /* PRQA S 2895,0306,3415 4 */   /* M3CM 1.1,13.5,11.4,11.6: OK WIN32 */
        HANDLE herr = GetStdHandle(STD_ERROR_HANDLE);
        unsigned long bcount;
        if ((INVALID_HANDLE_VALUE == herr)
            || (0 == WriteFile(herr, "\n", 1, &bcount, NULL)))
#endif
        {
            /* We don't have a valid stderr */
            _altiaLogOutput("cerror.log");
        }
    }
#endif /* WINDOWS */

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR1( "AtOpenConnection(");
        if (portName != NULL)
        {
            PRINTERR2( "port = %s,", _localStr2ErrorStr(portName));
        }
        else
        {
            PRINTERR1( "port = NULL,");
        }
        if (optionName != NULL)
        {
            PRINTERR2( " optionName = %s,", _localStr2ErrorStr(optionName));
        }
        else
        {
            PRINTERR1( " optionName = NULL,");
        }
        PRINTERR3( " argc = %d, argv = 0x%x)\n", argc, argv);
        for (i = 0; i < argc; i++)
        {
            PRINTERR3( "                 argv[%d] = %s\n",
                      i, _localStr2ErrorStr(argv[i]));
        }
    }
#endif /* ALTIAAPIDEBUG */

    altiaRetryCount(0);
    retVal = altiaConnect(portName);
    altiaRetryCount(count);

    if (retVal < 0)
    {

#ifdef ALTIAAPIDEBUG
        PRINTERR1( "AtOpenConnection: altiaConnect() failed\n");
#endif /* ALTIAAPIDEBUG */

        altiaClearConnect();
        return retVal;
    }

    _AtConnected = 1;

    if (_AtDebugLevel > 2)
    {
        altiaSelectAllEvents();
    }

    /* If we have any toolkit data structures at some point, we
     * want to set up a cleanup function pointer.
     */
    _AtCleanupFunc = _AtCleanupDataStructs;

    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int altiaStartInterface(CONST AltiaCharType *dsnFile,
                            int editMode, CONST AltiaNativeChar *cmdLine)
#else
    int altiaStartInterface(dsnFile, editMode, cmdLine)
    CONST AltiaCharType *dsnFile;
    int editMode;
    CONST AltiaNativeChar *cmdLine;
#endif
{
    /* A simplified version of the original altiaStartInterface().
     * The only thing we will look for in the cmdLine string is
     * a -debug.
     */
    if ((cmdLine != NULL) && (*cmdLine != (AltiaCharType) ('\0')))
    {
        if (ALT_STRNCMP(cmdLine, ALT_TEXT("-debug"), 6) == 0)
        {
            _AtDebugLevel = 1; /* set default level to 1 */
        }
    }

    return AtStartInterface(dsnFile, NULL, editMode, 0, NULL);
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    AtConnectId AtStartInterface(CONST AltiaCharType *dsnFile,
                                 CONST AltiaCharType *rtmFile,
                                 int editMode, int argc,
                                 CONST AltiaNativeChar *argv[])
#else
    AtConnectId AtStartInterface(dsnFile, rtmFile, editMode, argc, argv)
    CONST AltiaCharType *dsnFile;
    CONST AltiaCharType *rtmFile;
    int editMode;
    int argc;
    CONST AltiaNativeChar *argv[];
#endif
{
#ifdef ALTIAUNICODEAPI
    static AltiaCharType *_altiaTestEvent = (AltiaCharType *) (ALT_TEXT("altiaTeStEvEnT"));
    static AltiaCharType *_altiaDesignEvent = (AltiaCharType *) (ALT_TEXT("altiaDesignLoaded"));
#else
    static char *_altiaTestEvent = (char *) ("altiaTeStEvEnT");
    static char *_altiaDesignEvent = (char *) ("altiaDesignLoaded");
#endif

    int i;
    int retVal;
    AltiaEventType eventVal;
    AtConnectId connection;

    /* First protect us from other toolkit calls */
    TargetAltiaSemGet(1);

    /* We want to check to see if debug is on.  If it is then
     * we will set _AtDebugLevel and open the debug file.
     * We do this before anything else so the debug file will be
     * open so we can write error messages to it, that's why the
     * two passes of the arguments.
     */
    for (i = 0; i < argc; i++)
    {
        /* PRQA S 3415 1 */   /* M3CM 13.5: OK no side effects of evaluations */
        if ((argc > (i+1)) && (ALT_STRCMP(argv[i], ALT_TEXT("-debug")) == 0))
        {
            _AtDebugLevel = 1; /* set default level to 1 */
            /* Set debug level as requested by user. */
            if (ALT_SSCANF(argv[i+1], ALT_TEXT("%d"), &retVal) == 1)
            {
                _AtDebugLevel = retVal;
                break;
            }
        }
    }

#if defined(WIN16) || defined(WIN32)
    if (0 != _AtDebugLevel)
    {
#if !defined(WIN16) && !defined(UNDER_CE)
        /* PRQA S 2895,0306,3415 4 */   /* M3CM 1.1,13.5,11.4,11.6: OK WIN32 */
        HANDLE herr = GetStdHandle(STD_ERROR_HANDLE);
        unsigned long bcount;
        if ((INVALID_HANDLE_VALUE == herr)
            || (0 == WriteFile(herr, "\n", 1, &bcount, NULL)))
#endif
        {
            /* We don't have a valid stderr */
            _altiaLogOutput("cerror.log");
        }
    }
#endif /* WINDOWS */

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR1( "AtStartInterface: executing\n");
    }
#endif /* ALTIAAPIDEBUG */

    connection = localOpenConnection(NULL, NULL, argc, argv);
    if (connection >= 0)
    {

        /* Everything worked.  Hooray! */

        /* For this type of connection where we are controlling things,
         * set retry count to infinity.
         */
        altiaRetryCount(0);

        /* Send an event and then poll it so we wait until interface is
         * truly ready and design is loaded.  We will wait as long as it
         * takes.
         */
        altiaSendEvent(_altiaTestEvent, (AltiaEventType) 0);

        /* If our made-up test event cannot be polled after we sent it
         * a value, things are very wrong.
         */
        if (altiaPollEvent(_altiaTestEvent, &eventVal) >= 0)
        {
            if (altiaPollEvent(_altiaDesignEvent, &eventVal) >= 0)
            {
                /* Hooray! We got a response so design is now loaded */

                TargetAltiaSemRelease(1);
                return connection;
            }
        }
        else
        {
            /* Bummer! We didn't get a response. Shut things down.
             * Note this is a level 2 destroy.  It will NOT leave
             * pid in tact.  Also note that we used to call
             * AtStopInterface() here to do the destroy, but now
             * that would require us to release our semaphore and
             * we don't want to do that.  So, we will just do
             * the destroy directly.
             */
            _AtDestroyConnection(connection, 2);

#ifdef ALTIAAPIDEBUG
            if (_AtNoConnectErrors == 0)
            {
                PRINTERR1( "AtStartInterface: Altia connection failed mysteriously\n");
            }
#endif /* ALTIAAPIDEBUG */

            TargetAltiaSemRelease(1);
            return -1;
        }
    }

    /* Couldn't connect to interface. */

#ifdef ALTIAAPIDEBUG
    if (_AtNoConnectErrors == 0)
    {
        PRINTERR1( "AtStartInterface: AtOpenConnection() failed\n");
    }
#endif /* ALTIAAPIDEBUG */
 
    TargetAltiaSemRelease(1);
    return -1;
}


/*****************************************************************************/
void altiaStopInterface(void)
{
    /* First protect us from other tookit calls. */
    TargetAltiaSemGet(1);

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR1("altiaStopInterface: called for connection 0\n");
    }
#endif /* ALTIAAPIDEBUG */

    /* Note this is a level 2 destroy.  It will try NOT to leave pid in
     * tact; however, we probably don't know the pid to kill so
     * _AtDestroyConnection will have to just send an altiaQuit event
     * to do the job and hope that it works.
     */
    _AtDestroyConnection(0, 2);

    TargetAltiaSemRelease(1);
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtStopInterface(AtConnectId connection)
#else
    void AtStopInterface(connection)
    AtConnectId connection;
#endif
{
    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR2("AtStopInterface: called with connect id = %d\n", connection);
    }
#endif /* ALTIAAPIDEBUG */

    /* Note this is a level 2 destroy.  It will NOT leave pid in tact */
    _AtDestroyConnection(connection, 2);

    TargetAltiaSemRelease(1);
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    AtConnectId AtOpenConnection(CONST AltiaCharType *portName,
                                 CONST AltiaCharType *optionName,
                                 int argc, CONST AltiaNativeChar *argv[])
#else
    AtConnectId AtOpenConnection(portName, optionName, argc, argv)
    CONST AltiaCharType *portName;
    CONST AltiaCharType *optionName;
    int argc;
    CONST AltiaNativeChar *argv[];
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    /* Now call local function to do the real work for us */
    retVal = localOpenConnection(portName, optionName, argc, argv);

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtCloseConnection(AtConnectId connection)
#else
    void AtCloseConnection(connection)
    AtConnectId connection;
#endif
{
    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (0 != _AtDebugLevel)
    {
        PRINTERR2( "AtCloseConnection(id = %d)\n", connection);
    }
#endif /* ALTIAAPIDEBUG */

    /* Note this is a level 1 destroy.  It will leave pid in tact */
    _AtDestroyConnection(connection, 1);

    TargetAltiaSemRelease(1);
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtSendEvent(AtConnectId connection,
                    CONST AltiaCharType *eventName, AltiaEventType eventValue)
#else
    int AtSendEvent(connection, eventName, eventValue)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AltiaEventType eventValue;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaSendEvent(eventName, eventValue);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        #if !defined(ALTIAINTAPI) && defined(ALTIAFLOAT)
        PRINTERR4( "AtSendEvent(id = %d, event = %s, value = %g)",
                  connection, _localStr2ErrorStr(eventName), eventValue);
        #else
        PRINTERR4( "AtSendEvent(id = %d, event = %s, value = %d)",
                  connection, _localStr2ErrorStr(eventName), eventValue);
        #endif
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtSendText(AtConnectId connection,
                    CONST AltiaCharType *eventName, CONST AltiaCharType *text)
#else
    int AtSendText(connection, eventName, text)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    CONST AltiaCharType *text;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaSendText(eventName, text);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR4( "AtSendText(id = %d, event = %s, text = %s)",
                  connection, _localStr2ErrorStr(eventName), text);
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtGetText(AtConnectId connection,
                  CONST AltiaCharType *textName, AltiaCharType *buffer,
                  int bufSize)
#else
    int AtGetText(connection, textName, buffer, bufSize)
    AtConnectId connection;
    CONST AltiaCharType *textName;
    AltiaCharType *buffer;
    int bufSize;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        /* Use a utility function that gets the text without
         * taking a toolkit semaphore because we already have the
         * tookit sempahore.
         */
        retVal = _altiaGetTextInternal(textName, textName, buffer, bufSize);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR4( "AtGetText(id = %d, event = %s, text = %s)",
                  connection, _localStr2ErrorStr(textName), buffer);
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/***************************************************************************/
#if AltiaFunctionPrototyping
    int AtGetTextExternalConn(AtConnectId connection,
                  CONST AltiaCharType *inName, CONST AltiaCharType *outName,
                  AltiaCharType *buffer, int bufSize)
#else
    int AtGetTextExternalConn(connection, inName, outName, buffer, bufSize)
    AtConnectId connection;
    CONST AltiaCharType *inName;
    CONST AltiaCharType *outName;
    AltiaCharType *buffer;
    int bufSize;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        /* Use a utility function that gets the text without
         * taking a toolkit semaphore because we already have the
         * tookit sempahore.
         */
        retVal = _altiaGetTextInternal(inName, outName, buffer, bufSize);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR4( "AtGetTextExternalConn(id = %d, inName = %s, text = %s)",
                  connection, _localStr2ErrorStr(inName), buffer);
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtCacheOutput(AtConnectId connection, int onoff)
#else
    int AtCacheOutput(connection, onoff)
    AtConnectId connection;
    int onoff;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaCacheOutput(onoff);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR3( "AtCacheOutput(id = %d, onoff = %d)", connection, onoff);
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtFlushOutput(AtConnectId connection)
#else
    int AtFlushOutput(connection)
    AtConnectId connection;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaFlushOutput();
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR2( "AtFlushOutput(id = %d)", connection);
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtPollEvent(AtConnectId connection,
                    CONST AltiaCharType *eventName,
                    AltiaEventType *eventValueOut)
#else
    int AtPollEvent(connection, eventName, eventValueOut)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AltiaEventType *eventValueOut;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaPollEvent(eventName, eventValueOut);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        #if !defined(ALTIAINTAPI) && defined(ALTIAFLOAT)
        PRINTERR4( "AtPollEvent(id = %d, event = %s, valueOut = %g)",
                  connection, _localStr2ErrorStr(eventName), *eventValueOut);
        #else
        PRINTERR4( "AtPollEvent(id = %d, event = %s, valueOut = %d)",
                  connection, _localStr2ErrorStr(eventName), *eventValueOut);
        #endif
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtLocalPollEvent(AtConnectId connection,
                         CONST AltiaCharType *eventName,
                         AltiaEventType *eventValueOut)
#else
    int AtLocalPollEvent(connection, eventName, eventValueOut)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AltiaEventType *eventValueOut;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaLocalPollEvent(eventName, eventValueOut);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        #if !defined(ALTIAINTAPI) && defined(ALTIAFLOAT)
        PRINTERR4( "AtLocalPollEvent(id = %d, event = %s, valueOut = %g)",
                  connection, _localStr2ErrorStr(eventName), *eventValueOut);
        #else
        PRINTERR4( "AtLocalPollEvent(id = %d, event = %s, valueOut = %d)",
                  connection, _localStr2ErrorStr(eventName), *eventValueOut);
        #endif
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtCheckEvent(AtConnectId connection,
                    CONST AltiaCharType *eventName,
                    AltiaEventType *eventValueOut)
#else
    int AtCheckEvent(connection, eventName, eventValueOut)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
    AltiaEventType *eventValueOut;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaCheckEvent(eventName, eventValueOut);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        #if !defined(ALTIAINTAPI) && defined(ALTIAFLOAT)
        PRINTERR4( "AtCheckEvent(id = %d, event = %s, valueOut = %g)",
                  connection, _localStr2ErrorStr(eventName), *eventValueOut);
        #else
        PRINTERR4( "AtCheckEvent(id = %d, event = %s, valueOut = %d)",
                  connection, _localStr2ErrorStr(eventName), *eventValueOut);
        #endif
        if (retVal > 0)
        {
            PRINTERR1( " found\n");
        }
        else if (retVal == 0)
        {
            PRINTERR1( " doesn't exist\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtNextEvent(AtConnectId connection,
                    CONST AltiaCharType **eventNameOut,
                    AltiaEventType *eventValueOut)
#else
    int AtNextEvent(connection, eventNameOut, eventValueOut)
    AtConnectId connection;
    CONST AltiaCharType **eventNameOut;
    AltiaEventType *eventValueOut;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaNextEvent((AltiaCharType **) eventNameOut, eventValueOut);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        #if !defined(ALTIAINTAPI) && defined(ALTIAFLOAT)
        PRINTERR4( "AtNextEvent(id = %d, eventOut = %s, valueOut = %g)",
                  connection, _localStr2ErrorStr(*eventNameOut), *eventValueOut);
        #else
        PRINTERR4( "AtNextEvent(id = %d, eventOut = %s, valueOut = %d)",
                  connection, _localStr2ErrorStr(*eventNameOut), *eventValueOut);
        #endif
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/***************************************************************************
 * 09/29/06 Function:  Version of AtNextEvent() that returns the event
 * name as its return value.  This can be used in C# applications when
 * DeepScreen code is compiled as a DLL.  The regular version of 
 * AtNextEvent() does not work because the C# code cannot pass a
 * AltiaCharType **.  Note that the function always returns a pointer to
 * a valid string and the string is the empty string if there was a failure.
 ***************************************************************************/
/* PRQA S 3447 2 */   /* M3CM 8.5: Allow local extern declaration */
extern AltiaCharType *AtNextEventReturnsName(AtConnectId connection,
                                             AltiaEventType *eventValueOut);
AltiaCharType *AtNextEventReturnsName(AtConnectId connection,
                                      AltiaEventType *eventValueOut)
{
    AltiaCharType *tempPtr;
    if (AtNextEvent(connection, &tempPtr, eventValueOut) < 0)
    {
        return apiEmptyString;
    }
    return tempPtr;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtPending(AtConnectId connection)
#else
    int AtPending(connection)
    AtConnectId connection;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaPending();
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR3( "AtPending(id = %d) returned %d\n", connection, retVal);
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtLocalPending(AtConnectId connection)
#else
    int AtLocalPending(connection)
    AtConnectId connection;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaLocalPending();
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR3( "AtLocalPending(id = %d) returned %d\n", connection, retVal);
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtSelectEvent(AtConnectId connection, CONST AltiaCharType *eventName)
#else
    int AtSelectEvent(connection, eventName)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaSelectEvent(eventName);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR3( "AtSelectEvent(id = %d, event = %s)",
                  connection, _localStr2ErrorStr(eventName));
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtSelectAllEvents(AtConnectId connection)
#else
    int AtSelectAllEvents(connection)
    AtConnectId connection;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaSelectAllEvents();
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR2( "AtSelectAllEvents(id = %d)", connection);
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtEventSelected(AtConnectId connection, CONST AltiaCharType *name)
#else
    int AtEventSelected(connection, name)
    AtConnectId connection;
    CONST AltiaCharType *name;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaEventSelected(name);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR3( "AtEventSelected(id = %d, name = %s) ",
                  connection, _localStr2ErrorStr(name));
        if (retVal > 0)
        {
            PRINTERR1( "yes\n");
        }
        else if (retVal == 0)
        {
            PRINTERR1( "no\n");
        }
        else
        {
            PRINTERR1( "FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtUnselectEvent(AtConnectId connection, CONST AltiaCharType *eventName)
#else
    int AtUnselectEvent(connection, eventName)
    AtConnectId connection;
    CONST AltiaCharType *eventName;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else if (_AtDebugLevel > 2)
    {
        retVal = 0;
    }
    else
    {
        retVal = altiaUnselectEvent(eventName);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR3( "AtUnselectEvent(id = %d, event = %s)",
                  connection, _localStr2ErrorStr(eventName));
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtUnselectAllEvents(AtConnectId connection)
#else
    int AtUnselectAllEvents(connection)
    AtConnectId connection;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else if (_AtDebugLevel > 2)
    {
        retVal = 0;
    }
    else 
    {
        retVal = altiaUnselectAllEvents();
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR2( "AtUnselectAllEvents(id = %d)", connection);
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtInputNumber(AtConnectId connection)
#else
    int AtInputNumber(connection)
    AtConnectId connection;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaInputNumber();
    }

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    int AtOutputNumber(AtConnectId connection)
#else
    int AtOutputNumber(connection)
    AtConnectId connection;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaOutputNumber();
    }

    TargetAltiaSemRelease(1);
    return retVal;
}


/*****************************************************************************/
/*  NOTE:  This function is so trivial it doesn't need to protect
 *         itself with a semaphore.
 */
#if AltiaFunctionPrototyping
    void AtSuppressErrors(int yes)
#else
    void AtSuppressErrors(yes)
    int yes;
#endif
{
    if (0 != yes)
    {
        _AtNoConnectErrors = 1;
        altiaSuppressErrors(1);
    }
    else
    {
        _AtNoConnectErrors = 0;
        altiaSuppressErrors(0);
    }
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtRetryCount(AtConnectId connection, int count)
#else
    void AtRetryCount(connection, count)
    AtConnectId connection;
    int count;
#endif
{
    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 == connection) && (0 != _AtConnected))
    {
        altiaRetryCount(count);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR3( "AtRetryCount(id = %d, count = %d)", connection, count);
        if ((0 == connection) && (0 != _AtConnected))
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return;
}


/*****************************************************************************/
#if AltiaFunctionPrototyping
    void AtSuppressExit(AtConnectId connection, int yes)
#else
    void AtSuppressExit(connection, yes)
    AtConnectId id;
    int yes;
#endif
{
    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 == connection) && (0 != _AtConnected))
    {
        altiaSuppressExit(yes);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR3( "AtSuppressExit(id = %d, yes = %d)", connection, yes);
        if ((0 == connection) && (0 != _AtConnected))
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return;
}


/*****************************************************************************/
/*  NOTE:  This function is so trivial it doesn't need to protect
 *         itself with a semaphore.
 */
#if AltiaFunctionPrototyping
    CONST AltiaCharType *AtGetPortName(AtConnectId connection)
#else
    CONST AltiaCharType *AtGetPortName(connection)
    AtConnectId connection;
#endif
{
    CONST AltiaCharType *retVal;

    if ((0 != connection) || (0 == _AtConnected))
    {
        retVal = NULL;
    }
    else
    {
        /* Get port name from base library if this is Runtime Libary version */
        retVal = _altiaGetDefaultPortBase();
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR2( "AtGetPortName(id = %d)", connection);
        if (retVal != NULL)
        {
            PRINTERR2( " returning %s\n", _localStr2ErrorStr(retVal));
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    return retVal;
}


/****************************************************************************
 * _AtDestroyConnection() is only called when user does an explicit
 * AtStopInterface(), altiaStopInterface(), or AtCloseConnection().
 *
 *   NOTE:  This function should not protect itself with a semaphore.
 *          It is the caller's job to do that.  However, it does need
 *          to protect some base library calls that it makes because
 *          they don't protect themselves.
 *
 ****************************************************************************/
/* PRQA S 0602 1 */   /* M3CM 1.3,21.2: OK reserved name for back compat */
void _AtDestroyConnection(AtConnectId connection, int level)
{
    /* We'll clear the _altiaBreakLoop that was set on the last DeepScreen
     * window close whenever the user explicitly disconnects.  Do it
     * immediately if we are not currently connected because we're going
     * to return right away.
     */
    if ((0 == connection) && (0 == _AtConnected))
    {
        _altiaBreakLoop = 0;
    }

    if ((0 != connection) || (0 == _AtConnected))
    {
        return;
    }

    /* While disconnecting, set retry count to infinity */
    altiaRetryCount(0);

    /* If this is a level 2 or higher destroy, we would go for a termination
     * of the interface using the standard "altiaQuit" function if this were
     * a full version of the API.  Since this API is linked in directly to
     * the graphics code, we want the user's application code to do the
     * exit if that's specifically what they want to do.
     */

    /* Lets make sure all events have been sent. */
    altiaFlushOutput();

    /* We need to protect the calls we make from here until we return
     * because they don't protect themselves.
     */
    TargetAltiaSemGet(0);

    /* Used to do an altiaDisconnect(), but its functionality
     * changed slightly with PC release.  Now, altiaShutdown() is what we
     * want to do.
     */
    loc_altiaShuttingDown = ALTIA_TRUE;

    if (level > 1)
    {
        altiaRemoveConnect();
    }

    /* Clear out connection structures within Altia base library */
    altiaClearConnect();

    /* If we have any toolkit data structures at some point, we
     * want to clean them up here.
     */
    if (_AtCleanupFunc != NULL)
    {
        (*_AtCleanupFunc)(connection);
    }

    /* Do shutdown last so that driver cleanup is last */
    altiaShutdown();

    /* We'll clear the _altiaBreakLoop that was set on the last DeepScreen
     * window close whenever the user explicitly disconnects.
     */
    _altiaBreakLoop = 0;
    loc_altiaShuttingDown = ALTIA_FALSE;

    TargetAltiaSemRelease(0);
}


/*****************************************************************************/
/* Next function is internally called by _AtDestroyConnection() or possibly
 * by base library before doing an exit.
 *
 *  NOTE:  This function should not protect itself with any kind of
 *         semaphore.  It is the caller's job to do that.
 */
/* PRQA S 0602 1 */   /* M3CM 1.3,21.2: OK reserved name for back compat */
void _AtCleanupDataStructs(AtConnectId connection)
{

    if ((0 != connection) || (0 == _AtConnected))
    {
        return;
    }

    _AtConnected = 0;
}


/*****************************************************************************
 * Next function must always exist for linking DeepScreen generated code with
 * Simulink RTW Altia block even if no support for opening multiple designs.
 ****************************************************************************/
#if AltiaFunctionPrototyping
    int AtOpenDesignFile(AtConnectId connectId, int designId,
                         CONST AltiaCharType *fileName)
#else
    int AtOpenDesignFile(connectId, designId, fileName)
    AtConnectId connectId;
    int designId;
    CONST AltiaCharType *fileName;
#endif
{
    return -1;
}

/*****************************************************************************/
/*                                                                           */
/*                        Animation ID Functions                             */
/*                                                                           */
/* Animation ID versions of existing APIs.  The Animation ID must be         */
/* obtained programatically using the altiaGetAnimationID() or the           */
/* AtGetAnimationID() function.                                              */
/*                                                                           */
/*****************************************************************************/
#if AltiaFunctionPrototyping
    int altiaGetAnimationId(CONST AltiaCharType *name)
#else
    int altiaGetAnimationId(name)
    CONST AltiaCharType *name;
#endif
{
    int retVal;
    CONST ALTIA_CHAR *localName;

    if (name == NULL)
    {
#ifdef ALTIAAPITEST
printf("ERROR altiaGetAnimationId:  event name is bad\n\n");
#endif
        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    localName = apiStr2LocalStr(name);
    retVal = AltiaAnimationNameToId(localName);

    TargetAltiaSemRelease(0);

    return retVal;
}

#if AltiaFunctionPrototyping
    int AtGetAnimationId(AtConnectId connectId, CONST AltiaCharType *name)
#else
    int AtGetAnimationId(connectId, name)
    AtConnectId connectId;
    CONST AltiaCharType *name;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connectId) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else 
    {
        retVal = altiaGetAnimationId(name);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR3( "altiaGetAnimationId(id = %d, event = %s)",
                  connectId, _localStr2ErrorStr(name));
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    int altiaSendEventId(int nameId, AltiaEventType eventValue)
#else
    int altiaSendEventId(nameId, eventValue)
    int nameId; 
    AltiaEventType eventValue;
#endif
{
    int retVal;
    LocalEventType localValue;

    /* Validate Id is within bounds */
    if (nameId < 0)
    {
        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    localValue = apiValue2LocalValueId(nameId, eventValue);

    if (localAltiaAnimateId(nameId, localValue) < 0)
    {
        retVal = -1;
    }

    TargetAltiaSemRelease(0);
    return retVal;
}

#if AltiaFunctionPrototyping
    int AtSendEventId(AtConnectId connectId, int nameId, AltiaEventType value)
#else
    int AtSendEventId(connectId, nameId, value)
    AtConnectId connectId;
    int nameId;
    AltiaEventType value;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connectId) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else
    {
        retVal = altiaSendEventId(nameId, value);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        #if !defined(ALTIAINTAPI) && defined(ALTIAFLOAT)
        PRINTERR4( "AtSendEventId(id = %d, event = %d, value = %g)",
                  connectId, nameId, value);
        #else
        PRINTERR4( "AtSendEventId(id = %d, event = %d, value = %d)",
                  connectId, nameId, value);
        #endif
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}

/*****************************************************************************/
#if AltiaFunctionPrototyping
    int altiaSendTextId(int nameId, CONST AltiaCharType *text)
#else
    int altiaSendTextId(nameId, text)
    int nameId;
    CONST AltiaCharType *text;
#endif
{
    unsigned int charSize = sizeof(AltiaCharType);
    int ioBuffered = _altiaAPICaching;
    int retVal = 0;

    /* Validate Id is within bounds */
    if (nameId < 0)
    {
        return -1;
    }

    if (text == NULL)
    {
        return altiaSendEventId(nameId, (AltiaEventType) 0);
    }

    /* Cache events for whole string if caching is off. */
    if (0 == ioBuffered)
    {
        retVal = altiaCacheOutput(1);
    }

    TargetAltiaSemGet(0);

    /* Send until end of string */
    while((*text != (AltiaCharType) ('\0')) && (retVal >= 0))
    {
        /* Mask character to avoid sign extension if character
         * has upper bit set.
         */
        /* PRQA S 2995,2996 2 */  /* M3CM 2.2: OK not always true or always false */
        /* PRQA S 2991,2992 1 */  /* M3CM 14.3: OK not always be true or always false */
        if (charSize < 2U)
        {
            /* PRQA S 2985 4 */ /* M3CM 2.2, OK operation not always redundant */
            /* PRQA S 2880 3 */ /* M3CM 2.8: OK this code is reachable */
            /* PRQA S 4394 2 */ /* M3CM 10.8: OK cast unsigned to event type */
            retVal = localAltiaAnimateId(nameId,
                       (LocalEventType) (((unsigned int) (*text)) & 0x0ffU));
        }
        else
        {
            /* PRQA S 2985 4 */ /* M3CM 2.2, OK operation not always redundant */
            /* PRQA S 2880 3 */ /* M3CM 2.8: OK this code is reachable */
            /* PRQA S 4394 2 */ /* M3CM 10.8: OK cast unsigned to event type */
            retVal = localAltiaAnimateId(nameId,
                       (LocalEventType) (((unsigned int) (*text)) & 0x0ffffU));
        }
        ++text;
    }

    /* Send a terminator char */
    if (retVal >= 0)
    {
        retVal = localAltiaAnimateId(nameId, (LocalEventType) 0);
    }

    /* Release semaphore 0 now so that altiaCacheOutput() can use it. */
    TargetAltiaSemRelease(0);

    /* 7/20/93 Addition: Flush and turn off caching if caching was off. */
    if ((0 == ioBuffered) && (retVal >= 0))
    {
        retVal = altiaCacheOutput(0);
    }

    if (retVal < 0)
    {
        return -1;
    }

    return 0;
}
    
#if AltiaFunctionPrototyping
    int AtSendTextId(AtConnectId connectId, int nameId, CONST AltiaCharType *text)
#else
    int AtSendTextId(connectId, nameId, text)
    AtConnectId connectId;
    int nameId;
    CONST AltiaCharType *text;
#endif
{
    int retVal;

    /* First protect us from other toolkit calls. */
    TargetAltiaSemGet(1);

    if ((0 != connectId) || (0 == _AtConnected))
    {
        retVal = -1;
    }
    else 
    {
        retVal = altiaSendTextId(nameId, text);
    }

#ifdef ALTIAAPIDEBUG
    /* DEBUG PRINT-OUT */
    if (_AtDebugLevel > 1)
    {
        PRINTERR4( "AtSendTextId(id = %d, event = %d, text = %s)",
                  connectId, nameId, text);
        if (retVal >= 0)
        {
            PRINTERR1( " succeeded\n");
        }
        else
        {
            PRINTERR1( " FAILED\n");
        }
    }
#endif /* ALTIAAPIDEBUG */

    TargetAltiaSemRelease(1);
    return retVal;
}



/*****************************************************************************/
/*                                                                           */
/*                     AT View Functions                                     */
/*                                                                           */
/* Note: AtMoveView(), AtMagnifyView(), and AtSizeView() must always exist   */
/*       for linking DeepScreen generated code with Simulink RTW Altia block.*/
/*       Define these functions always even when code generation for views   */
/*       is not chosen.                                                      */
/*****************************************************************************/

/*****************************************************************************
 * Next function must always exist for linking DeepScreen generated code 
 * with Simulink RTW Altia block even if code gen for views is not chosen.
 ****************************************************************************/
#if AltiaFunctionPrototyping
    int AtMoveView(AtConnectId connectId, int viewId,
                   int x, int y)
#else
    int AtMoveView(connectId, viewId, x, y)
    AtConnectId connectId;
    int viewId;
    int x;
    int y;
#endif
{

    return -1;


}

/*****************************************************************************
 * Next function must always exist for linking DeepScreen generated code 
 * with Simulink RTW Altia block even if code gen for views is not chosen.
 ****************************************************************************/
#if AltiaFunctionPrototyping
    int AtMagnifyView(AtConnectId connectId, int viewId,
                      ALT_DOUBLE mag)
#else
    int AtMagnifyView(connectId, viewId, mag)
    AtConnectId connectId;
    int viewId;
    ALT_DOUBLE mag;
#endif
{

    return -1;


}

/*****************************************************************************
 * Next function must always exist for linking DeepScreen generated code 
 * with Simulink RTW Altia block even if code gen for views is not chosen.
 ****************************************************************************/
#if AltiaFunctionPrototyping
    int AtSizeView(AtConnectId connectId, int viewId,
                   int width, int height)
#else
    int AtSizeView(connectId, viewId, width, height)
    AtConnectId connectId;
    int viewId;
    int width;
    int height;
#endif
{

    return -1;


}







/***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaTaskRun(unsigned long milliSeconds)
#else
    int altiaTaskRun(milliSeconds)
    unsigned long milliSeconds;
#endif
{
    int retVal;

#ifdef ALTIA_TASKING
    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    retVal = TargetAltiaTaskRun(milliSeconds);

    TargetAltiaSemRelease(0);
#else
    retVal = -1;
    #ifdef ALTIAAPITEST
    printf("ERROR altiaTaskRun:  ALTIA_TASKING is not defined.\n\n");
    #endif
#endif /* ALTIA_TASKING */

    return retVal;
}

/***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaTaskStatus(AltiaTaskStatusType * status)
#else
    int altiaTaskStatus(status)
    AltiaTaskStatusType * status;
#endif
{
    int retVal;

    if (status == NULL)
    {

#ifdef ALTIAAPITEST
printf("ERROR altiaTaskStatus:  'status' parameter is NULL.\n\n");
#endif

        return -1;
    }

#ifdef ALTIA_TASKING
    retVal = altiaConnect(NULL);
    if (0 == retVal)
    {
        AltiaQueueStatusType target;

        TargetAltiaSemGet(0);

        /* Task Queue */
        TargetAltiaTaskStatus(&target);
        status->task_size           = target.size;
        status->task_current_used   = target.current;
        status->task_peak_used      = target.peak;
        status->task_overflow       = target.error;

        /* Object Queue */
        TargetAltiaObjectStatus(&target);
        status->object_size         = target.size;
        status->object_current_used = target.current;
        status->object_peak_used    = target.peak;
        status->object_overflow     = target.error;
        status->object_max_id       = target.id;
        status->object_max_time     = target.time;

        TargetAltiaSemRelease(0);
    }
#else
    retVal = -1;
    #ifdef ALTIAAPITEST
    printf("ERROR altiaTaskStatus:  ALTIA_TASKING is not defined.\n\n");
    #endif
#endif /* ALTIA_TASKING */

    return retVal;
}

/***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaGetTextWidth(CONST AltiaCharType *name, AltiaCharType *text)
#else
    int altiaGetTextWidth(name, text)
    CONST AltiaCharType *name;
    AltiaCharType *text;
#endif
{
    int retVal;
    CONST ALTIA_CHAR *localName;
    CONST ALTIA_CHAR *localText;
#if defined(UNICODE) && !defined(ALTIAUNICODEAPI)
    #define MAX_fontStr2LocalStr_SIZE 256
    ALTIA_CHAR textStr[MAX_fontStr2LocalStr_SIZE];
#endif

    /* Validate Animation */
    if (NULL == name)
    {
        return -1;
    }

    if (NULL == text)
    {
        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return -1;
    }

    TargetAltiaSemGet(0);

#if defined(UNICODE) && !defined(ALTIAUNICODEAPI)
    /* We only need to convert text if we are on a UNICODE target, but API
     * interface is not UNICODE.  Then, we need to take the 8-bit version of
     * the string and turn it into a wide char version.
     */
    mbstowcs(textStr, text, MAX_fontStr2LocalStr_SIZE); 
    localText = (CONST ALTIA_CHAR *)textStr;
#else
    localText = (CONST ALTIA_CHAR *)text;
#endif

    /* Get the size */
    localName = apiStr2LocalStr(name);
    retVal = TargetGetTextWidth(localName, localText);

    TargetAltiaSemRelease(0);
    return retVal;
}

/***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaGetTextWidthId(int nameId, AltiaCharType *text)
#else
    int altiaGetTextWidthId(nameId, text)
    int nameId;
    AltiaCharType *text;
#endif
{
    int retVal;
    CONST ALTIA_CHAR *localText;

    /* Validate Animation */
    if (nameId < 0)
    {
        return -1;
    }

    if (NULL == text)
    {
        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return -1;
    }

    TargetAltiaSemGet(0);

    /* Get the size */
    localText = apiStr2LocalStr(text);
    retVal = TargetGetTextWidthId(nameId, localText);

    TargetAltiaSemRelease(0);
    return retVal;
}
 
/***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaGetFont(CONST AltiaCharType *name)
#else
    int altiaGetFont(name)
    CONST AltiaCharType *name;
#endif
{
    int retVal;
    CONST ALTIA_CHAR *localName;

    /* Validate Animation */
    if (NULL == name)
    {
        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    /* Get the font */
    localName = apiStr2LocalStr(name);
    retVal = TargetGetFont(localName);

    TargetAltiaSemRelease(0);
    return retVal;
}
 
/***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaGetFontId(int nameId)
#else
    int altiaGetFontId(nameId)
    int nameId;
#endif
{
    int retVal;

    /* Validate Animation */
    if (nameId < 0)
    {
        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    /* Get the font */
    retVal = TargetGetFontId(nameId);

    TargetAltiaSemRelease(0);
    return retVal;
}
 
/***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaSetFont(CONST AltiaCharType *name, int fontId)
#else
    int altiaSetFont(name, fontId)
    CONST AltiaCharType *name;
    int fontId;
#endif
{
    int retVal;
    CONST ALTIA_CHAR *localName;

    /* Validate Animation */
    if (NULL == name)
    {
        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    /* Change the font */
    localName = apiStr2LocalStr(name);
    retVal = TargetSetFont(localName, fontId);
    if (0 == retVal)
    {
        if (0 == _altiaAPICaching)
        {
            int checkReturn;

            TargetAltiaUpdate();

            /* Were about to check for events so clear check count. */
            localNoCheckCount = 0;

           /* Do one loop of system event processing */
           if (TargetCheckEvent(&checkReturn) < 0)
           {
               /* Assume we need to break due to DeepScreen window close */
               _altiaBreakLoop = 1;
               return -1;
           }
        }
        else
        {
            _altiaAPINeedUpdate = 1;
        }
    }

    TargetAltiaSemRelease(0);
    return retVal;
}

/***************************************************************************/
#if AltiaFunctionPrototyping
    int altiaSetFontId(int nameId, int fontId)
#else
    int altiaSetFontId(nameId, fontId)
    int nameId;
    int fontId;
#endif
{
    int retVal;

    /* Validate Animation */
    if (nameId < 0)
    {
        return -1;
    }

    retVal = altiaConnect(NULL);
    if (0 != retVal)
    {
        return retVal;
    }

    TargetAltiaSemGet(0);

    /* Change the font */
    retVal = TargetSetFontId(nameId, fontId);
    if (0 == retVal)
    {
        if (0 == _altiaAPICaching)
        {
            int checkReturn;

            TargetAltiaUpdate();

            /* Were about to check for events so clear check count. */
            localNoCheckCount = 0;

           /* Do one loop of system event processing */
           if (TargetCheckEvent(&checkReturn) < 0)
           {
               /* Assume we need to break due to DeepScreen window close */
               _altiaBreakLoop = 1;
               return -1;
           }
        }
        else
        {
            _altiaAPINeedUpdate = 1;
        }
    }

    TargetAltiaSemRelease(0);
    return retVal;
}

