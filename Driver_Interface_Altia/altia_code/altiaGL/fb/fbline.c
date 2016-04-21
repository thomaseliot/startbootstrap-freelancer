/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/*
 * Id: fbline.c,v 1.1 1999/11/02 03:54:45 keithp Exp $
 *
 * Copyright � 1998 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/* $XFree86: xc/programs/Xserver/fb/fbline.c,v 1.7 2001/01/17 07:40:02 keithp Exp $ */

#include "fb.h"

#ifndef NO_WIDELINE
#define WIDELINES
#endif

void
fbZeroLine (DrawablePtr	pDrawable,
	    GCPtr	pGC,
	    int		mode,
	    int		npt,
	    DDXPointPtr	ppt)
{
    if (pGC->lineWidth > 1)
    {
        fbPolyLine(pDrawable, pGC, mode, npt, ppt);
    }
    else
    {
        /* FbGCPrivPtr	    pPriv = fbGetGCPrivate (pGC); */
        int		    x1, y1, x2, y2;
        int		    x, y;
        int		    dashOffset;
        /* int		    totalDash; */

        x = pDrawable->x;
        y = pDrawable->y;
        x1 = ppt->x;
        y1 = ppt->y;
        dashOffset = pGC->dashOffset;
        /* 
        totalDash = 0;
        if (pGC->lineStyle != LineSolid)
	    totalDash = pPriv->dashLength;
        */
        while (--npt)
        {
	    ++ppt;
	    x2 = ppt->x;
	    y2 = ppt->y;
	    if (mode == CoordModePrevious)
	    {
	        x2 += x1;
	        y2 += y1;
	    }
	    fbSegment (pDrawable, pGC, x1 + x, y1 + y, 
		       x2 + x, y2 + y, 
		       npt == 1 && pGC->capStyle != CapNotLast,
		       &dashOffset);
	    x1 = x2;
	    y1 = y2;
        }
    }
}

void
fbFixCoordModePrevious (int	    npt,
			DDXPointPtr ppt)
{
    int	    x, y;

    x = ppt->x;
    y = ppt->y;
    npt--;
    while (npt--)
    {
	ppt++;
	x = (ppt->x += x);
	y = (ppt->y += y);
    }
}

void
fbPolyLine (DrawablePtr	pDrawable,
	    GCPtr	pGC,
	    int		mode,
	    int		npt,
	    DDXPointPtr	ppt)
{
    void	(*line) (DrawablePtr, GCPtr, int mode, int npt, DDXPointPtr ppt);
    
    if (pGC->lineWidth == 0)
    {
	line = fbZeroLine;
#ifndef FBNOPIXADDR
	if (pGC->fillStyle == FillSolid &&
	    pGC->lineStyle == LineSolid &&
	    REGION_NUM_RECTS (fbGetCompositeClip(pGC)) == 1)
	{
	    switch (pDrawable->bitsPerPixel) {
#ifdef FB_8BIT
	    case 8:  line = fbPolyline8; break;
#endif
#ifdef FB_16BIT
	    case 16: line = fbPolyline16; break;
#endif
#ifdef FB_24BIT
	    case 24: line = fbPolyline24; break;
#endif
#ifdef FB_32BIT
	    case 32: line = fbPolyline32; break;
#endif
	    }
	}
#endif
    }
    else
    {
#ifdef WIDELINES
	line = miWideLine;
#else
	line = fbZeroLine;
#endif
    }
    (*line) (pDrawable, pGC, mode, npt, ppt);
}
