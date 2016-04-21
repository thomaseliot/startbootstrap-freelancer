/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/*
 * Id: fbfill.c,v 1.1 1999/11/02 03:54:45 keithp Exp $
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
/* $XFree86: xc/programs/Xserver/fb/fbfill.c,v 1.4 2001/05/29 04:54:09 keithp Exp $ */

#include "fb.h"

void
fbFill (DrawablePtr pDrawable,
	GCPtr	    pGC,
	int	    x,
	int	    y,
	int	    width,
	int	    height)
{
    FbBits	    *dst;
    FbStride	    dstStride;
    int		    dstBpp;
    int		    dstXoff, dstYoff;
    FbGCPrivPtr	    pPriv = fbGetGCPrivate(pGC);
    
    fbGetDrawable (pDrawable, dst, dstStride, dstBpp, dstXoff, dstYoff);

    switch (pGC->fillStyle) {
    case FillSolid:
	fbSolid (dst + (INT32)(y + dstYoff) * (INT32)dstStride, 
		 dstStride, 
		 (x + dstXoff) * dstBpp,
		 dstBpp,
		 width * dstBpp, height,
		 pPriv->and, pPriv->xor);
	break;
    case FillStippled:
    case FillOpaqueStippled: {
	PixmapPtr   pStip = pGC->stipple;
	int	    stipWidth = pStip->drawable.width;
	int	    stipHeight = pStip->drawable.height;
	
	if (dstBpp == 1)
	{
	    int		alu;
	    FbBits	*stip;
	    FbStride    stipStride;
	    int		stipBpp;
	    int		stipXoff, stipYoff; /* XXX assumed to be zero */

	    if (pGC->fillStyle == FillStippled)
		alu = FbStipple1Rop(pGC->alu,pGC->fgPixel);
	    else
		alu = FbOpaqueStipple1Rop(pGC->alu,pGC->fgPixel,pGC->bgPixel);
	    fbGetDrawable (&pStip->drawable, stip, stipStride, stipBpp, stipXoff, stipYoff);
	    fbTile (pGC,
		    dst + (INT32)(y + dstYoff) * (INT32)dstStride,
		    dstStride,
		    x + dstXoff,
		    width, height,
		    stip,
		    stipStride,
		    stipWidth,
		    stipHeight,
		    alu,
		    pPriv->pm,
		    dstBpp,
		    
		    (pGC->patOrg.x + pDrawable->x),
		    pGC->patOrg.y + pDrawable->y - y);
	}
	else
	{
	    FbStip	*stip;
	    FbStride    stipStride;
	    int		stipBpp;
	    int		stipXoff, stipYoff; /* XXX assumed to be zero */
	    FbBits	fgand, fgxor, bgand, bgxor;

	    fgand = pPriv->and;
	    fgxor = pPriv->xor;
	    if (pGC->fillStyle == FillStippled)
	    {
		bgand = fbAnd(GXnoop,(FbBits) 0,FB_ALLONES);
		bgxor = fbXor(GXnoop,(FbBits) 0,FB_ALLONES);
	    }
	    else
	    {
		bgand = pPriv->bgand;
		bgxor = pPriv->bgxor;
	    }

	    fbGetStipDrawable (&pStip->drawable, stip, stipStride, stipBpp, stipXoff, stipYoff);
	    fbStipple (dst + (INT32)y * (INT32)dstStride, 
		       dstStride, 
		       x * dstBpp,
		       dstBpp,
		       width * dstBpp, height,
		       stip,
		       stipStride,
		       stipWidth,
		       stipHeight,
		       pPriv->evenStipple,
		       fgand, fgxor,
		       bgand, bgxor,
		       pGC->patOrg.x + pDrawable->x,
		       pGC->patOrg.y + pDrawable->y - y);
	}
	break;
    }
    }
    fbValidateDrawable (pDrawable);
}
