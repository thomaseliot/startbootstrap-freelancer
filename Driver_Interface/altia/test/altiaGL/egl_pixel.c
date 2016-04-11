/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.6 $    $Date: 2009-05-06 00:03:00 $
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

/** FILE:  pixel.c ****************************************************
 **
 ** This file contains the pixel functions for a general purpose
 ** graphics library. 
 **
 ***************************************************************************/

#include <stdio.h>
#include "egl_Wrapper.h"
#include "Xprotostr.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#if EGL_DO_ALPHA_BLENDING
#include <fb.h>
#endif /* EGL_DO_ALPHA_BLENDING */

extern DrawablePtr lastDrawable;

#define DRAWABLE(win) ((DrawableRec*)((AltiaDevInfo*)win)->drawable)

/***************************************************************************/
EGL_STATUS egl_PixelSet(ALTIA_WINDOW win, EGL_GC_ID gc, EGL_POS x, EGL_POS y,
                        EGL_COLOR colorRef)
{
    GCPtr pGC = (GCPtr)gc;
    xPoint pt;
    pt.x = x;
    pt.y = y;

#if EGL_DO_ALPHA_BLENDING
    if(pGC->alpha == EGL_TRANSPARENT_ALPHA)
    {
        return EGL_STATUS_OK;
    }

    if(pGC->alpha != EGL_OPAQUE_ALPHA &&
        (((DrawablePtr)DRAWABLE(win))->type == DRAWABLE_PIXMAP ||
        ((DrawablePtr)DRAWABLE(win))->type == DRAWABLE_BUFFER))
    {
        EGL_COLOR dst = egl_PixelGet(win, gc, x, y);

        colorRef = (EGL_COLOR)FBBLEND(pGC->alpha, colorRef, dst);
    }
#endif /* EGL_DO_ALPHA_BLENDING */

    if (pGC->fgPixel != colorRef)
    {
        egl_ForegroundColorSet(pGC, colorRef);
        local_GcAttributesSet(win, pGC, 0, 0);
    }
    else if (DRAWABLE(win) != lastDrawable)
    {
        local_GcAttributesSet(win, pGC, 0, 0);
    }
    else if (pGC->stateChanges != 0)
    {
        local_GcAttributesSet(win, pGC, 0, 0);
    }

    ((GCPtr)gc)->ops->PolyPoint(DRAWABLE(win), pGC, CoordModeOrigin, 1, &pt);

    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_COLOR egl_PixelGet(ALTIA_WINDOW win, EGL_GC_ID gc, EGL_POS x, EGL_POS y)
{
    int widths = 1;
    xPoint pt;
    long data  = 0;
    pt.x = x;
    pt.y = y;

    (*eglScreen.GetSpans)(DRAWABLE(win), (GCPtr)gc, ((GCPtr)gc)->lineWidth, &pt , &widths, 1, (char*)&data);

    return (EGL_COLOR)data;
}
