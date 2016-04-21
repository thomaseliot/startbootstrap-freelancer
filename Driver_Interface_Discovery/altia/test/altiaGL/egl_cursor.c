/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.2 $    $Date: 2009-05-06 00:03:00 $
 * Copyright (c) 2007 Altia Inc.
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

/** FILE:  egl_cursor.c ********************************************************
 **
 ** This file contains the cursor management functions for AltiaGL.
 **
 ******************************************************************************/

#include "egl_Wrapper.h"
#include "altiaBase.h"
#include "altiaTypes.h"
#ifdef USE_FB
#include "fb.h"
#endif

typedef struct
{
    EGL_POINT hotSpot;  /* cursor hot spot point */
    PixmapPtr ddb;      /* cursor device dependent bitmap image */
} CURSOR;

extern AltiaDevInfo * altiaLibGetDevInfo();

static CURSOR * currentCursor = NULL;
static AltiaDevInfo * devInfo = NULL;

static EGL_POS lastX = 0;
static EGL_POS lastY = 0;

/******************************************************************************/
EGL_STATUS egl_CursorInit (EGL_DEVICE_ID devId, EGL_POS w, EGL_POS h,
                           EGL_POS cursorX, EGL_POS cursorY)
{
    devInfo = altiaLibGetDevInfo();

    return EGL_STATUS_OK;
}

/******************************************************************************/
EGL_CDDB_ID egl_CursorBitmapCreate (EGL_DEVICE_ID devId, EGL_CDIB * pCDib)
{
    EGL_UINT16 x, y;
    EGL_UINT8 * pSrc = (EGL_UINT8 *)pCDib->pImage;
    EGL_ARGB * cursorDIBData = (EGL_ARGB *)xalloc(
        pCDib->width * pCDib->height * sizeof(EGL_ARGB));
    EGL_ARGB * pDst = (EGL_ARGB *)cursorDIBData;
    EGL_ARGB * pClut = (EGL_ARGB *)pCDib->pClut;
    CURSOR * cursor = NULL;

    EGL_DIB cursorDIB =
    {
        (EGL_SIZE)pCDib->width,                 /* width */
        (EGL_SIZE)pCDib->height,                /* height */
        (EGL_SIZE)pCDib->width,                 /* stride */
        (EGL_POS8)EGL_DIB_DIRECT,               /* image format */
        (EGL_COLOR_FORMAT)EGL_ARGB8888,         /* color format */
        (EGL_SIZE)0,                            /* clut size */
        (void *)0,                              /* pointer to clut */
        (void *)cursorDIBData                   /* pointer to image */
    };

    if(cursorDIBData == NULL)
    {
        return (EGL_CDDB_ID)0;
    }

    /*
     * populate the temporary cursor dib buffer with full 32 bit
     * ARGB color data.
     */
    for(y = 0; y < pCDib->height; y++)
    {
        for(x = 0; x < pCDib->width; x++)
        {
            if(pSrc[x] < pCDib->clutSize)
            {
                pDst[x] = pClut[pSrc[x]];
            }
            else
            {
                pDst[x] = EGL_MAKE_ARGB(0x00, 0xff, 0xff, 0xff);
            }
        }

        pSrc += pCDib->stride;
        pDst += cursorDIB.stride;
    }

    /*
     * allocate a cursor data structure
     */
    if((cursor = xalloc(sizeof(CURSOR))) == NULL)
    {
        return (EGL_CDDB_ID)0;
    }

    /*
     * create a device dependent bitmap from the dib data
     */
    if((cursor->ddb = (PixmapPtr)egl_BitmapCreate(
        devId, &cursorDIB, EGL_DIB_INIT_DATA, 0, NULL)) == (PixmapPtr)0)
    {
        return (EGL_CDDB_ID)0;
    }

    /*
     * save the cursor's hot spot
     */
    cursor->hotSpot = pCDib->hotSpot;

    /*
     * free working buffer that held the temporary cursor dib data
     */
    xfree(cursorDIBData);

    return (EGL_CDDB_ID)cursor;
}

/******************************************************************************/
EGL_STATUS egl_CursorBitmapDestroy (EGL_DEVICE_ID devId, EGL_CDDB_ID pCImage)
{
    CURSOR * cursor = (CURSOR *)pCImage;

    if(cursor == NULL)
    {
        return EGL_STATUS_ERROR;
    }
    
    egl_BitmapDestroy (devId, (EGL_DDB_ID)cursor->ddb, 0x01);
    xfree(cursor);

    return EGL_STATUS_OK;
}

/******************************************************************************/
EGL_STATUS egl_CursorImageSet (EGL_DEVICE_ID devId, EGL_CDDB_ID pCImage)
{
    CURSOR * cursor = (CURSOR *)pCImage;

    if(cursor != currentCursor)
    {
        if(devInfo != NULL && currentCursor != NULL)
        {
            GCPtr pGC = (GCPtr)devInfo->gc;

            /*
             * repair the area of the current cursor position by
             * copying from the off screen buffer to the display.
             */
            (*pGC->ops->CopyArea)(
                (DrawablePtr)devInfo->memBitmap,
                (DrawablePtr)devInfo->display,
                pGC,
                lastX - currentCursor->hotSpot.x,
                lastY - currentCursor->hotSpot.y,
                currentCursor->ddb->drawable.width,
                currentCursor->ddb->drawable.height,
                lastX - currentCursor->hotSpot.x,
                lastY - currentCursor->hotSpot.y);
        }

        currentCursor = cursor;
    }

    return EGL_STATUS_OK;
}

/******************************************************************************/
EGL_CDDB_ID egl_CursorImageGet (EGL_DEVICE_ID devId)
{
    return (EGL_CDDB_ID)currentCursor;
}

/******************************************************************************/
EGL_STATUS egl_CursorOn (EGL_DEVICE_ID devId)
{
    return EGL_STATUS_OK;
}

/******************************************************************************/
EGL_STATUS egl_CursorDeinit (EGL_DEVICE_ID devId)
{
    currentCursor = NULL;

    return EGL_STATUS_OK;
}

/******************************************************************************/
void egl_CursorMove (EGL_DEVICE_ID devId, EGL_POS x, EGL_POS y)
{
    if(devInfo != NULL && currentCursor != NULL)
    {
        GCPtr pGC = (GCPtr)devInfo->gc;

        /*
         * first, repair the area of the current cursor position by
         * copying from the off screen buffer to the display
         */
        (*pGC->ops->CopyArea)(
            (DrawablePtr)devInfo->memBitmap,
            (DrawablePtr)devInfo->display,
            pGC,
            lastX - currentCursor->hotSpot.x,
            lastY - currentCursor->hotSpot.y,
            currentCursor->ddb->drawable.width,
            currentCursor->ddb->drawable.height,
            lastX - currentCursor->hotSpot.x,
            lastY - currentCursor->hotSpot.y);

        /*
         * now paint the cursor in the new position
         */
        (*pGC->ops->AlphaCopyArea)(
            (DrawablePtr)currentCursor->ddb,
            (DrawablePtr)devInfo->display,
            (DrawablePtr)fbGetAlphasPixmap((DrawablePtr)currentCursor->ddb),
            pGC, 0, 0,
            currentCursor->ddb->drawable.width,
            currentCursor->ddb->drawable.height,
            x - currentCursor->hotSpot.x,
            y - currentCursor->hotSpot.y);

        /*
         * save the current x and y
         */
        lastX = x; lastY = y;
    }
}

/******************************************************************************/
void egl_CursorRepaint (EGL_DEVICE_ID devId)
{
    if(devInfo != NULL && currentCursor != NULL)
    {
        GCPtr pGC = (GCPtr)devInfo->gc;

        /*
         * repaint the cursor in the last reported cursor position
         */
        (*pGC->ops->AlphaCopyArea)(
            (DrawablePtr)currentCursor->ddb,
            (DrawablePtr)devInfo->display,
            (DrawablePtr)fbGetAlphasPixmap((DrawablePtr)currentCursor->ddb),
            pGC, 0, 0,
            currentCursor->ddb->drawable.width,
            currentCursor->ddb->drawable.height,
            lastX - currentCursor->hotSpot.x,
            lastY - currentCursor->hotSpot.y);
    }
}
