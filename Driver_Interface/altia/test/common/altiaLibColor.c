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

/** FILE:  altiaLibColor.c *************************************************
 **
 ** This file contains routines to convert Altia color values to
 ** target specific color values that are suitable in calls to the
 ** target graphics library.
 ***************************************************************************/

#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "altiaLibDraw.h"
#include "egl_Wrapper.h"


/*--- altiaLibGetColor() --------------------------------------------------
 *
 * Take an Altia color value and convert it to a target specific
 * color value.
 *-------------------------------------------------------------------------*/
EGL_COLOR altiaLibGetColor(ALTIA_WINDOW devinfo, ALTIA_COLOR pixel)
{
    EGL_ARGB ARGBcolor;
    EGL_COLOR targetColor;
    ALTIA_UINT32 r,g,b,a;

    r = pixel & 0x0ff;
    g = (pixel >> 8) & 0x0ff;
    b = (pixel >> 16) & 0x0ff;
	a = (pixel >> 24) & 0x0ff;

    ARGBcolor = EGL_MAKE_ARGB(a, r, g, b);

    egl_ColorAlloc(((AltiaDevInfo *) devinfo)->devId, &ARGBcolor, NULL,
                   &targetColor, 1);

    return targetColor;
}
