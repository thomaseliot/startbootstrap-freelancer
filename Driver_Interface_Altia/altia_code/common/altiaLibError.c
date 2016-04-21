/* $Revision: 1.5 $    $Date: 2009-05-06 00:04:10 $
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

/** FILE:  altiaLibDraw.c ***************************************************
 **
 ** This file contains a routine that any Altia code can call to request
 ** the displaying of an error message.  How the message is displayed is
 ** target specific or the displaying of the message can be totally
 ** suppressed if the target has no way to display it.
 ***************************************************************************/

#include <stdio.h>
#include "../altiaBase.h"

#ifdef ALTIA_DRV_ERRORMSG
extern void driverErrorMessage(ALTIA_CONST ALTIA_CHAR * error);
#endif

/*--- _altiaErrorMessage() ------------------------------------------------
 *
 * Function called from any Altia code to request the display of an
 * error message.  How that message is displayed is target specific.
 * If the target has no means to display the message, it can be ignored.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void _altiaErrorMessage(ALTIA_CONST ALTIA_CHAR *error)
{
#ifdef ALTIA_DRV_ERRORMSG
    driverErrorMessage(error);
#else
    #ifndef UNICODE
    printf("Altia code gen message: %s\n", error);
    #else
    printf("Altia code gen message: ");
    while(*error != 0)
        printf("%c", (*error++) & 0x0ff);
    printf("\n");
    #endif
#endif /* ALTIA_DRV_ERRORMSG */
}

void _altiaErrorOutput(ALTIA_CONST ALTIA_CHAR *error)
{
#ifdef ALTIA_DRV_ERRORMSG
    driverErrorMessage(error);
#else
    #ifndef UNICODE
    printf("Error: %s\n", error);
    #else
    printf("Error: ");
    while(*error != 0)
        printf("%c", (*error++) & 0x0ff);
    printf("\n");
    #endif
#endif /* ALTIA_DRV_ERRORMSG */
}
