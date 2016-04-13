/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.8 $    $Date: 2009-05-06 00:03:00 $
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

/** FILE:  polygon.c ****************************************************
 **
 ** This file contains the polygon functions for a general purpose
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
#include <altiaBase.h>
#include <altiaTypes.h>
#include <fb.h>
#endif /* EGL_DO_ALPHA_BLENDING */

#define DRAWABLE(win) ((DrawableRec*)((AltiaDevInfo*)win)->drawable)

/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_Polygon(ALTIA_WINDOW win, EGL_GC_ID gc, 
 *                         EGL_ORD numPoints, EGL_POS *data, EGL_BOOL fill)
 *
 * Draws a polygon using the data array of values as a sequence of
 * coordinate pairs.  For example, data[0] with data[1] gives the x,y
 * starting point for the polygon and data[(numPoints - 1) * 2] with
 * data[((numPoints - 1) * 2) + 1] gives the x,y ending point for the
 * polygon.
 *
 * To have a closed polygon, the starting point and ending point must match.
 *
 * The polygon is drawn onto the bitmap for the given gc as previously
 * set by egl_DefaultBitmapSet().  If the bitmap was never set, then
 * the polygon is drawn directly to the display.
 *
 * The polygon's outline is drawn using the current foreground color,
 * line style, and line width of the gc.  To draw the polygon without
 * an outline, set fill to true
 *
 * The polygon is filled using the current background color and fill
 * pattern for the gc.  To draw an unfilled polygon, use a background
 * color of EGL_COLOR_TRANSPARENT.
 *
 * Any portion of the polygon that falls outside of the gc's current
 * clipping rectangle is not drawn.  
 *
 * The current raster mode for the gc determines how the drawing colors
 * are applied to the pixels affected by the drawing operation.  Depending
 * on the raster mode setting, colors replace the current colors of
 * the pixels, are ANDed with the current colors, or are ORed with the
 * current colors.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_Polygon(ALTIA_WINDOW win, EGL_GC_ID gc, 
                       EGL_ORD numPoints, EGL_POS *data, EGL_BOOL fill)
{
    xPoint *xPtr = (xPoint *)data;
    GCPtr pGC = (GCPtr)gc;

#if EGL_DO_ALPHA_BLENDING
    if(pGC->alpha == EGL_TRANSPARENT_ALPHA)
    {
        return EGL_STATUS_OK;
    }

    if(pGC->alpha != EGL_OPAQUE_ALPHA &&
        (((DrawablePtr)DRAWABLE(win))->type == DRAWABLE_PIXMAP ||
        ((DrawablePtr)DRAWABLE(win))->type == DRAWABLE_BUFFER))
    {
        PixmapPtr front, alpha;
        Altia_Extent_type extent;
        int width, height;
        GCPtr pUtilityGC;
        int adjust = pGC->lineWidth > 1 ? pGC->lineWidth / 2 : 0;
        unsigned char aluSave = GXcopy;
        int i;

        /*
         * Calculate the extent of this polygon
         */
        extent.x0 = extent.x1 = data[0];
        extent.y0 = extent.y1 = data[1];

        for (i = 2; i < numPoints * 2; i += 2)
        {
            if (data[i] < extent.x0)
                extent.x0 = data[i];
            else if (data[i] > extent.x1)
                extent.x1 = data[i];

            if (data[i+1] < extent.y0)
                extent.y0 = data[i+1];
            else if (data[i+1] > extent.y1)
                extent.y1 = data[i+1];
        }

        extent.x0 -= adjust;
        extent.y0 -= adjust;

        width  = (extent.x1 - extent.x0) + 1 + adjust;
        height = (extent.y1 - extent.y0) + 1 + adjust;

        /*
         * Translate the points of this polygon
         */
        for (i = 0; i < numPoints * 2; i += 2)
        {
            data[i]   -= extent.x0;
            data[i+1] -= extent.y0;
        }

        if((front = GetScratchPixmap(width, height)) == (PixmapPtr) 0)
        {
            return (EGL_STATUS) -1;
        }

        if((alpha = GetScratchAlphaPixmap(
            width, height, EGL_TRANSPARENT_ALPHA)) == (PixmapPtr) 0)
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

            /*
             * Draw unfilled polygon into front pixmap
             */
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

            (*pGC->funcs->ValidateGC)(
                pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)front);
            (*pGC->ops->Polylines)((DrawablePtr)front, pUtilityGC,
                CoordModeOrigin, numPoints, xPtr);

            /*
             * Draw unfilled polygon into alpha pixmap
             */
            if(pUtilityGC->fgPixel != pGC->alpha)
            {
                pUtilityGC->fgPixel = pGC->alpha;
                pUtilityGC->stateChanges |= GCForeground;
            }

            (*pGC->funcs->ValidateGC)(
                pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)alpha);
            (*pGC->ops->Polylines)((DrawablePtr)alpha, pUtilityGC,
                CoordModeOrigin, numPoints, xPtr);
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

            /*
             * Draw filled polygon into front pixmap
             */
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

                if(pUtilityGC->bgPixel != pGC->fgPixel)
                {
                    pUtilityGC->bgPixel = pGC->fgPixel;
                    pUtilityGC->stateChanges |= GCBackground;
                }
            }

            (*pGC->funcs->ValidateGC)(
                pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)front);
            (*pGC->ops->FillPolygon)((DrawablePtr)front, pUtilityGC, Complex,
                CoordModeOrigin, numPoints, xPtr);

            /*
             * Draw filled polygon into alpha pixmap
             */
            if(pUtilityGC->fgPixel != pGC->alpha)
            {
                pUtilityGC->fgPixel = pGC->alpha;
                pUtilityGC->stateChanges |= GCForeground;
            }

            if(pUtilityGC->bgPixel != pGC->alpha)
            {
                pUtilityGC->bgPixel = pGC->alpha;
                pUtilityGC->stateChanges |= GCForeground;
            }

            (*pGC->funcs->ValidateGC)(
                pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)alpha);
            (*pGC->ops->FillPolygon)((DrawablePtr)alpha, pUtilityGC, Complex,
                CoordModeOrigin, numPoints, xPtr);
        }

        FreeScratchGC(pUtilityGC);

        if(pGC->alu != GXcopy)
        {
            aluSave = pGC->alu;
            pGC->alu = GXcopy;
            pGC->stateChanges |= GCFunction;
        }

        (*pGC->funcs->ValidateGC)(pGC, pGC->stateChanges, DRAWABLE(win));

        (*pGC->ops->AlphaCopyArea)(
            (DrawablePtr)front,
            (DrawablePtr)DRAWABLE(win),
            (DrawablePtr)alpha,
            pGC, 0, 0,
            front->drawable.width,
            front->drawable.height,
            extent.x0, extent.y0);

        if(aluSave != GXcopy)
        {
            pGC->alu = aluSave;
            pGC->stateChanges |= GCFunction;
        }
    }
    else
#endif
    {
        if (fill == FALSE && 
            ((pGC->colorFlip == 0  && pGC->fgPixel != EGL_COLOR_TRANSPARENT) ||
            (pGC->colorFlip == 1  && pGC->bgPixel != EGL_COLOR_TRANSPARENT)))
        {
            local_GcAttributesSet(win, pGC, 0, 0);
            (*pGC->ops->Polylines)(DRAWABLE(win), pGC, CoordModeOrigin,
                numPoints, xPtr);
        }
        else if (fill == TRUE && 
            ((pGC->colorFlip == 0 && pGC->bgPixel != EGL_COLOR_TRANSPARENT) ||
            (pGC->colorFlip == 1 && pGC->fgPixel != EGL_COLOR_TRANSPARENT)))
        {
            local_GcAttributesSet(win, pGC, 1, 0);
            (*pGC->ops->FillPolygon)(DRAWABLE(win), pGC, Complex,
                CoordModeOrigin, numPoints, xPtr);
        }
    }

    return EGL_STATUS_OK;
}
