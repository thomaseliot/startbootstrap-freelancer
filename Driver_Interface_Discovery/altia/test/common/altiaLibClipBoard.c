/* $Revision: 1.5 $ $Date: 2011/09/23 10:15:40 $
 * Copyright (c) 2005-2011 Altia Inc.
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





#ifndef WIN32
/*====================================================================*/
/* Targets with no WIN32 Clipboard can use the following code         */
/* to implement text copy/paste with just an internal buffer that     */
/* allows the transfer of text between dynamic text objects within    */
/* the DeepScreen generated code.                                     */
/*====================================================================*/

#include "egl_Wrapper.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "../altiaData.h"

static int textLen = 0;

#define TEXTBUFSIZE 64
static ALTIA_CHAR textBuf[TEXTBUFSIZE];



/*----------------------------------------------------------------------*/
#ifdef Altiafp
void altiaLibCopyTextToClipBoard(ALTIA_CHAR *str)
#else
void altiaLibCopyTextToClipBoard(str)
ALTIA_CHAR *str;
#endif
{
    textLen = ALT_STRLEN(str);

    

    /* If clipboard text is too large for our buffer, then just copy
     * as much of it as possible.
     */
    if (textLen >= TEXTBUFSIZE)
    {
        ALT_STRNCPY(textBuf, str, TEXTBUFSIZE - 1);
        textBuf[TEXTBUFSIZE - 1] = '\0';
        textLen = TEXTBUFSIZE - 1;
    }
    else
        ALT_STRCPY(textBuf, str);

    return;
}

/*----------------------------------------------------------------------*/
ALTIA_CHAR *altiaLibGetTextFromClipBoard()
{
    if (textLen > 0)
        return textBuf;

    return NULL;
}

#else /* THIS TARGET HAS WIN32 */

/*====================================================================*/
/* Targets with a WIN32 Clipboard can use the following code          */
/* to copy/paste text between dynamic text objects and copy/paste     */
/* text to/from other applications on the desktop.                    */
/*====================================================================*/

#include <windows.h>
#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "../altiaData.h"


#define TEXTBUFSIZE 64
static ALTIA_CHAR textbuf[TEXTBUFSIZE];



/*----------------------------------------------------------------------*/
#ifdef Altiafp
void altiaLibCopyTextToClipBoard(ALTIA_CHAR *str)
#else
void altiaLibCopyTextToClipBoard(str)
ALTIA_CHAR *str;
#endif
{
    // Support is lacking in older versions of Windows CE
#if defined(UNDER_CE) && defined(_WIN32_WCE) && _WIN32_WCE < 400
    return;
#else
    HANDLE h_data;
    ALTIA_CHAR *h_text;

    if ((h_data = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
                             (DWORD) (sizeof(ALTIA_CHAR)
                             * (ALT_STRLEN((ALTIA_CHAR *) str) + 1)))) == NULL
        || (h_text = (ALTIA_CHAR *) GlobalLock(h_data)) == NULL)
    {
        if (h_data != NULL) GlobalFree(h_data);
        return;
    }
    ALT_STRCPY(h_text, (ALTIA_CHAR *) str);

    GlobalUnlock(h_data);

    if (!OpenClipboard(NULL) || !EmptyClipboard())
    {
        GlobalFree(h_data);
        return;
    }

#if defined(UNICODE)
    SetClipboardData(CF_UNICODETEXT, h_data);
#else
    SetClipboardData(CF_TEXT, h_data);
#endif

    CloseClipboard();

    // The Microsoft clipboard documentation implies
    // that h_data is ours once the clipboard is closed.
    // But testing suggests that this is not the case and
    // we must not free it otherwise problems occur when
    // trying to paste into other applications.  Sample
    // code from MSDN and the net also indicate that this
    // is the case.
    // GlobalFree(h_data);

#endif /* UNDER_CE && _WIN32_WCE && _WIN32_WCE < 400 */
}

/*----------------------------------------------------------------------*/
ALTIA_CHAR *altiaLibGetTextFromClipBoard()
{
    // Support is lacking in older versions of Windows CE
#if defined(UNDER_CE) && defined(_WIN32_WCE) && _WIN32_WCE < 400
    return nil;
#else
    HANDLE h_data, h_text;
    ALTIA_CHAR *lp_data, *lp_text;
    int lp_len;

    if (!OpenClipboard(NULL))
        return nil;

#if defined(UNICODE)
    if ((h_data = GetClipboardData(CF_UNICODETEXT)) == NULL)
#else
    if ((h_data = GetClipboardData(CF_TEXT)) == NULL)
#endif
    {
        CloseClipboard();
        return nil;
    }

    if ((h_text = GlobalAlloc(GMEM_MOVEABLE, GlobalSize(h_data))) == NULL)
    {
        CloseClipboard();
        return nil;
    }

    if ((lp_data = (ALTIA_CHAR*) GlobalLock(h_data)) == NULL)
    {
        CloseClipboard();
        GlobalFree(h_text);
        return nil;
    }

    if ((lp_text = (ALTIA_CHAR*) GlobalLock(h_text)) == NULL)
    {
        GlobalUnlock(h_data);
        CloseClipboard();
        GlobalFree(h_text);
        return nil;
    }

    ALT_STRCPY(lp_text, lp_data);
    lp_len = ALT_STRLEN(lp_text);

    

    // If clipboard text is too large for our buffer, then just copy
    // as much of it as possible.
    if (lp_len >= TEXTBUFSIZE)
    {
        ALT_STRNCPY(textbuf, lp_text, TEXTBUFSIZE - 1);
        textbuf[TEXTBUFSIZE - 1] = '\0';
    }
    else
    {
        ALT_STRCPY(textbuf, lp_text);
    }

    GlobalUnlock(h_data);
    CloseClipboard();
    GlobalUnlock(h_text);
    GlobalFree(h_text);

    if (lp_len == 0)
        return nil;

    return textbuf;

#endif /* UNDER_CE && _WIN32_WCE && _WIN32_WCE < 400 */
}

#endif /* WIN32 */



