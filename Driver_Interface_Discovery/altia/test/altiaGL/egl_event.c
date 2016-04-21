/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.4 $    $Date: 2009-05-06 00:03:00 $
 * Copyright (c) 2002 Altia Inc.
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

/** FILE:  event.c ****************************************************
 **
 ** This file contains the event handling functions for a general purpose
 ** graphics library.  
 **
 **
 ***************************************************************************/

#include "os_Wrapper.h"
#include "egl_Wrapper.h"

#include <stdio.h>

extern EGL_STATUS driver_getEvent(EGL_EVENT_Q_ID eventQID, EGL_EVENT *pevent,
                                  EGL_SIZE eventSize, EGL_TIMEOUT timemout);


/*****************************************************************
 * Event handling functions for receiving keyboard and mouse events
 *****************************************************************/


/***************************************************************************/
EGL_STATUS egl_EventGet (EGL_EVENT_Q_ID eventQId, EGL_EVENT * pEvent, 
                         EGL_SIZE eventSize, EGL_TIMEOUT timeout)
{
    return driver_getEvent(eventQId, pEvent, eventSize, timeout);

}

/***************************************************************************/
EGL_EVENT_Q_ID egl_EventQCreate (EGL_EVENT_SERVICE_ID eventServiceId, 
                                 EGL_SIZE queueSize)
{
    /* Return a safe value to make Altia code happy. */
    return (EGL_EVENT_Q_ID) 1;
}

/***************************************************************************/
EGL_STATUS egl_EventQDestroy (EGL_EVENT_SERVICE_ID eventServiceId, 
                              EGL_EVENT_Q_ID qId)
{
    return EGL_STATUS_OK;
}
