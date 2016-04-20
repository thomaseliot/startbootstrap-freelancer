/*
 * Id: fbbits.h,v 1.1 1999/11/02 03:54:45 keithp Exp $
 *
 * Copyright © 1998 Keith Packard
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
/* $XFree86: xc/programs/Xserver/fb/fbbits.h,v 1.13 2001/11/18 05:00:25 torrey Exp $ */

/*
 * This file defines functions for drawing some primitives using
 * underlying datatypes instead of masks
 */

#define isClipped(c,ul,lr)  ((((c) - (ul)) | ((lr) - (c))) & 0x80008000)

#ifdef BITSMUL
#define MUL BITSMUL
#else
#define MUL 1
#endif

#ifdef BITSSTORE
#define STORE(b,x)  BITSSTORE(b,x)
#else
#define STORE(b,x)  (*(b) = (x))
#endif

#ifdef BITSRROP
#define RROP(b,a,x)	BITSRROP(b,a,x)
#else
#define RROP(b,a,x)	(*(b) = FbDoRRop (*(b), (a), (x)))
#endif

#ifdef BITSUNIT
#define UNIT BITSUNIT
#define USE_SOLID
#else
#define UNIT BITS
#endif

/*
 * Define the following before including this file:
 *
 *  BRESSOLID	name of function for drawing a solid segment
 *  BRESDASH	name of function for drawing a dashed segment
 *  DOTS	name of function for drawing dots
 *  ARC		name of function for drawing a solid arc
 *  BITS	type of underlying unit
 */

#ifdef BRESSOLID
void
BRESSOLID (DrawablePtr	pDrawable,
	   GCPtr	pGC,
	   int		dashOffset,
	   int		signdx,
	   int		signdy,
	   int		axis,
	   int		x1,
	   int		y1,
	   int		e,
	   int		e1,
	   int		e3,
	   int		len)
{
    FbBits	*dst;
    FbStride	dstStride;
    int		dstBpp;
    int		dstXoff, dstYoff;
    FbGCPrivPtr	pPriv = fbGetGCPrivate (pGC);
    UNIT	*bits;
    FbStride	bitsStride;
    FbStride	majorStep, minorStep;
    BITS	xor = (BITS) pPriv->xor;
    
    fbGetDrawable (pDrawable, dst, dstStride, dstBpp, dstXoff, dstYoff);
    bits = ((UNIT *) (dst + (INT32)((y1 + dstYoff) * (INT32)dstStride))) + (INT32)(x1 + dstXoff) * (INT32)MUL;
    bitsStride = dstStride * (sizeof (FbBits) / sizeof (UNIT));
    if (signdy < 0)
	bitsStride = -bitsStride;
    if (axis == X_AXIS)
    {
	majorStep = signdx * MUL;
	minorStep = bitsStride;
    }
    else
    {
	majorStep = bitsStride;
	minorStep = signdx * MUL;
    }
    while (len--)
    {
	STORE(bits,xor);
	bits += majorStep;
	e += e1;
	if (e >= 0)
	{
	    bits += minorStep;
	    e += e3;
	}
    }
}
#endif


#ifdef DOTS
void
DOTS (FbBits	    *dst,
      FbStride	    dstStride,
      int	    dstBpp,
      BoxPtr	    pBox,
      xPoint	    *ptsOrig,
      int	    npt,
      int	    xoff,
      int	    yoff,
      FbBits	    and,
      FbBits	    xor)
{
    INT32    	*pts = (INT32 *) ptsOrig;
    UNIT	*bits = (UNIT *) dst;
    UNIT	*point;
    BITS	bxor = (BITS) xor;
    BITS	band = (BITS) and;
    FbStride	bitsStride = dstStride * (sizeof (FbBits) / sizeof (UNIT));
    INT32    	ul, lr;
    INT32    	pt;

    ul = coordToInt(pBox->x1 - xoff,     pBox->y1 - yoff);
    lr = coordToInt(pBox->x2 - xoff - 1, pBox->y2 - yoff - 1);

    bits += (INT32)bitsStride * (INT32)yoff + (INT32)xoff * (INT32)MUL;
    
    if (and == 0)
    {
	while (npt--)
	{
	    pt = *pts++;
	    if (!isClipped(pt,ul,lr))
	    {
		point = bits + (INT32)intToY(pt) * (INT32)bitsStride + (INT32)intToX(pt) * (INT32)MUL;
		STORE(point,bxor);
	    }
	}
    }
    else
    {
	while (npt--)
	{
	    pt = *pts++;
	    if (!isClipped(pt,ul,lr))
	    {
		point = bits + (INT32)intToY(pt) * (INT32)bitsStride + (INT32)intToX(pt) * (INT32)MUL;
		RROP(point,band,bxor);
	    }
	}
    }
}
#endif


#ifdef POLYLINE
void
POLYLINE (DrawablePtr	pDrawable,
	  GCPtr		pGC,
	  int		mode,
	  int		npt,
	  DDXPointPtr	ptsOrig)
{
    INT32	    *pts = (INT32 *) ptsOrig;
    int		    xoff = pDrawable->x;
    int		    yoff = pDrawable->y;
    unsigned int    bias = miGetZeroLineBias(pDrawable->pScreen);
    BoxPtr	    pBox = REGION_EXTENTS (pDrawable->pScreen, fbGetCompositeClip (pGC));
    
    FbBits	    *dst;
    int		    dstStride;
    int		    dstBpp;
    int		    dstXoff, dstYoff;
    
    UNIT	    *bits, *bitsBase;
    FbStride	    bitsStride;
    BITS	    xor = fbGetGCPrivate(pGC)->xor;
    BITS	    and = fbGetGCPrivate(pGC)->and;
    int		    dashoffset = 0;
    
    INT32	    ul, lr;
    INT32	    pt1, pt2;

    int		    e, e1, e3, len;
    int		    stepmajor, stepminor;
    int		    octant;

    if (mode == CoordModePrevious)
	fbFixCoordModePrevious (npt, ptsOrig);
    
    fbGetDrawable (pDrawable, dst, dstStride, dstBpp, dstXoff, dstYoff);
    bitsStride = dstStride * (sizeof (FbBits) / sizeof (UNIT));
    bitsBase = ((UNIT *) dst) + (INT32)(yoff + dstYoff) * (INT32)bitsStride + (INT32)(xoff + dstXoff) * (INT32)MUL;
    ul = coordToInt(pBox->x1 - xoff,     pBox->y1 - yoff);
    lr = coordToInt(pBox->x2 - xoff - 1, pBox->y2 - yoff - 1);

    pt1 = *pts++;
    npt--;
    pt2 = *pts++;
    npt--;
    for (;;)
    {
	if (isClipped (pt1, ul, lr) | isClipped (pt2, ul, lr))
	{
	    fbSegment (pDrawable, pGC, 
		       intToX(pt1) + xoff, intToY(pt1) + yoff,
		       intToX(pt2) + xoff, intToY(pt2) + yoff,
		       npt == 0 && pGC->capStyle != CapNotLast,
		       &dashoffset);
	    if (!npt)
		return;
	    pt1 = pt2;
	    pt2 = *pts++;
	    npt--;
	}
	else
	{
	    bits = bitsBase + (INT32)intToY(pt1) * (INT32)bitsStride + (INT32)intToX(pt1) * (INT32)MUL;
	    for (;;)
	    {
		CalcLineDeltas (intToX(pt1), intToY(pt1),
				intToX(pt2), intToY(pt2),
				len, e1, stepmajor, stepminor, 1, bitsStride,
				octant);
		stepmajor *= MUL;
		if (len < e1)
		{
		    e3 = len;
		    len = e1;
		    e1 = e3;

		    e3 = stepminor;
		    stepminor = stepmajor;
		    stepmajor = e3;
		    SetYMajorOctant(octant);
		}
		e = -len;
		e1 <<= 1;
		e3 = e << 1;
		FIXUP_ERROR (e, octant, bias);
		if (and == 0)
		{
		    while (len--)
		    {
			STORE(bits,xor);
			bits += stepmajor;
			e += e1;
			if (e >= 0)
			{
			    bits += stepminor;
			    e += e3;
			}
		    }
		}
		else
		{
		    while (len--)
		    {
			RROP(bits,and,xor);
			bits += stepmajor;
			e += e1;
			if (e >= 0)
			{
			    bits += stepminor;
			    e += e3;
			}
		    }
		}
		if (!npt)
		{
		    if (pGC->capStyle != CapNotLast && 
			pt2 != *((INT32 *) ptsOrig))
		    {
			RROP(bits,and,xor);
		    }
		    return;
		}
		pt1 = pt2;
		pt2 = *pts++;
		--npt;
		if (isClipped (pt2, ul, lr))
		    break;
    	    }
	}
    }
}
#endif


#undef MUL
#undef STORE
#undef RROP
#undef UNIT
#undef USE_SOLID

#undef isClipped
