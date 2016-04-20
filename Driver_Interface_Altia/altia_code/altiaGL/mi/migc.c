/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Xorg: migc.c,v 1.4 2001/02/09 02:05:21 xorgcvs Exp $ */
/*

Copyright 1993, 1998  The Open Group

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

/* $XFree86: xc/programs/Xserver/mi/migc.c,v 1.9 2001/12/14 20:00:23 dawes Exp $ */

#include "scrnintstr.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "migc.h"

/* ARGSUSED */

void
miDestroyGC(pGC)
    GCPtr           pGC;
{
    if (pGC->pRotatedPixmap)
	(*pGC->pScreen->DestroyPixmap) (pGC->pRotatedPixmap);
    if (pGC->freeCompClip)
	REGION_DESTROY(pGC->pScreen, pGC->pCompositeClip);
    miDestroyGCOps(pGC->ops);
    miDestroyClip(pGC);
}


void
miDestroyGCOps(ops)
    GCOpsPtr        ops;
{
    if (ops->devPrivate.val)
	xfree(ops);
}


void
miDestroyClip(pGC)
    GCPtr           pGC;
{
    if (pGC->clientClipType == CT_NONE)
	return;
    else if (pGC->clientClipType == CT_PIXMAP)
    {
	(*pGC->pScreen->DestroyPixmap) ((PixmapPtr) (pGC->clientClip));
    }
    else
    {
	/*
	 * we know we'll never have a list of rectangles, since ChangeClip
	 * immediately turns them into a region
	 */
	REGION_DESTROY(pGC->pScreen, pGC->clientClip);
    }
    pGC->clientClip = NULL;
    pGC->clientClipType = CT_NONE;
}

void
miChangeClip(pGC, type, pvalue, nrects)
    GCPtr           pGC;
    int             type;
    pointer         pvalue;
    int             nrects;
{
    (*pGC->funcs->DestroyClip) (pGC);
    if (type == CT_REGION)
    {
	/* stuff the region in the GC */
	pGC->clientClip = pvalue;
    }
    else if (type != CT_NONE)
    {
	pGC->clientClip = (pointer) RECTS_TO_REGION(pGC->pScreen, nrects,
						      (xRectangle *) pvalue,
								    type);
    }
    pGC->clientClipType = (type != CT_NONE && pGC->clientClip) ? CT_REGION : CT_NONE;
    pGC->stateChanges |= GCClipMask;
}

void
miCopyClip(pgcDst, pgcSrc)
    GCPtr           pgcDst, pgcSrc;
{
    RegionPtr       prgnNew;

    switch (pgcSrc->clientClipType)
    {
      case CT_PIXMAP:
	((PixmapPtr) pgcSrc->clientClip)->refcnt++;
	/* Fall through !! */
      case CT_NONE:
	(*pgcDst->funcs->ChangeClip) (pgcDst, (int) pgcSrc->clientClipType,
				   pgcSrc->clientClip, 0);
	break;
      case CT_REGION:
	prgnNew = REGION_CREATE(pgcSrc->pScreen, NULL, 1);
	REGION_COPY(pgcDst->pScreen, prgnNew,
					(RegionPtr) (pgcSrc->clientClip));
	(*pgcDst->funcs->ChangeClip) (pgcDst, CT_REGION, (pointer) prgnNew, 0);
	break;
    }
}

void
miComputeCompositeClip(pGC, pDrawable)
    GCPtr           pGC;
    DrawablePtr     pDrawable;
{
    ScreenPtr       pScreen;
    BoxRec          pixbounds;

    /* This prevents warnings about pScreen not being used. */
    pGC->pScreen = pScreen = pGC->pScreen;


    /* XXX should we translate by drawable.x/y here ? */
    /* If you want pixmaps in offscreen memory, yes */
    pixbounds.x1 = pDrawable->x;
    pixbounds.y1 = pDrawable->y;
    pixbounds.x2 = pDrawable->x + pDrawable->width;
    pixbounds.y2 = pDrawable->y + pDrawable->height;

    if (pGC->freeCompClip)
    {
	REGION_RESET(pScreen, pGC->pCompositeClip, &pixbounds);
    }
    else
    {
	pGC->freeCompClip = TRUE;
	pGC->pCompositeClip = REGION_CREATE(pScreen, &pixbounds, 1);
    }

    if (pGC->clientClipType == CT_REGION)
    {
	if(pDrawable->x || pDrawable->y) {
	    REGION_TRANSLATE(pScreen, pGC->clientClip,
				      pDrawable->x + pGC->clipOrg.x, 
				      pDrawable->y + pGC->clipOrg.y);
	    REGION_INTERSECT(pScreen, pGC->pCompositeClip,
			    pGC->pCompositeClip, pGC->clientClip);
	    REGION_TRANSLATE(pScreen, pGC->clientClip,
				      -(pDrawable->x + pGC->clipOrg.x), 
				      -(pDrawable->y + pGC->clipOrg.y));
	} else {
	    REGION_TRANSLATE(pScreen, pGC->pCompositeClip,
				     -pGC->clipOrg.x, -pGC->clipOrg.y);
	    REGION_INTERSECT(pScreen, pGC->pCompositeClip,
			    pGC->pCompositeClip, pGC->clientClip);
	    REGION_TRANSLATE(pScreen, pGC->pCompositeClip,
				     pGC->clipOrg.x, pGC->clipOrg.y);
	}
    }
} /* end miComputeCompositeClip */
