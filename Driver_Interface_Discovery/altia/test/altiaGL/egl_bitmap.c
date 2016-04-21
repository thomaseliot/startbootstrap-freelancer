/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.32 $    $Date: 2010-02-17 01:03:57 $
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

/** FILE:  bitmap.c ****************************************************
 **
 ** This file contains the bitmap functions for a general purpose
 ** graphics library.
 **
 ***************************************************************************/

#include "egl_Wrapper.h"
#include "Xprotostr.h"
#include "gc.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#ifdef USE_FB
#include "fb.h"
#endif

#include <stdio.h>

/* Windows only */
#if defined(WIN32) && !defined(MICROWIN)
#pragma warning( disable: 4244 )
#endif

#define DRAWABLE(win) ((DrawableRec*)((AltiaDevInfo*)win)->drawable)

extern EGL_STATUS driver_ColorAlloc(EGL_DEVICE_ID devId, ScreenPtr pScreen,
                                 EGL_ARGB *pAllocColors,
                                 EGL_ORD *pIndex, EGL_COLOR *pEglColors,
                                 EGL_SIZE numColors);



/* This global indicates if the SetSpan function is  able to do a stipple
 * fill. If the user has a span driver he can set it as required.  Since
 * the frame buffer's SetSpan function does not support this we initialize
 * it to a 0.  The user's functions can override this as required.
 */
int altiaStippleSpan = 0;

PixmapPtr
AllocatePixmap(pScreen, pixDataSize)
    ScreenPtr pScreen;
    int pixDataSize;
{
    PixmapPtr pPixmap;
#ifdef PIXPRIV
    char *ptr;
    DevUnion *ppriv;
    unsigned *sizes;
    unsigned size;
    int i;

    pPixmap = (PixmapPtr)xalloc(pScreen->totalPixmapSize + pixDataSize);
    if (!pPixmap)
        return NullPixmap;
    ppriv = (DevUnion *)(pPixmap + 1);
    pPixmap->devPrivates = ppriv;
    sizes = pScreen->PixmapPrivateSizes;
    ptr = (char *)(ppriv + pScreen->PixmapPrivateLen);
    for (i = pScreen->PixmapPrivateLen; --i >= 0; ppriv++, sizes++)
    {
        if ((size = *sizes) != 0)
        {
            ppriv->ptr = (pointer)ptr;
            ptr += size;
        }
        else
            ppriv->ptr = (pointer)NULL;
    }
#else
    pPixmap = (PixmapPtr)xalloc(sizeof(PixmapRec) + pixDataSize);
#endif
    return pPixmap;
}

/*****************************************************************
 * Color bitmap (raster), color bitmap transparency (raster with
 * transparent bits), and monochrome bitmap (stencil) rendering functions.
 *
 * In addition, an offscreen bitmap is used for double-buffering.  This
 * requires the use of egl_BitmapBlt(), egl_BitmapCreate(),
 * egl_BitmapDestroy(),  and egl_DefaultBitmapSet().
 *****************************************************************/

#ifdef USE_FB
#ifdef FB_24BIT
#define FbDoTypeStore(b,t,x,s)  (*((t *) (b)) = (x) >> (s))
#if BITMAP_BIT_ORDER == LSBFirst
#define BITSSTORE(b,x)  ((unsigned long) (b) & 1 ? \
    (FbDoTypeStore (b, CARD8, x, 0), \
     FbDoTypeStore ((b) + 1, CARD16, x, 8)) : \
    (FbDoTypeStore (b, CARD16, x, 0), \
     FbDoTypeStore ((b) + 2, CARD8, x, 16)))
#else
#define BITSSTORE(b,x)  ((unsigned long) (b) & 1 ? \
    (FbDoTypeStore (b, CARD8, x, 16), \
     FbDoTypeStore ((b) + 1, CARD16, x, 0)) : \
    (FbDoTypeStore (b, CARD16, x, 8), \
     FbDoTypeStore ((b) + 2, CARD8, x, 0)))
#endif
#else
#define BITSSTORE(b,x)
#endif /* FB_24BIT */

/* Change the 24 bpp to x instead of x times 3 */
#define INC_BITS(b, x, bpp)  { \
    CARD8 *p8; \
    CARD16 *p16; \
    CARD32 *p32; \
    switch(bpp) \
    { \
        case 8: \
            p8 = ((CARD8*)b); \
            p8 += (x); \
            b = p8; \
            break; \
        case 16: \
            p16 = ((CARD16*)b); \
            p16 += (x); \
            b = p16; \
            break; \
        case 24: \
            p8 = ((CARD8*)b); \
            p8 += (x); \
            b = p8; \
            break; \
        case 32: \
            p32 = ((CARD32*)b); \
            p32 += (x); \
            b = p32; \
            break; \
    } \
}

#define SET_PIXEL(b, x, v, bpp)  { \
    CARD8 *p8; \
    CARD16 *p16; \
    CARD32 *p32; \
    switch(bpp) \
    { \
        case 8: \
            p8 = ((CARD8*)b); \
            *((p8) + (x)) = v; \
            break; \
        case 16: \
            p16 = ((CARD16*)b); \
            *((p16) + (x)) = v; \
            break; \
        case 24: \
            p8 = ((CARD8*)b); \
            BITSSTORE((p8) + (x) * 3, v); \
            break; \
        case 32: \
            p32 = ((CARD32*)b); \
            *((p32) + (x)) = v; \
            break; \
    } \
}

static int argb8888DIBtoDDB(
    EGL_DEVICE_ID devId, EGL_DIB * pDib, AltiaDevInfo * devInfo)
{
    EGL_ARGB *argbPtr;
    EGL_POS x, y;
    EGL_ARGB *argbData;
    EGL_ARGB lastArgb = 0xFFFFFFFF; /* this is white with solid alpha */
    EGL_COLOR deviceColor;
    EGL_COLOR lastDeviceColor;

    int ddbStride;
    int ddbBpp;
    int ddbXoff, ddbYoff;
    void * ddbBits;
    int ddbBitsStride;

#if EGL_DO_ALPHA_BLENDING
    int alphaStride;
    int alphaBpp;
    int alphaXoff, alphaYoff;
    void * alphaBits;
    int alphaBitsStride;
#endif /* EGL_DO_ALPHA_BLENDING */

    /* Start with pixel value of white as the last color we allocated */
    driver_ColorAlloc(devId, SCREENPTR,
        &lastArgb, NULL, &deviceColor, 1);
    lastDeviceColor = deviceColor;

    fbGetDrawable (
        devInfo->drawable, ddbBits, ddbStride, ddbBpp, ddbXoff, ddbYoff);

    /* If the bits per pixel differ from the access size this fixes it. We need
     * to treat 24 bit special since it usually is on a 32 bit boundary.
     */
    if (ddbBpp == 24 || ddbBpp < 8)
        ddbBitsStride = ddbStride * (sizeof(FbBits));
    else
        ddbBitsStride = ddbStride * (sizeof(FbBits)) / (ddbBpp >> 3);

    INC_BITS(ddbBits, (ddbBitsStride * ddbYoff) + ddbXoff, ddbBpp);

    if (pDib->imageFormat == EGL_DIB_DIRECT)
    {
        unsigned char * alphaPtr;

        /* The image data is an array of EGL_ARGB values so
         * let's access it through a pointer of this same type.
         */
        argbData = (EGL_ARGB *) pDib->pImage;

#if EGL_DO_ALPHA_BLENDING
        /*
         * Allocate an 8-bit pixmap to hold all the alpha values for this
         * DDB.  This will be used later to blend the pixmap against the
         * existing frame buffer.
         */
        if((devInfo->drawable->alphas = ((void *) (*eglScreen.CreatePixmap)(
            SCREENPTR, pDib->width, pDib->height, 8))) == (void *) 0)
        {
            return -1;
        }

        fbGetDrawable ((DrawablePtr)devInfo->drawable->alphas, alphaBits,
            alphaStride, alphaBpp, alphaXoff, alphaYoff);

        alphaBitsStride = alphaStride * (sizeof(FbBits));

        INC_BITS(
            alphaBits, (alphaBitsStride * alphaYoff) + alphaXoff, alphaBpp);
#endif /* EGL_DO_ALPHA_BLENDING */

        for (y = 0; y < pDib->height; y++)
        {
            argbPtr  = argbData;
#if EGL_DO_ALPHA_BLENDING
            alphaPtr = (unsigned char *)alphaBits;
#endif /* EGL_DO_ALPHA_BLENDING */

            for (x = 0; x < pDib->width; x++, argbPtr++)
            {
                if (*argbPtr != lastArgb)
                {
                    driver_ColorAlloc(
                        devId, SCREENPTR, argbPtr, NULL, &deviceColor, 1);
                    lastArgb = *argbPtr;
                    lastDeviceColor = deviceColor;
                }

#if EGL_DO_ALPHA_BLENDING
                *(alphaPtr++) = (unsigned char)EGL_ARGB_ALPHA(*argbPtr);
#endif /* EGL_DO_ALPHA_BLENDING */

                SET_PIXEL(ddbBits, x, lastDeviceColor, ddbBpp)
            }

            argbData += pDib->stride;
            INC_BITS(ddbBits, ddbBitsStride, ddbBpp);
#if EGL_DO_ALPHA_BLENDING
            INC_BITS(alphaBits, alphaBitsStride, alphaBpp);
#endif /* EGL_DO_ALPHA_BLENDING */
        }
    }
    else
    {
        EGL_UINT8 *indexPtr;
#ifndef KEEP8BITCOLOR
        EGL_ARGB nextArgb;

        /* The image data is an array of byte indexes into the color
         * table which is an array of EGL_ARGB values so we access
         * the image data through a byte pointer and the color
         * table through an EGL_ARGB pointer.
         */
        argbData = (EGL_ARGB *) pDib->pClut;
#endif /* KEEP8BITCOLOR */

        indexPtr = (EGL_UINT8 *) pDib->pImage;

        for (y = 0; y < pDib->height; y++)
        {
            EGL_UINT8 *idxp = indexPtr;
            for (x = 0; x < pDib->width; x++, idxp++)
            {
#ifdef KEEP8BITCOLOR
                SET_PIXEL(ddbBits, x, *idxp, ddbBpp)
#else
                if ((nextArgb = argbData[*idxp]) != lastArgb)
                {
                    driver_ColorAlloc(devId, SCREENPTR,
                        &nextArgb, NULL, &deviceColor, 1);
                    lastArgb = nextArgb;
                    lastDeviceColor = deviceColor;
                }
                SET_PIXEL(ddbBits, x, lastDeviceColor, ddbBpp)
#endif /* KEEP8BITCOLOR */
            }

            indexPtr += pDib->stride;
            INC_BITS(ddbBits, ddbBitsStride, ddbBpp);
        }
    }

    return 0;
}

static int rgb888DIBtoDDB(
    EGL_DEVICE_ID devId, EGL_DIB * pDib, AltiaDevInfo * devInfo)
{
    unsigned char *rgbPtr, *rgbData;
    EGL_POS x, y;
    EGL_ARGB lastArgb = -1; /* this is white with solid alpha */
    EGL_COLOR deviceColor;
    EGL_COLOR lastDeviceColor;

    int ddbStride;
    int ddbBpp;
    int ddbXoff, ddbYoff;
    void * ddbBits;
    int ddbBitsStride;
    int dibStride = pDib->stride * 3;

    /* Start with pixel value of white as the last color we allocated */
    driver_ColorAlloc(devId, SCREENPTR,
        &lastArgb, NULL, &deviceColor, 1);
    lastDeviceColor = deviceColor;

    fbGetDrawable (
        devInfo->drawable, ddbBits, ddbStride, ddbBpp, ddbXoff, ddbYoff);

    /* If the bits per pixel differ from the access size this fixes it. We need
     * to treat 24 bit special since it usually is on a 32 bit boundary.
     */
    if (ddbBpp == 24 || ddbBpp < 8)
        ddbBitsStride = ddbStride * (sizeof(FbBits));
    else
        ddbBitsStride = ddbStride * (sizeof(FbBits)) / (ddbBpp >> 3);

    INC_BITS(ddbBits, (ddbBitsStride * ddbYoff) + ddbXoff, ddbBpp);

    rgbData = (unsigned char *) pDib->pImage;

    /* 24-bit rgb bitmaps (no alpha) are always direct color.  An 8-bit bitmap
     * is always indexed with argb so we don't need to worry about handling
     * the indexed image format here.
     */
    for (y = 0; y < pDib->height; y++)
    {
        rgbPtr = rgbData;

        for (x = 0; x < pDib->width; x++, rgbPtr += 3)
        {
            /*
             * convert current rgb to argb using an alpha
             * value of 255 (full opacity) so we can call the driver
             * color allocation function.
             */
            EGL_ARGB currentArgb = EGL_MAKE_ARGB(
                (unsigned char)0xff,
                (unsigned char)rgbPtr[0],
                (unsigned char)rgbPtr[1],
                (unsigned char)rgbPtr[2]);

            if (currentArgb != lastArgb)
            {
                driver_ColorAlloc(
                    devId, SCREENPTR, &currentArgb, NULL, &deviceColor, 1);
                lastArgb = currentArgb;
                lastDeviceColor = deviceColor;
            }

            SET_PIXEL(ddbBits, x, lastDeviceColor, ddbBpp)
        }

        rgbData += dibStride;
        INC_BITS(ddbBits, ddbBitsStride, ddbBpp);
    }

    return 0;
}
#endif /* USE_FB */

/*----------------------------------------------------------------------------*/
void egl_ClearBitmap(EGL_DDB_ID DDB, EGL_UINT32 color, GCPtr gc)
{
    AltiaDevInfo devInfo;
    EGL_INT cleanup = FALSE;

    if(!gc) {
        cleanup = TRUE;
        gc = GetScratchGC(SCREENPTR->rootDepth, SCREENPTR);
        egl_FillPatternSet(gc, NULL);
    }

    egl_BackgroundColorSet(gc, color);
    devInfo.drawable = (DrawablePtr)DDB;
    egl_Rectangle(&devInfo, gc, (EGL_POS)0, (EGL_POS)0,
                  (EGL_POS)((DrawablePtr)DDB)->width,
                  (EGL_POS)((DrawablePtr)DDB)->height, TRUE, FALSE);

    if(cleanup) FreeScratchGC(gc);
}
/*----------------------------------------------------------------------------*/
void egl_ClearTransBitmap(EGL_DRAWABLE ddb)
{
}

/*-------------------------------------------------------------------------
 *  EGL_DDB_ID egl_BitmapCreate (EGL_DEVICE_ID devId,EGL_DIB *pDib,
 *                               EGL_DIB_CREATE_MODE createMode,
 *                               EGL_UINT32 initValue,
 *                               EGL_REFERENCE_ID refId)
 *
 * Creates a color device dependent bitmap (DDB) from the color device
 * independent bitmap (DIB) pointed to by pDib.
 *
 * The definition for the EGL_DIB data structure is:
 *
 * typedef struct egl_dib
 * {
 *     int          width;        // width of bitmap image in pixels
 *     int          height;       // height of bitmap image in pixels
 *     int          stride;       // distance in pixels between adjacent
 *                                // scan lines of the image
 *     char         imageFormat;  // is image data direct color or indexed
 *     unsigned long colorFormat; // format of a color in the clut or image
 *     int          clutSize;     // size of clut in number of elements
 *     void         *pClut;       // Pointer to color lookup table
 *     void         *pImage;      // Pointer to image data
 * } EGL_DIB;
 *
 * In Altia's usage, imageFormat is one of the following:
 *     #define EGL_INDEXED_8   8 // Image data is 8 bit index into CLUT
 *     #define EGL_DIB_DIRECT  0 // Image data is RGB colors themselves
 *
 * If imageFormat is EGL_INDEXED_8, clutSize gives the size, in elements,
 * of the color lookup table (typically 256), pClut points to the color
 * lookup table, colorFormat defines the format of the elements in the
 * color lookup table, and pImage points to an array of unsigned 8-bit
 * bytes where each byte contains an index into the color lookup table.
 * Please note that in Altia's usage, an indexed DIB is extremely uncommon
 * and so this format may not need to be implemented or at least should
 * be implemented last.
 *
 * If imageFormat is EGL_DIB_DIRECT, clutSize should be 0, pClut should be
 * NULL, colorFormat defines the format of the image data elements, and
 * pImage points to the array of image data elements.  The EGL_DIB_DIRECT
 * imageFormat is the typically format used by the Altia code and must
 * be implemented.
 *
 * In Altia's usage, colorFormat is always EGL_ARGB8888 (3).  This
 * format uses 32-bit data elements with 4 components in each element.
 * The most significant byte holds the alpha channel which is unused,
 * next byte holds the red component, next byte holds the green component
 * and the least significant byte holds the blue component.
 *
 * As an example, if imageFormat is EGL_DIB_DIRECT, colorFormat is
 * EGL_ARGB8888, width is 200 and height is 300, stride will be the same
 * as width and pImage will point to an array of (200 * 300) 32-bit
 * unsigned integers.  The first 32-bit value gives the color for the
 * top-left pixel of the new bitmap, the next 199 32-bit values give the
 * remaining colors for the top row of the bitmap, and so on until the
 * last 32-bit value gives the color for the bottom-right pixel of the
 * bitmap.
 *
 * In Altia's usage, the createMode argument can be one of the following:
 *    #define EGL_DIB_INIT_DATA   0
 *    #define EGL_DIB_INIT_VALUE  1
 *    #define EGL_DIB_INIT_TRANS  2
 *
 * If createMode is EGL_DIB_INIT_DATA, then the device dependent bitmap
 * should be initialized with the color data pointed to by pDib->pImage.
 *
 * If createMode is EGL_DIB_INIT_VALUE, then all pixels of the device
 * dependent bitmap should be given the color value from the initValue
 * argument (typically 0 to initialize all pixels to black) and
 * pDib->pImage can be NULL.
 *
 * If createMode is EGL_DIB_INIT_TRANS, then all pixels of the bitmap will
 * be given a transparent value.  This is used to only show the objects drawn
 * onto the bitmap and not it's background as needed for the Snapshot object.
 *
 * In Altia's usage, refId is a reference to the source graphic.
 *
 * This function returns a non-zero identification value of type
 * EGL_DDB_ID if it succeeds.  Otherwise, it returns NULL (0);
 *
 *-------------------------------------------------------------------------*/
EGL_DDB_ID egl_BitmapCreate(EGL_DEVICE_ID devId,EGL_DIB * pDib,
                            EGL_DIB_CREATE_MODE createMode,
                            EGL_UINT32 initValue,
                            EGL_REFERENCE_ID refId)
{
#ifdef USE_FB
    PixmapPtr xPixmap;
    AltiaDevInfo devInfo;
    GCPtr gc;

    /* Altia passes a "createMode" of either EGL_DIB_INIT_DATA or
     * EGL_DIB_INIT_VALUE and "initValue" is important if "createMode"
     * is EGL_DIB_INIT_VALUE.
     *
     * Fortunately, Altia always passes "unusedPoolId" as NULL.
     */

    /* If we are creating the bitmap from a simple initial value, things
     * are easy.
     */
    if (createMode == EGL_DIB_INIT_VALUE)
    {
        if ((xPixmap = (*eglScreen.CreatePixmap)(SCREENPTR, pDib->width,
                        pDib->height, SCREENPTR->rootDepth)) == (PixmapPtr) 0)
            return (EGL_DDB_ID) 0;

        /* Use the utility GC because it doesn't have any clipping
         * set, etc.  It's just a clean GC available for simple
         * operations.
         */
        gc = GetScratchGC(SCREENPTR->rootDepth, SCREENPTR);

        devInfo.drawable = (DrawablePtr)xPixmap;

        egl_BackgroundColorSet(gc, initValue);
        egl_FillPatternSet(gc, NULL);
        egl_Rectangle(&devInfo, gc, (EGL_POS)0, (EGL_POS)0,
            (EGL_POS)pDib->width, (EGL_POS)pDib->height, TRUE, FALSE);
    }
    else if (createMode == EGL_DIB_INIT_TRANS)
    {
        PixmapPtr xAlpha;
        if ((xPixmap = (*eglScreen.CreatePixmap)(SCREENPTR, pDib->width,
                        pDib->height, SCREENPTR->rootDepth)) == (PixmapPtr) 0)
            return (EGL_DDB_ID)0;
        if((xAlpha = (*eglScreen.CreatePixmap)(SCREENPTR, pDib->width, pDib->height,
                      8)) == (PixmapPtr) 0) {
            eglScreen.DestroyPixmap(xPixmap);
            return (EGL_DDB_ID)0;
        }
        xPixmap->drawable.alphas = (void*)xAlpha;

        gc = GetScratchGC(SCREENPTR->rootDepth, SCREENPTR);
        egl_FillPatternSet(gc, NULL);

        egl_ClearBitmap((EGL_DDB_ID)xAlpha, 0, gc);
        egl_ClearBitmap((EGL_DDB_ID)xPixmap, initValue, gc);
    }
    else
    {

        {
            if ((xPixmap = (*eglScreen.CreatePixmap)(SCREENPTR,
                            pDib->width, pDib->height,
                            (pDib->colorFormat != EGL_ALPHA8) ?
                            SCREENPTR->rootDepth : 8)) == (PixmapPtr) 0)
            {
                return (EGL_DDB_ID) 0;
            }

            gc = GetScratchGC((pDib->colorFormat != EGL_ALPHA8) ?
                SCREENPTR->rootDepth : 8, SCREENPTR);

            /* If we are creating the bitmap from a simple initial value,
             * things are easy.
             */
            devInfo.drawable = (DrawablePtr)xPixmap;


            if (pDib->colorFormat == EGL_ARGB8888
                && (pDib->imageFormat == EGL_DIB_DIRECT
                || pDib->imageFormat == EGL_INDEXED_8))
            {
                /* 32-bit DIB: 8-bits alpha; 8-bits red; 8-bits blue; 8-bits green */
                if(argb8888DIBtoDDB(devId, pDib, &devInfo) < 0)
                {
                    (*eglScreen.DestroyPixmap)(xPixmap);
                    return (EGL_DDB_ID)0;
                }
            }
            else if (pDib->colorFormat == EGL_RGB888
                && pDib->imageFormat == EGL_DIB_DIRECT)
            {
                /* 24-bit DIB: 8-bits red; 8-bits blue; 8-bits green */
                rgb888DIBtoDDB(devId, pDib, &devInfo);
            }
            else if (pDib->colorFormat == EGL_ALPHA8
                && pDib->imageFormat == EGL_DIB_DIRECT)
            {
                int alphaStride;
                int alphaBpp;
                int alphaXoff, alphaYoff;
                void * alphaBits;
                int alphaBitsStride;
                int x, y;

                /*
                 * set the alphas pixmap for this drawable equal to itself.
                 * this way we can indentify it later as being a native alpha
                 * pixmap.
                 */
                devInfo.drawable->alphas = (void *)xPixmap;

                fbGetDrawable (devInfo.drawable, alphaBits,
                    alphaStride, alphaBpp, alphaXoff, alphaYoff);

                alphaBitsStride = alphaStride * (sizeof(FbBits));

                INC_BITS(alphaBits, (alphaBitsStride * alphaYoff) + alphaXoff,
                    alphaBpp);

                for (y = 0; y < pDib->height; y++)
                {
                    unsigned char * alphaDibPtr = (unsigned char *)
                        &((unsigned char *)pDib->pImage)[y * pDib->stride];

                    for (x = 0; x < pDib->width; x++)
                    {
                        SET_PIXEL(alphaBits, x, alphaDibPtr[x], alphaBpp);
                    }

                    INC_BITS(alphaBits, alphaBitsStride, alphaBpp);
                }
            }
            else
            {
                /* Fill in code to create the bitmap from 8-bit color data.
                 * For now, use the color red.
                 */
                EGL_ARGB argbColor = EGL_MAKE_ARGB(0, 0xff, 0, 0);
                EGL_COLOR deviceColor = 0;

                egl_ColorAlloc(devId,
                    &argbColor, NULL, &deviceColor, 1);
                egl_BackgroundColorSet(gc, deviceColor);
                egl_FillPatternSet(gc, NULL);

                egl_Rectangle(&devInfo, gc, (EGL_POS)0, (EGL_POS)0,
                    (EGL_POS)pDib->width, (EGL_POS)pDib->height, TRUE, FALSE);
            }
        }
    }

    FreeScratchGC(gc);
    return xPixmap;
#else
    return (EGL_DDB_ID)0;
#endif /* USE_FB */
}

#ifdef ALTIA_USE_BACKGROUND_COLOR_FLAG
/*
 * some drivers need to flag certain rasters at the driver level.  we
 * do this by assigning a benign background color the the raster image.
 * the default colors we use is magenta (0) and cyan (1).
 */
#ifndef BACKGROUND_COLOR_DEFINITION0
/*
 * use magenta
 */
#define BACKGROUND_COLOR_DEFINITION0 (ALTIA_RED | ALTIA_BLUE)
#endif /* BACKGROUND_COLOR_DEFINITION0 */

#ifndef BACKGROUND_COLOR_DEFINITION1
/*
 * use cyan
 */
#define BACKGROUND_COLOR_DEFINITION1 (ALTIA_GREEN | ALTIA_BLUE)
#endif /* BACKGROUND_COLOR_DEFINITION1 */
#endif /* ALTIA_USE_BACKGROUND_COLOR_FLAG */

/***************************************************************************/
/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_BitmapBlt (EGL_GC_ID gc, EGL_BITMAP_ID srcBitmapId,
 *                            EGL_POS sourceLeft, EGL_POS sourceTop,
 *                            EGL_POS sourceRight, EGL_POS sourceBottom,
 *                            EGL_DDB_ID dstBitmapId, EGL_FLOAT destX,
 *                            EGL_FLOAT destY, EGL_INT hasAlpha)
 *
 *
 * This function blits the contents of one device dependent bitmap
 * (identified by srcBitmapId) to another device dependent bitmap
 * (identified by dstBitmapId).
 *
 * The area of the source bitmap to blit is determined by sourceLeft,
 * sourceTop, sourceRight, and sourceBottom.  The left, top corner of
 * this area will end up on the destination bitmap at the point destX,destY.
 *
 * If any pixel falls outside of the gc's current clipping rectangle,
 * the pixel is not blitted.
 *
 * The current raster mode for the gc determines how the source pixel colors
 * are applied to the destination pixels affected by the drawing operation.
 * Depending on the raster mode setting, source pixel colors replace the
 * current destination pixel colors, are ANDed with the destination pixel
 * colors, or are ORed with the destination pixel colors.
 *
 * This is one of the more difficult functions to write because Altia blits
 * source bitmaps that are monochrome (EGL_MDDB_ID), color (EGL_DDB_ID),
 * and color with transparent bits (EGL_TDDB_ID).  However, it is common
 * for a target graphics library to blit each of the different types in
 * a much different way and this typically requires separate graphics
 * library function calls for each type.
 *
 * If the target graphics library needs a separate function to blit
 * monochrome bitmaps, "egl_Wrapper.h" should define EGL_HAS_MONOBITMAPBLT
 * as non-zero (1).  In this case, the Altia code will call
 * egl_MonoBitmapBlt() instead of egl_BitmapBlt() to blit monochrome
 * device dependent bitmaps.
 *
 * If the target graphics library needs a separate function to blit
 * color bitmaps with transparent bits, "egl_Wrapper.h" should define
 * EGL_HAS_TRANSBITMAPBLT as non-zero (1).  In this case, the Altia code
 * will call egl_TransBitmapBlt() instead of egl_BitmapBlt() to blit
 * a color bitmap that has a monochrome bitmap transparency mask.
 *
 * The data structure for the different types of device dependent bitmaps
 * should share a common looking header, but then the data itself depends
 * on the bitmap type and how the graphic library chooses to store the
 * data for the bitmap.  Usually, the storage method is dependent on
 * the format required by the graphics chip set.
 *
 * typedef void *EGL_BITMAP_ID;          // A common way to refer to all
 *                                       // device dependent bitmap types.
 *
 * The destination dstBitmapId can be:
 *
 *  -  Just a color bitmap in which case dstBitmapId refers to the
 *     EGL_DDB_ID value for the bitmap.
 *
 *  -  The current default bitmap for the gc in which case dstBitmapID
 *     must be set to the macro EGL_DEFAULT_ID ((EGL_DDB_ID)(NULL)).  This
 *     default bitmap would typically be an offscreen bitmap used for double
 *     buffering, but it could be the display if egl_DefaultBitmapSet()
 *     was never called to assign a default bitmap other than the display.
 *
 *  -  The display itself in which case dstBitmapID must be set to the
 *     macro EGL_DISPLAY_ID ((EGL_DDB_ID)(-1)).  This will happen if
 *     double-buffering is enabled and it is time to blit from the
 *     offscreen bitmap to the real display.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_BitmapBlt(EGL_GC_ID gc, EGL_DDB_ID srcBitmapId,
                         EGL_POS sourceLeft, EGL_POS sourceTop,
                         EGL_POS sourceRight, EGL_POS sourceBottom,
                         EGL_DDB_ID dstBitmapId, EGL_FLOAT fdestX,
                         EGL_FLOAT fdestY, EGL_INT hasAlpha)
{
    GCPtr pGC = (GCPtr)gc;
    EGL_POS destX = (EGL_POS) ALTIA_ROUND(fdestX);
    EGL_POS destY = (EGL_POS) ALTIA_ROUND(fdestY);

#ifdef ALTIA_DRV_CUSTOMBLT
    /* For this case, driver's CopyArea wants to handle everything.
     * The bitmap already has the alpha in it so we can use CopyArea.
     */
    (*pGC->ops->CopyArea)(  (DrawablePtr)srcBitmapId,
                            (DrawablePtr)dstBitmapId,
                            pGC,
                            sourceLeft,
                            sourceTop,
                            ((EGL_DIB *)srcBitmapId)->width,
                            ((EGL_DIB *)srcBitmapId)->height,
                            destX,
                            destY);
#else
    int width, height;
#if EGL_DO_ALPHA_BLENDING
    PixmapPtr alpha;
    PixmapPtr front;
#endif /* EGL_DO_ALPHA_BLENDING */

    /* Don't do anything if a bitmap is our bogus safe value */
    if (srcBitmapId == (EGL_BITMAP_ID) 1 || dstBitmapId == (EGL_BITMAP_ID) 1)
    {
         return EGL_STATUS_OK;
    }

#if EGL_DO_ALPHA_BLENDING
#ifdef ALTIA_USE_BACKGROUND_COLOR_FLAG
    if(pGC->alpha == EGL_TRANSPARENT_ALPHA &&
        (pGC->bgPixel != BACKGROUND_COLOR_DEFINITION0 &&
		pGC->bgPixel != BACKGROUND_COLOR_DEFINITION1))
    {
        return EGL_STATUS_OK;
    }
#else
    if(pGC->alpha == EGL_TRANSPARENT_ALPHA)
    {
        return EGL_STATUS_OK;
    }
#endif /* !ALTIA_USE_BACKGROUND_COLOR_FLAG */

    alpha = fbGetAlphasPixmap((DrawablePtr)srcBitmapId);

#ifdef ALTIA_USE_BACKGROUND_COLOR_FLAG
    if((pGC->alpha != EGL_OPAQUE_ALPHA ||
        pGC->bgPixel == BACKGROUND_COLOR_DEFINITION0 ||
		pGC->bgPixel == BACKGROUND_COLOR_DEFINITION1) &&
        (((DrawablePtr)dstBitmapId)->type == DRAWABLE_PIXMAP ||
        ((DrawablePtr)dstBitmapId)->type == DRAWABLE_BUFFER))
#else
    if(pGC->alpha != EGL_OPAQUE_ALPHA &&
        (((DrawablePtr)dstBitmapId)->type == DRAWABLE_PIXMAP ||
        ((DrawablePtr)dstBitmapId)->type == DRAWABLE_BUFFER))
#endif /* ALTIA_USE_BACKGROUND_COLOR_FLAG */
    {
        width  = ((DrawablePtr)srcBitmapId)->width;
        height = ((DrawablePtr)srcBitmapId)->height;

        if((alpha = GetScratchAlphaPixmap(
            width, height, pGC->alpha)) == (PixmapPtr) 0)
        {
            return -1;
        }

        /*
         * If srcBitmapId has an alpha pixmap associated to it then merge the
         * alpha pixmaps together.
         */
        if(fbGetAlphasPixmap((DrawablePtr)srcBitmapId) != (PixmapPtr) 0)
        {
            MergeAlphaPixmaps(
                fbGetAlphasPixmap((DrawablePtr)srcBitmapId), alpha);
        }
    }
    else
#endif /* EGL_DO_ALPHA_BLENDING */
    {
        width  = sourceRight - sourceLeft + 1;
        height = sourceBottom - sourceTop + 1;
    }

    (*pGC->funcs->ValidateGC)(
        pGC, pGC->stateChanges, (DrawablePtr)dstBitmapId);

#if EGL_DO_ALPHA_BLENDING
    if(alpha != (PixmapPtr) 0 &&
        (((DrawablePtr)dstBitmapId)->type == DRAWABLE_PIXMAP ||
        ((DrawablePtr)dstBitmapId)->type == DRAWABLE_BUFFER))
    {
        if(fbGetAlphasPixmap((DrawablePtr)srcBitmapId) ==
            (PixmapPtr)srcBitmapId)
        {
            GCPtr pUtilityGC;
            xRectangle xrect;

            xrect.x = sourceLeft;
            xrect.y = sourceTop;
            xrect.width  = (sourceRight - sourceLeft) + 1;
            xrect.height = (sourceBottom - sourceTop) + 1;

            pUtilityGC = GetScratchGC(SCREENPTR->rootDepth, SCREENPTR);

            if((front = GetScratchPixmap(
                (sourceRight - sourceLeft) + 1,
                (sourceBottom - sourceTop) + 1)) == (PixmapPtr) 0)
            {
                return EGL_STATUS_ERROR;
            }

            if(pUtilityGC->fillStyle != FillSolid)
            {
                pUtilityGC->fillStyle = FillSolid;
                pUtilityGC->stateChanges |= GCFillStyle;
            }

            if(pGC->colorFlip == 0)
            {
                if(pUtilityGC->fgPixel != pGC->fgPixel)
                {
                    pUtilityGC->fgPixel = pGC->fgPixel;
                    pUtilityGC->stateChanges |= GCForeground;
                }

                if(pUtilityGC->bgPixel != pGC->bgPixel)
                {
                    pUtilityGC->bgPixel = pGC->bgPixel;
                    pUtilityGC->stateChanges |= GCBackground;
                }
            }
            else
            {
                if(pUtilityGC->fgPixel != pGC->bgPixel)
                {
                    pUtilityGC->fgPixel = pGC->bgPixel;
                    pUtilityGC->stateChanges |= GCForeground;
                }

                if(pUtilityGC->bgPixel != pGC->bgPixel)
                {
                    pUtilityGC->bgPixel = pGC->fgPixel;
                    pUtilityGC->stateChanges |= GCBackground;
                }
            }

            /*
             * Draw filled rectangle into front pixmap
             */
            (*pGC->funcs->ValidateGC)(
                pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)front);
            (*pGC->ops->PolyFillRect)(
                (DrawablePtr)front, pUtilityGC, 1, &xrect);

            FreeScratchGC(pUtilityGC);

            srcBitmapId = (EGL_DDB_ID)front;
        }

        (*pGC->ops->AlphaCopyArea)(
            (DrawablePtr)srcBitmapId,
            (DrawablePtr)dstBitmapId,
            (DrawablePtr)alpha,
            pGC, sourceLeft, sourceTop,
            width, height,
            destX, destY);
    }
    else
#endif /* EGL_DO_ALPHA_BLENDING */
    {
        (*pGC->ops->CopyArea)(
            (DrawablePtr)srcBitmapId,
            (DrawablePtr)dstBitmapId,
            pGC, sourceLeft, sourceTop,
            width, height,
            destX, destY);
    }

#endif /* ALTIA_DRV_CUSTOMBLT */
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_BitmapDestroy(EGL_DEVICE_ID devId,EGL_DDB_ID ddbId,
                             EGL_INT hasAlpha)
{
    /* Don't do anything if the bitmap is our bogus safe value
     * or nothing at all.
     */
    if (ddbId == (EGL_DDB_ID) 1 || ddbId == (EGL_DDB_ID) 0)
        return EGL_STATUS_OK;

#if EGL_DO_ALPHA_BLENDING
    /*
     * Free alpha pixmap if it was allocated
     */
    if(fbGetAlphasPixmap((DrawablePtr)ddbId) != (PixmapPtr) 0 &&
        fbGetAlphasPixmap((DrawablePtr)ddbId) != (PixmapPtr) ddbId)
    {
        (*eglScreen.DestroyPixmap)(fbGetAlphasPixmap((DrawablePtr)ddbId));
    }
#endif /* EGL_DO_ALPHA_BLENDING */

    (*eglScreen.DestroyPixmap)((PixmapPtr)ddbId);

    return EGL_STATUS_OK;
}

#if EGL_HAS_MONOBITMAPBLT
/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_MonoBitmapBlt (EGL_GC_ID gc, EGL_MDDB_ID srcBitmapId,
 *                                EGL_POS sourceLeft, EGL_POS sourceTop,
 *                                EGL_POS sourceRight, EGL_POS sourceBottom,
 *                                EGL_DDB_ID dstBitmapId, EGL_FLOAT destX,
 *                                EGL_FLOAT destY, EGL_INT hasAlpha)
 *
 * If "egl_Wrapper.h" defines EGL_HAS_MONOBITMAPBLT as non-zero (1), it
 * means the target graphics library needs to blit monochrome bitmaps
 * differently than color bitmaps.  This would be the typical case for
 * interfacing to most graphics libraries.  In this case, the Altia code
 * calls egl_MonoBitmapBlt() instead of egl_BitmapBlt() to blit the
 * monochrome device dependent bitmap srcBitmapId to the destination
 * dstBitmapId.
 *
 * If EGL_HAS_MONOBITMAPBLT is defined as 0, egl_BitmapBlt() is called
 * to blit monochrome bitmaps as well as color bitmaps.
 *
 * The parameters to this function are the same as egl_BitmapBlt() except
 * that srcBitmapId is assumed to always refer to a device dependent
 * monochrome bitmap.  See egl_BitmapBlt() for further details.
 *
 * The destination can be a color bitmap (EGL_DDB_ID), the default
 * bitmap (EGL_DEFAULT_ID) (which might be the offscreen bitmap
 * used for double-buffering or the display if double-buffering
 * is disabled), or the display itself (EGL_DISPLAY_ID) when
 * double-buffering is enabled and its time to blit to the display.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_MonoBitmapBlt(EGL_GC_ID gc, EGL_MDDB_ID srcBitmapId,
                             EGL_POS sourceLeft, EGL_POS sourceTop,
                             EGL_POS sourceRight, EGL_POS sourceBottom,
                             EGL_DDB_ID dstBitmapId, EGL_FLOAT fdestX,
                             EGL_FLOAT fdestY, EGL_INT hasAlpha)
{


    EGL_POS destX = (EGL_POS) ALTIA_ROUND(fdestX);
    EGL_POS destY = (EGL_POS) ALTIA_ROUND(fdestY);
    xRectangle xrect;
    GCPtr pGC = (GCPtr)gc;

    /* Don't do anything if the source bitmap is our bogus safe value */
    if (srcBitmapId == (EGL_BITMAP_ID) 1)
        return EGL_STATUS_OK;

#if EGL_DO_ALPHA_BLENDING
    if(pGC->alpha == EGL_TRANSPARENT_ALPHA)
    {
        return EGL_STATUS_OK;
    }
#endif /* EGL_DO_ALPHA_BLENDING */

    xrect.width = sourceRight - sourceLeft + 1;
    xrect.height = sourceBottom - sourceTop + 1;

    if (pGC->colorFlip)
    {
        /*
         * Undo the flip since we want the fg to really be the foreground
         */
        unsigned long saveFg = pGC->fgPixel;
        pGC->fgPixel = pGC->bgPixel;
        pGC->bgPixel = saveFg;
        pGC->stateChanges |= GCForeground|GCBackground;
    }

#if EGL_DO_ALPHA_BLENDING
    if(pGC->alpha != EGL_OPAQUE_ALPHA &&
        (((DrawablePtr)dstBitmapId)->type == DRAWABLE_PIXMAP ||
        ((DrawablePtr)dstBitmapId)->type == DRAWABLE_BUFFER))
    {
        PixmapPtr front, alpha;
        GCPtr pUtilityGC;

        xrect.x = 0;
        xrect.y = 0;

        if((front = GetScratchPixmap(
            xrect.width, xrect.height)) == (PixmapPtr) 0)
        {
            return -1;
        }

        if((alpha = GetScratchAlphaPixmap(
            xrect.width, xrect.height, EGL_TRANSPARENT_ALPHA)) ==
            (PixmapPtr) 0)
        {
            return -1;
        }

        pUtilityGC = GetScratchGC(SCREENPTR->rootDepth, SCREENPTR);

        /*
         * Draw stippled rectangle into front pixmap
         */
        pUtilityGC->fgPixel = pGC->fgPixel;
        pUtilityGC->bgPixel = pGC->bgPixel;
        pUtilityGC->stateChanges |= GCForeground|GCBackground;

        pUtilityGC->fillStyle = FillStippled;
        pUtilityGC->stipple = srcBitmapId;
        pUtilityGC->stateChanges |= GCStipple|GCFillStyle;

        (*pGC->funcs->ValidateGC)(
            pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)front);
        (*pGC->ops->PolyFillRect)(
            (DrawablePtr)front, pUtilityGC, 1, &xrect);

        /*
         * Draw the same stippled rectangle into the alpha pixmap
         */
        pUtilityGC->fgPixel = pGC->alpha;
        pUtilityGC->bgPixel = pGC->alpha;
        pUtilityGC->stateChanges |= GCForeground|GCBackground;

        (*pGC->funcs->ValidateGC)(
            pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)alpha);
        (*pGC->ops->PolyFillRect)(
            (DrawablePtr)alpha, pUtilityGC, 1, &xrect);

        (*pGC->funcs->ValidateGC)(
            pGC, pGC->stateChanges, (DrawablePtr)dstBitmapId);

        (*pGC->ops->AlphaCopyArea)(
            (DrawablePtr)front,
            (DrawablePtr)dstBitmapId,
            (DrawablePtr)alpha,
            pGC, 0, 0,
            xrect.width,
            xrect.height,
            destX, destY);
    }
    else
#endif /* EGL_DO_ALPHA_BLENDING */
    {
        int savefill = pGC->fillStyle;
        PixmapPtr saveStipple = pGC->stipple;
        xPoint saveOrigin;

        xrect.x = destX;
        xrect.y = destY,

        saveOrigin.x = pGC->patOrg.x;
        saveOrigin.y = pGC->patOrg.y;

        pGC->patOrg.x = destX;
        pGC->patOrg.y = destY;
        pGC->fillStyle = FillStippled;
        pGC->stipple = srcBitmapId;
        pGC->stateChanges |= GCStipple|GCFillStyle;

        (*pGC->funcs->ValidateGC)(
            (GCPtr)gc, ((GCPtr)gc)->stateChanges, (DrawablePtr)dstBitmapId);
        (*pGC->ops->PolyFillRect)(
            (DrawablePtr)dstBitmapId, pGC, 1, &xrect);

        pGC->fillStyle = savefill;
        pGC->stipple = saveStipple;
        pGC->patOrg.x = saveOrigin.x;
        pGC->patOrg.y = saveOrigin.y;

        pGC->stateChanges |= GCStipple|GCFillStyle;
    }

    if (pGC->colorFlip)
    {
        /*
         * Redo what we undid
         */
        unsigned long saveFg = pGC->fgPixel;
        pGC->fgPixel = pGC->bgPixel;
        pGC->bgPixel = saveFg;
        pGC->stateChanges |= GCForeground|GCBackground;
    }

    return EGL_STATUS_OK;

}
#endif /* EGL_HAS_MONOBITMAPBLT */


/*-------------------------------------------------------------------------
 *  EGL_MDDB_ID egl_MonoBitmapCreate (EGL_DEVICE_ID devId, EGL_MDIB *mDib,
 *                                    EGL_DIB_CREATE_MODE createMode,
 *                                    EGL_UINT32 initValue,
 *                                    EGL_REFERENCE_ID refId)
 *
 * Creates a monochrome device dependent bitmap (MDDB) from the monochrome
 * device independent bitmap (MDIB) pointed to by mDib.  This function
 * is needed for drawing Altia Stencil objects (monochrome bitmaps), Altia
 * Raster objects (color bitmaps) that have transparent bits, and
 * rectangle, polygon, or ellipse fill patterns.
 *
 * The definition for the EGL_MDIB data structure is:
 *
 * typedef struct egl_mdib
 * {
 *     int        width;      // width of bitmap image in pixels
 *     int        height;     // height of bitmap image in pixels
 *     int        stride;     // distance in pixels between image data rows
 *     unsigned char *pImage; // Pointer to image data
 * } EGL_MDIB;
 *
 * The width element gives the width of the image in pixels.
 *
 * The height element gives the height of the image in pixels.
 *
 * The stride element gives the number of pixels between adjust
 * rows of the image.  The stride is usually a value that is
 * exactly divisible by 8 so that each row of monochrome bitmap
 * data pointed to by pImage ends on a byte boundary.  The width
 * value is typically 1 to 7 less than the stride or exactly
 * equal to the stride if it is exactly divisible by 8.
 *
 * Finally, pImage points to an array of 8-bit bytes where each byte
 * contains the next 8 pixel values for the monochrome bitmap.  A
 * single byte can hold 8 pixel values because each bit is associated
 * with one pixel.  A "1" for a bit means it is on.  A "0" for a bit
 * means it is off.  When the resulting MDDB is later blit using
 * egl_BitmapBlt(), the foreground color of the gc will be used for
 * the on bits and the background color of the gc will be used for
 * the off bits.
 *
 * The MSB of the first byte pointed to by pImage contains the pixel
 * data for the top left corner of the image.  The next bit contains
 * the data for pixel to the right of the first pixel and so on for
 * each pixel in each row until the LSB of the last byte pointed to
 * by pImage which contains the pixel data for the bottom right
 * corner of the image.
 *
 * In Altia's usage, the createMode argument can be one of the following:
 *    #define EGL_DIB_INIT_DATA   0
 *    #define EGL_DIB_INIT_VALUE  1
 *    #define EGL_DIB_INIT_TRANS  2 - Is not used here.
 *
 * If createMode is EGL_DIB_INIT_DATA, then the MDDB should be initialized
 * with the pixel data pointed to by mDib->pImage.
 *
 * If createMode is EGL_DIB_INIT_VALUE, then all pixels of the MDDB should
 * be given the value from the initValue argument (typically 1 to initialize
 * all pixels to on or 0 to initialize all pixels to off) and mDib-pImage
 * can be NULL.
 *
 * In Altia's usage, refId is a reference to the source graphic.
 *
 * This function returns a non-zero identification value of type
 * EGL_MDDB_ID if it succeeds.  Otherwise, it returns NULL (0);
 *
 *-------------------------------------------------------------------------*/
EGL_MDDB_ID egl_MonoBitmapCreate(EGL_DEVICE_ID devId,EGL_MDIB *pMDib,
                                 EGL_DIB_CREATE_MODE createMode,
                                 EGL_UINT32 initValue,
                                 EGL_REFERENCE_ID refId)
{
    PixmapPtr xPixmap;
    AltiaDevInfo devInfo;
    GCPtr gc;

    /* Altia passes a "createMode" of either EGL_DIB_INIT_DATA or
     * EGL_DIB_INIT_VALUE and "initValue" is important if "createMode"
     * is EGL_DIB_INIT_VALUE.
     *
     * Fortunately, Altia always passes "unusedPoolId" as NULL.
     */

    if (createMode == EGL_DIB_INIT_DATA)
    {

        {
            int width, height;
            EGL_UINT8 *dataPtr;
            EGL_POS x, y;
            int bitMask;

            width = pMDib->width;
            height = pMDib->height;

            if ((xPixmap = (*eglScreen.CreatePixmap)(SCREENPTR, width, height,
                            1)) == (PixmapPtr) 0)
                return (EGL_MDDB_ID) 0;

            /* Use the monochrome utility GC because it doesn't have any
             * clipping set, etc.  It's just a clean GC available for simple
             * monochrome operations.
             */
            gc = GetScratchGC(1, SCREENPTR);
            devInfo.drawable = (DrawablePtr)xPixmap;
            egl_AlphaChannelSet(gc, EGL_OPAQUE_ALPHA);
            egl_FillPatternSet(gc, NULL);
            egl_BackgroundColorSet(gc, 0);
            egl_ForegroundColorSet(gc, EGL_COLOR_TRANSPARENT);
            egl_Rectangle(&devInfo, gc, (EGL_POS)0, (EGL_POS)0,
                (EGL_POS)pMDib->width, (EGL_POS)pMDib->height, TRUE, FALSE);

            for (y = 0; y < height; y++)
            {
                dataPtr = pMDib->pImage + ((pMDib->stride / 8) * y);
                bitMask = 0x80;

                for (x = 0; x < width; x++)
                {
                    if (*dataPtr & bitMask)
                        egl_PixelSet(&devInfo, gc, x, y, (unsigned long) 1);

                    if ((bitMask >>= 1) == 0)
                    {
                        bitMask = 0x80;
                        dataPtr++;
                    }
                }
            }
            FreeScratchGC(gc);
        }
    }
    else
    {


        if ((xPixmap = (*eglScreen.CreatePixmap)(SCREENPTR, pMDib->width,
                        pMDib->height, 1)) == (PixmapPtr) 0)
            return (EGL_MDDB_ID) 0;

        gc = GetScratchGC(1, SCREENPTR);
        devInfo.drawable = (DrawablePtr)xPixmap;
        egl_BackgroundColorSet(gc, initValue);
        egl_ForegroundColorSet(gc, EGL_COLOR_TRANSPARENT);
        egl_FillPatternSet(gc, NULL);

        /* For X, the filled rectangle is INCLUSIVE of the dimensions
         * and that is exactly what we want.
         */
        egl_Rectangle(&devInfo, gc, (EGL_POS)0, (EGL_POS)0,
            (EGL_POS)pMDib->width, (EGL_POS)pMDib->height, TRUE, FALSE);
        FreeScratchGC(gc);

    }

    return xPixmap;
}


/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_MonoBitmapDestroy (EGL_DEVICE_ID devId, EGL_MDDB_ID mDdbId)
 *
 * Destroys the monochrome device dependent bitmap (MDDB) identified by
 * mDdbId.
 *
 * This function will free any data structures or bitmap memory associated
 * with mDdbId.  The calling code should not use the value of mDdbId in
 * any other graphics library calls that can take a EGL_MDDB_ID.  To do so
 * may cause a memory access error.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_MonoBitmapDestroy(EGL_DEVICE_ID devId,
                                 EGL_MDDB_ID mDdbId)
{
    /* Don't do anything if the bitmap is our bogus safe value
     * or nothing at all.
     */
    if (mDdbId == (EGL_MDDB_ID) 1 || mDdbId == (EGL_MDDB_ID) 0)
        return EGL_STATUS_OK;

    (*eglScreen.DestroyPixmap)((PixmapPtr)mDdbId);

    return EGL_STATUS_OK;
}


#if EGL_HAS_TRANSBITMAPBLT
/*-------------------------------------------------------------------------
 * EGL_STATUS egl_TransBitmapBlt(EGL_GC_ID gc,
 *                               EGL_DDB_ID srcBitmapId,
 *                               EGL_MDDB_ID srcMaskId,
 *                               EGL_POS sourceLeft, EGL_POS sourceTop,
 *                               EGL_POS sourceRight, EGL_POS sourceBottom,
 *                               EGL_DDB_ID dstBitmapId,
 *                               EGL_FLOAT destX, EGL_FLOAT destY,
 *                               EGL_INT hasAlpha)
 *
 * If "egl_Wrapper.h" defines EGL_HAS_TRANSBITMAPBLT as non-zero (1), it
 * means the target graphics library needs to blit color bitmaps with
 * transparent masks differently than regular color bitmaps.  This would
 * be the typical case for interfacing to most graphics libraries.  In
 * this case, the Altia code calls egl_TransBitmapBlt() instead of
 * egl_BitmapBlt() to blit the color device dependent bitmap srcBitmapId
 * using the monochrome device dependent bitmap srcMaskId as a mask.  The
 * destination is the bitmap dstBitmapId.
 *
 * If EGL_HAS_TRANSBITMAPBLT is defined as 0, egl_BitmapBlt() is called
 * to blit transparent bitmaps as well as color bitmaps.
 *
 * The other parameters to this function are the same as egl_BitmapBlt().
 * See egl_BitmapBlt() for further details.
 *
 * The destination can be a color bitmap (EGL_DDB_ID), the default
 * bitmap (EGL_DEFAULT_ID) (which might be the offscreen bitmap
 * used for double-buffering or the display if double-buffering
 * is disabled), or the display itself (EGL_DISPLAY_ID) when
 * double-buffering is enabled and its time to blit to the display.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_TransBitmapBlt(EGL_GC_ID gc,
                              EGL_DDB_ID srcBitmapId,
                              EGL_MDDB_ID srcMaskId,
                              EGL_POS sourceLeft, EGL_POS sourceTop,
                              EGL_POS sourceRight, EGL_POS sourceBottom,
                              EGL_DDB_ID dstBitmapId,
                              EGL_FLOAT fdestX, EGL_FLOAT fdestY,
                              EGL_INT hasAlpha)
{

    EGL_POS destX = (EGL_POS) ALTIA_ROUND(fdestX);
    EGL_POS destY = (EGL_POS) ALTIA_ROUND(fdestY);
    AltiaDevInfo devInfo;
    int savefill;
    PixmapPtr saveStipple;
    xPoint saveOrigin;

    /* Don't do anything if the sources are our bogus safe value */
    if (srcBitmapId == (EGL_DDB_ID) 1 || srcMaskId == (EGL_MDDB_ID) 1)
    {
         return EGL_STATUS_OK;
    }

#if EGL_DO_ALPHA_BLENDING
    if(((GCPtr)gc)->alpha == EGL_TRANSPARENT_ALPHA)
    {
        return EGL_STATUS_OK;
    }
#endif /* EGL_DO_ALPHA_BLENDING */

    if (((DrawablePtr)dstBitmapId)->type == DRAWABLE_PIXMAP ||
         altiaStippleSpan == 0)
    {
#if EGL_DO_ALPHA_BLENDING
        if((((GCPtr)gc)->alpha != EGL_OPAQUE_ALPHA ||
            ((DrawablePtr)srcBitmapId)->alphas != (void *) 0) &&
            (((DrawablePtr)dstBitmapId)->type == DRAWABLE_PIXMAP ||
            ((DrawablePtr)dstBitmapId)->type == DRAWABLE_BUFFER))
        {
            PixmapPtr alpha;
            GCPtr pUtilityGC;
            xRectangle xrect;

            xrect.x = 0;
            xrect.y = 0;
            xrect.width  = ((DrawablePtr)srcBitmapId)->width;
            xrect.height = ((DrawablePtr)srcBitmapId)->height;

            if((alpha = GetScratchAlphaPixmap(
                xrect.width, xrect.height, EGL_TRANSPARENT_ALPHA)) ==
                (PixmapPtr) 0)
            {
                return -1;
            }

            pUtilityGC = GetScratchGC(SCREENPTR->rootDepth, SCREENPTR);

            pUtilityGC->fgPixel = ((GCPtr)gc)->alpha;
            pUtilityGC->bgPixel = ((GCPtr)gc)->alpha;
            pUtilityGC->stateChanges |= GCForeground|GCBackground;

            pUtilityGC->fillStyle = FillStippled;
            pUtilityGC->stipple = srcMaskId;
            pUtilityGC->stateChanges |= GCStipple|GCFillStyle;

            (*((GCPtr)gc)->funcs->ValidateGC)(
                pUtilityGC, pUtilityGC->stateChanges, (DrawablePtr)alpha);

            (*((GCPtr)gc)->ops->PolyFillRect)(
                (DrawablePtr)alpha, pUtilityGC, 1, &xrect);

            /*
             * If srcBitmapId has an alpha pixmap associated to it then merge
             * the alpha pixmaps together.
             */
            if(fbGetAlphasPixmap((DrawablePtr)srcBitmapId) != (PixmapPtr) 0)
            {
                MergeAlphaPixmaps(
                    fbGetAlphasPixmap((DrawablePtr)srcBitmapId), alpha);
            }

            (*((GCPtr)gc)->funcs->ValidateGC)(
                (GCPtr)gc, ((GCPtr)gc)->stateChanges, (DrawablePtr)dstBitmapId);

            (*((GCPtr)gc)->ops->AlphaCopyArea)(
                (DrawablePtr)srcBitmapId,
                (DrawablePtr)dstBitmapId,
                (DrawablePtr)alpha,
                (GCPtr)gc, sourceLeft, sourceTop,
                sourceRight - sourceLeft + 1,
                sourceBottom - sourceTop + 1,
                destX, destY);
        }
        else
#endif /* EGL_DO_ALPHA_BLENDING */
        {
            /*
             * The pixmap and frameBuffer driver does not support Stippled
             * SetSpans so we have to do it the xor way
             */
            unsigned long bgsave  = ((GCPtr)gc)->bgPixel;
            unsigned long fgsave  = ((GCPtr)gc)->fgPixel;
            unsigned char alusave = ((GCPtr)gc)->alu;

            ((GCPtr)gc)->bgPixel = -1;
            ((GCPtr)gc)->fgPixel = 0;
            ((GCPtr)gc)->alu = GXxor;
            ((GCPtr)gc)->stateChanges |= GCForeground|GCBackground|GCFunction;

            (*((GCPtr)gc)->funcs->ValidateGC)(
                (GCPtr)gc, ((GCPtr)gc)->stateChanges, (DrawablePtr)dstBitmapId);

            (*((GCPtr)gc)->ops->CopyArea)(
                (DrawablePtr)srcBitmapId,
                (DrawablePtr)dstBitmapId,
                (GCPtr)gc, sourceLeft, sourceTop,
                sourceRight - sourceLeft + 1,
                sourceBottom - sourceTop + 1,
                destX, destY);

            ((GCPtr)gc)->alu = GXand;

            ((GCPtr)gc)->stateChanges |= GCFunction;
            (*((GCPtr)gc)->funcs->ValidateGC)(
                (GCPtr)gc, ((GCPtr)gc)->stateChanges, (DrawablePtr)dstBitmapId);

            (*((GCPtr)gc)->ops->CopyPlane)(
                (DrawablePtr)srcMaskId,
                (DrawablePtr)dstBitmapId,
                (GCPtr)gc, sourceLeft, sourceTop,
                sourceRight - sourceLeft + 1,
                sourceBottom - sourceTop + 1,
                destX, destY, 1);

            ((GCPtr)gc)->alu = GXxor;
            ((GCPtr)gc)->stateChanges |= GCFunction;

            (*((GCPtr)gc)->funcs->ValidateGC)(
                (GCPtr)gc, ((GCPtr)gc)->stateChanges, (DrawablePtr)dstBitmapId);

            (*((GCPtr)gc)->ops->CopyArea)(
                (DrawablePtr)srcBitmapId,
                (DrawablePtr)dstBitmapId,
                (GCPtr)gc, sourceLeft, sourceTop,
                sourceRight - sourceLeft + 1,
                sourceBottom - sourceTop + 1,
                destX, destY);

            ((GCPtr)gc)->bgPixel = bgsave;
            ((GCPtr)gc)->fgPixel = fgsave;
            ((GCPtr)gc)->alu = alusave;
            ((GCPtr)gc)->stateChanges |= GCForeground|GCBackground|GCFunction;
            (*((GCPtr)gc)->funcs->ValidateGC)(
                (GCPtr)gc, ((GCPtr)gc)->stateChanges, (DrawablePtr)dstBitmapId);
        }
    }
    else
    {
        /*
         * Our display driver supports Stippled SetSpans
         */
        devInfo.drawable = (DrawablePtr)dstBitmapId;
        local_GcAttributesSet(&devInfo, gc, 0, 0);
        savefill = ((GCPtr)gc)->fillStyle;
        saveStipple = ((GCPtr)gc)->stipple;
        saveOrigin.x = ((GCPtr)gc)->patOrg.x;
        saveOrigin.y = ((GCPtr)gc)->patOrg.y;
        ((GCPtr)gc)->patOrg.x = destX;
        ((GCPtr)gc)->patOrg.y = destY;
        ((GCPtr)gc)->fillStyle = FillStippled;
        ((GCPtr)gc)->stipple = (PixmapPtr)srcMaskId;
        (*((GCPtr)gc)->ops->CopyArea)(
            (DrawablePtr)srcBitmapId,
            (DrawablePtr)dstBitmapId,
            (GCPtr)gc, sourceLeft, sourceTop,
            sourceRight - sourceLeft + 1,
            sourceBottom - sourceTop + 1,
            destX, destY);
        ((GCPtr)gc)->fillStyle = savefill;
        ((GCPtr)gc)->stipple = saveStipple;
        ((GCPtr)gc)->patOrg.x = saveOrigin.x;
        ((GCPtr)gc)->patOrg.y = saveOrigin.y;
    }

    return EGL_STATUS_OK;
}
#endif /* EGL_HAS_TRANSBITMAPBLT */


#if EGL_HAS_TRANSBITMAPCREATE
/*-------------------------------------------------------------------------
 *  EGL_TDDB_ID egl_TransBitmapCreateFromDDB(EGL_DEVICE_ID devId,
 *                                        EGL_DDB_ID ddbId,
 *                                        EGL_MDDB_ID mDdbId,
 *                                        EGL_REFERENCE_ID refId,
 *                                        EGL_INT hasAlpha)
 *
 * If "egl_Wrapper.h" defines EGL_HAS_TRANSBITMAPCREATE as non-zero (1),
 * this function is called to create a device dependent bitmap with
 * transparent bits for rendering Altia Raster objects with one or more
 * transparent pixels.
 *
 * This function creates the transparent device dependent bitmap (TDDB)
 * from a regular color DDB and monohrome DDB (MDDB).  The EGL_DDB_ID ddbId
 * comes from a previous call to egl_BitmapCreate() and the EGL_MDDB_ID
 * mDdbId comes from a previous call to egl_MonoBitmapCreate().  Both
 * bitmaps should have the same width and height.
 *
 * If EGL_HAS_TRANSBITMAPCREATE is defined as 0, the Altia code does not
 * use this function.  Instead, it calls egl_BitmapCreate() to create a
 * regular color (DDB), calls egl_MonoBitmapCreate() to create a MDDB to
 * hold a mask that identifies the transparent bits, and keeps track of
 * the DDB and MDDB separately.  This is how it would be done for
 * interfacing to most target graphics libraries.
 *
 * A 0 pixel in the MDDB identifies a pixel in the resulting TDDB that
 * should be transparent while a 1 pixel in the MDDB is an indication that
 * the actual color for the pixel, as provided in the regular color DDB,
 * should be used in the resulting TDDB.
 *
 * The data referenced by ddbId and mDdbId becomes referenced by the
 * EGL_TDDB_ID identification value returned by this function.  For this
 * reason, neither ddbId or mDdbId should be individually destroyed.  Doing
 * this will corrupt the data referenced by the EGL_TDDB_ID return value.
 * Instead, egl_TransBitmapDestroy() should be called to destroy all of
 * the data simultaneously.
 *
 * In Altia's usage, refId is a reference to the source graphic.
 *
 * This function returns a non-zero identification value of type
 * EGL_TDDB_ID if it succeeds.  Otherwise, it returns NULL (0);
 *
 *-------------------------------------------------------------------------*/
EGL_TDDB_ID egl_TransBitmapCreateFromDDB(EGL_DEVICE_ID devId,
                                         EGL_DDB_ID ddbId,
                                         EGL_MDDB_ID mDdbId,
                                         EGL_REFERENCE_ID refId,
                                         EGL_INT hasAlpha)
{
    /* An X11 target doesn't have a concept of a transparent bitmap.
     * Instead, a bitmap with transparent bits is the association of a
     * regular color bitmap with a monochrome bitmap that defines the
     * transparent bits.  For an X11 target, "egl_Wrapper.h" should
     * define EGL_HAS_TRANBITMAPCREATE as 0.  Just return the ddbId
     * argument as the transparent bitmap if this function happens to
     * get called (but it won't even get referenced by the Altia code
     * if EGL_HAS_TRANSBITMAPCREATE is defined as 0 in "egl_Wrapper.h").
     */
    return (EGL_TDDB_ID) ddbId;
}
#endif /* EGL_HAS_TRANSBITMAPCREATE */


#if EGL_HAS_TRANSBITMAPCREATE
/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_TransBitmapDestroy (EGL_DEVICE_ID devId, EGL_TDDB_ID tDdbId,
 *                                     EGL_INT hasAlpha)
 *
 * If "egl_Wrapper.h" defines EGL_HAS_TRANSBITMAPCREATE as non-zero (1),
 * this function is called to destroy the transparent device dependent
 * bitmap (TDDB) identified by tDdbId and presumably created by a previous
 * call to egl_TransBitmapCreateFromDDB().  A TDDB is used by the Altia
 * code to render Altia Raster objects that also have transparent bits.
 *
 * This function also destroys the regular color device dependent bitmap
 * (DDB) and monochrome device dependent bitmap (MDDB) data referenced
 * by tDdbId (see egl_TransBitmapCreateFromDDB() for more details).
 *
 * This function will free any data structures or bitmap memory associated
 * with tDdbId.  The calling code should not use the value of tDdbId in
 * any other graphics library calls that can take a EGL_TDDB_ID.  To do so
 * may cause a memory access error.
 *
 * If "egl_Wrapper.h" defines EGL_HAS_TRANSBITMAPCREATE as 0, this function
 * is not called.  Instead, the Altia code explicitly destroys the color
 * device dependent bitmap (DDB) and monochrome device dependent bitmap
 * (MDDB) separately.  This is how it would be done for interfacing to
 * most target graphics libraries.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_TransBitmapDestroy(EGL_DEVICE_ID devId,
                                  EGL_TDDB_ID tDdbId, EGL_INT hasAlpha)
{
    /* Don't do anything if the bitmap is our bogus safe value
     * or nothing at all.
     */
    if (tDdbId == (EGL_TDDB_ID) 1 || tDdbId == (EGL_TDDB_ID) 0)
        return EGL_STATUS_OK;

    /* An X11 target doesn't have a concept of a transparent bitmap.
     * Instead, a bitmap with transparent bits is a combination of a
     * regular color bitmap and a monochrome bitmap that defines the
     * transparent bits.  For an X11 target, "egl_Wrapper.h" should
     * define EGL_HAS_TRANBITMAPCREATE as 0.  Just destroy the tDdbId
     * as a regular color bitmap if this function happens to get
     * called (but it won't even get referenced by the Altia code if
     * EGL_HAS_TRANSBITMAPCREATE is defined as 0 in "egl_Wrapper.h").
     */
    egl_BitmapDestroy(devId, (EGL_DDB_ID) tDdbId, hasAlpha);

    return EGL_STATUS_OK;
}
#endif /* EGL_HAS_TRANSBITMAPCREATE */



/**************************************************************************
 * Bitmap and Clip region management functions required for rendering
 * scaled/rotated/stretched Rasters (color bitmaps), scaled/rotated/
 * stretched Stencils (monochrome bitmaps), and scaled/rotated/stretched
 * text.
 *
 * NOTE:  If support for these features is not required in the target or
 *        not possible, just leave these functions empty and always
 *        generate DeepScreen code with Bitmap/Text scaling disabled.
 ***************************************************************************/




#if !EGL_HAS_MONOBITMAPREAD || !EGL_USE_ALTIABITSFORSCALING




#ifndef FB_DIRECT_COLOR_BLENDING
extern CARD32 * pDeviceClut;
#endif /* !FB_DIRECT_COLOR_BLENDING */


extern EGL_COLOR altiaLibGetColor(ALTIA_WINDOW devinfo, ALTIA_COLOR pixel);
static int maskShift(unsigned long p)
{
    int s;

    if (!p) return 0;
    s = 0;
    while (!(p & 1))
    {
        s++;
        p >>= 1;
    }
    return s;
}
static int maskNorm(unsigned int p)
{
    int s = 0;
    while (p & 1)
    {
        s++;
        p >>= 1;
    }
    return ((s < 8) ? 8 - s : 0);
}
/*-------------------------------------------------------------------------
 * EGL_STATUS egl_BitmapRead (EGL_DEVICE_ID devId, EGL_DDB_ID ddbId,
 *                            EGL_POS srcLeft, EGL_POS srcTop,
 *                            EGL_POS srcRight, EGL_POS srcBottom,
 *                            EGL_DIB *pDib, EGL_POS dstX, EGL_POS dstY,
 *                            EGL_INT hasAlpha)
 *
 *  NOTE:  THE IMPLEMENTATION OF THIS FUNCTION IS ONLY REQUIRED FOR
 *         SUPPORTING THE RENDERING OF SCALED, ROTATE, AND/OR STRETCHED
 *         COLOR BITMAPS AND TEXT.  IF THESE FEATURES ARE NOT REQUIRED
 *         BY TARGET APPLICATIONS, THIS FUNCTION DOES NOT NEED TO BE
 *         IMPLEMENTED.
 *
 *         EVEN IF SCALED/ROTATED/STRETCHED COLOR BITMAP AND TEXT
 *         RENDERING MUST BE SUPPORTED, THIS FUNCTION IS STILL NOT
 *         REQUIRED IF IT IS POSSIBLE TO IMPLEMENT egl_MonoBitmapRead()
 *         AND IF IT IS POSSIBLE TO READ THE ALTIA RASTER DATA FOR
 *         BUILDING A SCALED/ROTATED/STRETCHED VERSION OF AN ALTIA
 *         RASTER OBJECT.  IT SHOULD ALWAYS BE POSSIBLE TO READ THE
 *         ALTIA RASTER DATA FOR THIS PURPOSE SO THE ABILITY TO
 *         IMPLEMENT egl_MonoBitmapRead() IS USUALLY THE DETERMINING
 *         FACTOR.
 *
 * In Altia's usage (if it is actually necessary for Altia to use
 * this function), the source DDB "ddbId" is a device dependent color
 * bitmap and the destination "pDib" is always a true color DIB (image
 * format of EGL_DIB_DIRECT) with the pixel color format of
 * EGL_DEVICE_COLOR_32 if hasAlpha and EGL_DEVICE_COLOR 24 bit otherwise.
 *
 * A detailed description for this graphics library function is not
 * yet written.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_BitmapRead(EGL_DEVICE_ID devId, EGL_DDB_ID ddbId,
                          EGL_POS srcLeft, EGL_POS srcTop,
                          EGL_POS srcRight, EGL_POS srcBottom,
                          EGL_DIB *pDib, EGL_POS dstX, EGL_POS dstY,
                          EGL_INT hasAlpha)
{
    PixmapPtr xPixmap;
    int width, height;
    EGL_POS x, y;
    AltiaDevInfo devInfo;
    GCPtr tempgc;
    EGL_COLOR r,g,b;

    static unsigned char initmasks = 0;
    static unsigned int rBitsShift;
    static unsigned int rBitsMask;
    static unsigned int rBitsNorm;
    static unsigned int gBitsShift;
    static unsigned int gBitsMask;
    static unsigned int gBitsNorm;
    static unsigned int bBitsShift;
    static unsigned int bBitsMask;
    static unsigned int bBitsNorm;

    if (ddbId == NULL || pDib == NULL)
    {
        return ~(EGL_STATUS_OK);
    }

    if (!initmasks)
    {
        EGL_ARGB ARGBcolor;

        initmasks = 1;

        /* With the introduction of Alpha Mask, we cannot rely upon
        ** altiaLibGetColor() to obtain the red, green, and blue
        ** values.  Instead we must manually ask the driver using
        ** an empty screenptr (which should force a full color
        ** mapping of the ARGB value).
        */
        ARGBcolor = EGL_MAKE_ARGB(0xff, 0xff, 0, 0);
        driver_ColorAlloc(devId, NULL, &ARGBcolor, NULL, &r, 1);
        ARGBcolor = EGL_MAKE_ARGB(0xff, 0, 0xff, 0);
        driver_ColorAlloc(devId, NULL, &ARGBcolor, NULL, &g, 1);
        ARGBcolor = EGL_MAKE_ARGB(0xff, 0, 0, 0xff);
        driver_ColorAlloc(devId, NULL, &ARGBcolor, NULL, &b, 1);

#ifndef FB_DIRECT_COLOR_BLENDING
        /*
         * r, g, b colors are indexed so lookup them up.
         */
        r = (unsigned long)pDeviceClut[r];
        g = (unsigned long)pDeviceClut[g];
        b = (unsigned long)pDeviceClut[b];
#endif /* !FB_DIRECT_COLOR_BLENDING */

        /* BitsShift indicates how many right shifts are needed
         * to get the color's bits into the low bits of the low byte.
         * BitsMask is these low bits of the low byte turned on (1).
         * BitsNorm is left shifts needed to normalize to 8-bit intensity.
         */
        rBitsShift   = maskShift(r);
        rBitsMask    = (unsigned int)((r) >> rBitsShift);
        rBitsNorm    = maskNorm(rBitsMask);
        gBitsShift   = maskShift(g);
        gBitsMask    = (unsigned int)((g) >> gBitsShift);
        gBitsNorm    = maskNorm(gBitsMask);
        bBitsShift   = maskShift(b);
        bBitsMask    = (unsigned int)((b) >> bBitsShift);
        bBitsNorm    = maskNorm(bBitsMask);
    }

    xPixmap = (PixmapPtr) (ddbId);
    devInfo.drawable = (DrawablePtr)xPixmap;

    /* We won't handle any dstX and dstY values other than 0.
     * Altia won't request any other values so there is no
     * code here to do it any other way.
     */
    dstX = dstY = 0;

    /* We won't deal with trying to read from the MDDB in any way
     * other than top-left to bottom-right.  Altia won't ask us
     * to do it any other way so there is no code here for handling
     * the other cases.
     */
    if (srcRight < srcLeft)
    {
        x = srcRight;
        srcRight = srcLeft;
        srcLeft = x;
    }
    if (srcBottom < srcTop)
    {
        y = srcBottom;
        srcBottom = srcTop;
        srcTop = y;
    }

    width = srcRight - srcLeft + 1;
    height = srcBottom - srcTop + 1;

    tempgc = GetScratchGC(SCREENPTR->rootDepth, SCREENPTR);
    /* Our algorithm is actually incomplete because we are assuming
     * that dstY and dstX are always 0.  To handle other values,
     * we would need to take into account starting or ending on a
     * bit that isn't a multiple of 8.  This would require us to
     * preserve existing bits from the MDIB by masking them as we
     * start and end each line.  We don't do it.
     */
#if EGL_DO_ALPHA_BLENDING
    if (hasAlpha)
    {
        EGL_UINT32 *dataPtr;
        if (NULL != xPixmap->drawable.alphas)
        {
            FbBits * aBits;
            FbStride aStride;
            int xoff, yoff, bpp;

            /* Alpha image metrics (stride in FbBits units) */
            fbGetDrawable((DrawablePtr)(xPixmap->drawable.alphas), aBits, aStride, bpp, xoff, yoff);

            /* Fix stride to be pixels */
            aStride = (aStride * sizeof(FbBits)) / (bpp >> 3);

            for (y = 0; y < height; y++)
            {
                CARD8 * alphaPtr = (CARD8 *)aBits + (aStride * y);
                dataPtr = (EGL_UINT32*)
                          ((EGL_UINT8 *)pDib->pImage + ((pDib->stride*4) * y));
                for (x = 0; x < width; x++)
                {
                    EGL_COLOR alpha;
                    EGL_COLOR pixel =  egl_PixelGet((ALTIA_WINDOW)&devInfo,
                                                    tempgc, x, y);
#ifndef FB_DIRECT_COLOR_BLENDING
                    pixel = (unsigned long)pDeviceClut[pixel];
#endif /* !FB_DIRECT_COLOR_BLENDING */
                    alpha = (EGL_COLOR)*alphaPtr++;

                    r = ((pixel >> rBitsShift) & rBitsMask) << rBitsNorm;
                    g = ((pixel >> gBitsShift) & gBitsMask) << gBitsNorm;
                    b = ((pixel >> bBitsShift) & bBitsMask) << bBitsNorm;
                    *dataPtr = EGL_MAKE_ARGB(alpha, r, g, b);
                    dataPtr++;
                }
            }
        }
        else
        {
            for (y = 0; y < height; y++)
            {
                dataPtr = (EGL_UINT32*)
                          ((EGL_UINT8 *)pDib->pImage + ((pDib->stride*4) * y));
                for (x = 0; x < width; x++)
                {
                    EGL_COLOR pixel =  egl_PixelGet((ALTIA_WINDOW)&devInfo,
                                                    tempgc, x, y);
#ifndef FB_DIRECT_COLOR_BLENDING
                    pixel = (unsigned long)pDeviceClut[pixel];
#endif /* !FB_DIRECT_COLOR_BLENDING */
                    r = ((pixel >> rBitsShift) & rBitsMask) << rBitsNorm;
                    g = ((pixel >> gBitsShift) & gBitsMask) << gBitsNorm;
                    b = ((pixel >> bBitsShift) & bBitsMask) << bBitsNorm;
                    *dataPtr = EGL_MAKE_ARGB(0xff, r, g, b);
                    dataPtr++;
                }
            }
        }
    }
    else
#endif
    { /* 24 bit */
        EGL_UINT8 *dataPtr;
        for (y = 0; y < height; y++)
        {
            dataPtr = (EGL_UINT8 *)pDib->pImage + ((pDib->stride*3) * y);
            for (x = 0; x < width; x++)
            {
                EGL_COLOR pixel =  egl_PixelGet((ALTIA_WINDOW)&devInfo,
                                                tempgc, x, y);
#ifndef FB_DIRECT_COLOR_BLENDING
                pixel = (unsigned long)pDeviceClut[pixel];
#endif /* !FB_DIRECT_COLOR_BLENDING */
                *(dataPtr++) = ((pixel >> rBitsShift) & rBitsMask) << rBitsNorm;
                *(dataPtr++) = ((pixel >> gBitsShift) & gBitsMask) << gBitsNorm;
                *(dataPtr++) = ((pixel >> bBitsShift) & bBitsMask) << bBitsNorm;
            }
        }
    }
    FreeScratchGC(tempgc);
    return EGL_STATUS_OK;
}



#endif  /* NOT EGL_HAS_MONOBITMAPREAD || NOT EGL_USE_ALTIABITSFORSCALING */





#if EGL_HAS_MONOBITMAPREAD
/*-------------------------------------------------------------------------
 * extern EGL_STATUS egl_MonoBitmapRead(EGL_GC_ID gc, EGL_MDDB_ID mDdbId,
 *                                      EGL_POS srcLeft, EGL_POS srcTop,
 *                                      EGL_POS srcRight, EGL_POS srcBottom,
 *                                      EGL_MDIB *pMDib,
 *                                      EGL_POS dstX, EGL_POS dstY)
 *
 *  NOTE:  THE IMPLEMENTATION OF THIS FUNCTION IS ONLY REQUIRED FOR
 *         SUPPORTING THE RENDERING OF SCALED, ROTATE, AND/OR STRETCHED
 *         TEXT.  IF THIS FEATURE IS NOT REQUIRED BY TARGET APPLICATIONS,
 *         THIS FUNCTION DOES NOT NEED TO BE IMPLEMENTED.
 *
 * If the target graphics library can properly draw to a monochrome bitmap
 * and read back from it, it makes rendering of scaled/stretched/rotated
 * text much easier.  In such cases, "egl_Wrapper.h" should define
 * EGL_HAS_MONOBITMAPREAD as non-zero (1) and Altia code will use
 * egl_MonoBitmapRead().  If EGL_HAS_MONOBITMAPREAD is defined as 0, the
 * Altia code will create a color device dependent bitmap (DDB) and use the
 * more difficult approach of calling egl_BitmapRead() and egl_ColorConvert().
 * On some targets (e.g., pre X11R6 servers), it is ABSOLUTELY NECESSARY to
 * have egl_MonoBitmapRead() because there is a limited pool of DDB IDs
 * available and the pool is easily exhausted by frequently creating and
 * destroying temporary DDBs.
 *
 * The source "mDdbId" is a monochrome device dependent bitmap (MDDB)
 * and the destination "pMDib" is just a simple monchrome device
 * independent bitmap (MDIB).
 *
 * This function reads the data of the source MDDB from (srcLeft,srcTop)
 * to (srcRight,srcBottom) and puts the data into the destination MDIB
 * starting at (dstX,dstY).
 *
 * In Altia's usage, dstX and dstY values are always 0.  The Altia code
 * will not request any other values so implementation of all other cases
 * is unnecessary.
 *
 * In Altia's usage, (srcLeft,srcTop) is always top-left and (srcRight,
 * srcBottom) is always bottom-right.  The Altia code will not ask to
 * do it any other way.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_MonoBitmapRead(EGL_GC_ID gc, EGL_MDDB_ID mDdbId,
                              EGL_POS srcLeft, EGL_POS srcTop,
                              EGL_POS srcRight, EGL_POS srcBottom,
                              EGL_MDIB *pMDib,
                              EGL_POS dstX, EGL_POS dstY)
{
    PixmapPtr xPixmap;
    EGL_UINT8 *dataPtr;
    int width, height;
    EGL_POS x, y;
    int bitMask;
    AltiaDevInfo devInfo;
    GCPtr monogc;

    if (mDdbId == NULL || pMDib == NULL)
        return ~(EGL_STATUS_OK);

    xPixmap = (PixmapPtr) (mDdbId);
    devInfo.drawable = (DrawablePtr)xPixmap;

    /* We won't handle any dstX and dstY values other than 0.
     * Altia won't request any other values so there is no
     * code here to do it any other way.
     */
    dstX = dstY = 0;

    /* We won't deal with trying to read from the MDDB in any way
     * other than top-left to bottom-right.  Altia won't ask us
     * to do it any other way so there is no code here for handling
     * the other cases.
     */
    if (srcRight < srcLeft)
    {
        x = srcRight;
        srcRight = srcLeft;
        srcLeft = x;
    }
    if (srcBottom < srcTop)
    {
        y = srcBottom;
        srcBottom = srcTop;
        srcTop = y;
    }

    width = srcRight - srcLeft + 1;
    height = srcBottom - srcTop + 1;

    monogc = GetScratchGC(1, SCREENPTR);
    /* Our algorithm is actually incomplete because we are assuming
     * that dstY and dstX are always 0.  To handle other values,
     * we would need to take into account starting or ending on a
     * bit that isn't a multiple of 8.  This would require us to
     * preserve existing bits from the MDIB by masking them as we
     * start and end each line.  We don't do it.
     */
    for (y = 0; y < height; y++)
    {
        dataPtr = pMDib->pImage + ((pMDib->stride / 8) * y);
        bitMask = 0x80;

        for (x = 0; x < width; x++)
        {
            if (bitMask == 0x80)
                *dataPtr = 0;

            if (egl_PixelGet(&devInfo, monogc, x, y) != 0)
                *dataPtr |= bitMask;

            if ((bitMask >>= 1) == 0)
            {
                dataPtr++;
                bitMask = 0x80;
            }
        }
    }
    FreeScratchGC(monogc);

    return EGL_STATUS_OK;
}
#endif  /* EGL_HAS_MONOBITMAPREAD */


#if EGL_HAS_MONOBITMAPWRITE
/*-------------------------------------------------------------------------
 * EGL_STATUS egl_MonoBitmapWrite(EGL_GC_ID unusedGc, EGL_MDIB *pMDib,
 *                                EGL_POS srcLeft, EGL_POS srcTop,
 *                                EGL_POS srcRight, EGL_POS srcBottom,
 *                                EGL_MDDB_ID mDdbId,
 *                                EGL_POS dstX, EGL_POS dstY)
 *
 *  NOTE:  THE IMPLEMENTATION OF THIS FUNCTION IS ONLY REQUIRED FOR
 *         SUPPORTING THE RENDERING OF SCALED, ROTATE, AND/OR STRETCHED
 *         MONOCHROME BITMAPS (ALTIA STENCIL OBJECTS) OR TEXT.  IF THESE
 *         FEATURES ARE NOT REQUIRED BY TARGET APPLICATIONS, THIS FUNCTION
 *         DOES NOT NEED TO BE IMPLEMENTED.
 *
 * If the target graphics library can properly write monochrome device
 * independent bitmap (MDIB) data to an existing monochrome device
 * dependent bitmap (MMDB), it makes rendering of scaled/stretched/rotated
 * Altia Stencil objects and text much easier.  In such cases,
 * "egl_Wrapper.h" should define EGL_HAS_MONOBITMAPWRITE as non-zero (1)
 * and Altia code will use egl_MonoBitmapWrite().  If "egl_Wrapper.h"
 * defines EGL_HAS_MONOBITMAPWRITE as 0, the Altia code will create an
 * entirely new MDDB.  On some targets (e.g., pre X11R6 servers), it is
 * ABSOLUTELY NECESSARY to have egl_MonoBitmapWrite() because there is
 * a limited pool of MDDB IDs available and the pool is easily exhausted
 * by frequently creating and destroying temporary MDDBs.
 *
 * The source "pMDib" is a pointer to a monochrome device independent
 * bitmap (MDIB) data structure and the destination "mDdbId" is the
 * ID for a monochrome device dependent bitmap (MDDB).
 *
 * This function writes the data of the source MDIB from (srcLeft,srcTop)
 * to (srcRight,srcBottom) and puts the data into the destination MDDB
 * starting at (dstX,dstY).
 *
 * In Altia's usage, dstX and dstY values are always 0.  The Altia code
 * will not request any other values so implementation of all other cases
 * is unnecessary.
 *
 * In Altia's usage, (srcLeft,srcTop) is always top-left (0,0) and
 * (srcRight, srcBottom) is always bottom-right (pMDib->width - 1,
 * pMDib->height - 1).  The Altia code will not ask to do it any
 * other way so implementation of all other cases is unnecessary.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_MonoBitmapWrite(EGL_GC_ID unusedGc, EGL_MDIB *pMDib,
                               EGL_POS srcLeft, EGL_POS srcTop,
                               EGL_POS srcRight, EGL_POS srcBottom,
                               EGL_MDDB_ID mDdbId,
                               EGL_POS dstX, EGL_POS dstY)
{
    GCPtr gc;
    PixmapPtr xPixmap;
    int width, height;
    EGL_UINT8 *dataPtr;
    EGL_POS x, y;
    int bitMask;
    AltiaDevInfo devInfo;

    if (mDdbId == NULL || pMDib == NULL || pMDib->pImage == NULL)
        return ~(EGL_STATUS_OK);

    width = pMDib->width;
    height = pMDib->height;

    if ((xPixmap = (PixmapPtr) (mDdbId)) == (PixmapPtr) 0)
        return ~(EGL_STATUS_OK);

    gc = GetScratchGC(1, SCREENPTR);
    devInfo.drawable = (DrawablePtr)xPixmap;
    /* Since we are dealing with text most often and text
     * is usually contains more zero's then ones, it should be
     * faster to clear the bitmap and then add just the ones
     */
    egl_BackgroundColorSet(gc, 0);
    egl_FillPatternSet(gc, NULL);
    egl_Rectangle(&devInfo, gc, (EGL_POS)0, (EGL_POS)0,
                  (EGL_POS)width, (EGL_POS)height, TRUE, FALSE);
    for (y = 0; y < height; y++)
    {
        dataPtr = pMDib->pImage + ((pMDib->stride / 8) * y);
        bitMask = 0x80;

        for (x = 0; x < width; x++)
        {
            if (*dataPtr & bitMask)
                egl_PixelSet(&devInfo, gc, x, y, 1);

            if ((bitMask >>= 1) == 0)
            {
                bitMask = 0x80;
                dataPtr++;
            }
        }
    }
    FreeScratchGC(gc);

    return EGL_STATUS_OK;
}
#endif  /* EGL_HAS_MONOBITMAPWRITE */


/*-------------------------------------------------------------------------
 * EGL_STATUS egl_BitmapSize(EGL_DDB_ID bitmap, int *width, int *height,
 *                           EGL_INT hasAlpha)
 *
 *  NOTE:  THE IMPLEMENTATION OF THIS FUNCTION IS ONLY REQUIRED FOR
 *         SUPPORTING THE RENDERING OF SCALED, ROTATE, AND/OR STRETCHED
 *         MONOCHROME BITMAPS (ALTIA STENCIL OBJECTS) OR TEXT.  IF THESE
 *         FEATURES ARE NOT REQUIRED BY TARGET APPLICATIONS, THIS FUNCTION
 *         DOES NOT NEED TO BE IMPLEMENTED.
 *
 *  Return the width and height of a bitmap
 */
EGL_STATUS egl_BitmapSize(EGL_DDB_ID bitmap, int *width, int *height,
                          EGL_INT hasAlpha)
{
    DrawablePtr drawable = (DrawablePtr)bitmap;
    *width = drawable->width;
    *height = drawable->height;
    return EGL_STATUS_OK;
}



