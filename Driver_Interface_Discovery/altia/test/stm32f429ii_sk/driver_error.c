/**
 * @brief   Target error message / debug functionality
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

#include "altiaBase.h"
#include "altiaDebug.h"

/*
 * BSP includes
 */
#include "serial.h"


#ifdef ALTIA_DRV_ERRORMSG

void driverErrorMessage(ALTIA_CONST ALTIA_CHAR * error)
{
#ifndef UNICODE
    SerialWriteStr(UART2_PORT, "ALTIA:  ERROR:  ");
    SerialWriteStr(UART2_PORT, (const char *) error);
    SerialWriteStr(UART2_PORT, "\r\n");

#else
    SerialWriteStr(UART2_PORT, "ALTIA:  ERROR:  ");
    while (*error != 0)
    {
        char tmpStringBuffer[32];
        
        if (*error <= 0x7F)
        {
            tmpStringBuffer[0] = (char) (*error & 0x007F);
            tmpStringBuffer[1] = NULL;
        }
        else
        {
            snprintf(tmpStringBuffer, (sizeof(tmpStringBuffer)/sizeof(tmpStringBuffer[0])), "%#04x ", *error);
        }
        
        SerialWriteStr(UART2_PORT, (const char *) tmpStringBuffer);
        error++;
    }
    SerialWriteStr(UART2_PORT, "\r\n");
#endif  // #ifndef UNICODE
}

#endif  // #ifdef ALTIA_DRV_ERRORMSG

