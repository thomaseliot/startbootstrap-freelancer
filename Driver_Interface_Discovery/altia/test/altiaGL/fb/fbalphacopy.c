/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.5 $    $Date: 2009-05-06 00:03:30 $
 * Copyright © 2006 Altia, Inc.
 * Copyright © 1998 Keith Packard
 *
 * This file has been created by Altia Inc. based on original work by Keith
 * Packard.
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

#include "fb.h"
#ifdef IN_MODULE
#include "xf86_ansic.h"
#endif

#ifdef USE_FB_BLEND

static void
fbAlphaCopyNtoN (DrawablePtr    pSrcDrawable,
             DrawablePtr    pDstDrawable,
             DrawablePtr    pAlphaDrawable,
             GCPtr      pGC,
             BoxPtr     pbox,
             int        nbox,
             int        dx,
             int        dy,
             Bool       reverse,
             Bool       upsidedown,
             Pixel      bitplane,
             void       *closure)
{
    CARD8   alu = pGC ? pGC->alu : GXcopy;
    FbBits  pm = pGC ? fbGetGCPrivate(pGC)->pm : FB_ALLONES;
    FbBits  *src;
    FbStride    srcStride;
    int     srcBpp;
    int     srcXoff, srcYoff;
    FbBits  *dst;
    FbStride    dstStride;
    int     dstBpp;
    int     dstXoff, dstYoff;
    void    *alphas;
    FbStride    alphasStride;
    int     alphasBpp;
    int     alphasXoff, alphasYoff;

    fbGetDrawable (pSrcDrawable, src, srcStride, srcBpp, srcXoff, srcYoff);
    fbGetDrawable (pDstDrawable, dst, dstStride, dstBpp, dstXoff, dstYoff);
    fbGetDrawable (pAlphaDrawable, alphas, alphasStride, alphasBpp, alphasXoff,
    alphasYoff);

    alphasStride *= sizeof(FbBits);

    while (nbox--)
    {
    fbAlphaBlt (pGC,
        src + (INT32)(pbox->y1 + dy + srcYoff) * (INT32)srcStride,
        srcStride,
        (pbox->x1 + dx + srcXoff) * srcBpp,
    
        dst + (INT32)(pbox->y1 + dstYoff) * (INT32)dstStride,
        dstStride,
        (pbox->x1 + dstXoff) * dstBpp,
    
        ((CARD8 *)alphas) + (INT32)((pbox->y1 + dy + alphasYoff) * (INT32)alphasStride),
        alphasStride,
        (pbox->x1 + dx + alphasXoff),

        (pbox->x2 - pbox->x1) * dstBpp,
        (pbox->y2 - pbox->y1),
    
        alu,
        pm,
        dstBpp,
    
        reverse,
        upsidedown);
    pbox++;
    }
}

static void
fbAlphaCopyRegion (DrawablePtr      pSrcDrawable,
               DrawablePtr      pDstDrawable,
               DrawablePtr      pAlphaDrawable,
               GCPtr        pGC,
               RegionPtr        pDstRegion,
               int          dx,
               int          dy,
               fbAlphaCopyProc  copyProc,
               Pixel        bitPlane,
               void         *closure)
{
    int     careful;
    Bool    reverse;
    Bool    upsidedown;
    BoxPtr  pbox;
    int     nbox;
    BoxPtr  pboxNew1, pboxNew2, pboxBase, pboxNext, pboxTmp;
    
    pbox = REGION_RECTS(pDstRegion);
    nbox = REGION_NUM_RECTS(pDstRegion);
    
    /* XXX we have to err on the side of safety when both are windows,
     * because we don't know if IncludeInferiors is being used.
     */
    careful = ((pSrcDrawable == pDstDrawable) ||
           ((pSrcDrawable->type == DRAWABLE_WINDOW) &&
        (pDstDrawable->type == DRAWABLE_WINDOW)));

    pboxNew1 = NULL;
    pboxNew2 = NULL;
    if (careful && dy < 0)
    {
    upsidedown = TRUE;

    if (nbox > 1)
    {
        /* keep ordering in each band, reverse order of bands */
        pboxNew1 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
        if(!pboxNew1)
        return;
        pboxBase = pboxNext = pbox+nbox-1;
        while (pboxBase >= pbox)
        {
        while ((pboxNext >= pbox) &&
               (pboxBase->y1 == pboxNext->y1))
            pboxNext--;
        pboxTmp = pboxNext+1;
        while (pboxTmp <= pboxBase)
        {
            *pboxNew1++ = *pboxTmp++;
        }
        pboxBase = pboxNext;
        }
        pboxNew1 -= nbox;
        pbox = pboxNew1;
    }
    }
    else
    {
    /* walk source top to bottom */
    upsidedown = FALSE;
    }

    if (careful && dx < 0)
    {
    /* walk source right to left */
    if (dy <= 0)
        reverse = TRUE;
    else
        reverse = FALSE;

    if (nbox > 1)
    {
        /* reverse order of rects in each band */
        pboxNew2 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
        if(!pboxNew2)
        {
        if (pboxNew1)
            DEALLOCATE_LOCAL(pboxNew1);
        return;
        }
        pboxBase = pboxNext = pbox;
        while (pboxBase < pbox+nbox)
        {
        while ((pboxNext < pbox+nbox) &&
               (pboxNext->y1 == pboxBase->y1))
            pboxNext++;
        pboxTmp = pboxNext;
        while (pboxTmp != pboxBase)
        {
            *pboxNew2++ = *--pboxTmp;
        }
        pboxBase = pboxNext;
        }
        pboxNew2 -= nbox;
        pbox = pboxNew2;
    }
    }
    else
    {
    /* walk source left to right */
    reverse = FALSE;
    }

    (*copyProc) (pSrcDrawable,
         pDstDrawable,
         pAlphaDrawable,
         pGC,
         pbox,
         nbox,
         dx, dy,
         reverse, upsidedown, bitPlane, closure);
    
    if (pboxNew1)
    DEALLOCATE_LOCAL (pboxNew1);
    if (pboxNew2)
    DEALLOCATE_LOCAL (pboxNew2);
}

static RegionPtr
fbAlphaDoCopy (DrawablePtr  pSrcDrawable,
           DrawablePtr  pDstDrawable,
           DrawablePtr  pAlphaDrawable,
           GCPtr        pGC,
           int      xIn, 
           int      yIn,
           int      widthSrc, 
           int      heightSrc,
           int      xOut, 
           int      yOut,
           fbAlphaCopyProc  copyProc,
           Pixel        bitPlane,
           void     *closure)
{
    RegionPtr   prgnSrcClip = NULL; /* may be a new region, or just a copy */
    Bool    freeSrcClip = FALSE;
    RegionPtr   prgnExposed = NULL;
    RegionRec   rgnDst;
    int     dx;
    int     dy;
    int     numRects;
    BoxRec  box;
    Bool    fastSrc = FALSE;    /* for fast clipping with pixmap source */
    Bool    fastDst = FALSE;    /* for fast clipping with one rect dest */
    /* Bool    fastExpose = FALSE;  for fast exposures with pixmap source */

    /* Short cut for unmapped windows */

    if (pDstDrawable->type == DRAWABLE_WINDOW)
    {
    return NULL;
    }

    /* Compute source clip region */
    if (pSrcDrawable->type == DRAWABLE_PIXMAP)
    {
    if ((pSrcDrawable == pDstDrawable) && (pGC->clientClipType == CT_NONE))
        prgnSrcClip = fbGetCompositeClip(pGC);
    else
        fastSrc = TRUE;
    }
    else
    {
    prgnSrcClip = fbGetCompositeClip(pGC);
    }

    xIn += pSrcDrawable->x;
    yIn += pSrcDrawable->y;
    
    xOut += pDstDrawable->x;
    yOut += pDstDrawable->y;

    box.x1 = xIn;
    box.y1 = yIn;
    box.x2 = xIn + widthSrc;
    box.y2 = yIn + heightSrc;

    dx = xIn - xOut;
    dy = yIn - yOut;

    /* Don't create a source region if we are doing a fast clip */
    if (fastSrc)
    {
    RegionPtr cclip;
    
    /* fastExpose = TRUE; */
    /*
     * clip the source; if regions extend beyond the source size,
     * make sure exposure events get sent
     */
    if (box.x1 < pSrcDrawable->x)
    {
        box.x1 = pSrcDrawable->x;
        /* fastExpose = FALSE; */
    }
    if (box.y1 < pSrcDrawable->y)
    {
        box.y1 = pSrcDrawable->y;
        /* fastExpose = FALSE; */
    }
    if (box.x2 > pSrcDrawable->x + (int) pSrcDrawable->width)
    {
        box.x2 = pSrcDrawable->x + (int) pSrcDrawable->width;
        /* fastExpose = FALSE; */
    }
    if (box.y2 > pSrcDrawable->y + (int) pSrcDrawable->height)
    {
        box.y2 = pSrcDrawable->y + (int) pSrcDrawable->height;
        /* fastExpose = FALSE; */
    }
    
    /* Translate and clip the dst to the destination composite clip */
        box.x1 -= dx;
        box.x2 -= dx;
        box.y1 -= dy;
        box.y2 -= dy;

    /* If the destination composite clip is one rectangle we can
       do the clip directly.  Otherwise we have to create a full
       blown region and call intersect */

    cclip = fbGetCompositeClip(pGC);
        if (REGION_NUM_RECTS(cclip) == 1)
        {
        BoxPtr pBox = REGION_RECTS(cclip);

        if (box.x1 < pBox->x1) box.x1 = pBox->x1;
        if (box.x2 > pBox->x2) box.x2 = pBox->x2;
        if (box.y1 < pBox->y1) box.y1 = pBox->y1;
        if (box.y2 > pBox->y2) box.y2 = pBox->y2;
        fastDst = TRUE;
    }
    }
    
    /* Check to see if the region is empty */
    if (box.x1 >= box.x2 || box.y1 >= box.y2)
    {
    REGION_INIT(pGC->pScreen, &rgnDst, NullBox, 0);
    }
    else
    {
    REGION_INIT(pGC->pScreen, &rgnDst, &box, 1);
    }
    
    /* Clip against complex source if needed */
    if (!fastSrc)
    {
    REGION_INTERSECT(pGC->pScreen, &rgnDst, &rgnDst, prgnSrcClip);
    REGION_TRANSLATE(pGC->pScreen, &rgnDst, -dx, -dy);
    }

    /* Clip against complex dest if needed */
    if (!fastDst)
    {
    REGION_INTERSECT(pGC->pScreen, &rgnDst, &rgnDst,
             fbGetCompositeClip(pGC));
    }

    /* Do bit blitting */
    numRects = REGION_NUM_RECTS(&rgnDst);
    if (numRects && widthSrc && heightSrc)
    fbAlphaCopyRegion (pSrcDrawable, pDstDrawable, pAlphaDrawable, pGC,
              &rgnDst, dx, dy, copyProc, bitPlane, closure);

    /*
    if (!fastExpose && pGC->fExpose)
    prgnExposed = miHandleExposures(pSrcDrawable, pDstDrawable, pGC,
                    xIn - pSrcDrawable->x,
                    yIn - pSrcDrawable->y,
                    widthSrc, heightSrc,
                    xOut - pDstDrawable->x,
                    yOut - pDstDrawable->y,
                    (unsigned long) bitPlane);
    */
    REGION_UNINIT(pGC->pScreen, &rgnDst);
    if (freeSrcClip)
    REGION_DESTROY(pGC->pScreen, prgnSrcClip);
    fbValidateDrawable (pDstDrawable);
    return prgnExposed;
}

#define INC_APTR(p, s)  { \
    CARD8 *c = ((CARD8*)(p)); \
    c += (s); \
    (p) = (void *) c; \
}

RegionPtr
fbAlphaCopyArea (DrawablePtr    pSrcDrawable,
             DrawablePtr    pDstDrawable,
             DrawablePtr    pAlphaDrawable,
             GCPtr      pGC,
             int        xIn, 
             int        yIn,
             int        widthSrc, 
             int        heightSrc,
             int        xOut, 
             int        yOut)
{
    fbAlphaCopyProc copy;

    if(pDstDrawable->alphas) {
        DrawablePtr dDrawable = (DrawablePtr)pDstDrawable->alphas;
        FbBits *sBits, *dBits;
        FbStride sStride, dStride;
        int sBpp, dBpp, sXoff, sYoff, dXoff, dYoff, x, y, sW, sH;

        fbGetDrawable (pAlphaDrawable, sBits, sStride, sBpp, sXoff, sYoff);
        fbGetDrawable (dDrawable, dBits, dStride, dBpp, dXoff, dYoff);

        sStride *= sizeof(FbBits);
        dStride *= sizeof(FbBits);

        INC_APTR(sBits, (sStride *(-yOut)) -xOut);

        sH = heightSrc +yOut;
        sW = widthSrc +xOut;

        for(y = 0; y < dDrawable->height && y < sH; y++) {
            if(y >= yOut) {
                CARD8 *sp = (CARD8*)sBits;
                CARD8 *dp = (CARD8*)dBits;
                for(x = 0; x < dDrawable->width && x < sW; x++) {
                    if(x >= xOut) {
                        CARD8 h = sp[x];
                        CARD8 g = dp[x];
                        if(0 == g) {
                            sp[x] = 255;
                            dp[x] = h;
                        } else {
                            int t = g +g *h /255;
                            dp[x] = (CARD8)(t > 255 ? 255 : t);
                        }
                    }
                }
            }
            INC_APTR(sBits, sStride);
            INC_APTR(dBits, dStride);
        }
    }

    /*
     * src and dst pixel formats should always be the same
     */
    copy = fbAlphaCopyNtoN;
    return fbAlphaDoCopy (pSrcDrawable, pDstDrawable, pAlphaDrawable, pGC, xIn,
                          yIn, widthSrc, heightSrc, xOut, yOut, copy, 0, 0);
}

#endif /* USE_FB_BLEND */
