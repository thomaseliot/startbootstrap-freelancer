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

/** FILE:  line.c ****************************************************
 **
 ** This file contains the line drawing function for the general purpose
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
#include <altiaBase.h>
#include <altiaTypes.h>

static int _DrawAlphaLines(DrawablePtr dst, GCPtr pGC, int numPoints,
    EGL_POS * data)
{
    PixmapPtr front, alpha;
    Altia_Extent_type extent;
    int width, height;
    GCPtr pUtilityGC;
    int adjust = pGC->lineWidth > 1 ? pGC->lineWidth / 2 : 0;
    unsigned char aluSave = GXcopy;
    int i;

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

    for (i = 0; i < numPoints * 2; i += 2)
    {
        data[i]   -= extent.x0;
        data[i+1] -= extent.y0;
    }

    if((front = GetScratchPixmap(width, height)) == (PixmapPtr) 0)
    {
        return -1;
    }

    if((alpha = GetScratchAlphaPixmap(
        width, height, EGL_TRANSPARENT_ALPHA)) == (PixmapPtr) 0)
    {
        return -1;
    }

    pUtilityGC = GetScratchGC(SCREENPTR->rootDepth, SCREENPTR);

    if(pUtilityGC->lineWidth != pGC->lineWidth)
    {
        pUtilityGC->lineWidth = pGC->lineWidth;
        pUtilityGC->stateChanges |= GCLineWidth;
    }

    /*
     * Draw line segment(s) into front pixmap
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
    (*pGC->ops->Polylines)(
        (DrawablePtr)front, pUtilityGC, CoordModeOrigin, numPoints,
        (xPoint *)data);

    /*
     * Draw line segment(s) into alpha pixmap
     */
    if(pUtilityGC->fgPixel != pGC->alpha)
    {
        pUtilityGC->fgPixel = pGC->alpha;
        pUtilityGC->stateChanges |= GCForeground;
    }

    (*pGC->funcs->ValidateGC)(
        pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)alpha);
    (*pGC->ops->Polylines)(
        (DrawablePtr)alpha, pUtilityGC, CoordModeOrigin, numPoints,
        (xPoint *)data);

    FreeScratchGC(pUtilityGC);

    if(pGC->alu != GXcopy)
    {
        aluSave = pGC->alu;
        pGC->alu = GXcopy;
        pGC->stateChanges |= GCFunction;
    }

    (*pGC->funcs->ValidateGC)(pGC, pGC->stateChanges, dst);

    (*pGC->ops->AlphaCopyArea)(
        (DrawablePtr)front,
        (DrawablePtr)dst,
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

    return 0;
}
#endif /* EGL_DO_ALPHA_BLENDING */

#define DRAWABLE(win) ((DrawableRec*)((AltiaDevInfo*)win)->drawable)

/***************************************************************************/
EGL_STATUS egl_Line(ALTIA_WINDOW win, EGL_GC_ID gc, 
                    EGL_POS x1, EGL_POS y1, EGL_POS x2, EGL_POS y2)
{
    DDXPointRec pts[2];
    GCPtr pGC = (GCPtr)gc;

    pts[0].x = x1;
    pts[0].y = y1;
    pts[1].x = x2;
    pts[1].y = y2;

#if EGL_DO_ALPHA_BLENDING
    if(pGC->alpha == EGL_TRANSPARENT_ALPHA)
    {
        return EGL_STATUS_OK;
    }

    if(pGC->alpha != EGL_OPAQUE_ALPHA &&
        (((DrawablePtr)DRAWABLE(win))->type == DRAWABLE_PIXMAP ||
        ((DrawablePtr)DRAWABLE(win))->type == DRAWABLE_BUFFER))
    {
        if(_DrawAlphaLines(DRAWABLE(win), pGC, 2, (EGL_POS *)&pts) == -1)
        {
            return (EGL_STATUS) -1;
        }
    }
    else
#endif /* EGL_DO_ALPHA_BLENDING */
    {
        (*pGC->funcs->ValidateGC)(
            pGC, pGC->stateChanges, DRAWABLE(win));

        (*pGC->ops->Polylines)(DRAWABLE(win), pGC, CoordModeOrigin, 2, pts);
    }

    return EGL_STATUS_OK;
}
#if EGL_HAS_LINES
/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_Lines(EGL_GC_ID gc, EGL_ORD numPoints, EGL_POS *data)
 *
 * If "egl_Wrapper.h" defines EGL_HAS_LINES as non-zero (1), it means that
 * the target graphics library can draw connected line segments with a single
 * call and the Altia code will call egl_Lines() to do this whenever possible.
 * If EGL_HAS_LINES is defined as 0 in "egl_Wrapper.h", the Altia code will
 * simply call egl_Line() multiple times.  This may not look very good for
 * thick lines if the line end cap style on the target is too severe (e.g.,
 * squared corners).
 *
 * The line segments are drawn using the data array of values as a sequence
 * of coordinate pairs.  For example, data[0] with data[1] gives the x,y
 * starting point for the line and data[(numPoints - 1) * 2] with
 * data[((numPoints - 1) * 2) + 1] gives the x,y ending point for the
 * line.
 *
 * The line is drawn onto the bitmap for the given gc as previously
 * set by egl_DefaultBitmapSet().  If the bitmap was never set, then
 * the line is drawn directly to the display.
 *
 * The color of the line is determined by the current foreground color
 * for the gc.  The style of the line is determined by the current line
 * style for the gc.  The width of the line is determined by the current
 * line width for the gc.
 *
 * To draw a line as completely transparent (i.e., really don't draw it
 * at all), the current foreground color should be EGL_COLOR_TRANSPARENT
 * and the current line width should be 0.
 *
 * Any portion of the line that falls outside of the gc's current clipping
 * rectangle is not drawn.  
 *
 * The current raster mode for the gc determines how the foreground color
 * is applied to the pixels affected by the line.  Depending on the raster
 * mode setting, the color replaces the current colors of the pixels, is
 * ANDed with the current colors, or is ORed with the current colors.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_Lines(ALTIA_WINDOW win, EGL_GC_ID gc, 
                     EGL_ORD numPoints, EGL_POS *data)
{
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
        if(_DrawAlphaLines(DRAWABLE(win), pGC, numPoints, data) == -1)
        {
            return (EGL_STATUS) -1;
        }
    }
    else
#endif /* EGL_DO_ALPHA_BLENDING */
    {
        xPoint *xPtr = (xPoint *)data;
    
        local_GcAttributesSet(win, pGC, 0 /* not filled */, 0 /* not text */);

        (*pGC->ops->Polylines)(DRAWABLE(win), pGC, CoordModeOrigin, numPoints,
            xPtr);
    }

    return EGL_STATUS_OK;
}
#endif /* EGL_HAS_LINES */
