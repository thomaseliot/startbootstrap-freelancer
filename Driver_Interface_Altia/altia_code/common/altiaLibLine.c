/* $Revision: 1.7 $    $Date: 2009-05-06 00:04:10 $
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

/** FILE:  altiaLibLine.c ***************************************************
 **
 ** This file contains routines to actually draw lines.  This requires
 ** calls to the target graphics library.  Line drawing is required for
 ** lines, line plots, textio, ticks, and splines.  If there are no
 ** such objects in a design, this file contains no code.
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

extern void altiaLibClearDC(
#ifdef Altiafp
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    ALTIA_UINT32 hbrush,
    ALTIA_UINT32 hpen
#endif
);


/*--- _altiaLibLineDraw() -------------------------------------------------
 *
 * Function called from other target specific Altia code to draw a SOLID
 * line.  This normally requires calls to the target graphics library.
 * There can be more than just 2 points specified in which case lines
 * should be drawn between the points from the first point to the last
 * point.  This function only draws SOLID lines.  Drawing dashed
 * lines is handled by routines in other target specific Altia code.
 * It is assumed that if total is NULL, the points are already in the
 * test_altiaDrawPts array so just draw them.
 *-------------------------------------------------------------------------*/
void _altiaLibLineDraw(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                       Altia_GraphicState_type *gs,
                       ALTIA_CONST Altia_Coord_type *coords, ALTIA_SHORT count,
                       Altia_Transform_type *total)
{
    int i,j;
    ALTIA_UINT32 holdBrush = 0;
    ALTIA_UINT32 holdPen = 0;
    Altia_Extent_type wex;
    ALTIA_COORD x, y;
    int posCnt = (count - 1) * 2;

    altiaLibGetFullWindowExtent(win, &wex);
    if (total != NULL)
    {
	for (i = j = 0; i < count; i++, j += 2)
	{
	    altiaLibTransform(total, coords[i].x, coords[i].y, &x, &y);

	    /* On this target, the top of the screen is the origin
	     * (whereas Altia's origin is the bottom) and a line is
	     * inclusive of its points so y is (wex.y1 - y).
	     */
	    test_altiaDrawPts[j] = x;
	    test_altiaDrawPts[j + 1] = wex.y1 - y;
	}
    }

    altiaLibSetOutlineDC(win, dc, gs, wex.y1, &holdBrush, &holdPen);

#if EGL_HAS_LINES
    /* If the target graphics library can draw connected line
     * segments with a single call, make it so.
     */
    if (posCnt > 2)
	egl_Lines(win, (EGL_GC_ID) dc, count, test_altiaDrawPts);
    else if (posCnt == 2)
	egl_Line(win, (EGL_GC_ID) dc,
	         test_altiaDrawPts[0], test_altiaDrawPts[1],
	         test_altiaDrawPts[2], test_altiaDrawPts[3]);

#else /* NOT EGL_HAS_LINES */
    for (i = 0; i < posCnt; i += 2)
	egl_Line(win, (EGL_GC_ID) dc,
	         test_altiaDrawPts[i], test_altiaDrawPts[i + 1],
	         test_altiaDrawPts[i + 2], test_altiaDrawPts[i + 3]);
#endif /* EGL_HAS_LINES */

    altiaLibClearDC(win, dc, holdBrush, holdPen);
}





