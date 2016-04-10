/* $Xorg: gcstruct.h,v 1.4 2001/02/09 02:05:15 xorgcvs Exp $ */
/***********************************************************

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


/* $XFree86: xc/programs/Xserver/include/gcstruct.h,v 1.6 2001/12/14 19:59:54 dawes Exp $ */

#ifndef GCSTRUCT_H
#define GCSTRUCT_H

#include "gc.h"

#include "miscstruct.h"
#include "region.h"
#include "pixmap.h"
#include "screenint.h"

/*
 * functions which modify the state of the GC
 */

typedef struct _GCFuncs {
    void    (* ValidateGC)(
#if NeedNestedPrototypes
        GCPtr /*pGC*/,
        unsigned long /*stateChanges*/,
        DrawablePtr /*pDrawable*/
#endif
);

    void    (* ChangeClip)(
#if NeedNestedPrototypes
        GCPtr /*pGC*/,
        int /*type*/,
        pointer /*pvalue*/,
        int /*nrects*/
#endif
);

    void    (* DestroyClip)(
#if NeedNestedPrototypes
        GCPtr /*pGC*/
#endif
);

    void    (* CopyClip)(
#if NeedNestedPrototypes
        GCPtr /*pgcDst*/,
        GCPtr /*pgcSrc*/
#endif
);
    DevUnion    devPrivate;
} GCFuncs;

/*
 * graphics operations invoked through a GC
 */

typedef struct _GCOps {
    void    (* FillSpans)(
#if NeedNestedPrototypes
        DrawablePtr /*pDrawable*/,
        GCPtr /*pGC*/,
        int /*nInit*/,
        DDXPointPtr /*pptInit*/,
        int * /*pwidthInit*/,
        int /*fSorted*/
#endif
);

    void    (* SetSpans)(
#if NeedNestedPrototypes
        DrawablePtr /*pDrawable*/,
        GCPtr /*pGC*/,
        char * /*psrc*/,
        DDXPointPtr /*ppt*/,
        int * /*pwidth*/,
        int /*nspans*/,
        int /*fSorted*/
#endif
);

    RegionPtr   (* CopyArea)(
#if NeedNestedPrototypes
        DrawablePtr /*pSrc*/,
        DrawablePtr /*pDst*/,
        GCPtr /*pGC*/,
        int /*srcx*/,
        int /*srcy*/,
        int /*w*/,
        int /*h*/,
        int /*dstx*/,
        int /*dsty*/
#endif
);

    RegionPtr   (* CopyPlane)(
#if NeedNestedPrototypes
        DrawablePtr /*pSrcDrawable*/,
        DrawablePtr /*pDstDrawable*/,
        GCPtr /*pGC*/,
        int /*srcx*/,
        int /*srcy*/,
        int /*width*/,
        int /*height*/,
        int /*dstx*/,
        int /*dsty*/,
        unsigned long /*bitPlane*/
#endif
);
    void    (* PolyPoint)(
#if NeedNestedPrototypes
        DrawablePtr /*pDrawable*/,
        GCPtr /*pGC*/,
        int /*mode*/,
        int /*npt*/,
        DDXPointPtr /*pptInit*/
#endif
);

    void    (* Polylines)(
#if NeedNestedPrototypes
        DrawablePtr /*pDrawable*/,
        GCPtr /*pGC*/,
        int /*mode*/,
        int /*npt*/,
        DDXPointPtr /*pptInit*/
#endif
);

    void    (* PolyRectangle)(
#if NeedNestedPrototypes
        DrawablePtr /*pDrawable*/,
        GCPtr /*pGC*/,
        int /*nrects*/,
        xRectangle * /*pRects*/
#endif
);

    void    (* FillPolygon)(
#if NeedNestedPrototypes
        DrawablePtr /*pDrawable*/,
        GCPtr /*pGC*/,
        int /*shape*/,
        int /*mode*/,
        int /*count*/,
        DDXPointPtr /*pPts*/
#endif
);

    void    (* PolyFillRect)(
#if NeedNestedPrototypes
        DrawablePtr /*pDrawable*/,
        GCPtr /*pGC*/,
        int /*nrectFill*/,
        xRectangle * /*prectInit*/
#endif
);

    RegionPtr   (* AlphaCopyArea)(
#if NeedNestedPrototypes
        DrawablePtr /*pSrc*/,
        DrawablePtr /*pDst*/,
        DrawablePtr /*pAlpha*/,
        GCPtr /*pGC*/,
        int /*srcx*/,
        int /*srcy*/,
        int /*w*/,
        int /*h*/,
        int /*dstx*/,
        int /*dsty*/
#endif
);

#ifdef ALTIA_DRV_LOADRASTER
    void (* DrvLoadRaster)(
#if NeedNestedPrototypes
        Altia_Raster_type * /*raster*/
#endif
);

    void (* DrvDeleteRaster)(
#if NeedNestedPrototypes
        Altia_Raster_type * /*raster*/
#endif
);
#endif /* ALTIA_DRV_LOADRASTER */

#ifdef ALTIA_DRV_LOADMONO
    void * (* DrvLoadMono)(
#if NeedNestedPrototypes
    unsigned char *, /*pixels*/
    unsigned long, /*pixelcnt*/
    int,           /*width*/
    int            /*height*/
#endif
);

    void (* DrvDeleteMono)(
#if NeedNestedPrototypes
    void *  /*MDDB*/
#endif
);
#endif /* ALTIA_DRV_LOADMONO */

    DevUnion    devPrivate;
} GCOps;

/* there is padding in the bit fields because the Sun compiler doesn't
 * force alignment to 32-bit boundaries.  losers.
 */
typedef struct _GC
{
    ScreenPtr       pScreen;
    unsigned char   depth;
    unsigned char   alu;
    unsigned short  lineWidth;
    unsigned short  dashOffset;
    unsigned short  numInDashList;
    unsigned char  *dash;
    unsigned int    alpha               : 8;
    unsigned int    lineStyle           : 2;
    unsigned int    capStyle            : 2;
    unsigned int    joinStyle           : 2;
    unsigned int    fillStyle           : 2;
    unsigned int    fillRule            : 1;
    unsigned int    arcMode             : 1;
    unsigned int    subWindowMode       : 1;
    unsigned int    graphicsExposures   : 1;
    unsigned int    clientClipType      : 2; /* CT_<kind> */
    unsigned int    miTranslate         : 1; /* should mi things translate? */
    unsigned int    tileIsPixel         : 1; /* tile is solid pixel */
    unsigned int    fExpose             : 1; /* Call exposure handling */
    unsigned int    freeCompClip        : 1; /* Free composite clip */
    unsigned int    colorFlip           : 1; /* If back/foreground have been flipped */
    unsigned int    transDraw           : 1; /* If destination alpha should be calculated (transparent frame buffers) */
    unsigned int    alphaBuffer         : 1; /* If we're drawing to an alpha buffer */
    unsigned int    unused              : 3; /* see comment above */
    unsigned long   planemask;
    unsigned long   fgPixel;
    unsigned long   bgPixel;

    /*
     * alas -- both tile and stipple must be here as they
     * are independently specifiable
     */
    PixUnion        tile;
    PixmapPtr       stipple;
    DDXPointRec     patOrg;     /* origin for (tile, stipple) */
    void           *font;
    DDXPointRec     clipOrg;
    DDXPointRec     lastWinOrg; /* position of window last validated */
    pointer         clientClip;
    unsigned long   stateChanges;   /* masked with GC_<kind> */
    unsigned long   serialNumber;
    GCFuncs        *funcs;
    GCOps          *ops;
    DevUnion       *devPrivates;

    /*
     * The following were moved here from private storage to allow device-
     * independent access to them from screen wrappers.
     * --- 1997.11.03  Marc Aurele La France (tsi@xfree86.org)
     */
    PixmapPtr       pRotatedPixmap; /* tile/stipple rotated for alignment */
    RegionPtr       pCompositeClip;

    /* fExpose & freeCompClip defined above */

#if EGL_HAS_RASTER_TRANSFORMATION_LIB
    unsigned char   hasTrans;
    void           *egl_trans;
#endif /* EGL_HAS_RASTER_TRANSFORMATION_LIB */
} GC;

#endif /* GCSTRUCT_H */
