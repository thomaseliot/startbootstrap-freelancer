/* $Revision: 1.5 $    $Date: 2008-12-31 04:31:53 $
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

/** altiaLibSem.c **********************************************************
 ** Target specific functions for getting and releasing semaphores to
 ** protect the code when it is called from multiple threads:
 **
 **   void TargetAltiaSemGet(int index);
 **   void TargetAltiaSemRelease(int index);
 **
 ***************************************************************************/

#include "os_Wrapper.h"

/* Forward declarations for global functions in this file that can be
 * used externally.
 */
#if defined(__cplusplus) || defined(c_plusplus)
extern "C"
{
#endif

    void TargetAltiaSemGet(int index);
    void TargetAltiaSemRelease(int index);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


/*--- TargetAltiaSemGet() and TargetAltiaSemRelease() ---------------------
 *
 * These functions should get and release a mutual exclusion type semaphore
 * as provided by the given target.  By having these functions here, other
 * code that is target independent can call them to do the target specific
 * work of protecting the DeepScreen code from being entered by more than
 * one task or thread at a time.  For example, these functions are used by
 * the DeepScreen version of the Altia API library and TargetSleep() and
 * possibly TargetAltiaUpdate() in altiaUtils.c.  These functions are not
 * called by any other DeepScreen code which means they must be called
 * directly by user code if the Altia API is not being used.
 *
 * The standalone version of the Altia API needs 3 unique semaphores to
 * protect DeepScreen code so the caller can pass 0, 1, or 2 to indicate
 * which semaphore to use.  Semaphore 0 is the fundamental semaphore,
 * semaphore 1 is the semaphore to protect more complicated Altia API
 * calls like those that start/stop/open/close the interface, and
 * semaphore 2 is used on some targets to protect the updating of
 * graphics in TargetAltiaUpdate() in multi-threaded environments.
 *
 *
 * WARNING:  The names for these functions cannot change because these
 *           functions are called from Altia API code.
 *-------------------------------------------------------------------------*/

#define ALTIA_MUTEX_COUNT 3
static OS_SEM_ID targetMutex[ALTIA_MUTEX_COUNT] = { NULL, NULL, NULL };
void TargetAltiaSemGet(int index)
{
    if (index < 0 || index >= ALTIA_MUTEX_COUNT)
        return;

    if (targetMutex[index] == NULL)
        targetMutex[index] = os_semBCreate(index, OS_SEM_Q_PRIORITY, 
	                                   OS_SEM_FULL);

    if (targetMutex[index] != NULL)
        os_semTake(targetMutex[index], OS_WAIT_FOREVER);
}

void TargetAltiaSemRelease(int index)
{
    if (index < 0 || index >= ALTIA_MUTEX_COUNT)
        return;

    if (targetMutex[index] != NULL)
        os_semGive(targetMutex[index]);
}
