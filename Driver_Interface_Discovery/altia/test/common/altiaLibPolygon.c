/* $Revision: 1.6 $    $Date: 2009-05-06 00:04:10 $
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

/** FILE:  altiaLibPolygon.c ***********************************************
 **
 ** This file contains the routines to draw rectangles and polygons
 ** that do NOT have an outline in a dashed line style.  Other routines in
 ** a different source file for this target handle drawing the outline of
 ** rectangles and polygons using a DASHED line style.  This file will
 ** always contain code because the polygon drawing routines are
 ** unconditionally generated.  Polygons are ubiquitous (like the Web!).
 ** Ellipses, ticks, closed splines, filled splines, filled plots, 3d
 ** rectangles, pie charts, and even rectangles (when they are rotated)
 ** need to draw polygons.
 ***************************************************************************/

#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "altiaLibDraw.h"
#include "egl_Wrapper.h"

extern void  altiaLibSetOutlineDC(
#ifdef Altiafp
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_COORD winMaxY,
    ALTIA_UINT32 *holdBrush,
    ALTIA_UINT32 *holdPen
#endif
);

extern void  altiaLibSetFillDC(
#ifdef Altiafp
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_COORD winMaxY,
    ALTIA_UINT32 *holdBrush,
    ALTIA_UINT32 *holdPen
#endif
);

extern void altiaLibClearDC(
#ifdef Altiafp
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    ALTIA_UINT32 hbrush,
    ALTIA_UINT32 hpen
#endif
);


/*--- _altiaLibPolygonDraw() ----------------------------------------------
 *
 * Function called from target specific Altia code to draw the outline of
 * a polygon in a SOLID line style.  Other routines in the target specific
 * Altia code handle drawing polygon outlines in a DASHED line style.
 * We assume that if total is NULL, the points are already in
 * the test_altiaDrawPts array so just draw them.
 *-------------------------------------------------------------------------*/
void _altiaLibPolygonDraw(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                          Altia_GraphicState_type *gs,
                          ALTIA_CONST Altia_Coord_type *coords,
                          ALTIA_SHORT count, Altia_Transform_type *total)
{
    int i, j;
    ALTIA_UINT32 holdBrush = 0;
    ALTIA_UINT32 holdPen = 0;
    Altia_Extent_type wex;
    ALTIA_COORD x, y;

    altiaLibGetFullWindowExtent(win, &wex);
    if (total != NULL)
    {
	for (i = j = 0; i < count; i++, j += 2)
	{
	    altiaLibTransform(total, coords[i].x, coords[i].y, &x, &y);

	    /* On this target, the top of the screen is the origin
	     * (whereas Altia's origin is the bottom) and a polygon is
	     * inclusive of its points so y is (wex.y1 - y).
	     */
	    test_altiaDrawPts[j] = x;
	    test_altiaDrawPts[j + 1] = wex.y1 - y;
	}
    }

    /* On this target, the end point must match the start point to
     * draw a closed polygon.  If necessary, add an end point that
     * matches the start point.
     */
    if ((i = (count - 1) * 2) > 0
        && (test_altiaDrawPts[i] != test_altiaDrawPts[0]
            || test_altiaDrawPts[i + 1] != test_altiaDrawPts[1]))
    {
	test_altiaDrawPts[i + 2] = test_altiaDrawPts[0];
	test_altiaDrawPts[i + 3] = test_altiaDrawPts[1];
	count++;
    }

    altiaLibSetOutlineDC(win, dc, gs, wex.y1, &holdBrush, &holdPen);

    egl_Polygon(win, (EGL_GC_ID) dc, count, test_altiaDrawPts, FALSE);

    altiaLibClearDC(win, dc, holdBrush, holdPen);
}


/*--- altiaLibFillPolygonDraw() -------------------------------------------
 *
 * Function called from other Altia code to draw the filled area of a
 * polygon. We assume that if total is NULL, the points are already in
 * the test_altiaDrawPts array so just draw them.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibFillPolygonDraw(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                             Altia_GraphicState_type *gs,
                             ALTIA_CONST Altia_Coord_type *coords,
                             ALTIA_SHORT count, Altia_Transform_type *total)
{
    int i, j;
    ALTIA_UINT32 holdBrush = 0;
    ALTIA_UINT32 holdPen = 0;
    Altia_Extent_type wex;
    ALTIA_COORD x, y;

    altiaLibGetFullWindowExtent(win, &wex);
    if (total != NULL)
    {
	for (i = j = 0; i < count; i++, j += 2)
	{
	    altiaLibTransform(total, coords[i].x, coords[i].y, &x, &y);

	    /* On this target, the top of the screen is the origin
	     * (whereas Altia's origin is the bottom) and a polygon is
	     * inclusive of its points so y is (wex.y1 - y).
	     */
	    test_altiaDrawPts[j] = x;
	    test_altiaDrawPts[j + 1] = wex.y1 - y;
	}
    }

    /* On this target, the end point must match the start point to
     * draw a closed polygon.  If necessary, add an end point that
     * matches the start point.
     */
    if ((i = (count - 1) * 2) > 0
        && (test_altiaDrawPts[i] != test_altiaDrawPts[0]
            || test_altiaDrawPts[i + 1] != test_altiaDrawPts[1]))
    {
	test_altiaDrawPts[i + 2] = test_altiaDrawPts[0];
	test_altiaDrawPts[i + 3] = test_altiaDrawPts[1];
	count++;
    }

    altiaLibSetFillDC(win, dc, gs, wex.y1, &holdBrush, &holdPen);

    egl_Polygon(win, (EGL_GC_ID) dc, count, test_altiaDrawPts, TRUE);

    altiaLibClearDC(win, dc, holdBrush, holdPen);
}



/*--- _altiaLibRectDraw() -------------------------------------------------
 *
 * Function called from generic and target specific Altia code to draw
 * the filled area of a rectangle or the outline of a rectangle in a SOLID
 * line style.  Other routines in the target specific Altia code handle
 * drawing rectangle outlines in a DASHED line style.  If the rectangle is
 * rotated at an angle not compatible with 90 degrees, this function uses
 * the polygon drawing routines to draw the rectangle.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void _altiaLibRectDraw(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                       Altia_GraphicState_type *gs,
                       ALTIA_CONST Altia_Rect_type *rect,
                       ALTIA_BYTE filled, Altia_Transform_type *total)
{
    Altia_Extent_type wex;
    ALTIA_UINT32 holdBrush = 0;
    ALTIA_UINT32 holdPen = 0;
    EGL_BOOL eglFill = FALSE;

    if (filled)
        eglFill = TRUE;
    altiaLibGetFullWindowExtent(win, &wex);
    if (ALTIA_ROTATED(total)  && !ALTIA_ROTATED90(total))
    {
        Altia_Coord_type coords[4];

	coords[0].x = rect->x0;
	coords[0].y = rect->y0;
	coords[1].x = rect->x1;
	coords[1].y = rect->y0;
	coords[2].x = rect->x1;
	coords[2].y = rect->y1;
	coords[3].x = rect->x0;
	coords[3].y = rect->y1;
	if (filled)
	    altiaLibFillPolygonDraw(win, dc, gs, coords, 4, total);
        else
	    _altiaLibPolygonDraw(win, dc, gs, coords, 4, total);

    }else
    {
        ALTIA_COORD left, bottom, right, top, temp;
	altiaLibTransform(total, rect->x0, rect->y0, &left, &bottom);
	altiaLibTransform(total, rect->x1, rect->y1, &right, &top);

	/* On this target, the top of the screen is the origin
	 * (whereas Altia's origin is the bottom) and a rectangle
	 * is inclusive of its points so bottom is (wex.y1 - bottom)
	 * and top is (wex.y1 - top).
	 */
	bottom = wex.y1 - bottom;
	top = wex.y1 - top;
	if (left > right)
	{
	    temp = left; left = right; right = temp;
	}
	if (top > bottom)
	{
	    temp = bottom; bottom = top; top = temp;
	}
	if (filled)
	{
	    altiaLibSetFillDC(win, dc, gs, wex.y1,
	                      &holdBrush, &holdPen);
	}else
	{
	    altiaLibSetOutlineDC(win, dc, gs, wex.y1,
	                         &holdBrush, &holdPen);
	}

	/* On this target, the rectangle is inclusive thus the right
	 * and bottom boundaries are drawn.
	 */
	egl_Rectangle(win, (EGL_GC_ID) dc, left, top, right, bottom, eglFill, FALSE);

	altiaLibClearDC(win, dc, holdBrush, holdPen);
    }
}

