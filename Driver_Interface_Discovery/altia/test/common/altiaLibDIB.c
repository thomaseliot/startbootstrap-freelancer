/* $Revision: 1.17 $    $Date: 2009-07-31 00:34:35 $
 * Copyright (c) 2001 Altia Inc.
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

/** FILE:  altiaLibDIB.c ***************************************************
 **
 ** This file contains routines to create and initialize target specific
 ** device independent bitmaps (DIBs) and convert them to target specific
 ** device dependent bitmaps (DDBs).  If monochrome DIBs and DDBs are a
 ** different format on the target, then this file contains the routines
 ** to handle creation, initialization, and conversion of these as well.
 ** The routines in this file are only required if code was generated for
 ** Altia rasters, stencils, and/or scaled/stretched/rotated text.  If a
 ** design has no such objects, this file contains no code.
 **
 ** Monochrome and color bitmap creation, initialization, and manipulation
 ** are tedious operations to port from one target to another, but usually
 ** not a significant technical challenge.  For the most part, all of the
 ** routines in this file will need some modification to support a different
 ** target.
 ***************************************************************************/

#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "../altiaImageAccess.h"
#include <string.h>
#include <stdlib.h>
#include "egl_Wrapper.h"

/* The following routine is generated because the Altia design
 * has rasters or scaled text.  This routine is not generated
 * if a design has stencils, but no rasters or scaled text.
 */

/*--- altiaPaletteSize() --------------------------------------------------
 *
 * Given a a number of color bits, this function computes the maximum
 * size, in fundamental units (e.g., bytes), of the color palette
 * that would be needed for the bitmap.  For example, a bitmap in
 * indexed format with 8-bit indexes may need a 256 color palette.
 *
 * Used for DIB initialization or bilinear raster scaling (including the
 * scaling of Alpha Mask objects).
 *-------------------------------------------------------------------------*/
int altiaPaletteSize(int numBits)
{
    int palSize;
    switch (numBits)
    {
        case 1:
            palSize = 2 * sizeof(EGL_ARGB);
            break;
        default:
            palSize = (numBits <= 8) ? 256 * sizeof(EGL_ARGB) : 0;
            break;
    }
    return palSize;
}




/* The following macros allow for determining the number of bytes
 * in a row of image data given the number of bits of image data
 * in the row.  The first macro gives the number of bytes aligned
 * on a 32-bit boundary.  The second macro gives the number of
 * bytes aligned on an 8-bit byte boundary.  Some targets require
 * the alignment of each row of data on a 32-bit boundary and the
 * BITSTOINT32(bits) macro should be used in these cases.  Others
 * targets, like this one, only require that a row of image data
 * align on a byte boundary and the BITSTOBYTES(bits) is used.
 */
#define BITSTOINT32(bits)    ((((bits) + 31) / 32) * 4)
#define BITSTOBYTES(bits)    (((bits) + 7) / 8)


/* Here are global message strings that all target specific Altia
 * code can use if allocation of the various types of bitmaps fail.
 */
ALTIA_CHAR *altiaMsgNoMemColorDDB
    = ALT_TEXT("Not enough memory to allocate color DDB");
ALTIA_CHAR *altiaMsgNoMemColorDIB
    = ALT_TEXT("Not enough memory to allocate color DIB");
ALTIA_CHAR *altiaMsgNoMemMonoDDB
    = ALT_TEXT("Not enough memory to allocate mono DDB");
ALTIA_CHAR *altiaMsgNoMemMonoDIB
    = ALT_TEXT("Not enough memory to allocate mono DIB");


/* This file begins with routines that are needed if an Altia
 * design has rasters, stencils, or scaled text objects.  All
 * of these types of objects need the following routines.
 */

/*--- altiaLibInitMonoDIB() -----------------------------------------------
 *
 * Initialize a target specific monochrome device independent bitmap (MDIB)
 * object.  On this target, an MDIB is different from a color DIB so we need
 * this function.  Other targets may not.  The MDIB pointer must actually
 * reference memory that is large enough for the MDIB data structure and
 * the bytes to hold the bitmap data.  If the dataPtr argument is not NULL,
 * it is set to point to the data section of the MDIB so that the caller
 * can fill the data area if desired.
 *-------------------------------------------------------------------------*/
void altiaLibInitMonoDIB(EGL_MDIB *mdibPtr, int width,
                         int height, ALTIA_UBYTE **dataPtr)
{


    /* On this target, the stride defines how many bits make
     * up 1 row of image data.  We are going to align our rows
     * of data on byte boundaries so the stride is the length
     * of a row in bytes * 8.  Some bits of the last byte in a
     * row may go unused.
     */
    mdibPtr->stride = BITSTOBYTES(width) * 8;


    mdibPtr->width = width;
    mdibPtr->height = height;

    /* One complete data area was allocated for the monochrome
     * bitmap structure and data so the data is going to start
     * immediately following the structure.
     */
    mdibPtr->pImage = (ALTIA_UBYTE *) mdibPtr + sizeof(EGL_MDIB);
    if (dataPtr != NULL)
        *dataPtr = mdibPtr->pImage;
}


/*--- altiaLibCreateMonoDIB() ---------------------------------------------
 *
 * Function called from target specific Altia code to create a target
 * specific monochrome device independent bitmap (MDIB) data structure
 * that is ready to be loaded with data.  The function returns a pointer
 * to the MDIB data structure or NULL.
 *
 * If memory allocation is NOT allowed, a global array for creating
 * temporary DIB/MDIB data structures is used (except for some special
 * situations given below).  Otherwise, memory is allocated for the MDIB
 * and it is up to the caller to free the memory when they are finished
 * with the MDIB.
 *
 * If the forTempScaledBits flag is true and memory allocation is NOT
 * allowed, the function will use the secondary bitmap data area which is
 * large enough to hold temporary scaled data.  On the other hand, if
 * there really is scaled text in a design AND forScaledText is true AND
 * forTempScaledBits is false, this function will force the use of memory
 * allocation because it isn't guaranteed that the global data area is
 * large enough to hold the MDIB data structure.  In this case, the
 * caller must free the memory when they are finished with the MDIB.
 *
 * If the dataPtr argument is not NULL, it is set to point to the data
 * section of the MDIB so that the caller can fill the data area if desired.
 *-------------------------------------------------------------------------*/
EGL_MDIB *altiaLibCreateMonoDIB(int width, int height,
                                ALTIA_BOOLEAN forTempScaledBits,
                                ALTIA_BOOLEAN forScaledText,
                                ALTIA_UBYTE **dataPtr)
{
    EGL_MDIB *mdibPtr = NULL;
    ALTIA_INT total;
    int widthBytes;


    /* In a monochrome bitmap where 8 bits are packed into a
     * byte, the width in bytes must be sufficient to hold the
     * bits for 1 row of the image.  Some of the trailing bits
     * of the last byte in a row may be unused if the bit width
     * is not a multiple of 8.
     *
     * If the rows of bits must be aligned in some special way
     * for the target, the widthBytes variable can be adjusted
     * to account for the alignment.  Say each row must align
     * on a 32-bit boundary.  The BITSTOINT32(bits) macro
     * defined at the beginning of this file can be used to do:
     *     widthBytes = BITSTOINT32(width * byteSize);
     *
     * For this target, nothing special is necessary so:
     *     widthBytes = BITSTOBYTES(width) * byteSize;
     */
    widthBytes = BITSTOBYTES(width) * sizeof(ALTIA_UBYTE);

    /* For this target, the device independent bitmap structure is
     * pretty simple so all we need is enough space to hold the
     * data bytes and the structure itself.
     */
    total = (widthBytes * height) + sizeof(EGL_MDIB);

#if !ALTIA_DIB_MALLOC

    /* We are NOT allowed to malloc and bitmap scaling
     * is allowed so use global arrays for temporary bitmap
     * data if possible, but make sure they are large enough.
     */

    {
        if (forTempScaledBits)
        {
            /* Caller wants a MDIB for the temporary pixels of a scaled
             * stencil.  If there's not enough room to scale the stencil,
             * return NULL.
             */
            if (total > test_altiaMaxBitmap2)
            {
                _altiaErrorMessage(ALT_TEXT("No space to scale stencil"));
                return NULL;
            }
            mdibPtr = (EGL_MDIB *) test_altiaBitmapData2;
        }
        else
        {
            mdibPtr = (EGL_MDIB *) test_altiaBitmapData;
        }
    }


#endif /* !ALTIA_DIB_MALLOC */

    /* Fill in the monochrome device independent bitmap (MDIB) data
     * structure.
     */
    altiaLibInitMonoDIB(mdibPtr, width, height, dataPtr);

    return mdibPtr;
}


/*--- altiaLibDIBToMonoBitmap() -------------------------------------------
 *
 * Create a target specific monochrome device dependent bitmap (MDDB) from
 * a target specific monochrome device independent bitmap (MDIB).
 * The MDIB pointer that is passed as an argument must be completely
 * initialized.  The MDDB identifier or 0 is returned by this function.
 * If the retBits argument is not NULL, it is set to point to the data
 * section of the MDDB if the data section is public on this target.  If
 * it is not public, it is just set to the MDDB identifier (which is also
 * the return value).  On this target, an MDDB is different from a color
 * DDB so we need this function.  Other targets may not.  Finally, creation
 * of the MDDB by the target graphics library is likely to require memory
 * allocation on most target.  It is the caller's responsibility to
 * delete the MDDB when it is no longer needed.
 *-------------------------------------------------------------------------*/
ALTIA_UINT32 altiaLibDIBToMonoBitmap(EGL_DEVICE_ID devId, EGL_MDIB *mdibPtr,
                                     ALTIA_UINT32 *retBits)
{
    EGL_MDDB_ID mddb;  /* Hold id of device-dependent mono bitmap (MDDB) */
 
    if (mdibPtr == NULL)
        return 0;

    /* Create the monochrome DDB.  use the retBits parameter as a reference
    ** ID.  This can be used as an indicator to egl_MonoBitmapCreate() as
    ** to what the monochrome DDB is used for.  Currently the retBits
    ** parameter will be non-NULL for the following cases:
    **
    ** 1. mask for drawing area object image
    ** 2. mask for image object from file system
    ** 
    **/
    mddb = egl_MonoBitmapCreate(devId, mdibPtr, EGL_DIB_INIT_DATA, 
                                0, (EGL_REFERENCE_ID)retBits);
    if (NULL == mddb)
    {
        _altiaErrorMessage(altiaMsgNoMemMonoDDB);
        return 0;
    }

    /* Caller may need pointer to actual MDDB data for bitmap scaling.
     * For this target graphics library, we don't have a direct way to
     * get this data from MDDB structure so just pass MDDB id.
     */
    if (retBits != NULL)
        *retBits = (ALTIA_UINT32)mddb;
 
    /* Return MDDB id */
    return (ALTIA_UINT32)mddb;
}

/* The following routines are generated because the Altia design
 * has rasters or stencils.  These routines are not generated
 * if a design has scaled text, but no rasters or stencils.
 */



/*--- altiaLibDecodePixel() -----------------------------------------------
 *
 * This function gets the next color or bit value from an Altia
 * generated array of color or monochrome bitmap data.  These arrays
 * are compressed to minimize data size.
 *
 * WARNING: This function assumes you are getting pixels or bits
 *          sequentally.  For reasons of optimization, the code only
 *          works this way.
 *
 * NOTE:  This is one of the few or only functions in this file that
 *        should NOT require any changes between targets.
 *
 * count - the pixel or bit number that you want.
 * index - the array index number of the last count byte (byte count
 *         byte where n means the next n bytes are the same and
 *         -n means the next n bytes are different
 * indexCnt - the pixel number where the next byte following the
 *            index byte is.
 *-------------------------------------------------------------------------*/
ALTIA_UBYTE altiaLibDecodePixel(ALTIA_CONST ALTIA_UBYTE *pixels, 
                                ALTIA_INT *count,
                                ALTIA_INT *index, ALTIA_INT *indexCnt)
{
     int cur = *index;
     register int cntByte = (signed char)pixels[cur];    
     register int offset = *count - *indexCnt;

     if (cntByte < 0)
     { /* index points to n different bytes */
         if (offset < -cntByte)
         { /* we are within the offset */
             (*count)++;
             return pixels[cur + offset + 1];
         }else
         { /* we need to look at next index byte */
             *index -= cntByte - 1;
             *indexCnt -= cntByte;
             (*count)++;
             return pixels[*index + 1];
         }
     }else
     { /* index points to n+1 same bytes */
         if (offset < cntByte)
         { /* we are withing the offset */
             (*count)++;
             return pixels[cur + 1];
         }else
         { /* we need to look at next index byte */
             *index += 2;
             *indexCnt += cntByte;
             (*count)++;
             return pixels[*index + 1];
         }
     }
}




/*--- altiaLibLoadMonoDIB() -----------------------------------------------
 *
 * Function called from target specific Altia code to initialize and load
 * data from Altia monochrome bitmap information into a target specific
 * monochrome device independent bitmap (MDIB) data structure.  A pointer
 * to the MDIB is provided by the caller.  This operation must be done for
 * a stencil as well as the transparent bits of a raster and it may be
 * necessary when a transformed stencil or transformed raster with
 * transparent bits is drawn if scaled bitmaps are allowed.
 *-------------------------------------------------------------------------*/
void altiaLibLoadMonoDIB (ALTIA_CONST ALTIA_UBYTE *altiaBitPtr,
                          ALTIA_UINT32 altiaTotalBits,
                          ALTIA_SHORT width, ALTIA_SHORT height,
                          EGL_MDIB *destDIB)
{
    ALTIA_UBYTE *dataPtr;

    int byteSize;
    int widthBytes;
    int remCnt;
    long totalBits = altiaTotalBits;
    ALTIA_UBYTE *minPtr;
    ALTIA_INT nextCount = 0;
    ALTIA_INT nextIndex = 0;
    ALTIA_INT nextIndexCnt = 0;

    int widthCnt = 0;

    /* On this target, we store 8 bits of monochrome bitmap
     * data in a byte element.
     */
    byteSize = sizeof(ALTIA_UBYTE);

    /* In a monochrome bitmap where 8 bits are packed into a
     * byte, the width in bytes must be sufficient to hold the
     * bits for 1 row of the image.  Some of the trailing bits
     * of the last byte in a row may be unused if the bit width
     * is not a multiple of 8.
     *
     * If the rows of bits must be aligned in some special way
     * for the target, the widthBytes variable can be adjusted
     * to account for the alignment.  Say each row must align
     * on a 32-bit boundary.  The BITSTOINT32(bits) macro
     * defined at the beginning of this file can be used to do:
     *     widthBytes = BITSTOINT32(width * byteSize);
     * remCnt should still = BITSTOBYTES(width) * byteSize;
     *
     * For this target, nothing special is necessary so:
     *     widthBytes = BITSTOBYTES(width) * byteSize;
     * and remCnt is equivalent to widthBytes.
     */
    remCnt = widthBytes = BITSTOBYTES(width) * byteSize;

    /* Initialize our monochrome device independent bitmap (MDIB)
     * data structure.  It returns a pointer to the data section
     * of the MDIB for us.
     */
    altiaLibInitMonoDIB(destDIB, width, height, &dataPtr);


    /* For this target, the bitmap data must be organized
     * from top row to bottom row.  The Altia data
     * array goes from bottom row to top row so we need to
     * unpack it in reverse (i.e., fill the last row of
     * the local data array first and so on).
     */
    minPtr = dataPtr;
    dataPtr += (height - 1) * widthBytes;

    while (nextIndex < totalBits && dataPtr >= minPtr)
    {
        /* The bit data is compressed so decode 8 more bits. */


        *dataPtr = altiaLibDecodePixel(altiaBitPtr, &nextCount, &nextIndex,
                                       &nextIndexCnt);


        /* Increment our byte pointer and count by the number of bytes
         * used by this latest set of 8 bits.
         */
        dataPtr += byteSize;
        widthCnt += byteSize;

        /* If we are finished with a row... */
        if (widthCnt == remCnt)
        {
            /* If the target requires aligning each row of
             * data on a 16-bit or 32-bit boundary, the following
             * code will move the dataPtr the required number
             * of bytes for the alignment.
             */
            while (widthCnt < widthBytes)
            {
                dataPtr++;
                widthCnt++;
            }
            widthCnt = 0;

            /* Remember we are copying the rows from last to first
             * for this target.  So, to get to the first pointer of
             * the previous row when we just finished the current row,
             * jump backwards 2 rows.
             */
            dataPtr -= 2 * widthBytes;
        }
    }

}


/*--- altiaLibInitDIB() ---------------------------------------------------
 *
 * Initialize a target specific device independent bitmap (DIB) object.
 * The DIB pointer must actually reference memory that is large enough
 * for the DIB data structure, color palette if it needs one, and the
 * pixel data for the DIB.  If the dataPtr argument is not NULL, it is
 * set to point to the data section of the DIB so that the caller can
 * fill the data area if desired.
 *
 * A final caution to the caller regarding the colorPtr argument.  If
 * colorPtr is NOT NULL, memory may be reserved in the DIB for a color
 * palette with up to 256 colors and a pointer to this memory is returned
 * via colorPtr.
 *-------------------------------------------------------------------------*/
void altiaLibInitDIB(EGL_DIB *dibPtr, int width, int height, int numBits,
                     EGL_ARGB **colorPtr, ALTIA_UBYTE **dataPtr)
{
    int colorSize;
    int dibSize;
    int colorCount;

        /* Based on number of bits per pixel, determine image
         * format and the largest possible colors in the color
         * table for an indexed image.
         */
        switch (numBits)
        {
        case 1:
            dibPtr->imageFormat = EGL_INDEXED_1;
            dibPtr->colorFormat = EGL_ARGB8888;
            colorCount = 2;
            break;

        case 8:
            dibPtr->imageFormat = EGL_INDEXED_8;
            dibPtr->colorFormat = EGL_ARGB8888;
            colorCount = 256;
            break;

        case 24:
            dibPtr->imageFormat = EGL_DIB_DIRECT;
            dibPtr->colorFormat = EGL_RGB888;
            colorCount = 0;
            break;

        case 32:
            dibPtr->imageFormat = EGL_DIB_DIRECT;
            dibPtr->colorFormat = EGL_ARGB8888;
            colorCount = 0;
            break;


        default:
            dibPtr->imageFormat = EGL_DIB_DIRECT;
            dibPtr->colorFormat = EGL_ARGB8888;

            /* Usually, an image with more than 8 bits per color
             * doesn't need a color palette, but it is possible in
             * certain situations.  In any case, the color palette isn't
             * allowed to be more than 256 entries.
             */
            if (colorPtr != NULL)
                colorCount = 256;
            else
                colorCount = 0;
            break;
        }

        dibPtr->stride = width;

    dibPtr->width = width;
    dibPtr->height = height;

    dibSize = sizeof(EGL_DIB);

    /* Make dibSize a multiple of EGL_ARGB size so that
     * color and data values are on EGL_ARGB boundaries
     */
    if (dibSize % sizeof(EGL_ARGB))
        dibSize += sizeof(EGL_ARGB) - (dibSize % sizeof(EGL_ARGB));

    /* One complete data area was allocated for the DIB
     * structure, color palette (if needed), and data.
     * If we need a color palette, we'll set it up to follow the
     * DIB structure.
     */
    if (colorPtr != NULL)
    {
        *colorPtr = (EGL_ARGB *)((ALTIA_UBYTE *)dibPtr + dibSize);
        dibPtr->clutSize = colorCount;
        if (colorCount > 0)
        {
            dibPtr->pClut = *colorPtr;
            colorSize = altiaPaletteSize(numBits);
        }
        else
        {
            dibPtr->pClut = NULL;
            colorSize = 0;
        }
    }
    else
    {
        dibPtr->clutSize = 0;
        dibPtr->pClut = NULL;
        colorSize = 0;
    }



    /* Set up the DIB data itself to follow the color palette */
    dibPtr->pImage = (ALTIA_UBYTE *) dibPtr + dibSize + colorSize;
    if (dataPtr != NULL)
        *dataPtr = dibPtr->pImage;
}

/*--- altiaLibCreateDIB() -------------------------------------------------
 *
 * Function called from target specific Altia code to create a target
 * specific color device independent bitmap (DIB) data structure.  The
 * function returns a pointer to the DIB data structure or NULL.
 *
 * If memory allocation is NOT allowed, a global array for creating
 * temporary DIB/MDIB data structures is used (except for some special
 * situations given below).  Otherwise, memory is allocated for the DIB
 * and it is up to the caller to free the memory when they are finished
 * with the DIB.
 *
 * If memory allocation is NOT allowed AND bitmap scaling possibly with 
 * text scaling is enabled, some special logic is present.  First, if the
 * forPermRGB888ScalableBits flag is true, the caller still wants memory
 * allocation used because the DIB needs to be permanent and the caller must
 * free it when it is nolonger needed.  Next, if forPermRGB888ScalableBits
 * is false AND forTempScaledBits is true, this function will use the
 * secondary bitmap data area which is large enough to hold temporary
 * scaled data.  Finally, if there really is scaled text in a design AND
 * forScaledText is true AND forPermRGB888ScalableBits is false AND
 * forTempScaledBits is false, this function will force the use of memory
 * allocation because it isn't guaranteed that the global data area is
 * large enough to hold the DIB data structure.  In this case, the
 * caller must free the memory when they are finished with the DIB.
 *
 * If the dataPtr argument is not NULL, it is set to point to the data
 * section of the DIB so that the caller can fill the data area if desired.
 *
 * A final caution to the caller regarding the colors and colorCnt arguments.
 * If colors is NOT -1 and colorCnt is > 0, memory may be reserved in the
 * DIB for a color palette with up to 256 colors.
 *-------------------------------------------------------------------------*/
EGL_DIB *altiaLibCreateDIB(int width, int height,
                           ALTIA_SHORT rasterType,
                           ALTIA_INDEX colors,
                           ALTIA_SHORT colorCnt,
                           ALTIA_BOOLEAN forPermRGB888ScalableBits,
                           ALTIA_BOOLEAN forTempScaledBits,
                           ALTIA_BOOLEAN forScaledText,
                           ALTIA_UBYTE **dataPtr)
{
    EGL_DIB *dibPtr;
    int numBits;
    int total = 0;
    EGL_ARGB *colorPtr = NULL;
    int pixelBytes;

        if (rasterType == ALTIA_RASTER_8_RLE)
        {
            /* If the raster is an index of colors, then each pixel is an
             * 8-bit byte.
             */
            numBits = 8;
            pixelBytes = sizeof(ALTIA_UBYTE);
        }

#if defined(EGL_USE_ARGB_DIBS_ONLY) && (EGL_USE_ARGB_DIBS_ONLY > 0)
        /* Some targets always need 32-bit DIBS for full color bitmaps. */
        else if (forPermRGB888ScalableBits
                 && rasterType == ALTIA_RASTER_24_RLE)
#else
        else if (rasterType == ALTIA_RASTER_24_RLE)
#endif
        {
            /* 24-bit DIB:
             * 8-bits red, 8-bits green, 8-bits blue
             */
            numBits = 24;
            pixelBytes = 3;
        }
        else
        {
            /* 32-bit DIB:
             * 8-bits alpha, 8-bits red, 8-bits green, 8-bits blue
             */
            numBits = 32;
            pixelBytes = sizeof(EGL_ARGB);
        }

        /* If the DIB is not for keeping raster data to be used for smoothing
         * pixels in the scaled version of the raster AND the raster has a
         * color palette, leave room for it.
         */
        if (!forPermRGB888ScalableBits && colors != -1 && colorCnt > 0)
            total += altiaPaletteSize(numBits);

        /* Figure out the worst case total number of bytes that we
         * need.  Throw in the size of an extra 32-bit int in case
         * the size of the EGL_DIB doesn't align with the size of
         * a pixel.
         */
        total += (width * pixelBytes * height)
            + sizeof(EGL_DIB) + sizeof(ALTIA_UINT32);

#if !ALTIA_DIB_MALLOC

    /* We are NOT allowed to malloc and bitmap scaling
     * is allowed so use global arrays for temporary bitmap
     * data if possible, but make sure they are large enough.
     */

    {
        if (forPermRGB888ScalableBits)
        {
            /* Caller wants a permanent DIB for the pixels of a raster
             * to scale it.  Our only option is to allocate memory for
             * this since it needs to be permanent.  If there's not
             * enough memory, return NULL.
             */
            if ((dibPtr = (EGL_DIB *) ALTIA_MALLOC(total)) == NULL)
            {
                _altiaErrorMessage(altiaMsgNoMemColorDIB);
                return NULL;
            }
        }
        else if (forTempScaledBits)
        {
            /* Caller wants a DIB for the temporary pixels of a scaled
             * raster.  If there's not enough room to scale the raster,
             * return NULL.
             */
            if (total > test_altiaMaxBitmap2)
            {
                _altiaErrorMessage(ALT_TEXT("No space to scale raster"));
                return NULL;
            }
            dibPtr = (EGL_DIB *) test_altiaBitmapData2;
        }
        else
        {
            dibPtr = (EGL_DIB *) test_altiaBitmapData;
        }
    }


#endif /* !ALTIA_DIB_MALLOC */

    /* Initialize the color device independent bitmap (DIB)
     * data structure.  It returns a pointer to the data section
     * of the DIB for the caller.
     */
    altiaLibInitDIB(dibPtr, width, height, numBits,
        (!forPermRGB888ScalableBits
        && colors != -1 && colorCnt > 0) ? &colorPtr : NULL,
        dataPtr);

    /* If the caller passed a valid index to an Altia color table (presumably
     * because the raster data is in indexed color format), add the colors
     * to the color table of the DIB data structure.
     */
    if (!forPermRGB888ScalableBits && colors != -1 && colorCnt > 0)
    {
        Altia_ColorEntry_type *altiaColors = _altiaRasterColors(colors);
        if (dibPtr->clutSize < colorCnt)
            colorCnt = dibPtr->clutSize;
        else
            dibPtr->clutSize = colorCnt;

        while (colorCnt--)
        {
            *colorPtr++ = EGL_MAKE_ARGB(0x0ff, altiaColors->red,
                altiaColors->green,
                altiaColors->blue);
            altiaColors++;
        }
    }

    return dibPtr;
}


/* The following routines are generated because the Altia design
 * has rasters.  These routines are not generated if a design
 * has stencils or scaled text, but no rasters.
 */

/*--- altiaLibLoadDIB() ---------------------------------------------------
 *
 * Function called from target specific Altia code to create, initialize
 * and load data into a target specific device independent bitmap (DIB)
 * data structure from Altia raster data.  It returns a pointer to the
 * target specific DIB or NULL.   IF memory allocation is enabled, it is
 * a pointer to allocated memory and the caller must free the memory when
 * they are finished with the DIB.  NULL is returned if the function
 * cannot create, initialize, or load the DIB.  The caller can request
 * a permanent (i.e., malloced) DIB of 24-bit colors for use in smoothing
 * a scaled version of a raster by passing a non-zero value in
 * the argument forPermRGB888ScalableBits.
 *-------------------------------------------------------------------------*/
EGL_DIB *altiaLibLoadDIB(Altia_Raster_type *raster,
                         ALTIA_BOOLEAN forPermRGB888ScalableBits)
{
    EGL_DIB *dibPtr;
    ALTIA_UBYTE *dataPtr;

    int pixelBytes;
    int widthBytes;
    int widthCnt;
    int remCnt;
    ALTIA_UBYTE *minPtr;
    long total;
    ALTIA_INT acount = 0;
    ALTIA_INT rcount = 0;
    ALTIA_INT gcount = 0;
    ALTIA_INT bcount = 0;
    ALTIA_INT aindex = 0;
    ALTIA_INT rindex = 0;
    ALTIA_INT gindex = 0;
    ALTIA_INT tindex = 0;

    ALTIA_INT aindexCnt = 0;
    ALTIA_INT rindexCnt = 0;
    ALTIA_INT gindexCnt = 0;
    ALTIA_INT bindexCnt = 0;
    ALTIA_UBYTE *rpixels;
    ALTIA_UBYTE *gpixels = NULL;
    ALTIA_UBYTE *bpixels = NULL;
    ALTIA_UBYTE *apixels = NULL;

    ALTIA_SHORT rasterTypeForDIB;
    ALTIA_SHORT width = raster->width;
    ALTIA_SHORT height = raster->height;

    /* If caller wants us to load this DIB to be permanent (i.e., malloced)
     * data in EGL_RGB888 format for use in smoothing a scaled version
     * of a raster, our raster type for creating the DIB should always be
     * ALTIA_RASTER_24_RLE or ALTIA_RASTER_32_RLE because we are going to
     * populate the DIB with 8-bit (alpha), red, green, and blue values.
     * Otherwise, it should be the actual raster type.
     */

    if(forPermRGB888ScalableBits)
    {
        if(raster->apixels != -1)
            rasterTypeForDIB = ALTIA_RASTER_32_RLE;
        else
            rasterTypeForDIB = ALTIA_RASTER_24_RLE;
    }
    else

        rasterTypeForDIB = raster->rasterType;

    /* Immediately create a device-independent bitmap (DIB) structure to
     * hold the bitmap data.  Presumably, the caller is going to convert
     * it into a device-dependent bitmap (DDB).  If we cannot create a
     * DIB, we are done.
     */
    if ((dibPtr = altiaLibCreateDIB(width, height, rasterTypeForDIB,
                     raster->colors, raster->colorCnt,
                     forPermRGB888ScalableBits /* Bits for smooth scaling? */,
                     false /* Not for temporary scaled bits */,
                     false /* Not for scaled text bits */,
                     &dataPtr)) == NULL)
        return NULL;



    /* For this target, determine the byte size of
     * the data type that will be used to hold a
     * pixel.  On some targets, each pixel must be
     * 16-bit or 32-bit aligned so the size of the data
     * type might be less than optimal.
     */
    if (rasterTypeForDIB == ALTIA_RASTER_8_RLE)
    {
        /* This is indexed data.  Each pixel coming from
         * the Altia data is an 8-bit index into a color
         * map.  On this target, the index is held as an
         * 8-bit data item.
         */
        pixelBytes = sizeof(ALTIA_UBYTE);
    }

#if defined(EGL_USE_ARGB_DIBS_ONLY) && (EGL_USE_ARGB_DIBS_ONLY > 0)
    /* Some targets always need 32-bit DIBS for full color bitmaps. */
    else if (forPermRGB888ScalableBits
             && rasterTypeForDIB == ALTIA_RASTER_24_RLE)
#else
    else if (rasterTypeForDIB == ALTIA_RASTER_24_RLE)
#endif
    {
        pixelBytes = 3;
    }
    else
    {
        pixelBytes = sizeof(EGL_ARGB);
    }

    /* Set a variable that gives us the size of each row
     * of the bitmap in bytes.  If the rows of pixels must
     * be aligned in some special way for the target, the
     * variable can be adjusted to account for the alignment.
     * Say each row must align on a 32-bit boundary.
     * The BITSTOINT32(bits) macro defined at the beginning
     * of this file can be used to do:
     *     widthBytes = BITSTOINT32(width * pixelBytes * 8);
     *
     * For this target, nothing special is necessary and we
     * can just set widthBytes = width * pixelBytes.
     */
    widthBytes = width * pixelBytes;

    /* Get the start of the red, green, blue, and alpha pixel information.
     * If this is an indexed bitmap, then it is possible that only
     * the red data is valid.  In this case, it isn't really just
     * red data but rather indexes into a color table of red, green,
     * and blue data.
     */
#ifndef ALTIA_DRV_LOADASSET_CHUNKS
    rpixels = (ALTIA_UBYTE *)altiaImageAddress(raster->rpixels);
    if (raster->gpixels != -1)
        gpixels = (ALTIA_UBYTE *)altiaImageAddress(raster->gpixels);
    if (raster->bpixels != -1)
        bpixels = (ALTIA_UBYTE *)altiaImageAddress(raster->bpixels);
    if (raster->apixels != -1)
        apixels = (ALTIA_UBYTE *)altiaImageAddress(raster->apixels);
#else
    rpixels = (ALTIA_UBYTE *)altiaImageAddressChunk(raster->rpixels, raster->rCnt);
    if (raster->gpixels != -1)
        gpixels = (ALTIA_UBYTE *)altiaImageAddressChunk(raster->gpixels, raster->gCnt);
    if (raster->bpixels != -1)
        bpixels = (ALTIA_UBYTE *)altiaImageAddressChunk(raster->bpixels, raster->bCnt);
    if (raster->apixels != -1)
        apixels = (ALTIA_UBYTE *)altiaImageAddressChunk(raster->apixels, raster->aCnt);

    /* Get the address of 0 with size 0.  For drivers that are loading
     * assets in chunks this will "commit" the request.
     */
    (void)altiaImageAddressChunk((ALTIA_BINDEX)0, (ALTIA_UINT32)0);
#endif /* ALTIA_DRV_LOADASSET_CHUNKS */

    /* For this target, the bitmap data must be organized
     * from top row to bottom row.  The Altia data
     * array goes from bottom row to top row so we need to
     * unpack it in reverse (i.e., fill the last row of
     * the local data array first and so on).
     */
    minPtr = dataPtr;
    dataPtr += (height - 1) * widthBytes;
#if !ALTIA_DIB_MALLOC
    /* We are NOT allowed to malloc and if we are NOT creating a
     * permanent DIB with data for smoothing the scaled bitmap,
     * we really are using a global array to hold the temporary
     * DIB.  We want to check where the end of it is so we don't
     * go beyond it.
     */
    if (!forPermRGB888ScalableBits
        && (dataPtr + widthBytes - 1) > &test_altiaBitmapData[test_altiaMaxBitmap - 1])
        return NULL;
#endif /* !ALTIA_DIB_MALLOC */
    if (rasterTypeForDIB == ALTIA_RASTER_8_RLE)
    {
        /* We have 8-bit indexed data from Altia and for the DIB.
         * Unpack it now...
         */

        widthCnt = 0;
        remCnt = width * pixelBytes;
        total = raster->rCnt;
        while (rindex < total && dataPtr >= minPtr)
        {
            /* The pixel data is compressed so decode the next element */


            *dataPtr = altiaLibDecodePixel(rpixels, &rcount,
                                           &rindex, &rindexCnt);

            /* Increment our byte pointer and count by the number of bytes
             * used by this latest index value.
             */
            dataPtr += pixelBytes;
            widthCnt += pixelBytes;

            /* If we are finished with a row... */
            if (widthCnt == remCnt)
            {
                /* If the target requires aligning each row of
                 * data on a 16-bit or 32-bit boundary, the following
                 * code will move the dataPtr the required number
                 * of bytes for the alignment.
                 */
                while (widthCnt < widthBytes)
                {
                    dataPtr++;
                    widthCnt++;
                }
                widthCnt = 0;

                /* Remember we are copying the rows from last to first
                 * for this target.  So, to get to the first pointer of
                 * the previous row when we just finished the current row,
                 * jump backwards 2 rows.
                 */
                dataPtr -= 2 * widthBytes;
            }
        }
    }
    else
    {
        /* We have 8-bit indexed 24-bit red,green,blue, or 32-bit
         * alpha,red,green,blue data from Altia, but the DIB is definitely
         * (A)RGB color data in one of 3 formats. Unpack the Altia data into
         * the DIB now...
         */

        ALTIA_UBYTE red, green, blue, alpha;
        Altia_ColorEntry_type *altiaColors = NULL;
        ALTIA_SHORT altiaColorCnt = 0;

        widthCnt = 0;
        remCnt = width * pixelBytes;

        if (raster->rasterType == ALTIA_RASTER_8_RLE)
        {
            /* Altia raster data is 8-bit indexes into a color map.
             * Get a pointer to the color map if it really exists.
             */
            if (raster->colors != -1 && raster->colorCnt != 0)
            {
                altiaColors = _altiaRasterColors(raster->colors);
                altiaColorCnt = raster->colorCnt;
            }

            total = raster->rCnt;
        }
        else
            total = raster->bCnt;

        while (tindex < total && dataPtr >= minPtr)
        {
          if (raster->rasterType == ALTIA_RASTER_8_RLE)
          {
            /* We can only possibly get to here because
             * forPermRGB888ScalableBits is true and the Altia data is
             * 8-bit indexed.  Unpack the next pixel as an index and get
             * its RGB for the DIB using the Altia color map for the raster.
             */

            ALTIA_UBYTE altiaColorIndex;

            /* The pixel data is compressed so decode the next element */


            altiaColorIndex = altiaLibDecodePixel(rpixels, &rcount,
                                                  &tindex, &rindexCnt);

            /* Get the 3 bytes of red, green and blue from the Altia color
             * map if the color map is valid.  Otherwise, just use black for
             * the color.
             */
            if (altiaColorIndex < altiaColorCnt && altiaColors != NULL)
            {
                dataPtr[0] = altiaColors[altiaColorIndex].red;
                dataPtr[1] = altiaColors[altiaColorIndex].green;
                dataPtr[2] = altiaColors[altiaColorIndex].blue;
            }
            else
                dataPtr[0] = dataPtr[1] = dataPtr[2] = 0;
          }
          else
          {
            /* The Altia data is 24 bits (8 bits each for red, green,
             * and blue) or 32 bits (8 bits each for alpha, red, green, and
             * blue).  Unpack the alpha (if available), red, green, and blue
             * components for the next pixel into the DIB data area.
             */

            /* The pixel data is compressed so decode the next blue value */


            blue = altiaLibDecodePixel(bpixels, &bcount, &tindex,
                                       &bindexCnt);

            /* The pixel data is compressed so decode the next green value */
            if (bpixels != gpixels)


                green = altiaLibDecodePixel(gpixels, &gcount, &gindex,
                                            &gindexCnt);

            else
                green = blue;

            /* The pixel data is compressed so decode the next red value */
            if (bpixels != rpixels)


                red = altiaLibDecodePixel(rpixels, &rcount, &rindex,
                                          &rindexCnt);

            else
                red = blue;

            if(raster->apixels != -1)
            {
            /* The pixel data is compressed so decode the next alpha value */
            if (bpixels != apixels)


                alpha = altiaLibDecodePixel(apixels, &acount, &aindex,
                                            &aindexCnt);

            else
                alpha = blue;
            }
            else
                alpha = 0xff;

#if defined(EGL_USE_ARGB_DIBS_ONLY) && (EGL_USE_ARGB_DIBS_ONLY > 0)
            /* Some targets always need 32-bit DIBS for full color bitmaps. */
            if (forPermRGB888ScalableBits
                && raster->rasterType == ALTIA_RASTER_24_RLE)
#else
            if (raster->rasterType == ALTIA_RASTER_24_RLE)
#endif
            {
                /* For this case, the DIB data area is just an array of
                 * red, green, and blue bytes.
                 */
                dataPtr[0] = red;
                dataPtr[1] = green;
                dataPtr[2] = blue;
            }
            else
            {
                /* This target has a macro to convert the alpha, red, green, and
                 * blue into a target specific ARGB value.
                 */
                *((EGL_ARGB *) dataPtr)
                    = EGL_MAKE_ARGB(alpha, red, green, blue);
            }
          }

          /* Increment our byte pointer and count by the number of bytes
           * used by this latest (A)RGB value.
           */
          dataPtr += pixelBytes;
          widthCnt += pixelBytes;

          /* If we are finished with a row... */
          if (widthCnt == remCnt)
          {
            /* If the target requires aligning each row of
             * data on a 16-bit or 32-bit boundary, the following
             * code will move the dataPtr the required number
             * of bytes for the alignment.
             */
            while (widthCnt < widthBytes)
            {
                dataPtr++;
                widthCnt++;
            }
            widthCnt = 0;

            /* Remember we are copying the rows from last to first
             * for this target.  So, to get to the first pointer of
             * the previous row when we just finished the current row,
             * jump backwards 2 rows.
             */
            dataPtr -= 2 * widthBytes;
          }
        }
    }

    /* We did it!  Return pointer to DIB data structure. */
    return dibPtr;
}


/*--- altiaLibDIBToBitmap() -----------------------------------------------
 *
 * Create a target specific device dependent bitmap (DDB) from a target
 * specific device independent bitmap (DIB).  The DIB pointer that is
 * passed as an argument must be completely initialized.  The DDB
 * identifier or 0 is returned by this function.  If the retBits argument
 * is not NULL, it is set to point to the data section of the DDB if the
 * data section is public on this target.  If it is not public, it is
 * just set to the DDB identifier (which is also the return value).
 * The creation of the DDB by the target graphics library is likely to
 * require memory allocation on most target.  It is the caller's
 * responsibility to delete the MDDB when it is no longer needed.
 *-------------------------------------------------------------------------*/
ALTIA_UINT32 altiaLibDIBToBitmap(EGL_DEVICE_ID devId, EGL_DIB *dibPtr,
                                 ALTIA_UINT32 *retBits, 
                                 EGL_REFERENCE_ID ref)
{
    EGL_DDB_ID ddb;  /* Hold id of device-dependent bitmap (DDB) */
 
    if (dibPtr == NULL)
        return 0;
 
    if ((ddb = egl_BitmapCreate(devId, dibPtr, EGL_DIB_INIT_DATA, 0, ref))
        == NULL)
    {
        _altiaErrorMessage(altiaMsgNoMemColorDDB);
        return 0;
    }

    /* Caller may need pointer to actual DDB data for bitmap scaling.
     * For this target graphics library, we don't have a direct way to
     * get this data from DDB structure so just pass DDB id.
     */
    if (retBits != NULL)
        *retBits = (ALTIA_UINT32) ddb;
 
    /* Return DDB id */
    return (ALTIA_UINT32) ddb;
}


