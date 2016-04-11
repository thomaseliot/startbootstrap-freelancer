/* $Xorg: scrnintstr.h,v 1.4 2001/02/09 02:05:15 xorgcvs Exp $ */
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
/* $XFree86: xc/programs/Xserver/include/scrnintstr.h,v 1.10 2001/12/14 19:59:56 dawes Exp $ */

#ifndef SCREENINTSTRUCT_H
#define SCREENINTSTRUCT_H

#include "egl_md.h"
#include "Xproto.h"
#include "screenint.h"
#include "miscstruct.h"
#include "validate.h"
#include "pixmap.h"
#include "gc.h"



typedef struct _PixmapFormat
{
    unsigned char	depth;
    unsigned char	bitsPerPixel;
    unsigned char	scanlinePad;
} PixmapFormatRec;

/*
 *  There is a typedef for each screen function pointer so that code that
 *  needs to declare a screen function pointer (e.g. in a screen private
 *  or as a local variable) can easily do so and retain full type checking.
 */

typedef    void (* GetSpansProcPtr)(
#if NeedNestedPrototypes
    DrawablePtr     /* pDrawable    */,
    GCPtr           /* pGC          */,
    int             /* wMax         */,
    DDXPointPtr     /* ppt          */,
    int*            /* pwidth       */,
    int             /* nspans       */,
    char *          /* pdstStart    */
#endif
);

typedef    void (* PointerNonInterestBoxProcPtr)(
#if NeedNestedPrototypes
    ScreenPtr       /* pScreen      */,
    BoxPtr          /* pBox         */
#endif
);

typedef    void (* SourceValidateProcPtr)(
#if NeedNestedPrototypes
    DrawablePtr     /* pDrawable    */,
    int             /* x            */,
    int             /* y            */,
    int             /* width        */,
    int             /* height       */
#endif
);

typedef    PixmapPtr (* CreatePixmapProcPtr)(
#if NeedNestedPrototypes
    ScreenPtr       /* pScreen      */,
    int             /* width        */,
    int             /* height       */,
    int             /* depth        */
#endif
);

typedef    Bool (* DestroyPixmapProcPtr)(
#if NeedNestedPrototypes
    PixmapPtr       /* pPixmap      */
#endif
);

#ifdef NEED_SCREEN_REGIONS

typedef    RegionPtr (* RegionCreateProcPtr)(
#if NeedNestedPrototypes
    BoxPtr          /* rect         */,
    int             /* size         */
#endif
);

typedef    void (* RegionInitProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* pReg         */,
    BoxPtr          /* rect         */,
    int             /* size         */
#endif
);

typedef    Bool (* RegionCopyProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* dst          */,
    RegionPtr       /* src          */
#endif
);

typedef    void (* RegionDestroyProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* pReg         */
#endif
);

typedef    void (* RegionUninitProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* pReg         */
#endif
);

typedef    Bool (* IntersectProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* newReg       */,
    RegionPtr       /* reg1         */,
    RegionPtr       /* reg2         */
#endif
);

typedef    Bool (* UnionProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* newReg       */,
    RegionPtr       /* reg1         */,
    RegionPtr       /* reg2         */
#endif
);

typedef    Bool (* SubtractProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* regD         */,
    RegionPtr       /* regM         */,
    RegionPtr       /* regS         */
#endif
);

typedef    Bool (* InverseProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* newReg       */,
    RegionPtr       /* reg1         */,
    BoxPtr          /* invRect      */
#endif
);

typedef    void (* RegionResetProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* pReg         */,
    BoxPtr          /* pBox         */
#endif
);

typedef    void (* TranslateRegionProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* pReg         */,
    int             /* x            */,
    int             /* y            */
#endif
);

typedef    int (* RectInProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* region       */,
    BoxPtr          /* prect        */
#endif
);

typedef    Bool (* PointInRegionProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* pReg         */,
    int             /* x            */,
    int             /* y            */,
    BoxPtr          /* box          */
#endif
);

typedef    Bool (* RegionNotEmptyProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* pReg         */
#endif
);

typedef    Bool (* RegionBrokenProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* pReg         */
#endif
);

typedef    Bool (* RegionBreakProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* pReg         */
#endif
);

typedef    void (* RegionEmptyProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* pReg         */
#endif
);

typedef    BoxPtr (* RegionExtentsProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* pReg         */
#endif
);

typedef    Bool (* RegionAppendProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* dstrgn       */,
    RegionPtr       /* rgn          */
#endif
);

typedef    Bool (* RegionValidateProcPtr)(
#if NeedNestedPrototypes
    RegionPtr       /* badreg       */,
    Bool*           /* pOverlap     */
#endif
);

#endif /* NEED_SCREEN_REGIONS */


#ifdef NEED_SCREEN_REGIONS

typedef    RegionPtr (* RectsToRegionProcPtr)(
#if NeedNestedPrototypes
    int             /* nrects       */,
    xRectangle*     /* prect        */,
    int             /* ctype        */
#endif
);

#endif /* NEED_SCREEN_REGIONS */


typedef    Bool (* CreateScreenResourcesProcPtr)(
#if NeedNestedPrototypes
    ScreenPtr       /* pScreen      */
#endif
);

typedef    Bool (* ModifyPixmapHeaderProcPtr)(
#if NeedNestedPrototypes
    PixmapPtr       /* pPixmap      */,
    int             /* width        */,
    int             /* height       */,
    int             /* depth        */,
    int             /* bitsPerPixel */,
    int             /* devKind      */,
    pointer         /* pPixData     */
#endif
);

typedef    PixmapPtr (* GetScreenPixmapProcPtr)(
#if NeedNestedPrototypes
    ScreenPtr       /* pScreen      */
#endif
);

typedef    void (* SetScreenPixmapProcPtr)(
#if NeedNestedPrototypes
    PixmapPtr       /* pPix         */
#endif
);

typedef struct _Screen
{
    int             myNum;    /* index of this instance in Screens[] */
    ATOM            id;
    short           width, height;
    short           xoff, yoff;
    //short         mmWidth, mmHeight;
    unsigned char   rootDepth;
    unsigned long   whitePixel, blackPixel;
    unsigned long   rgf;    /* array of flags; she's -- HUNGARIAN */
    GCPtr           GCperDepth[MAXFORMATS+1];

    /* next field is a stipple to use as default in
    ** a GC.  we don't build default tiles of all depths
    ** because they are likely to be of a color
    ** different from the default fg pixel, so
    ** we don't win anything by building
    ** a standard one.
    */
    PixmapPtr       PixmapPerDepth[1];
    pointer         devPrivate;
    int             GCPrivateLen;
    unsigned        *GCPrivateSizes;
    unsigned        totalGCSize;

    /* Random screen procedures */
    GetSpansProcPtr GetSpans;

    /* Pixmap procedures */
    CreatePixmapProcPtr  CreatePixmap;
    DestroyPixmapProcPtr DestroyPixmap;

    /* Region procedures */

#ifdef NEED_SCREEN_REGIONS
    RegionCreateProcPtr    RegionCreate;
    RegionInitProcPtr      RegionInit;
    RegionCopyProcPtr      RegionCopy;
    RegionDestroyProcPtr   RegionDestroy;
    RegionUninitProcPtr    RegionUninit;
    IntersectProcPtr       Intersect;
    UnionProcPtr           Union;
    SubtractProcPtr        Subtract;
    InverseProcPtr         Inverse;
    RegionResetProcPtr     RegionReset;
    TranslateRegionProcPtr TranslateRegion;
    RectInProcPtr          RectIn;
    PointInRegionProcPtr   PointInRegion;
    RegionNotEmptyProcPtr  RegionNotEmpty;
    RegionBrokenProcPtr    RegionBroken;
    RegionBreakProcPtr     RegionBreak;
    RegionEmptyProcPtr     RegionEmpty;
    RegionExtentsProcPtr   RegionExtents;
    RegionAppendProcPtr    RegionAppend;
    RegionValidateProcPtr  RegionValidate;
#endif /* NEED_SCREEN_REGIONS */
#ifdef NEED_SCREEN_REGIONS
    RectsToRegionProcPtr   RectsToRegion;
#endif /* NEED_SCREEN_REGIONS */

    /* anybody can get a piece of this array */
    DevUnion * devPrivates;

    CreateScreenResourcesProcPtr CreateScreenResources;
    ModifyPixmapHeaderProcPtr    ModifyPixmapHeader;

    GetScreenPixmapProcPtr    GetScreenPixmap;
    SetScreenPixmapProcPtr    SetScreenPixmap;

    PixmapPtr pScratchPixmap;        /* scratch pixmap "pool" */

#ifdef PIXPRIV
    int            PixmapPrivateLen;
    unsigned int * PixmapPrivateSizes;
    unsigned int   totalPixmapSize;
#endif
} ScreenRec;

typedef struct _LayerInfo
{
    ScreenPtr       screen;
    DrawablePtr     drawable;
    GCPtr           gc;
    PixmapPtr       pixmap;
} LayerInfo;

typedef struct _ScreenInfo
{
    int             numPixmapFormats;
    PixmapFormatRec formats[MAXFORMATS];
    ScreenPtr       screen;

} ScreenInfo;

extern ScreenInfo eglScreenInfo;
extern ScreenRec eglScreen;

extern void InitOutput(
#if NeedFunctionPrototypes
    ScreenInfo *    /* pScreenInfo  */,
    int             /* argc         */,
    char **         /* argv         */
#endif
);

extern ScreenPtr eglScreenPtr;
#define SCREENPTR eglScreenPtr

#endif /* SCREENINTSTRUCT_H */
