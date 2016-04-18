/* $Revision: 1.4 $    $Date: 2009-05-06 00:04:10 $
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

/** FILE:  altiaLibWFont.c **************************************************
 **
 ** This file contains routines that Altia code can call to determine if
 ** a font is fixed width and to determine a text strings extent (i.e.,
 ** the area occupied by the string) for a specific font.  This second
 ** function is very important because the Altia code cannot tell what
 ** area of the screen is affected by a text string unless it can compute
 ** its extent.  If a design does not use any fonts (i.e., it has no labels,
 ** textio, ticks with labels, etc.), this file contains no code.
 ***************************************************************************/



#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "altiaLibDraw.h"
#include "egl_Wrapper.h"

extern AltiaDevInfo *altiaLibGetDevInfo(void);


/*--- altiaLibIsFixedWidthFont() ------------------------------------------
 *
 * Function called from generic Altia code to determine if the given
 * font is fixed width.  This typically requires target graphics library
 * calls.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
ALTIA_BOOLEAN altiaLibIsFixedWidthFont(Altia_Font_type *font)
{
    EGL_FONT_METRICS metrics;
    AltiaDevInfo *devInfo = altiaLibGetDevInfo();

    if (devInfo->fontDrvId != (EGL_FONT_DRIVER_ID) 0
        && font->id != 0
        && egl_FontMetricsGet((EGL_FONT_ID) (font->id),
                              &metrics) == EGL_STATUS_OK
        && metrics.spacing == EGL_FONT_MONO_SPACED)
    {
	return true;
    }

    return false;
}


/*--- altiaLibFontExtent() ------------------------------------------------
 *
 * Function called from generic and target specific Altia code to determine
 * a text strings extent (i.e., the area occupied by the string) for a
 * specified font.  This typically requires target graphics library
 * calls.  This function is very important because the Altia code cannot
 * tell what area of the screen is affected by a text string unless it can
 * compute its extent.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibFontExtent(Altia_Font_type *font, ALTIA_CHAR *s, int len,
                        int *widthPtr, int *heightPtr, int *offsetPtr)
{
    AltiaDevInfo *devInfo = altiaLibGetDevInfo();

    /* Initialize width, height, offset return values to 0 by default */
    *widthPtr = 0;
    *heightPtr = 0;
    *offsetPtr = 0;

    if (devInfo->fontDrvId != (EGL_FONT_DRIVER_ID)0 && font->id != 0)
    {
#if EGL_HAS_TEXTSIZEGETWITHOFFSET
        /* This target supports a version of egl_TextSizeGet() that 
         * can return an offset.  The offset is the amount of empty
         * pixels preceding the first character of the string, but still
         * returned as part of the string's width.
         */
        EGL_SIZE width, height, offset;

        if (egl_TextSizeGet((EGL_FONT_ID)(font->id), &width, &height,
                            &offset, (EGL_SIZE)len, s) == EGL_STATUS_OK)
        {
            /* RTC-1498:  Match editor text sizes */
            *widthPtr = (int)width + (int)1;
            *heightPtr = (int)height;
            *offsetPtr = (int)offset;
        }
#else /* NO EGL_HAS_TEXTSIZEGETWITHOFFSET */
        /* This target cannot return an offset and must use a
         * different version of egl_TextSizeGet().
         */
        EGL_SIZE width, height;

        if (egl_TextSizeGet((EGL_FONT_ID)(font->id), &width, &height,
                            (EGL_SIZE)len, s) == EGL_STATUS_OK)
        {
            /* RTC-1498:  Match editor text sizes */
            *widthPtr = (int)width + (int)1;
            *heightPtr = (int)height;
        }
#endif /* EGL_HAS_TEXTSIZEGETWITHOFFSET */
    }
}

/*--- altiaLibFontWidth() ------------------------------------------------
 *
 * Function called from generic and target specific Altia code to determine
 * a characters advance width (how far in pixels to advance to next char) of
 * specified font.  This typically requires target graphics library
 * calls.  This function is used for scaled text.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
int altiaLibFontWidth(Altia_Font_type *font, ALTIA_CHAR ch)
{
    AltiaDevInfo *devInfo = altiaLibGetDevInfo();
    EGL_SIZE width;

    if (devInfo->fontDrvId != (EGL_FONT_DRIVER_ID) 0 && font->id != 0)
    {
       if (egl_TextWidthGet((EGL_FONT_ID)(font->id), &width, 1, &ch) ==
           EGL_STATUS_OK)
       {
           return width;
       }else
           return 0;
    } else
       return 0;
       
}


