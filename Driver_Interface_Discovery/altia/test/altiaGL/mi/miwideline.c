/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Xorg: miwideline.c,v 1.4 2001/02/09 02:05:22 xorgcvs Exp $ */
/*

Copyright 1988, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/
/* $XFree86: xc/programs/Xserver/mi/miwideline.c,v 1.12 2001/12/14 20:00:28 dawes Exp $ */

/* Author:  Keith Packard, MIT X Consortium */

/*
 * Mostly integer wideline code.  Uses a technique similar to
 * bresenham zero-width lines, except walks an X edge
 */

#include <stdio.h>
#ifdef _XOPEN_SOURCE
#include <math.h>
#else
#define _XOPEN_SOURCE	/* to get prototype for hypot on some systems */
#include <math.h>
#undef _XOPEN_SOURCE
#endif
#include "X.h"
#include "pixmapstr.h"
#include "gcstruct.h"
#include "miscstruct.h"
#include "miwideline.h"
#include "mi.h"

#ifdef WIN32
#if defined(UNDER_CE) || (defined(_MSC_VER) && (_MSC_VER >= 1600))
/* If WinCE or VS 2010 or newer, hypot is _hypot */
#define hypot _hypot
#endif
#endif

#ifdef ICEILTEMPDECL
ICEILTEMPDECL
#endif

/*
 * spans-based polygon filler
 */

void
miFillPolyHelper (pDrawable, pGC, pixel, spanData, y, overall_height,
		  left, right, left_count, right_count)
    DrawablePtr	pDrawable;
    GCPtr	pGC;
    unsigned long   pixel;
    SpanDataPtr	spanData;
    int		y;			/* start y coordinate */
    int		overall_height;		/* height of entire segment */
    PolyEdgePtr	left, right;
    int		left_count, right_count;
{
    register int left_x = 0, left_e = 0;
    int	left_stepx = 0;
    int	left_signdx = 0;
    int	left_dy = 0, left_dx = 0;

    register int right_x = 0, right_e = 0;
    int	right_stepx = 0;
    int	right_signdx = 0;
    int	right_dy = 0, right_dx = 0;

    int	height = 0;
    int	left_height = 0, right_height = 0;

    register DDXPointPtr ppt;
    DDXPointPtr pptInit = NULL;
    register int *pwidth;
    int *pwidthInit = NULL;
    XID		oldPixel;
    int		xorg;
    Spans	spanRec;

    left_height = 0;
    right_height = 0;
    
    if (!spanData)
    {
    	pptInit = (DDXPointPtr) ALLOCATE_LOCAL (overall_height * sizeof(*ppt));
    	if (!pptInit)
	    return;
    	pwidthInit = (int *) ALLOCATE_LOCAL (overall_height * sizeof(*pwidth));
    	if (!pwidthInit)
    	{
	    DEALLOCATE_LOCAL (pptInit);
	    return;
    	}
	ppt = pptInit;
	pwidth = pwidthInit;
    	oldPixel = pGC->fgPixel;
    	if (pixel != oldPixel)
    	{
    	    DoChangeGC (pGC, GCForeground, (XID *)&pixel, FALSE);
    	    ValidateGC (pDrawable, pGC);
    	}
    }
    else
    {
	spanRec.points = (DDXPointPtr) xalloc (overall_height * sizeof (*ppt));
	if (!spanRec.points)
	    return;
	spanRec.widths = (int *) xalloc (overall_height * sizeof (int));
	if (!spanRec.widths)
	{
	    xfree (spanRec.points);
	    return;
	}
	ppt = spanRec.points;
	pwidth = spanRec.widths;
    }

    xorg = 0;
    if (pGC->miTranslate)
    {
	y += pDrawable->y;
	xorg = pDrawable->x;
    }
    while ((left_count || left_height) &&
	   (right_count || right_height))
    {
	MIPOLYRELOADLEFT
	MIPOLYRELOADRIGHT

	height = left_height;
	if (height > right_height)
	    height = right_height;

	left_height -= height;
	right_height -= height;

	while (--height >= 0)
	{
	    if (right_x >= left_x)
	    {
		ppt->y = y;
		ppt->x = left_x + xorg;
		ppt++;
		*pwidth++ = right_x - left_x + 1;
	    }
    	    y++;
    	
	    MIPOLYSTEPLEFT

	    MIPOLYSTEPRIGHT
	}
    }
    if (!spanData)
    {
    	(*pGC->ops->FillSpans) (pDrawable, pGC, ppt - pptInit, pptInit, pwidthInit, TRUE);
    	DEALLOCATE_LOCAL (pwidthInit);
    	DEALLOCATE_LOCAL (pptInit);
    	if (pixel != oldPixel)
    	{
	    DoChangeGC (pGC, GCForeground, &oldPixel, FALSE);
	    ValidateGC (pDrawable, pGC);
    	}
    }
    else
    {
	spanRec.count = ppt - spanRec.points;
	AppendSpanGroup (pGC, pixel, &spanRec, spanData)
    }
}

static void
miFillRectPolyHelper (
    DrawablePtr	pDrawable,
    GCPtr	pGC,
    unsigned long   pixel,
    SpanDataPtr	spanData,
    int		x,
    int		y,
    int		w,
    int		h)
{
    register DDXPointPtr ppt;
    register int *pwidth;
    XID		oldPixel;
    Spans	spanRec;
    xRectangle  rect;

    if (!spanData)
    {
	rect.x = x;
	rect.y = y;
	rect.width = w;
	rect.height = h;
    	oldPixel = pGC->fgPixel;
    	if (pixel != oldPixel)
    	{
    	    DoChangeGC (pGC, GCForeground, (XID *)&pixel, FALSE);
    	    ValidateGC (pDrawable, pGC);
    	}
	(*pGC->ops->PolyFillRect) (pDrawable, pGC, 1, &rect);
    	if (pixel != oldPixel)
    	{
	    DoChangeGC (pGC, GCForeground, &oldPixel, FALSE);
	    ValidateGC (pDrawable, pGC);
    	}
    }
    else
    {
	spanRec.points = (DDXPointPtr) xalloc (h * sizeof (*ppt));
	if (!spanRec.points)
	    return;
	spanRec.widths = (int *) xalloc (h * sizeof (int));
	if (!spanRec.widths)
	{
	    xfree (spanRec.points);
	    return;
	}
	ppt = spanRec.points;
	pwidth = spanRec.widths;

    	if (pGC->miTranslate)
    	{
	    y += pDrawable->y;
	    x += pDrawable->x;
    	}
	while (h--)
	{
	    ppt->x = x;
	    ppt->y = y;
	    ppt++;
	    *pwidth++ = w;
	    y++;
	}
	spanRec.count = ppt - spanRec.points;
	AppendSpanGroup (pGC, pixel, &spanRec, spanData)
    }
}

/* static */ int
miPolyBuildEdge (x0, y0, k, dx, dy, xi, yi, left, edge)
    double	x0, y0;
    double	k;  /* x0 * dy - y0 * dx */
    register int dx, dy;
    int		xi, yi;
    int		left;
    register PolyEdgePtr edge;
{
    int	    x, y, e;
    int	    xady;

    if (dy < 0)
    {
	dy = -dy;
	dx = -dx;
	k = -k;
    }

#ifdef NOTDEF
    {
	double	realk, kerror;
    	realk = x0 * dy - y0 * dx;
    	kerror = Fabs (realk - k);
    	if (kerror > .1)
	    printf ("realk: %g k: %g\n", realk, k);
    }
#endif
    y = ICEIL (y0);
    xady = ICEIL (k) + y * dx;

    if (xady <= 0)
	x = - (-xady / dy) - 1;
    else
	x = (xady - 1) / dy;

    e = xady - x * dy;

    if (dx >= 0)
    {
	edge->signdx = 1;
	edge->stepx = dx / dy;
	edge->dx = dx % dy;
    }
    else
    {
	edge->signdx = -1;
	edge->stepx = - (-dx / dy);
	edge->dx = -dx % dy;
	e = dy - e + 1;
    }
    edge->dy = dy;
    edge->x = x + left + xi;
    edge->e = e - dy;	/* bias to compare against 0 instead of dy */
    return y + yi;
}

#define StepAround(v, incr, max) (((v) + (incr) < 0) ? (max - 1) : ((v) + (incr) == max) ? 0 : ((v) + (incr)))

/* static */ int
miPolyBuildPoly (vertices, slopes, count, xi, yi, left, right, pnleft, pnright, h)
    register PolyVertexPtr vertices;
    register PolySlopePtr  slopes;
    int		    count;
    int		    xi, yi;
    PolyEdgePtr	    left, right;
    int		    *pnleft, *pnright;
    int		    *h;
{
    int	    top, bottom;
    double  miny, maxy;
    register int i;
    int	    j;
    int	    clockwise;
    int	    slopeoff;
    register int s;
    register int nright, nleft;
    int	    y, lasty = 0, bottomy, topy = 0;

    /* find the top of the polygon */
    maxy = miny = vertices[0].y;
    bottom = top = 0;
    for (i = 1; i < count; i++)
    {
	if (vertices[i].y < miny)
	{
	    top = i;
	    miny = vertices[i].y;
	}
	if (vertices[i].y >= maxy)
	{
	    bottom = i;
	    maxy = vertices[i].y;
	}
    }
    clockwise = 1;
    slopeoff = 0;

    i = top;
    j = StepAround (top, -1, count);

    if (slopes[j].dy * slopes[i].dx > slopes[i].dy * slopes[j].dx)
    {
	clockwise = -1;
	slopeoff = -1;
    }

    bottomy = ICEIL (maxy) + yi;

    nright = 0;

    s = StepAround (top, slopeoff, count);
    i = top;
    while (i != bottom)
    {
	if (slopes[s].dy != 0)
	{
	    y = miPolyBuildEdge (vertices[i].x, vertices[i].y,
			slopes[s].k,
			slopes[s].dx, slopes[s].dy,
			xi, yi, 0,
			&right[nright]);
	    if (nright != 0)
	    	right[nright-1].height = y - lasty;
	    else
	    	topy = y;
	    nright++;
	    lasty = y;
	}

	i = StepAround (i, clockwise, count);
	s = StepAround (s, clockwise, count);
    }
    if (nright != 0)
	right[nright-1].height = bottomy - lasty;

    if (slopeoff == 0)
	slopeoff = -1;
    else
	slopeoff = 0;

    nleft = 0;
    s = StepAround (top, slopeoff, count);
    i = top;
    while (i != bottom)
    {
	if (slopes[s].dy != 0)
	{
	    y = miPolyBuildEdge (vertices[i].x, vertices[i].y,
			   slopes[s].k,
		       	   slopes[s].dx,  slopes[s].dy, xi, yi, 1,
		       	   &left[nleft]);
    
	    if (nleft != 0)
	    	left[nleft-1].height = y - lasty;
	    nleft++;
	    lasty = y;
	}
	i = StepAround (i, -clockwise, count);
	s = StepAround (s, -clockwise, count);
    }
    if (nleft != 0)
	left[nleft-1].height = bottomy - lasty;
    *pnleft = nleft;
    *pnright = nright;
    *h = bottomy - topy;
    /* Trb Added check for less then zero since it can cause a crash */
    if (*h < 0)
        *h = 0;
    return topy;
}

static void
miLineOnePoint (
    DrawablePtr	    pDrawable,
    GCPtr	    pGC,
    unsigned long   pixel,
    SpanDataPtr	    spanData,
    int		    x,
    int		    y)
{
    DDXPointRec pt;
    int	    wid;
    unsigned long	oldPixel;

    MILINESETPIXEL (pDrawable, pGC, pixel, oldPixel);
    if (pGC->fillStyle == FillSolid)
    {
	pt.x = x;
	pt.y = y;
	(*pGC->ops->PolyPoint) (pDrawable, pGC, CoordModeOrigin, 1, &pt);
    }
    else
    {
	wid = 1;
	if (pGC->miTranslate) 
	{
	    x += pDrawable->x;
	    y += pDrawable->y;
	}
	pt.x = x;
	pt.y = y;
	(*pGC->ops->FillSpans) (pDrawable, pGC, 1, &pt, &wid, TRUE);
    }
    MILINERESETPIXEL (pDrawable, pGC, pixel, oldPixel);
}

static void
miLineJoin (
    DrawablePtr	    pDrawable,
    GCPtr	    pGC,
    unsigned long   pixel,
    SpanDataPtr	    spanData,
    register LineFacePtr pLeft,
    register LineFacePtr pRight)
{
    double	    mx = 0, my = 0;
    double	    denom = ALTIA_C2D(0.0);
    PolyVertexRec   vertices[4];
    PolySlopeRec    slopes[4];
    int		    edgecount;
    PolyEdgeRec	    left[4], right[4];
    int		    nleft, nright;
    int		    y, height;
    int		    swapslopes;
    int		    joinStyle = pGC->joinStyle;
    int		    lw = pGC->lineWidth;

    if (lw == 1 && !spanData) {
	/* See if one of the lines will draw the joining pixel */
	if (pLeft->dx > 0 || (pLeft->dx == 0 && pLeft->dy > 0))
	    return;
	if (pRight->dx > 0 || (pRight->dx == 0 && pRight->dy > 0))
	    return;
	if (joinStyle != JoinRound) {
	    /*
    	    denom = - pLeft->dx * (double)pRight->dy + pRight->dx * (double)pLeft->dy;
	    */
    	    denom = ALTIA_I2D(-pLeft->dx * pRight->dy + pRight->dx * pLeft->dy);
	    if (denom == ALTIA_C2D(0.0))
	    	return;	/* no join to draw */
	}
	if (joinStyle != JoinMiter) {
	    miLineOnePoint (pDrawable, pGC, pixel, spanData, pLeft->x, pLeft->y);
	    return;
	}
    } else {
    	if (joinStyle == JoinRound)
    	{
	    /*
	    miLineArc(pDrawable, pGC, pixel, spanData,
		      pLeft, pRight,
		      (double)0.0, (double)0.0, TRUE);
	    */
	    return;
    	}
	/*
    	denom = - pLeft->dx * (double)pRight->dy + pRight->dx * (double)pLeft->dy;
	*/
    	denom =  ALTIA_I2D(-pLeft->dx * pRight->dy + pRight->dx * pLeft->dy);
    	if (denom == ALTIA_C2D(0.0))
	    return;	/* no join to draw */
    }

    swapslopes = 0;
    if (denom > 0)
    {
	pLeft->xa = -pLeft->xa;
	pLeft->ya = -pLeft->ya;
	pLeft->dx = -pLeft->dx;
	pLeft->dy = -pLeft->dy;
    }
    else
    {
	swapslopes = 1;
	pRight->xa = -pRight->xa;
	pRight->ya = -pRight->ya;
	pRight->dx = -pRight->dx;
	pRight->dy = -pRight->dy;
    }

    vertices[0].x = pRight->xa;
    vertices[0].y = pRight->ya;
    slopes[0].dx = -pRight->dy;
    slopes[0].dy =  pRight->dx;
    slopes[0].k = 0;

    vertices[1].x = 0;
    vertices[1].y = 0;
    slopes[1].dx =  pLeft->dy;
    slopes[1].dy = -pLeft->dx;
    slopes[1].k = 0;

    vertices[2].x = pLeft->xa;
    vertices[2].y = pLeft->ya;

    if (joinStyle == JoinMiter)
    {


	my = FDIV(FMULT(ALTIA_I2D(pLeft->dy), FMULT(pRight->xa, 
	              ALTIA_I2D(pRight->dy))
	             - FMULT(pRight->ya, ALTIA_I2D(pRight->dx)))
	     - FMULT(ALTIA_I2D(pRight->dy), FMULT(pLeft->xa, 
	              ALTIA_I2D(pLeft->dy))
	             - FMULT(pLeft->ya, ALTIA_I2D(pLeft->dx))),denom);


    	if (pLeft->dy != 0)
    	{


	    mx = pLeft->xa + FMULT(FDIV(my - pLeft->ya, ALTIA_I2D(pLeft->dy)),
	         ALTIA_I2D(pLeft->dx));

    	}
    	else
    	{


	    mx = pRight->xa + FMULT(FDIV(my - pRight->ya, 
	         ALTIA_I2D(pRight->dy)),
	         ALTIA_I2D(pRight->dx));

    	}
	/* check miter limit */


	if (FMULT(FMULT(mx,mx) + FMULT(my,my),ALTIA_C2D(4)) > 
	     (FMULT(FMULT(ALTIA_C2D(SQSECANT), ALTIA_I2D(lw)),ALTIA_I2D(lw))))


	    joinStyle = JoinBevel;
    }

    if (joinStyle == JoinMiter)
    {
	slopes[2].dx = pLeft->dx;
	slopes[2].dy = pLeft->dy;
	slopes[2].k =  pLeft->k;
	if (swapslopes)
	{
	    slopes[2].dx = -slopes[2].dx;
	    slopes[2].dy = -slopes[2].dy;
	    slopes[2].k  = -slopes[2].k;
	}
	vertices[3].x = mx;
	vertices[3].y = my;
	slopes[3].dx = pRight->dx;
	slopes[3].dy = pRight->dy;
	slopes[3].k  = pRight->k;
	if (swapslopes)
	{
	    slopes[3].dx = -slopes[3].dx;
	    slopes[3].dy = -slopes[3].dy;
	    slopes[3].k  = -slopes[3].k;
	}
	edgecount = 4;
    }
    else
    {
	double	scale, dx, dy, adx, ady;

	adx = dx = pRight->xa - pLeft->xa;
	ady = dy = pRight->ya - pLeft->ya;
	if (adx < 0)
	    adx = -adx;
	if (ady < 0)
	    ady = -ady;
	scale = ady;
	if (adx > ady)
	    scale = adx;


	slopes[2].k = FMULT(pLeft->xa + pRight->xa, dy) -
		      FMULT(pLeft->ya + pRight->ya, dx);
	slopes[2].k = FMULT(slopes[2].k, FDIV(32768, scale));
	slopes[2].dx = ALTIA_D2I(FMULT(FDIV(dx, scale),ALTIA_C2D(65536)));
	slopes[2].dy = ALTIA_D2I(FMULT(FDIV(dy, scale),ALTIA_C2D(65536)));

	edgecount = 3;
    }

    y = miPolyBuildPoly (vertices, slopes, edgecount, pLeft->x, pLeft->y,
		   left, right, &nleft, &nright, &height);
    miFillPolyHelper (pDrawable, pGC, pixel, spanData, y, height, left, right, nleft, nright);
}

#define CLIPSTEPEDGE(edgey,edge,edgeleft) \
    if (ybase == edgey) \
    { \
	if (edgeleft) \
	{ \
	    if (edge->x > xcl) \
		xcl = edge->x; \
	} \
	else \
	{ \
	    if (edge->x < xcr) \
		xcr = edge->x; \
	} \
	edgey++; \
	edge->x += edge->stepx; \
	edge->e += edge->dx; \
	if (edge->e > 0) \
	{ \
	    edge->x += edge->signdx; \
	    edge->e -= edge->dy; \
	} \
    }


static void
miWideSegment (
    DrawablePtr	    pDrawable,
    GCPtr	    pGC,
    unsigned long   pixel,
    SpanDataPtr	    spanData,
    register int    x1,
    register int    y1,
    register int    x2,
    register int    y2,
    Bool	    projectLeft,
    Bool	    projectRight,
    register LineFacePtr leftFace,
    register LineFacePtr rightFace)
{
    double	l, L, r;
    double	xa, ya;
    double	projectXoff = ALTIA_C2D(0.0), projectYoff = ALTIA_C2D(0.0);
    double	k;
    double	maxy;
    int		x, y;
    int		dx, dy;
    int		finaly;
    PolyEdgePtr left, right;
    PolyEdgePtr	top, bottom;
    int		lefty, righty, topy, bottomy;
    int		signdx;
    PolyEdgeRec	lefts[2], rights[2];
    LineFacePtr	tface;
    int		lw = pGC->lineWidth;

    /* draw top-to-bottom always */
    if (y2 < y1 || (y2 == y1 && x2 < x1))
    {
	x = x1;
	x1 = x2;
	x2 = x;

	y = y1;
	y1 = y2;
	y2 = y;

	x = projectLeft;
	projectLeft = projectRight;
	projectRight = x;

	tface = leftFace;
	leftFace = rightFace;
	rightFace = tface;
    }

    dy = y2 - y1;
    signdx = 1;
    dx = x2 - x1;
    if (dx < 0)
	signdx = -1;

    leftFace->x = x1;
    leftFace->y = y1;
    leftFace->dx = dx;
    leftFace->dy = dy;

    rightFace->x = x2;
    rightFace->y = y2;
    rightFace->dx = -dx;
    rightFace->dy = -dy;

    if (dy == 0)
    {
	rightFace->xa = 0;
	/*
	rightFace->ya = (double) lw / 2.0;
	*/
	rightFace->ya = FDIV(ALTIA_I2D(lw), ALTIA_C2D(2.0));
	/*
	rightFace->k = -(double) (lw * dx) / 2.0;
	*/
	rightFace->k = - FDIV(ALTIA_I2D(lw * dx), ALTIA_C2D(2.0));
	leftFace->xa = 0;
	leftFace->ya = -rightFace->ya;
	leftFace->k = rightFace->k;
	x = x1;
	if (projectLeft)
	    x -= (lw >> 1);
	y = y1 - (lw >> 1);
	dx = x2 - x;
	if (projectRight)
	    dx += ((lw + 1) >> 1);
	dy = lw;
	miFillRectPolyHelper (pDrawable, pGC, pixel, spanData,
			      x, y, dx, dy);
    }
    else if (dx == 0)
    {
	/*
	leftFace->xa =  (double) lw / 2.0;
	*/
	leftFace->xa = FDIV(ALTIA_I2D(lw), ALTIA_C2D(2.0));
	leftFace->ya = 0;
	/*
	leftFace->k = (double) (lw * dy) / 2.0;
	*/
	leftFace->k = FDIV(ALTIA_I2D(lw * dy), ALTIA_C2D(2.0));
	rightFace->xa = -leftFace->xa;
	rightFace->ya = 0;
	rightFace->k = leftFace->k;
	y = y1;
	if (projectLeft)
	    y -= lw >> 1;
	x = x1 - (lw >> 1);
	dy = y2 - y;
	if (projectRight)
	    dy += ((lw + 1) >> 1);
	dx = lw;
	miFillRectPolyHelper (pDrawable, pGC, pixel, spanData,
			      x, y, dx, dy);
    }
    else
    {
	/*
    	l = ((double) lw) / 2.0;
	*/
	l = FDIV(ALTIA_I2D(lw), ALTIA_C2D(2.0));
	/*
    	L = hypot ((double) dx, (double) dy);
	*/
	L = FHYPOT(ALTIA_I2D(dx), ALTIA_I2D(dy));

	if (dx < 0)
	{
	    right = &rights[1];
	    left = &lefts[0];
	    top = &rights[0];
	    bottom = &lefts[1];
	}
	else
	{
	    right = &rights[0];
	    left = &lefts[1];
	    top = &lefts[0];
	    bottom = &rights[1];
	}
	r = FDIV(l, L);

	/* coord of upper bound at integral y */
	ya = FMULT(-r,ALTIA_I2D(dx));
	xa = FMULT(r,ALTIA_I2D(dy));

	if (projectLeft | projectRight)
	{
	    projectXoff = -ya;
	    projectYoff = xa;
	}

    	/* xa * dy - ya * dx */
	k = FMULT(l, L);

	leftFace->xa = xa;
	leftFace->ya = ya;
	leftFace->k = k;
	rightFace->xa = -xa;
	rightFace->ya = -ya;
	rightFace->k = k;

	if (projectLeft)
	    righty = miPolyBuildEdge (xa - projectXoff, ya - projectYoff,
				      k, dx, dy, x1, y1, 0, right);
	else
	    righty = miPolyBuildEdge (xa, ya,
				      k, dx, dy, x1, y1, 0, right);

	/* coord of lower bound at integral y */
	ya = -ya;
	xa = -xa;

	/* xa * dy - ya * dx */
	k = - k;

	if (projectLeft)
	    lefty = miPolyBuildEdge (xa - projectXoff, ya - projectYoff,
				     k, dx, dy, x1, y1, 1, left);
	else
	    lefty = miPolyBuildEdge (xa, ya,
				     k, dx, dy, x1, y1, 1, left);

	/* coord of top face at integral y */

	if (signdx > 0)
	{
	    ya = -ya;
	    xa = -xa;
	}

	if (projectLeft)
	{
	    double xap = xa - projectXoff;
	    double yap = ya - projectYoff;


	    topy = miPolyBuildEdge (xap, yap, FMULT(xap, ALTIA_I2D(dx)) + 
	                            FMULT(yap, ALTIA_I2D(dy)),
				    -dy, dx, x1, y1, dx > 0, top);

	}
	else
	    topy = miPolyBuildEdge (xa, ya, ALTIA_C2D(0.0), -dy, dx, x1, 
	                            y1, dx > 0, top);

	/* coord of bottom face at integral y */

	if (projectRight)
	{
	    double xap = xa + projectXoff;
	    double yap = ya + projectYoff;


	    bottomy = miPolyBuildEdge (xap, yap, FMULT(xap, ALTIA_I2D(dx)) + 
	                               FMULT(yap, ALTIA_I2D(dy)),
				       -dy, dx, x2, y2, dx < 0, bottom);

	    maxy = -ya + projectYoff;
	}
	else
	{
	    bottomy = miPolyBuildEdge (xa, ya,
				       ALTIA_C2D(0.0), -dy, dx, x2, 
				       y2, dx < 0, bottom);
	    maxy = -ya;
	}

	finaly = ICEIL (maxy) + y2;

	if (dx < 0)
	{
	    left->height = bottomy - lefty;
	    right->height = finaly - righty;
	    top->height = righty - topy;
	}
	else
	{
	    right->height =  bottomy - righty;
	    left->height = finaly - lefty;
	    top->height = lefty - topy;
	}
	bottom->height = finaly - bottomy;
	miFillPolyHelper (pDrawable, pGC, pixel, spanData, topy,
		     bottom->height + bottomy - topy, lefts, rights, 2, 2);
    }
}

SpanDataPtr
miSetupSpanData (pGC, spanData, npt)
    register GCPtr pGC;
    SpanDataPtr	spanData;
    int		npt;
{
    if ((npt < 3 && pGC->capStyle != CapRound) || miSpansEasyRop(pGC->alu))
	return (SpanDataPtr) NULL;
    if (pGC->lineStyle == LineDoubleDash)
	miInitSpanGroup (&spanData->bgGroup);
    miInitSpanGroup (&spanData->fgGroup);
    return spanData;
}

void
miCleanupSpanData (pDrawable, pGC, spanData)
    DrawablePtr	pDrawable;
    GCPtr	pGC;
    SpanDataPtr	spanData;
{
    if (pGC->lineStyle == LineDoubleDash)
    {
	XID oldPixel, pixel;
	
	pixel = pGC->bgPixel;
	oldPixel = pGC->fgPixel;
    	if (pixel != oldPixel)
    	{
    	    DoChangeGC (pGC, GCForeground, &pixel, FALSE);
    	    ValidateGC (pDrawable, pGC);
    	}
	miFillUniqueSpanGroup (pDrawable, pGC, &spanData->bgGroup);
	miFreeSpanGroup (&spanData->bgGroup);
    	if (pixel != oldPixel)
    	{
	    DoChangeGC (pGC, GCForeground, &oldPixel, FALSE);
	    ValidateGC (pDrawable, pGC);
    	}
    }
    miFillUniqueSpanGroup (pDrawable, pGC, &spanData->fgGroup);
    miFreeSpanGroup (&spanData->fgGroup);
}

void
miWideLine (pDrawable, pGC, mode, npt, pPts)
    DrawablePtr	pDrawable;
    register GCPtr pGC;
    int		mode;
    register int npt;
    register DDXPointPtr pPts;
{
    int		    x1, y1, x2, y2;
    SpanDataRec	    spanDataRec;
    SpanDataPtr	    spanData;
    unsigned long   pixel;
    Bool	    projectLeft, projectRight;
    LineFaceRec	    leftFace, rightFace, prevRightFace;
    LineFaceRec	    firstFace;
    register int    first;
    Bool	    somethingDrawn = FALSE;
    Bool	    selfJoin;

    spanData = miSetupSpanData (pGC, &spanDataRec, npt);
    pixel = pGC->fgPixel;
    x2 = pPts->x;
    y2 = pPts->y;
    first = TRUE;
    selfJoin = FALSE;
    if (npt > 1)
    {
    	if (mode == CoordModePrevious)
    	{
	    int nptTmp;
	    DDXPointPtr pPtsTmp;
    
	    x1 = x2;
	    y1 = y2;
	    nptTmp = npt;
	    pPtsTmp = pPts + 1;
	    while (--nptTmp)
	    {
	    	x1 += pPtsTmp->x;
	    	y1 += pPtsTmp->y;
	    	++pPtsTmp;
	    }
	    if (x2 == x1 && y2 == y1)
	    	selfJoin = TRUE;
    	}
    	else if (x2 == pPts[npt-1].x && y2 == pPts[npt-1].y)
    	{
	    selfJoin = TRUE;
    	}
    }
    projectLeft = pGC->capStyle == CapProjecting && !selfJoin;
    projectRight = FALSE;
    while (--npt)
    {
	x1 = x2;
	y1 = y2;
	++pPts;
	x2 = pPts->x;
	y2 = pPts->y;
	if (mode == CoordModePrevious)
	{
	    x2 += x1;
	    y2 += y1;
	}
	if (x1 != x2 || y1 != y2)
	{
	    somethingDrawn = TRUE;
	    if (npt == 1 && pGC->capStyle == CapProjecting && !selfJoin)
	    	projectRight = TRUE;
	    miWideSegment (pDrawable, pGC, pixel, spanData, x1, y1, x2, y2,
		       	   projectLeft, projectRight, &leftFace, &rightFace);
	    if (first)
	    {
	    	if (selfJoin)
		    firstFace = leftFace;
	    	else if (pGC->capStyle == CapRound)
		{
		    if (pGC->lineWidth == 1 && !spanData)
			miLineOnePoint (pDrawable, pGC, pixel, spanData, x1, y1);
			/*
		    else
		    	miLineArc (pDrawable, pGC, pixel, spanData,
			       	   &leftFace, (LineFacePtr) NULL,
 			       	   (double)0.0, (double)0.0,
			       	   TRUE);
                        */
		}
	    }
	    else
	    {
	    	miLineJoin (pDrawable, pGC, pixel, spanData, &leftFace,
		            &prevRightFace);
	    }
	    prevRightFace = rightFace;
	    first = FALSE;
	    projectLeft = FALSE;
	}
	if (npt == 1 && somethingDrawn)
 	{
	    if (selfJoin)
		miLineJoin (pDrawable, pGC, pixel, spanData, &firstFace,
			    &rightFace);
	    else if (pGC->capStyle == CapRound)
	    {
		if (pGC->lineWidth == 1 && !spanData)
		    miLineOnePoint (pDrawable, pGC, pixel, spanData, x2, y2);
		    /*
		else
		    miLineArc (pDrawable, pGC, pixel, spanData,
			       (LineFacePtr) NULL, &rightFace,
			       (double)0.0, (double)0.0,
			       TRUE);
	             */
	    }
	}
    }
    /* handle crock where all points are coincedent */
    if (!somethingDrawn)
    {
	projectLeft = pGC->capStyle == CapProjecting;
	miWideSegment (pDrawable, pGC, pixel, spanData,
		       x2, y2, x2, y2, projectLeft, projectLeft,
		       &leftFace, &rightFace);
	/*
	if (pGC->capStyle == CapRound)
	{
	    miLineArc (pDrawable, pGC, pixel, spanData,
		       &leftFace, (LineFacePtr) NULL,
		       (double)0.0, (double)0.0,
		       TRUE);
	    rightFace.dx = -1;	
	    miLineArc (pDrawable, pGC, pixel, spanData,
		       (LineFacePtr) NULL, &rightFace,
 		       (double)0.0, (double)0.0,
		       TRUE);
	}
	 */
    }
    if (spanData)
	miCleanupSpanData (pDrawable, pGC, spanData);
}

#define V_TOP	    0
#define V_RIGHT	    1
#define V_BOTTOM    2
#define V_LEFT	    3


/* these are stubs to allow old ddx ValidateGCs to work without change */

void
miMiter()
{
}

void
miNotMiter()
{
}
