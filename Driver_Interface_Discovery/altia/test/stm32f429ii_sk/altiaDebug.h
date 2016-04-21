/**
 * @brief   Target debug functionality
 *
 * @copyright (c) 2014 Altia, Inc.
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

#ifndef _ALTIADEBUG_H_
#define _ALTIADEBUG_H_

#if DS_DEBUG
/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>

/*
 * BSP includes
 */
#include "serial.h"


/******************************************************************************
 * Definitions
 ******************************************************************************/

#define ALTIA_DEBUG_XSTR(x)             ALTIA_DEBUG_STR(x)
#define ALTIA_DEBUG_STR(x)              #x


#define DS_DBG_PRINT(...)  \
{ \
    char stringBuffer[256]; \
    int stringBufferLength = sizeof(stringBuffer) / sizeof(stringBuffer[0]); \
    char * pStringBuffer = stringBuffer; \
    int tmp; \
    \
    tmp = snprintf(pStringBuffer, stringBufferLength, "DS_DEBUG:  %s():  ", __FUNCTION__); \
    stringBufferLength -= tmp; \
    \
    if (stringBufferLength > 0) \
    { \
        pStringBuffer += tmp; \
        tmp = snprintf(pStringBuffer, stringBufferLength, __VA_ARGS__); \
        stringBufferLength -= tmp; \
    } \
    \
    if (stringBufferLength > 0) \
    { \
        pStringBuffer += tmp; \
        tmp = snprintf(pStringBuffer, stringBufferLength, "\r\n"); \
        stringBufferLength -= tmp; \
    } \
    \
    SerialWriteStr(UART2_PORT, (const char *) stringBuffer); \
}

#define DS_DBG_EXIT(x)                  exit(x)


/******************************************************************************
 * Types
 ******************************************************************************/


/******************************************************************************
 * Configuration
 *
 * :NOTE:
 * Uncomment these #defines to enable certain debug prints
 ******************************************************************************/


/******************************************************************************
 * APIs
 ******************************************************************************/
#else

/*
 * Build type is release, not debug
 */
#define DS_DBG_PRINT(...)
#define DS_DBG_EXIT(x)

#endif  // #ifdef DEBUG
#endif  // #ifndef _ALTIADEBUG_H_
