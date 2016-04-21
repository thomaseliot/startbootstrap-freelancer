/* $Revision: 1.18 $    $Date: 2009-05-06 00:04:10 $
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

/** FILE:  altiaDraw.c ******************************************************
 **
 ** This file contains many of the routines that the generic Altia code
 ** calls to do drawing for vector objects.  Other related routines
 ** are in this file as well.  The routines in this file may do target
 ** graphics library calls to set up clipping rectangles, foreground and
 ** background colors, patterns, and line widths before calling other
 ** routines deeper in the Altia target specific code to make the real
 ** drawing calls.
 **
 ** Code is conditionally generated for this file based on the objects
 ** in the design at code generation time.  This file, however, will never
 ** be totally empty because some of the routines are needed for drawing
 ** any kind of object.
 ***************************************************************************/



#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "../altiaData.h"
#include "altiaLibDraw.h"
#include "egl_Wrapper.h"
#ifndef ALTIAFIXPOINT
#include <math.h>
#endif

/* Windows only */
#if defined(WIN32) && !defined(MICROWIN)
#pragma warning( disable: 4244 )
#endif

#ifdef WIN32
#undef y0
#undef y1
#endif

#define ANGLEINC 1




extern void _altiaLibLineDraw(
    ALTIA_WINDOW,
    ALTIA_UINT32,
    Altia_GraphicState_type *,
    ALTIA_CONST Altia_Coord_type *,
    ALTIA_SHORT,
    Altia_Transform_type *
);






extern void _altiaLibRectDraw(
    ALTIA_WINDOW,
    ALTIA_UINT32,
    Altia_GraphicState_type *,
    ALTIA_CONST Altia_Rect_type *,
    ALTIA_BYTE,
    Altia_Transform_type *
);









extern EGL_COLOR altiaLibGetColor(
    ALTIA_WINDOW devinfo,
    ALTIA_COLOR pixel
);







/*--- altiaLibLineDraw() --------------------------------------------------
 *
 * Function called from other Altia code to draw a line which can be a
 * a single straight line or a set of connected line segments.  This
 * function determines if the line is solid or dashed and calls other
 * target specific Altia code to complete the drawing of the line.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibLineDraw(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                      Altia_GraphicState_type *gs,
                      ALTIA_CONST Altia_Coord_type *coords,
                      ALTIA_SHORT count, Altia_Transform_type *total)
{
    int nodash = 1;

    

    if (nodash)
        _altiaLibLineDraw(win, dc, gs, coords, count, total);
    
}












/*--- altiaLibRectDraw() --------------------------------------------------
 *
 * Function called from generic Altia code to draw a rectangle.  This
 * function determines if the rectangle is filled, solid outlined, or
 * dashed outlined and calls other target specific Altia code to complete
 * the drawing of the rectangle.  Squares are drawn as rectangles.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibRectDraw(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                      Altia_GraphicState_type *gs,
                      ALTIA_CONST Altia_Rect_type *rect,
                      ALTIA_BYTE filled, Altia_Transform_type *total)
{
    int nodash = 1;

    

    if (nodash || filled)
        _altiaLibRectDraw(win, dc, gs, rect, filled, total);
    
}












/*--- altiaLibSetFillDC() -------------------------------------------------
 *
 * Function called from target specific Altia code to set up the clip area,
 * foreground/background colors, and pattern for drawing a filled area.
 * This normally requires calls to the target graphics library.
 *-------------------------------------------------------------------------*/
void altiaLibSetFillDC(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                       Altia_GraphicState_type *gs,
                       ALTIA_COORD winMaxY,
                       ALTIA_UINT32 *holdBrush, ALTIA_UINT32 *holdPen)
{
    Altia_Pattern_type *apat = NULL;
    EGL_MDDB_ID newPattern = NULL;
    EGL_COLOR fillColor;
    EGL_COLOR lineColor;

    if (gs->clip.x0 <= gs->clip.x1 && gs->clip.y0 <= gs->clip.y1)
    { /* Clipping */

	/* On this target, clip rect is inclusive of the rectangle's
	 * coordinates.
	 */
	egl_ClipRectSet((EGL_GC_ID) dc,
	                (EGL_POS) gs->clip.x0,
	                (EGL_POS) (winMaxY - gs->clip.y1),
	                (EGL_POS) gs->clip.x1,
	                (EGL_POS) (winMaxY - gs->clip.y0)
	               );
    }


    apat = _altiaGetPatternEntry(gs->pattern);


   /* We have to fill some type of vector object so the current
    * pattern is of interest to us if we have one.  If not, apat
    * will simply be NULL at this point and newPattern will also
    * be NULL.
    */
    if (apat != NULL)
    {
	if (apat->patType == SOLID_PATTERN)
	{
	    /* A solid pattern is one that is rendered in the current
	     * foreground color for the object (the lowest-left pattern
	     * in the Altia editor's pattern palette).  In this case,
	     * we leave newPattern set to NULL for this target which
	     * will give us a solid pattern if we have a background color
	     * set.
	     */
	    fillColor = altiaLibGetColor(win,
	                      EGL_ADD_ALPHA(gs->foreground, gs->alpha));

	    /* Set the line color to nothing in case this target still
	     * draws an outline for filled objects even with the
	     * line width set to 0.
	     */
	    lineColor = EGL_COLOR_TRANSPARENT;
	}
	else if (apat->patType == CLEAR_PATTERN || apat->id == 0)
	{
	    /* A clear pattern is one that is rendered in the current
	     * background color for the object and usually there will
	     * be an outline around the object rendered in the current
	     * foreground color. This is the pattern directly to the
	     * right of the lowest-left pattern in the Altia editor's
	     * pattern palette.  In this case, we leave newPattern set
	     * to NULL for this target which will give us a solid
	     * pattern if we have a background color set.
	     */
	    fillColor = altiaLibGetColor(win,
	                      EGL_ADD_ALPHA(gs->background, gs->alpha));

	    /* Set the line color to nothing in case this target still
	     * draws an outline for filled objects even with the
	     * line width set to 0.
	     */
	    lineColor = EGL_COLOR_TRANSPARENT;
	}
	else
	{
	    /* We have a bitmap pattern (a stripe, dithered or
	     * checkered pattern) that must be rendered using a
	     * device dependent bitmap DDB on this target.  The
	     * DDB was created at initialization time when the
	     * generic Altia code called altiaLibLoadPattern()
	     * which is elsewhere in the Altia target specific code.
	     * The foreground and background color settings will
	     * determine the colors of the pattern.
	     */
	    newPattern = (EGL_MDDB_ID) apat->id;

	    fillColor = altiaLibGetColor(win,
	                      EGL_ADD_ALPHA(gs->background, gs->alpha));
            if (gs->background == gs->foreground)
		lineColor = fillColor;
            else
		lineColor = altiaLibGetColor(win,
	                      EGL_ADD_ALPHA(gs->foreground, gs->alpha));
	}
    }
    else
    {
	/* No pattern says to draw a filled object using the foreground
	 * color for the object.
	 */
	fillColor = altiaLibGetColor(win,
	                      EGL_ADD_ALPHA(gs->foreground, gs->alpha));

	/* Set the line color to nothing in case this target still
	 * draws an outline for filled objects even with the
	 * line width set to 0.
	 */
	lineColor = EGL_COLOR_TRANSPARENT;
    }

    /* Set the pattern which can still be NULL at this point.  That
     * indicates a solid pattern on this target.
     */
    egl_FillPatternSet((EGL_GC_ID) dc, newPattern);

    /* For drawing, set the graphics library's foreground color to
     * the computed line color (which may still be nothing if we are
     * drawing a solid pattern) and the background color to the
     * computed fill color.
     */
    egl_ForegroundColorSet((EGL_GC_ID) dc, lineColor);
    egl_BackgroundColorSet((EGL_GC_ID) dc, fillColor);

    egl_AlphaChannelSet((EGL_GC_ID) dc, gs->alpha);
}




