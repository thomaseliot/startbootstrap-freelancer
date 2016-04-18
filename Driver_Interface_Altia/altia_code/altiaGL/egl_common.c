/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.22 $    $Date: 2010-04-13 14:34:06 $
 * Copyright (c) 2002 Altia Inc.
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

/** FILE:  egl_common.c ****************************************************
 **
 ** This file contains the common functions for the general purpose
 ** graphics library.  These functions are used by all the drawing
 ** functions so they are in this common file.
 **
 ***************************************************************************/

#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#include "egl_Wrapper.h"

#include <stdio.h>

#include "scrnintstr.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "servermd.h"
#include "mi.h"
#ifdef USE_FB
#include "fb.h"
#endif

#ifdef UNDER_CE
#define NO_ASSERT
#endif

#define DRAWABLE(win) ((DrawableRec*)((AltiaDevInfo*)win)->drawable)

extern EGL_STATUS driver_ColorAlloc(EGL_DEVICE_ID devID, 
                                    ScreenPtr pScreen,
                                    EGL_ARGB *pAllocColors,
                                    EGL_ORD *pClutIndexes,
                                    EGL_COLOR *pUglColors,
                                    EGL_SIZE numColors);

extern BOOL driver_open(EGL_CHAR *name, ScreenInfo *screenInfo, 
                        ScreenPtr screen);

#if defined(MONOGC) || defined(EGL_DO_ALPHA_BLENDING)
extern AltiaDevInfo *altiaLibGetDevInfo(void);
#endif
#if EGL_DO_ALPHA_BLENDING
extern EGL_COLOR altiaLibGetColor(ALTIA_WINDOW devinfo, ALTIA_COLOR pixel);
#endif

#ifdef EXPANDED_VIRTUAL_MEMORY
extern BOOL driver_free();
#endif

ScreenRec eglScreen;
ScreenInfo eglScreenInfo;

static GCPtr utilityRootGc = 0;
static GCPtr utilityMonoGc = 0;

#if EGL_DO_ALPHA_BLENDING
static PixmapPtr scratchPixmap      = (PixmapPtr) 0;
static PixmapPtr scratchAlphaPixmap = (PixmapPtr) 0;
#endif /* EGL_DO_ALPHA_BLENDING */

#ifndef USE_FB
DrawableRec drawable;
#endif

DrawablePtr drawablePtr = (DrawablePtr)0;

PaddingInfo PixmapWidthPaddingInfo[33];

/* Active screen */
ScreenPtr eglScreenPtr = (ScreenPtr)0;



static GCPtr lastGc = 0;
DrawablePtr lastDrawable = 0;
GCPtr lastClipGc = 0;

EGL_POS lastClipLeft = -1234;
EGL_POS lastClipTop = -1234;
EGL_POS lastClipRight = -1234;
EGL_POS lastClipBottom = -1234;

/* Memory control */
unsigned long globalSerialNumber = 0;
unsigned long serverGeneration = 1;

/*
 * This array encodes the answer to the question "what is the log base 2
 * of the number of pixels that fit in a scanline pad unit?"
 * Note that ~0 is an invalid entry (mostly for the benefit of the reader).
 */
static const int answer[6][4] = {
    /* pad   pad   pad     pad*/
    /*  8     16    32    64 */

    {   3,     4,    5 ,   6 }, /* 1 bit per pixel */
    {   1,     2,    3 ,   4 }, /* 4 bits per pixel */
    {   0,     1,    2 ,   3 }, /* 8 bits per pixel */
    {   ~0,    0,    1 ,   2 }, /* 16 bits per pixel */
    {   ~0,    ~0,   0 ,   1 }, /* 24 bits per pixel */
    {   ~0,    ~0,   0 ,   1 }  /* 32 bits per pixel */
};
/*
 * This array gives the answer to the question "what is the first index for
 * the answer array above given the number of bits per pixel?"
 * Note that ~0 is an invalid entry (mostly for the benefit of the reader).
 */
static const int indexForBitsPerPixel[ 33 ] = {
    ~0, 0, ~0, ~0,  /* 1 bit per pixel */
    1, ~0, ~0, ~0,  /* 4 bits per pixel */
    2, ~0, ~0, ~0,  /* 8 bits per pixel */
    ~0,~0, ~0, ~0,
    3, ~0, ~0, ~0,  /* 16 bits per pixel */
    ~0,~0, ~0, ~0,
    4, ~0, ~0, ~0,  /* 24 bits per pixel */
    ~0,~0, ~0, ~0,
    5       /* 32 bits per pixel */
};

/*
 * This array gives the bytesperPixel value for cases where the number
 * of bits per pixel is a multiple of 8 but not a power of 2.
 */
static const int answerBytesPerPixel[ 33 ] = {
    ~0, 0, ~0, ~0,  /* 1 bit per pixel */
    0, ~0, ~0, ~0,  /* 4 bits per pixel */
    0, ~0, ~0, ~0,  /* 8 bits per pixel */
    ~0,~0, ~0, ~0,
    0, ~0, ~0, ~0,  /* 16 bits per pixel */
    ~0,~0, ~0, ~0,
    3, ~0, ~0, ~0,  /* 24 bits per pixel */
    ~0,~0, ~0, ~0,
    0       /* 32 bits per pixel */
};

/*
 * This array gives the answer to the question "what is the second index for
 * the answer array above given the number of bits per scanline pad unit?"
 * Note that ~0 is an invalid entry (mostly for the benefit of the reader).
 */
static const int indexForScanlinePad[ 65 ] = {
    ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0,
     0, ~0, ~0, ~0, /* 8 bits per scanline pad unit */
    ~0, ~0, ~0, ~0,
     1, ~0, ~0, ~0, /* 16 bits per scanline pad unit */
    ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0,
     2, ~0, ~0, ~0, /* 32 bits per scanline pad unit */
    ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0,
    ~0, ~0, ~0, ~0,
     3      /* 64 bits per scanline pad unit */
};

/* Used by mibitblt.c */
int ffs(int i)
{
    int j;
    for (j = 1; (i & 1) == 0; j++)
    i >>= 1;
    return j;
}

#ifdef MONOGC
static unsigned long monoSaveBG;
static unsigned long monoSaveFG;
static ALTIA_SHORT monoFillStyle;
static ALTIA_SHORT monoColorFlip;
#endif

#if EGL_HAS_RASTER_TRANSFORMATION_LIB
/***************************************************************************/
EGL_STATUS egl_RasterTransformSet(EGL_GC_ID gc, void *trans)
{
    GCPtr pGC = (GCPtr)gc;
    if (trans != NULL)
    {
        pGC->hasTrans = 1;
        pGC->egl_trans = trans;
    }
    else
    {
        pGC->hasTrans = 0;
        pGC->egl_trans = NULL;
    }
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_RasterTransformClear (EGL_GC_ID gc)
{
    GCPtr pGC = (GCPtr)gc;
    pGC->hasTrans = 0;
    pGC->egl_trans = NULL;
    return EGL_STATUS_OK;
}
#endif /* EGL_HAS_RASTER_TRANSFORMATION_LIB */

GCPtr GetScratchGC(unsigned int depth, ScreenPtr pScreen)
{
    GCPtr pGC;
#ifdef MONOGC
    AltiaDevInfo *devinfo = altiaLibGetDevInfo();
    if (depth != 1)
        return NULL; /* in monochrome we only support depth of one */

    monoSaveBG = ((GCPtr)(devinfo->gc))->bgPixel;
    monoSaveFG = ((GCPtr)(devinfo->gc))->fgPixel;
    monoFillStyle = ((GCPtr)(devinfo->gc))->fillStyle;
    monoColorFlip = ((GCPtr)(devinfo->gc))->colorFlip;
    return (GCPtr)(devinfo->gc);
#else
    if (1 == depth)
    {
        pGC = utilityMonoGc;
    }
    else
    {
        pGC = utilityRootGc;
#ifdef FB_TRANS
        /* Alpha mask 8-bit buffers require transdraw flag */
        if (8 == depth)
        {
            pGC->transDraw = 1;
        }
        else
        {
            pGC->transDraw = 0;
        }
#endif /* FB_TRANS */
    }
    pGC->alu = GXcopy;
    pGC->planemask = ~0;
    pGC->serialNumber = 0;
    pGC->fgPixel = 0;
    pGC->bgPixel = 1;
    pGC->alpha = EGL_OPAQUE_ALPHA;
    pGC->lineWidth = 0;
    pGC->lineStyle = LineSolid;
    pGC->capStyle = CapButt;
    pGC->joinStyle = JoinMiter;
    pGC->fillStyle = FillSolid;
    pGC->fillRule = EvenOddRule;
    pGC->arcMode = ArcChord;
    pGC->patOrg.x = 0;
    pGC->patOrg.y = 0;
    pGC->subWindowMode = ClipByChildren;
    pGC->graphicsExposures = FALSE;
    pGC->clipOrg.x = 0;
    pGC->clipOrg.y = 0;
    if (pGC->clientClipType != CT_NONE)
    (*pGC->funcs->ChangeClip) (pGC, CT_NONE, NULL, 0);
    pGC->stateChanges = (1 << (GCLastBit+1)) - 1;
#if EGL_HAS_RASTER_TRANSFORMATION_LIB
    egl_RasterTransformClear((EGL_GC_ID)pGC);
#endif /* EGL_HAS_RASTER_TRANSFORMATION_LIB */
    pGC->stipple = (PixmapPtr)0x00;
    return pGC;
#endif
}

void FreeScratchGC(pGC)
GCPtr pGC;
{
#ifdef MONOGC
    pGC->bgPixel = monoSaveBG;
    pGC->fgPixel = monoSaveFG;
    pGC->fillStyle = monoFillStyle;
    pGC->colorFlip = monoColorFlip;
    pGC->stateChanges |= GCForeground | GCBackground | GCFillStyle;
#endif
}

/*****************************************************************
 * General initialization and management functions
 *****************************************************************/

/***************************************************************************/
EGL_STATUS egl_UpdateEnd(void)
{

    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_BOOL egl_UpdateBlocked(void)
{
    /* Return TRUE if egl_UpdateStart or egl_BatchStart would block, waiting
    ** on the hardware.
    */
    return EGL_FALSE;
}

/***************************************************************************/
EGL_STATUS egl_UpdateStart(void)
{

    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_BatchEnd(EGL_GC_ID gc)
{
    if (driver_StopGraphics(gc))
        return EGL_STATUS_OK;

    return EGL_STATUS_ERROR;
}

/***************************************************************************/
EGL_STATUS egl_BatchStart(EGL_GC_ID gc)
{
    if (driver_StartGraphics(gc))
    {

        return EGL_STATUS_OK;
    }

    return EGL_STATUS_ERROR;
}

/***************************************************************************/
EGL_STATUS egl_Deinitialize(void)
{
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_DriverFind (EGL_UINT32 devType, EGL_UINT32 instance,
                           EGL_UINT32 *pDeviceId)
{
    /* In Altia's usage, "instance" is passed as EGL_DISPLAY_TYPE,
     * EGL_EVENT_SERVICE_TYPE, or EGL_FONT_ENGINE_TYPE.
     */

    /* Return a safe value just to make Altia code happy. */
    *pDeviceId = 1;

    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_Info (EGL_DEVICE_ID devId, EGL_INFO_REQ infoRequest, 
                     void *pInfo, int layerNo)
{
    if (NULL == pInfo)
        return EGL_STATUS_ERROR;

    /* Only process information requests */
    if (infoRequest == EGL_MODE_INFO_REQ)
    {
        AltiaDevInfo *modeInfo = (AltiaDevInfo*) pInfo;


        modeInfo->colorDepth = eglScreen.rootDepth;
        modeInfo->drawable   = drawablePtr;
        modeInfo->display    = drawablePtr;

    }

    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_Initialize(void)
{
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_FinishInit(EGL_DEVICE_ID devId)
{
    driver_finishInit();
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_BackgroundColorSet(EGL_GC_ID gc, EGL_COLOR colorRef)
{
    GCPtr pGC = (GCPtr)gc;

    if (pGC->colorFlip)
    { /* We had a color flip, Before we set a color we have to undo the
       * flip so we keep the colors straight. Color flips are used 
       * for background fills.
       */
        unsigned long tempPixel;
        pGC->colorFlip = 0;
        tempPixel = pGC->fgPixel;
        pGC->fgPixel = pGC->bgPixel;
        pGC->bgPixel = tempPixel;
        pGC->stateChanges |= GCForeground|GCBackground;
    }
    if (pGC->bgPixel != colorRef)
    {
        pGC->bgPixel = colorRef;
        pGC->stateChanges |= GCBackground;
    }
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_FillPatternSet(EGL_GC_ID gc, EGL_MDDB_ID patternBitmap)
{
    GCPtr pGC = (GCPtr)gc;
    /* In Altia's usage, "patternBitmap" is passed as NULL or as a
     * monochrome device dependent bitmap id.
     */

    if (patternBitmap == NULL || patternBitmap == (EGL_MDDB_ID) 1)
    {
        if (pGC->fillStyle != FillSolid)
        {
            pGC->fillStyle = FillSolid;
            pGC->stateChanges |= GCFillStyle;
        }
    }
    else
    {
        if (pGC->fillStyle != FillOpaqueStippled)
        {
            pGC->fillStyle = FillOpaqueStippled;
            pGC->stateChanges |= GCFillStyle;
        }
        if (pGC->stipple != (PixmapPtr)patternBitmap)
        {
            pGC->stipple = (PixmapPtr)patternBitmap;
            pGC->stateChanges |= GCStipple;
        }
    }

    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_ForegroundColorSet(EGL_GC_ID gc, EGL_COLOR colorRef)
{
    GCPtr pGC = (GCPtr)gc;

    if (pGC->colorFlip)
    { /* We had a color flip, Before we set a color we have to undo the
       * flip so we keep the colors straight. Color flips are used 
       * for background fills.
       */
        unsigned long tempPixel;
        pGC->colorFlip = 0;
        tempPixel = pGC->fgPixel;
        pGC->fgPixel = pGC->bgPixel;
        pGC->bgPixel = tempPixel;
        pGC->stateChanges |= GCForeground|GCBackground;
    }
    if (pGC->fgPixel != colorRef)
    {
        pGC->fgPixel = colorRef;
        pGC->stateChanges |= GCForeground;
    }
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_AlphaChannelSet(EGL_GC_ID gc, EGL_INT16 alpha)
{
    GCPtr pGC = (GCPtr)gc;
    EGL_UINT8 _alpha;

    if(alpha < EGL_TRANSPARENT_ALPHA || alpha > EGL_OPAQUE_ALPHA)
        _alpha = (EGL_UINT8)EGL_OPAQUE_ALPHA;
    else
        _alpha = (EGL_UINT8)alpha;

    if(pGC->alpha != _alpha)
    {
        pGC->alpha = _alpha;
        pGC->stateChanges |= GCAlphaChannel;
    }
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_ClearDC(EGL_GC_ID gc)
{
    GCPtr pGC = (GCPtr)gc;
    /* We have to clear out the pattern fill since we could leave it
     * set after a pattern fill and then do a blt (which would
     * not set it again).
     */
    if (pGC->fillStyle != FillSolid)
    {
        pGC->fillStyle = FillSolid;
        pGC->stateChanges |= GCFillStyle;
    }
    return EGL_STATUS_OK;
}

/*****************************************************************
 * Color management functions
 *****************************************************************/


/***************************************************************************/
EGL_STATUS egl_ColorAlloc(EGL_DEVICE_ID devId, EGL_ARGB *pAllocColors, 
                          EGL_ORD *pClutIndexes,
                          EGL_COLOR *pUglColors,
                          EGL_SIZE numColors)
{
   
    return driver_ColorAlloc(devId, SCREENPTR, pAllocColors, pClutIndexes, 
                             pUglColors, numColors);
}

/***************************************************************************/
EGL_STATUS egl_ColorConvert (EGL_DEVICE_ID devId, void *sourceArray, 
                             EGL_COLOR_FORMAT sourceFormat,
                                    void *destArray, 
                                    EGL_COLOR_FORMAT destFormat,
                                    EGL_SIZE arraySize)
{
    /* Altia uses this function to convert a source array of colors
     * in EGL_DEVICE_COLOR_32 color format to a destination array of
     * colors in EGL_RGB888 color format.  It does this because the
     * EGL_DEVICE_COLOR_32 format isn't public.  Whereas the EGL_RGB888
     * format is a simple array of 8-bit bytes where each set of 3 bytes
     * gives the red, green, and blue component for a color.
     */
    return EGL_STATUS_OK;
}



/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_RasterModeSet(EGL_GC_ID gc, EGL_RASTER_OP rasterOp)
 *
 * Sets the raster mode for the given gc.  The raster mode affects
 * how the colors of pixels are updated for almost all drawing operations
 * involving the given gc.
 *
 * The rasterOp argument can have one of 3 values as defined by the
 * following macros:
 *
 *     #define EGL_RASTER_OP_COPY       0x02020101L
 *     #define EGL_RASTER_OP_AND        0x00020002L
 *     #define EGL_RASTER_OP_OR         0x00020003L
 *     #define EGL_RASTER_OP_XOR        0x00020004L
 *
 * When the raster mode is EGL_RASTER_OP_COPY, new colors for pixels
 * completely overwrite the existing pixel colors for each drawing
 * operation (e.g., line draw, rectangle draw, bitmap blit, etc.).
 *
 * When the raster mode is EGL_RASTER_OP_AND, new colors for pixels
 * are ANDed with the existing pixel colors for each drawing operation.
 *
 * When the raster mode is EGL_RASTER_OP_OR, new colors for pixels
 * are ORed with the existing pixel colors for each drawing operation.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 * NOTE:  Altia passes rasterOp as EGL_RASTER_OP_COPY most of the time.
 *        However, for rendering Altia Stencils (monochrome bitmaps) and
 *        Rasters (color bitmaps) with transparent bits, this function is
 *        called with rasterOp sometimes set to EGL_RASTER_OP_AND and
 *        also EGL_RASTER_OP_OR.
 * 
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_RasterModeSet(EGL_GC_ID gc, EGL_RASTER_OP rasterOp)
{
    GCPtr pGC = (GCPtr)gc;
    unsigned char alu = GXcopy;
    if (rasterOp == EGL_RASTER_OP_AND)
        alu = GXand;
    else if (rasterOp == EGL_RASTER_OP_OR)
        alu = GXor;
    else if (rasterOp == EGL_RASTER_OP_XOR)
        alu = GXxor;
    if (pGC->alu != alu)
    {
        pGC->alu = alu;
        pGC->stateChanges |= GCFunction;
    }

    return EGL_STATUS_OK;
}

/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_ClipRectSet(EGL_GC_ID gc, EGL_POS left, EGL_POS top,
 *                             EGL_POS right, EGL_POS bottom)
 *
 * Sets the clip rectangle for the given GC.  Any future drawing
 * operations (e.g., lines, rectangles, text, bitmaps) performed with
 * this GC must account for the clip rectangle.  If a pixel affected by
 * the drawing operation is located outside of the clip rectangle, the
 * pixel must not change.
 *
 * The clipping rectangle includes the coordinates of its boundaries.
 * That is to say, any pixels affected by a drawing operation that are
 * on the left, top, right, or bottom coordinates of the clip rectangle
 * are rendered.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_ClipRectSet(EGL_GC_ID gc, EGL_POS left, EGL_POS top,
                           EGL_POS right, EGL_POS bottom)
{
    xRectangle rectangle;
    GCPtr pGC = (GCPtr)gc;

    /* If this is a request to set the clip rectangle to the same
     * clip rectangle last requested for the identical GC, we don't
     * actually need to do anything.
     */
    if (pGC == lastClipGc && left == lastClipLeft && top == lastClipTop
        && right == lastClipRight && bottom == lastClipBottom)
    {
#ifdef EGL_DEBUG
        printf("CLIP RECTANGLE UNCHANGED!\n");
#endif
        return EGL_STATUS_OK;
    }

    lastClipGc = pGC;
    lastClipLeft = left;
    lastClipTop = top;
    lastClipRight = right;
    lastClipBottom = bottom;

    rectangle.width = right - left + 1;
    rectangle.height = bottom - top + 1;
    rectangle.x = left;
    rectangle.y = top;

#ifdef EGL_DEBUG
    printf("Setting clip rectangle to x,y = %d,%d HxW = %d x %d\n",
           rectangle.x, rectangle.y, rectangle.width, rectangle.height);
#endif

    pGC->serialNumber |= GC_CHANGE_SERIAL_BIT;

    (*pGC->funcs->ChangeClip)(pGC, CT_UNSORTED, (pointer)&rectangle, 1);

    return EGL_STATUS_OK;
}

#if EGL_HAS_DEVICEOPEN
/*-------------------------------------------------------------------------
 *  EGL_DEVICE_ID egl_DeviceOpen(EGL_CHAR *pName)
 *
 * Initializes the target graphics library and opens the display device
 * with the name given by the text pointed to by pName.
 *
 * Returns a non-zero device ID of type EGL_DEVICE_ID if successful.
 * Otherwise, return (EGL_DEVICE_ID) 0.
 *
 * This function is only referenced by the Altia code if EGL_HAS_DEVICEOPEN
 * is non-zero (1) as defined in "egl_Wrapper.h".  This implies that the
 * target graphics library can support initializing itself and opening the
 * display device with this single function.  Similarly, it can close the
 * display device and uninitialize itself with the single function
 * egl_DeviceClose().
 *
 * If EGL_HAS_DEVICEOPEN is defined as 0 in "egl_Wrapper.h", the Altia
 * code calls egl_Initialize() first to initialize the target graphics
 * library.  It then calls egl_DriverFind() to find a display device,
 * calls it again to find an event service device, and calls it one
 * more time to find a font engine device.  Finally, it calls
 * egl_EventQCreate() to create an event queue for the event service
 * device.  To close and uninitialize everything, the Altia code calls
 * egl_EventQDestroy to destroy the event queue and then it calls
 * egl_Deinitialize() to uninitialize the graphics library.  As is
 * apparent, this is excessive overhead for a target graphics library
 * that can easily open and close the display device.  Hence, having
 * EGL_HAS_DEVICEOPEN defined as non-zero (1) is preferred and it is
 * the only approach demonstrated here.
 *
 *-------------------------------------------------------------------------*/
EGL_DEVICE_ID egl_DeviceOpen(EGL_CHAR *pName)
{
#ifdef EGL_DEBUG
    printf("Into egl_DeviceOpen():  initing graphics system...\n");
#endif
    /* Default screen is the current */
    eglScreenPtr = &eglScreen;

    xmemset(&eglScreen, 0, sizeof(ScreenRec));
    xmemset(&eglScreenInfo, 0, sizeof(ScreenInfo));

    eglScreenInfo.screen = &eglScreen;
    eglScreen.totalGCSize = ((sizeof(GC) + sizeof(long) - 1) / sizeof(long)) * sizeof(long);
    fbAllocatePrivates(&eglScreen, (int*) 0);

    if (driver_open(pName, &eglScreenInfo, &eglScreen))
    {
        int scanlinepad, format, depth, bitsPerPixel, j, k;

        for (format = 0; format < eglScreenInfo.numPixmapFormats; format++)
        {
            depth = eglScreenInfo.formats[format].depth;
            bitsPerPixel = eglScreenInfo.formats[format].bitsPerPixel;
            scanlinepad = eglScreenInfo.formats[format].scanlinePad;
            j = indexForBitsPerPixel[ bitsPerPixel ];
            k = indexForScanlinePad[ scanlinepad ];
            PixmapWidthPaddingInfo[ depth ].padPixelsLog2 = answer[j][k];
            PixmapWidthPaddingInfo[ depth ].padRoundUp = (scanlinepad/bitsPerPixel) - 1;
            j = indexForBitsPerPixel[ 8 ]; /* bits per byte */
            PixmapWidthPaddingInfo[ depth ].padBytesLog2 = answer[j][k];
            PixmapWidthPaddingInfo[ depth ].bitsPerPixel = bitsPerPixel;
            if (answerBytesPerPixel[bitsPerPixel])
            {
                PixmapWidthPaddingInfo[ depth ].notPower2 = 1;
                PixmapWidthPaddingInfo[ depth ].bytesPerPixel = answerBytesPerPixel[bitsPerPixel];
            }
            else
            {
                PixmapWidthPaddingInfo[ depth ].notPower2 = 0;
            }
        }
#ifdef USE_FB
        miCreateScreenResources(&eglScreen);
        drawablePtr = eglScreen.devPrivate;
        drawablePtr->x = eglScreen.xoff;
        drawablePtr->y = eglScreen.yoff;
        drawablePtr->type = DRAWABLE_BUFFER;
    #if EGL_DO_ALPHA_BLENDING
        if (-1 == fbInitAlphaBlending(
                &eglScreenInfo,
                (EGL_UINT32)altiaLibGetColor((ALTIA_WINDOW)altiaLibGetDevInfo(), 
                                             (ALTIA_COLOR)EGL_ADD_ALPHA(ALTIA_RED, EGL_OPAQUE_ALPHA)),
                (EGL_UINT32)altiaLibGetColor((ALTIA_WINDOW)altiaLibGetDevInfo(), 
                                             (ALTIA_COLOR)EGL_ADD_ALPHA(ALTIA_GREEN, EGL_OPAQUE_ALPHA)),
                (EGL_UINT32)altiaLibGetColor((ALTIA_WINDOW)altiaLibGetDevInfo(), 
                                             (ALTIA_COLOR)EGL_ADD_ALPHA(ALTIA_BLUE, EGL_OPAQUE_ALPHA))))
        {
            _altiaErrorMessage(ALT_TEXT("8-bit pixmap support required for alpha blending."));
            return (EGL_DEVICE_ID) 0;
        }
    #endif /* EGL_DO_ALPHA_BLENDING */
#else
        drawable.depth = eglScreen.rootDepth;
        drawable.bitsPerPixel = BitsPerPixel(eglScreen.rootDepth);
        drawable.x = eglScreen.xoff;
        drawable.y = eglScreen.yoff;
        drawable.width = eglScreen.width;
        drawable.height = eglScreen.height;
        drawable.pScreen = &eglScreen;
        drawable.type = DRAWABLE_BUFFER;
        drawablePtr = &drawable;
#endif /* USE_FB */

#ifndef MONOGC
        if (drawablePtr->depth > 1)
            utilityRootGc = egl_GcCreate((EGL_DEVICE_ID)1, drawablePtr->depth);
        utilityMonoGc = egl_GcCreate((EGL_DEVICE_ID)1, 1);
#endif


        /* Font Initialization (if necessary such as for FreeType runtime) */
        if (EGL_STATUS_ERROR == egl_FontOpen())
            return (EGL_DEVICE_ID)0;

        /* Success */
        return (EGL_DEVICE_ID)1;
    }

    return (EGL_DEVICE_ID)0;
}
#endif /* EGL_HAS_DEVICEOPEN */

#if EGL_HAS_DEVICEOPEN
/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_DeviceClose(EGL_DEVICE_ID devId)
 *
 * Closes the display device identified by devId and uninitializes the
 * target graphics library if necessary.  This should free all data
 * structures created by the graphics library or the code in this file.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 * This function is only referenced by the Altia code if EGL_HAS_DEVICEOPEN
 * is non-zero (1) as defined in "egl_Wrapper.h".  This implies that the
 * target graphics library can support initializing itself and opening the
 * display device with this single function.  Similarly, it can close the
 * display device and uninitialize itself with the single function
 * egl_DeviceClose().
 *
 * If EGL_HAS_DEVICEOPEN is defined as 0 in "egl_Wrapper.h", the Altia
 * code calls egl_Initialize() first to initialize the target graphics
 * library.  It then calls egl_DriverFind() to find a display device,
 * calls it again to find an event service device, and calls it one
 * more time to find a font engine device.  Finally, it calls
 * egl_EventQCreate() to create an event queue for the event service
 * device.  To close and uninitialize everything, the Altia code calls
 * egl_EventQDestroy to destroy the event queue and then it calls
 * egl_Deinitialize() to uninitialize the graphics library.  As is
 * apparent, this is excessive overhead for a target graphics library
 * that can easily open and close the display device.  Hence, having
 * EGL_HAS_DEVICEOPEN defined as non-zero (1) is preferred and it is
 * the only approach demonstrated here.
 *
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_DeviceClose(EGL_DEVICE_ID devId)
{
#ifdef EGL_DEBUG
    printf("Into egl_DeviceClose():  closing and freeing resources...\n");
#endif
    /* Default screen is the current */
    eglScreenPtr = &eglScreen;

    /* Font termination (if necessary such as for FreeType runtime) */
    egl_FontClose();

    if (driver_close())
    {
        if (utilityRootGc != NULL)
            egl_GcDestroy(utilityRootGc);
        if (utilityMonoGc != NULL)
            egl_GcDestroy(utilityMonoGc);
#ifdef USE_FB
        miCloseScreen(&eglScreen);
#endif
        ResetScreenPrivates(&eglScreen);
        ResetGCPrivates(&eglScreen);
#if EGL_DO_ALPHA_BLENDING
        CleanupScratchPixmaps();
#endif

#ifdef EXPANDED_VIRTUAL_MEMORY
        driver_free();
#endif
        /* Success */
        return (EGL_STATUS_OK);
    }

    return EGL_STATUS_ERROR;
}
#endif /* EGL_HAS_DEVICEOPEN */


/* CreateGC(pDrawable, mask, pval, pStatus)
   creates a default GC for the given drawable, using mask to fill
   in any non-default values.
   Returns a pointer to the new GC on success, NULL otherwise.
   returns status of non-default fields in pStatus
BUG:
   should check for failure to create default tile

*/

static GCPtr AllocateGC(ScreenPtr pScreen)
{
    GCPtr pGC;
    register char *ptr;
    register DevUnion *ppriv;
    register unsigned *sizes;
    register unsigned size;
    register int i;

    pGC = (GCPtr)xalloc(pScreen->totalGCSize);
    if (pGC)
    {
        ppriv = (DevUnion *)(pGC + 1);
        pGC->devPrivates = ppriv;
        sizes = pScreen->GCPrivateSizes;
        ptr = (char *)(ppriv + pScreen->GCPrivateLen);
        for (i = pScreen->GCPrivateLen; --i >= 0; ppriv++, sizes++)
        {
            if ( (size = *sizes) )
            {
            ppriv->ptr = (pointer)ptr;
            ptr += size;
            }
            else
            ppriv->ptr = (pointer)NULL;
        }
    }
    return pGC;
}

GCPtr CreateGC(ScreenPtr pScreen, int depth)
{
    register GCPtr pGC;

    pGC = AllocateGC(pScreen);
    if (!pGC)
    {
        return (GCPtr)NULL;
    }

    pGC->pScreen = pScreen;
    pGC->depth = depth;
    pGC->alu = GXcopy; /* dst <- src */
    pGC->planemask = ~0;
    pGC->serialNumber = GC_CHANGE_SERIAL_BIT;
    pGC->funcs = 0;

    pGC->fgPixel = 0;
    pGC->bgPixel = 1;
    pGC->alpha = EGL_OPAQUE_ALPHA;
    pGC->lineWidth = 0;
    pGC->lineStyle = LineSolid;
    pGC->capStyle = CapButt;
    pGC->joinStyle = JoinMiter;
    pGC->fillStyle = FillSolid;
    pGC->fillRule = EvenOddRule;
    pGC->arcMode = ArcPieSlice;
    pGC->colorFlip = 0;
    pGC->transDraw = 0;
    pGC->tileIsPixel = TRUE;
    pGC->tile.pixel = 0;

    pGC->patOrg.x = 0;
    pGC->patOrg.y = 0;
    pGC->subWindowMode = ClipByChildren;
    pGC->graphicsExposures = TRUE;
    pGC->clipOrg.x = 0;
    pGC->clipOrg.y = 0;
    pGC->clientClipType = CT_NONE;
    pGC->clientClip = (pointer)NULL;
    pGC->numInDashList = 2;
    pGC->dash = 0;    
    pGC->dashOffset = 0;
    pGC->lastWinOrg.x = 0;
    pGC->lastWinOrg.y = 0;

    /* use the default font and stipple */
    pGC->font = 0; 
    pGC->stipple = pScreen->PixmapPerDepth[0];

    pGC->stateChanges = (1 << (GCLastBit+1)) - 1;
#if EGL_HAS_RASTER_TRANSFORMATION_LIB
    egl_RasterTransformClear((EGL_GC_ID)pGC);
#endif /* EGL_HAS_RASTER_TRANSFORMATION_LIB */
    return (pGC);
}

/*-------------------------------------------------------------------------
 * EGL_SIZE egl_GetDepth(EGL_DEVICE_ID)
 *
 * Return the depth of the device
 *-------------------------------------------------------------------------*/
EGL_SIZE egl_GetDepth(EGL_DEVICE_ID devId)
{
    return (EGL_SIZE)SCREENPTR->rootDepth;
}

/*-------------------------------------------------------------------------
 *  EGL_GC_ID egl_GcCreate(EGL_DEVICE_ID devId, int depth)
 *
 * Creates a graphics context (GC) for the device given by devId and
 * returns an identification value for the GC to be used in other calls
 * to the graphics library.
 *
 * Depending on the graphics library's implementation of this function, an
 * EGL_GC_ID may be a simple data type such as int or a pointer to
 * some data structure whose contents are private to the graphics library.
 *
 * A complete graphics library should support the existence of multiple
 * graphics contexts.  This is required by the Altia code only for
 * scaled/rotated/stretched text.  If scaled/rotated/stretched text isn't
 * going to be supported, a graphics library can support just one
 * graphics context.
 *
 * If the graphics context cannot be created, this function should return
 * (EGL_GC_ID) 0.
 *
 * A graphics context holds attributes for future drawing operations
 * that use the graphics context.  The attributes are:
 *
 *  foreground color      Gives the color for drawing lines or outlines
 *                        around closed vector objects like rectangles.
 *                        Set with egl_ForegroundColorSet().
 *
 *  background color      Gives the color for drawing filled vector objects
 *                        like rectangles.  Set with egl_BackgroundColorSet().
 *
 *  line style            Gives the line style, solid or one of a variety
 *                        of dash styles, for drawing lines or outlines
 *                        around closed vector objects like rectangles.
 *                        Set with egl_LineStyleSet().
 *
 *  line width            Gives the line width in pixels, 0 to at most 100,
 *                        for drawing lines or outlines around closed vector
 *                        objects like rectangles.  Set with
 *                        egl_LineWidthSet().
 *
 *  fill pattern          Gives the fill pattern for drawing filled vector
 *                        objects like rectangles.  Set with
 *                        egl_FillPatternSet().
 *
 *  default bitmap        Gives the target of drawing operations if it
 *                        is not the actual display but instead other video
 *                        memory (for example, to do double-buffered drawing).
 *                        Set with egl_DefaultBitmapSet().
 *
 *  raster mode           Determines whether pixels in the target affected
 *                        by a drawing operation are replaced, ANDed with
 *                        new data, or ORed with new data.
 *                        Set with egl_RasterModeSet().
 *
 *  font                  Gives the font for drawing text.  Set with
 *                        egl_FontSet().
 *
 *  clipping rectangle    Defines the rectangular area of the target that
 *                        should only be affected by drawing operations.
 *                        Set with egl_ClipRectSet().
 *
 *  clipping region       Defines multiple rectangular areas of the target
 *                        that should only be affected by drawing operations.
 *                        The intersection of these regions with the clipping
 *                        rectangle determine the actual rectangular areas of
 *                        the target that should only be affected by drawing
 *                        operations.  Clipping regions are only needed if
 *                        scaled/rotated/stretched Rasters, Stencils, or
 *                        text support is required.  Set with
 *                        egl_ClipRegionSet() or egl_PolyRegionSet().
 *
 *-------------------------------------------------------------------------*/
EGL_GC_ID egl_GcCreate(EGL_DEVICE_ID devId, int depth)
{
    GCPtr pGC = CreateGC(SCREENPTR, depth);

#ifdef USE_FB
    fbGetRotatedPixmap(pGC) = 0;
    fbGetExpose(pGC) = 1;
    fbGetFreeCompClip(pGC) = 0;
    fbGetCompositeClip(pGC) = 0;
    fbGetGCPrivate(pGC)->bpp = BitsPerPixel (pGC->depth);
#endif

    pGC->clientClip = NULL;
    pGC->clientClipType = CT_NONE;
    driver_createGC(pGC);
    return pGC;
}
/*-------------------------------------------------------------------------
 *  EGL_GC_ID egl_GcDestroy(EGL_GC_ID gc)
 *
 * Destroy the graphics context given by gc.  This should free all data
 * structures created by the gc.
 *
 * The calling code should not use the value of gc in any other graphics
 * library calls that require an EGL_GC_ID.  To do so may cause a memory
 * access error.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_GcDestroy(EGL_GC_ID gc)
{
    /* If this was the last GC we drew with, reset lastGc
     * so that none of the last attributes (e.g., foreground,
     * background, fill, etc.) are valid.  This is extremely
     * important since the next GC we create may get the
     * same GC value and we can be duped into thinking that
     * the attributes don't need to be updated.
     */
    if (gc == lastGc)
        lastGc = (GCPtr) 0;

    driver_destroyGC((GCPtr)gc);
    miDestroyGC((GCPtr)gc);
    xfree(gc);

    return EGL_STATUS_OK;
}

/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_LineStyleSet(EGL_GC_ID gc, EGL_LINE_STYLE lineStyle)
 *
 * Sets the current line style for the given gc.
 *
 * In Altia's usage, "lineStyle" is only passed as EGL_LINE_STYLE_SOLID
 * (0xffffffff).
 *
 * Dashed line styles are done directly by the Altia code to guarantee
 * consistent appearance across platforms.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_LineStyleSet(EGL_GC_ID gc, EGL_LINE_STYLE lineStyle)
{
    GCPtr pGC = (GCPtr)gc;
    /* For better performance on this target, don't set the line
     * style just yet.  Instead, just save the style for later inspection.
     */
    if (lineStyle == EGL_LINE_STYLE_SOLID)
    {
        if (pGC->lineStyle != LineSolid)
    {
       pGC->lineStyle = LineSolid;
       pGC->stateChanges |= GCLineStyle; 
    }

#ifdef EGL_DEBUG
    printf("Setting line style to solid, last width was %d\n",
           nextLineWidth);
#endif
    }
#ifdef EGL_DEBUG
    else
        printf("egl_FillLineStyleSet:  Non-solid line unsupported\n");
#endif

    return EGL_STATUS_OK;
}


/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_LineWidthSet(EGL_GC_ID gc, EGL_SIZE lineWidth)
 *
 * Sets the current pixel width of lines for the given gc to the value
 * given by lineWidth which should be in the range 0 to 100.
 *
 * In Altia's usage, "lineWidth" is 0 (no line), 1, to 100.
 *
 * Note:  To simply disable drawing of lines, set a gc's line width
 *        to 0 and its foreground color to EGL_COLOR_TRANSPARENT
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_LineWidthSet(EGL_GC_ID gc, EGL_SIZE lineWidth)
{
    GCPtr pGC = (GCPtr)gc;
    if (lineWidth <= 100 && lineWidth >= 0)
    {
#ifdef EGL_DEBUG
        printf("Setting line width to %d, last style was solid\n", lineWidth);
#endif
        /* line width of zero looks same as line width of one
         * but it draws much faster
         */
        if (lineWidth < 1)
            lineWidth = 0;

        if (pGC->lineWidth != (unsigned short)lineWidth)
        {
            pGC->lineWidth = (unsigned short)lineWidth;
            pGC->stateChanges |= GCLineWidth;
        }
    }
#ifdef EGL_DEBUG
    else
        printf("egl_LineWidthSet:  %d line width unsupported\n", lineWidth);
#endif
    return EGL_STATUS_OK;
}
/*-------------------------------------------------------------------------
 *  void local_GcAttributesSet(ALTIA_WINDOW win, GC gc, int filled, int text)
 *
 * Local utility function to set the attributes for the given GC.
 *
 * To minimize how often the GC attributes are changed, the code for this
 * target uses variables to hold the last settings and the next settings.
 * Prior to each draw operation, the last setting and next setting for each
 * attribute is compared and the attribute is only set if absolutely
 * necessary.  This significantly improves performance for graphics
 * libraries such as X11 where a message must go to the server each time
 * an attribute is set.
 *-------------------------------------------------------------------------*/
void local_GcAttributesSet(ALTIA_WINDOW win, GCPtr pGC, int filled, int text)
{
    BOOL change = FALSE;

    if (pGC != lastGc)
        change = TRUE;

    if (filled)
    {
        /* Getting ready to draw a filled object */

        if (pGC->fillStyle == FillSolid)
        {
            /* We have to flip foreground and background since
             * our graphics code wants to fill with the foreground
             * color while Altia wants to fill with the background.
             */
            if (pGC->colorFlip == 0 && pGC->fgPixel != pGC->bgPixel)
            {
                /* The colors are not flipped but we are going to flip 
                * them now!
                */
                unsigned long tempPixel;

                pGC->colorFlip = 1;
                tempPixel = pGC->fgPixel;
                pGC->fgPixel = pGC->bgPixel;
                pGC->bgPixel = tempPixel;
                pGC->stateChanges |= GCForeground|GCBackground;
            }
        }
    }
    else
    {
        /* Not filled */
        if (pGC->colorFlip)
        {
            /* We had to flip foreground and background since
             * our graphics code wants to fill with the foreground
             * color while Altia wants to fill with the background.
             * Since we are no longer doing a fill we need to put
             * the colors back.
             */
            if (pGC->fgPixel != pGC->bgPixel)
            {
                unsigned long tempPixel = pGC->fgPixel;
                pGC->fgPixel = pGC->bgPixel;
                pGC->bgPixel = tempPixel;
                pGC->stateChanges |= GCForeground | GCBackground;
            }
            pGC->colorFlip = 0;
        }
    }
    lastGc = pGC;
    if (change || (pGC->stateChanges != 0) ||
        lastDrawable != DRAWABLE(win)) 
    {
        (*(pGC->funcs->ValidateGC))(pGC, pGC->stateChanges, DRAWABLE(win));
    }
    pGC->stateChanges = 0;
    lastDrawable = DRAWABLE(win);
}

/* These functions are so the old x code works most code
 * calls the functions directly via the pGC func
 */
void ValidateGC(DrawablePtr pDraw, GC *pGC)
{
    (*pGC->funcs->ValidateGC)(pGC, pGC->stateChanges, pDraw);
    pGC->stateChanges = 0;
}

#if EGL_DO_ALPHA_BLENDING
void MergeAlphaPixmaps (PixmapPtr src, PixmapPtr dst)
{
    void * srcBits, * dstBits;
    CARD8 * s, * d, * sp8, * dp8;
    FbStride stride;
    int xoff, yoff, bpp;
    int height, width;

    /*
     * Pixmaps have to be the same width and height and have a
     * color depth of 8.
     */
    if((src->drawable.width != dst->drawable.width) ||
        (src->drawable.height != dst->drawable.height) ||
        (src->drawable.depth != 8) || (dst->drawable.depth != 8))
    {
        return;
    }

    fbGetDrawable((DrawablePtr)src, srcBits, stride, bpp, xoff, yoff);
    fbGetDrawable((DrawablePtr)dst, dstBits, stride, bpp, xoff, yoff);

    stride = stride * sizeof(FbBits);

    sp8 = ((CARD8 *)srcBits);
    sp8 += (stride * yoff) + xoff;

    dp8 = ((CARD8 *)dstBits);
    dp8 += (stride * yoff) + xoff;

    height = src->drawable.height;

    while(height--)
    {
        s = sp8;
        d = dp8;

        sp8 += stride;
        dp8 += stride;

        width = src->drawable.width;

        while(width--)
        {
            *d = ((*s) * (*d)) / 255;
            d++;
            s++;
        }
    }
}

PixmapPtr GetScratchPixmap (int width, int height)
{
    if(scratchPixmap != (PixmapPtr) 0)
    {
        /*
         * We already have a pixmap created so check to see if it's the same
         * size.  If so, reuse it, otherwise free it and create a new one.
         */
        if ((scratchPixmap->drawable.width != width)   ||
            (scratchPixmap->drawable.height != height) ||
            (scratchPixmap->drawable.depth != SCREENPTR->rootDepth))
        {
            (*eglScreen.DestroyPixmap)(scratchPixmap);
            scratchPixmap = (PixmapPtr) 0;
        }
    }

    if (scratchPixmap == (PixmapPtr) 0)
    {
        /* Create a pixmap with the same color depth as the display */
        scratchPixmap = (*eglScreen.CreatePixmap)(SCREENPTR, width, height, SCREENPTR->rootDepth);
    }

    return scratchPixmap;
}

PixmapPtr GetScratchAlphaPixmap (int width, int height, int initValue)
{
    void * alphas;
    CARD8 *p8;
    FbStride alphasStride;
    int alphasXoff, alphasYoff;
    int alphasBpp;

    if (scratchAlphaPixmap != (PixmapPtr) 0)
    {
        /*
         * We already have a pixmap created so check to see if it's the same
         * size.  If so, reuse it, otherwise free it and create a new one.
         */
        if(scratchAlphaPixmap->drawable.width != width ||
            scratchAlphaPixmap->drawable.height != height)
        {
            (*eglScreen.DestroyPixmap)(scratchAlphaPixmap);
            scratchAlphaPixmap = (PixmapPtr) 0;
        }
    }

    if(scratchAlphaPixmap == (PixmapPtr) 0)
    {
        /*
         * Create a pixmap with a color depth of 8 to hold alpha values.
         */
        scratchAlphaPixmap = (*eglScreen.CreatePixmap)(SCREENPTR, width, height, 8);
        if (scratchAlphaPixmap == (PixmapPtr) 0)
            return (PixmapPtr)0;
    }

    /*
     * Initialize the pixmap
     */
    fbGetDrawable((DrawablePtr)scratchAlphaPixmap,
                   alphas, alphasStride, alphasBpp, alphasXoff, alphasYoff);

    alphasStride *= sizeof(FbBits);

    p8 = ((CARD8 *)alphas);
    p8 += (alphasStride * alphasYoff) + alphasXoff;
    alphas = p8;

    xmemset((CARD8 *)alphas, initValue, (alphasStride *
            scratchAlphaPixmap->drawable.height));

    return scratchAlphaPixmap;
}

void CleanupScratchPixmaps(void)
{
    if(scratchPixmap != (PixmapPtr) 0)
    {
        (*eglScreen.DestroyPixmap)(scratchPixmap);
        scratchPixmap = (PixmapPtr) 0;
    }

    if(scratchAlphaPixmap != (PixmapPtr) 0)
    {
        (*eglScreen.DestroyPixmap)(scratchAlphaPixmap);
        scratchAlphaPixmap = (PixmapPtr) 0;
    }
}
#endif /* EGL_DO_ALPHA_BLENDING */



