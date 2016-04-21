/* $Revision: 1.43 $    $Date: 2010-09-16 19:49:13 $
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
/* If this file is empty then the design had no stimulus in it. */


#include <string.h>
#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#include "altiaData.h"
#include <math.h>

/* For debug output... */
#ifdef DEBUG_WINDOW
#include <stdio.h>
#endif

#ifdef WIN32
#undef y0
#undef y1
#endif

/* Windows only */
#if defined(WIN32) && !defined(MICROWIN)
#pragma warning( disable: 4244 )
#endif


#define MAX_SENT 128
#define MAX_SAVE 64
#define CLOCKWISE 0
#define COUNTERCLOCKWISE 1


#define ZERO_TOL 1
#define NEG_ZERO_TOL -1






typedef struct
{
    ALTIA_CONST Altia_RectInput_type *rarea;
    ALTIA_CONST Altia_PolarInput_type *parea;
    Altia_IStateObj_type *sobj;
    Altia_Coord_type     pt;
    AltiaEventType cvalue;
    ALTIA_CHAR  *name;
    ALTIA_BOOLEAN interpolate;
    AltiaEventType ivalue;
    ALTIA_FLOAT eventx;
    ALTIA_FLOAT eventy;
    ALTIA_BOOLEAN outside_failure;
    ALTIA_CHAR  *enable;
    AltiaEventType enable_val;
} StimulusInterp_type;



typedef struct
{
    ALTIA_CHAR  *func;
    AltiaEventType  value;
    ALTIA_SHORT  count;
} StimulusSent_type;

typedef struct
{
    AltiaEventType        value;
    ALTIA_SHORT           count;
    Altia_IStateObj_type *state;
    ALTIA_BOOLEAN         enabled;
} StimulusSave_type;




/*----------------------------------------------------------------------*/
#ifdef Altiafp
AltiaEventType _altiaComputeValue(unsigned int executeCmd,
                                  ALTIA_CHAR  *outputFunc,
                                  AltiaEventType value)
#else
AltiaEventType _altiaComputeValue(executeCmd, outputFunc, value)
unsigned int executeCmd;
ALTIA_CHAR  *outputFunc;
AltiaEventType value;
#endif
{
    AltiaEventType res;
    AltiaEventType cvalue = 0;

    switch (executeCmd)
    {
        case ALTIA_INC_INPUT_CMD:
            /* only get the cur val if we have to for speed */

            cvalue = _altiaFindCurVal(outputFunc);

            res = cvalue + value;
            break;
        case ALTIA_DEC_INPUT_CMD:
            /* only get the cur val if we have to for speed */

            cvalue = _altiaFindCurVal(outputFunc);

            res = cvalue - value;
            break;
        case ALTIA_NOOP_INPUT_CMD:
        default:
            res = value;
            break;
    }

    return res;
}





/*----------------------------------------------------------------------*/
#ifdef Altiafp
ALTIA_BOOLEAN _altiaEnabled(ALTIA_BYTE enableCmd,
                            ALTIA_CHAR  *enableFunc,
                            AltiaEventType enableValue)
#else
ALTIA_BOOLEAN _altiaEnabled(enableCmd, enableFunc, enableValue)
ALTIA_BYTE enableCmd;
ALTIA_CHAR  *enableFunc;
AltiaEventType enableValue;
#endif
{
    ALTIA_BOOLEAN enabled = ALTIA_FALSE;
    AltiaEventType value;


    value = _altiaFindCurVal(enableFunc);


    switch (enableCmd)
    {
        case ALTIA_NOOP_INPUT_CMD:
            if (ALTIA_EQUAL(value, enableValue))
                enabled = ALTIA_TRUE;
            break;
        case ALTIA_LESS_INPUT_CMD:
            if (ALTIA_LESS(value, enableValue))
                enabled = ALTIA_TRUE;
            break;
        case ALTIA_GREAT_INPUT_CMD:
            if (ALTIA_GREATER(value, enableValue))
                enabled = ALTIA_TRUE;
            break;
        case ALTIA_NOT_EQUAL_INPUT_CMD:
            if (!ALTIA_EQUAL(value, enableValue))
                enabled = ALTIA_TRUE;
            break;
        case ALTIA_LESS_EQUAL_INPUT_CMD:
            if (ALTIA_LESS(value, enableValue) ||
                ALTIA_EQUAL(value, enableValue))
                enabled = ALTIA_TRUE;
            break;
        case ALTIA_GREAT_EQUAL_INPUT_CMD:
            if (ALTIA_GREATER(value, enableValue) ||
                ALTIA_EQUAL(value, enableValue))
                enabled = ALTIA_TRUE;
            break;
    }
    return enabled;
}





/*----------------------------------------------------------------------*/
/* If external application code needs to know the raw event that becomes
 * stimulus, it can assign a function to the global function pointer
 * variable AltiaReportEventPtr during its initialization.  The function
 * must take a byte argument that is the event type value (e.g.,
 * ALTIA_DOWN_EVENT_TYPE as defined in altiaTypes.h).  The function is
 * called when it is known that a raw event will cause animations to execute,
 * but before the animations actually execute.
 */
typedef void (*AltiaReportEventType)(
#ifdef Altiafp
ALTIA_BYTE eventType
#endif
);

AltiaReportEventType AltiaReportEventPtr = NULL;

/*----------------------------------------------------------------------*/
ALTIA_BOOLEAN altiaHandleInputEvent(Altia_InputEvent_type *event)
{
    /*
     * We look for input events and we do it from back to front.
     * Since input on the top gets executed first.
     */
    int i;
    StimulusSent_type sent[MAX_SENT];
    int sent_count = 0;


    if (event->eventType == ALTIA_TIMER_EVENT_TYPE)
    {
        ALTIA_INDEX tIndex = (ALTIA_INDEX)event->button;
        ALTIA_INDEX first;
        ALTIA_SHORT count;
        first = ALTIA_TIMER_FIRSTSTATE(tIndex);
        count = ALTIA_TIMER_COUNT(tIndex);
        if (ALTIA_TIMER_STOPPED(tIndex))
        {
            return ALTIA_FALSE; /* Timer is shut off */
        }

        for (i = 0; i < count; i++)
        {
            ALTIA_CONST Altia_TimerState_type *state = &test_timerState[first+i];
            if (_altiaEnabled((ALTIA_BYTE) state->enableCmd,
                state->enableFunc, state->enableVal))
            {
                AltiaEventType val = _altiaComputeValue(state->executeCmd,
                                        state->outputFunc, state->outputVal);
                sent[sent_count].func = state->outputFunc;
                sent[sent_count].value = val;
                sent[sent_count].count = sent_count;
                sent_count++;
                if (sent_count >= MAX_SENT)
                     break;
            }
        }
    }


    if (sent_count > 0)
    { /* We have some input events to send out.  We wait until we
       * have checked all inputs before we send any so they don't affect
       * each other.
       */
        ALTIA_BOOLEAN res = ALTIA_FALSE;

        /* If external application code needs to know that the raw event
         * became a stimulus, let them know and pass the raw event type.
         */
        if (NULL != AltiaReportEventPtr)
            (*AltiaReportEventPtr)(event->eventType);

        for (i = 0; i < sent_count; i++)
        { /* do the input events */
            if (AltiaAnimate(sent[i].func, sent[i].value))
                res = ALTIA_TRUE;
        }

        return res;
    }else
    {
        return ALTIA_FALSE;
    }
}


