/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.10 $    $Date: 2009-05-06 00:03:00 $
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

/** FILE:  rect.c ****************************************************
 **
 ** This file contains the rectangle functions for a general purpose
 ** graphics library.  
 **
 ***************************************************************************/

#include <stdio.h>
#include "egl_Wrapper.h"
#include "scrnintstr.h"
#include "Xprotostr.h"
#include "gcstruct.h"
#include "pixmapstr.h"

#if EGL_DO_ALPHA_BLENDING
#include <fb.h>
#endif /* EGL_DO_ALPHA_BLENDING */

#define DRAWABLE(win) ((DrawableRec*)((AltiaDevInfo*)win)->drawable)

/***************************************************************************/
EGL_STATUS egl_Rectangle(ALTIA_WINDOW win, EGL_GC_ID gc, EGL_POS left,
    EGL_POS top, EGL_POS right, EGL_POS bottom, EGL_BOOL fill,
    EGL_BOOL clear)
{
    xRectangle xrect;
    GCPtr pGC = (GCPtr)gc;
#if EGL_DO_ALPHA_BLENDING
    int adjust = 0;

    if(pGC->alpha == EGL_TRANSPARENT_ALPHA)
    {
        return EGL_STATUS_OK;
    }
#endif /* EGL_DO_ALPHA_BLENDING */

    xrect.x = left;
    xrect.y = top;
    xrect.width  = right - left;
    xrect.height = bottom - top;

    if (fill == FALSE && 
        ((pGC->colorFlip == 0 && pGC->fgPixel != EGL_COLOR_TRANSPARENT) ||
        (pGC->colorFlip == 1 && pGC->bgPixel != EGL_COLOR_TRANSPARENT)))
    {
#if EGL_DO_ALPHA_BLENDING
        adjust = (pGC->lineWidth == 0) ? 1 : pGC->lineWidth;
#endif /* EGL_DO_ALPHA_BLENDING */

        if(pGC->lineWidth > 1 && pGC->lineWidth <= xrect.width)
        {
            int o1 = pGC->lineWidth / 2;
            int o2 = pGC->lineWidth - 1;

            xrect.x += o1;
            xrect.y += o1;
            xrect.width -= o2;
            xrect.height -= o2;
        }
    }
    else if (fill == TRUE && 
        ((pGC->colorFlip == 0 && pGC->bgPixel != EGL_COLOR_TRANSPARENT) ||
        (pGC->colorFlip == 1 && pGC->fgPixel != EGL_COLOR_TRANSPARENT)))
    {
        xrect.width++;
        xrect.height++;
    }
    else
    {
        return EGL_STATUS_OK;
    }

#if EGL_DO_ALPHA_BLENDING
    if(pGC->alpha != EGL_OPAQUE_ALPHA &&
        (((DrawablePtr)DRAWABLE(win))->type == DRAWABLE_PIXMAP ||
        ((DrawablePtr)DRAWABLE(win))->type == DRAWABLE_BUFFER))
    {
        PixmapPtr front, alpha;
        GCPtr pUtilityGC;
        unsigned char aluSave = GXcopy;

        xrect.x -= left;
        xrect.y -= top;

        if((front = GetScratchPixmap(
            xrect.width + adjust,
            xrect.height + adjust)) == (PixmapPtr) 0)
        {
            return -1;
        }

        if((alpha = GetScratchAlphaPixmap(
            xrect.width + adjust,
            xrect.height + adjust,
            (fill == FALSE) ? EGL_TRANSPARENT_ALPHA : pGC->alpha)) ==
            (PixmapPtr) 0)
        {
            return -1;
        }

        pUtilityGC = GetScratchGC(SCREENPTR->rootDepth, SCREENPTR);

        if(fill == FALSE)
        {
            if(pUtilityGC->lineWidth != pGC->lineWidth)
            {
                pUtilityGC->lineWidth = pGC->lineWidth;
                pUtilityGC->stateChanges |= GCLineWidth;
            }

            if(pGC->colorFlip == 1)
            {
                if(pUtilityGC->fgPixel != pGC->bgPixel)
                {
                    pUtilityGC->fgPixel = pGC->bgPixel;
                    pUtilityGC->stateChanges |= GCForeground;
                }
            }
            else
            {
                if(pUtilityGC->fgPixel != pGC->fgPixel)
                {
                    pUtilityGC->fgPixel = pGC->fgPixel;
                    pUtilityGC->stateChanges |= GCForeground;
                }
            }

            /*
             * Draw unfilled rectangle into front pixmap
             */
            (*pGC->funcs->ValidateGC)(
                pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)front);
            (*pGC->ops->PolyRectangle)(
                (DrawablePtr)front, pUtilityGC, 1, &xrect);

            /*
             * Draw unfilled rectangle into alpha pixmap
             */
            if(pUtilityGC->fgPixel != pGC->alpha)
            {
                pUtilityGC->fgPixel = pGC->alpha;
                pUtilityGC->stateChanges |= GCForeground;
            }

            (*pGC->funcs->ValidateGC)(
                pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)alpha);
            (*pGC->ops->PolyRectangle)(
                (DrawablePtr)alpha, pUtilityGC, 1, &xrect);
        }
        else
        {
            if(pUtilityGC->fillStyle != pGC->fillStyle)
            {
                pUtilityGC->fillStyle = pGC->fillStyle;
                pGC->stateChanges |= GCFillStyle;
            }

            if(pUtilityGC->stipple != pGC->stipple)
            {
                pUtilityGC->stipple = pGC->stipple;
                pGC->stateChanges |= GCStipple;
            }

            if(pGC->colorFlip == 1 || pGC->fillStyle != FillSolid)
            {
                if(pUtilityGC->fgPixel != pGC->fgPixel)
                {
                    pUtilityGC->fgPixel = pGC->fgPixel;
                    pUtilityGC->stateChanges |= GCForeground;
                }

                if(pUtilityGC->bgPixel != pGC->bgPixel)
                {
                    pUtilityGC->bgPixel = pGC->bgPixel;
                    pUtilityGC->stateChanges |= GCBackground;
                }
            }
            else
            {
                if(pUtilityGC->fgPixel != pGC->bgPixel)
                {
                    pUtilityGC->fgPixel = pGC->bgPixel;
                    pUtilityGC->stateChanges |= GCForeground;
                }

                if(pUtilityGC->bgPixel != pGC->bgPixel)
                {
                    pUtilityGC->bgPixel = pGC->fgPixel;
                    pUtilityGC->stateChanges |= GCBackground;
                }
            }

            /*
             * Draw filled rectangle into front pixmap
             */
            (*pGC->funcs->ValidateGC)(
                pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)front);
            (*pGC->ops->PolyFillRect)(
                (DrawablePtr)front, pUtilityGC, 1, &xrect);
        }

        FreeScratchGC(pUtilityGC);

        if(pGC->alu != GXcopy)
        {
            aluSave = pGC->alu;
            pGC->alu = GXcopy;
            pGC->stateChanges |= GCFunction;
        }

        (*pGC->funcs->ValidateGC)(pGC, pGC->stateChanges, DRAWABLE(win));

        (*((GCPtr)gc)->ops->AlphaCopyArea)(
            (DrawablePtr)front,
            (DrawablePtr)DRAWABLE(win),
            (DrawablePtr)alpha,
            pGC, 0, 0,
            front->drawable.width,
            front->drawable.height,
            left, top);

        if(aluSave != GXcopy)
        {
            pGC->alu = aluSave;
            pGC->stateChanges |= GCFunction;
        }
    }
    else
#endif /* EGL_DO_ALPHA_BLENDING */
    {
        if(fill == FALSE)
        {
            local_GcAttributesSet(win, pGC, 0 /* filled */, 0 /* not text */);
            (*pGC->ops->PolyRectangle)(DRAWABLE(win), pGC, 1, &xrect);
        }
        else if (clear)
        {
            /* Disable transparent buffer drawing for clearing rectangles.
            ** This ensures the destination alpha is set to zero for the draw.
            */
            int trans = pGC->transDraw;
            pGC->transDraw = 0;
            local_GcAttributesSet(win, pGC, 1 /* filled */, 0 /* not text */);
            (*pGC->ops->PolyFillRect)(DRAWABLE(win), pGC, 1, &xrect);
            pGC->transDraw = trans;
        }
        else
        {
            local_GcAttributesSet(win, pGC, 1 /* filled */, 0 /* not text */);
            (*pGC->ops->PolyFillRect)(DRAWABLE(win), pGC, 1, &xrect);
        }
    }

    return EGL_STATUS_OK;
}
