/**
 * @brief   Target OS abstraction layer
 *
 * @copyright (c) 2015 Altia, Inc.
 *
 * Permission to use, copy, and modify this software for any purpose is
 * hereby granted, provided that this copyright notice appears in all
 * copies and that the entity using, copying, or modifying this software
 * has purchased an Altia Deep Screen license.  Permission to sell or
 * distribute this source code is denied.
 *
 * Altia makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 *
 * ALTIA INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL ALTIA INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>

#include "os_Wrapper.h"
#include "egl_Wrapper.h"
#include "altiaBase.h"
#include "input.h"
#include "altiaDebug.h"

/*
 * BSP includes
 */
#include "timer.h"


/******************************************************************************
 * Definitions
 ******************************************************************************/


/******************************************************************************
 * Function prototypes
 ******************************************************************************/


/******************************************************************************
 * Global variables
 ******************************************************************************/


/******************************************************************************
 * Private types
 ******************************************************************************/


/******************************************************************************
 * Private variables
 ******************************************************************************/


/******************************************************************************
 * Private functions
 ******************************************************************************/


/******************************************************************************
 * APIs
 ******************************************************************************/
int os_open(void)
{
    /*
     * Nothing to do
     */
    return (TRUE);
}


int os_close(void)
{
    /*
     * Nothing to do
     */
    return (TRUE);
}


int os_clock_gettime(int clock_id, OS_timespec *curTime)
{
    uint32_t msecs = bspGetSystemTimeMilliseconds();

    if (clock_id != OS_CLOCK_REALTIME)
    {
        DS_DBG_PRINT("WARNING:  Unexpected clock ID:  %d", clock_id);
    }

    curTime->tv_sec  = (unsigned long)(msecs / 1000);
    curTime->tv_nsec = (unsigned long)((msecs % 1000) * 1000000);

    return (0);
}


int os_select(int width, OS_fd_set *readfds, OS_fd_set *writefds, OS_fd_set *exceptfds, OS_timeval *timeout)
{
    SDelayTime_t delayTime;

    /*
     * Set up BSP delay time structure
     * Then just do a normal delay per the BSP API
     */
    delayTime.seconds = timeout->tv_sec;
    delayTime.milliseconds = timeout->tv_usec / 1000;   // Divide number of microseconds by 1000 to get milliseconds
    delayTime.microseconds = timeout->tv_usec % 1000;   // Capture any microsecond remainder

    bspDelay(&delayTime);

    return (0);
}


OS_SEM_ID os_semBCreate(int idx, int options, int initialState)
{
    return ((OS_SEM_ID) NULL);
}


int os_semTake(OS_SEM_ID semId, int timeout)
{
    return (-1);
}


int os_semGive(OS_SEM_ID semId)
{
    return (-1);
}

