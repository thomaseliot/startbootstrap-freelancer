/* $Revision: 1.42 $    $Date: 2010-03-22 04:26:04 $
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

/** FILE:  altiaLibRaster.c *************************************************
 **
 ** This file contains the routines to create color bitmaps and draw
 ** them.  Color bitmaps are required for rendering Altia raster
 ** objects.  If a design has no such objects, this file contains no code.
 ** Also note that this file will contain additional code if one or
 ** more raster objects has a scale, stretch, or rotation and the
 ** scaled bitmaps option was enabled when code was generated.
 **
 ** Monochrome and color bitmap drawing are usually the most difficult
 ** operations to port from one target to another.  The comment header
 ** for each routine in this file identifies the amount of target specific
 ** code in the routine to help determine the degree of modification that
 ** may be required to support a different target.
 ***************************************************************************/



#include "egl_Wrapper.h"

#include <string.h>
#include <stdlib.h>
#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "../altiaData.h"
#include "altiaLibDraw.h"
#include "../altiaImageAccess.h"

/* Disable raster setting if not specified explicitly */
#ifndef EGL_HAS_RASTER_SET
#define EGL_HAS_RASTER_SET 0
#endif

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

/* The next macro defines the largest raster area that we will
 * scale/stretch/rotate and save the resulting transformed data for
 * future drawing.  For larger areas, the data is freed after it is
 * used to draw.  This is done to conserve memory.  Increasing
 * the value in the macro will improve performance at the cost of
 * memory.
 */
#define RASTERPIXSIZE 200000

#ifdef ALTIA_DRV_LOADRASTER

/* Driver APIs for target-specific raster loading */
extern ALTIA_BOOLEAN driverLoadRaster(
#ifdef Altiafp
Altia_Raster_type * raster
#endif
);
extern void driverDeleteRaster(
#ifdef Altiafp
Altia_Raster_type * raster
#endif
);

#error ALTIA_DRV_LOADRASTER not compatible with Scaled Bitmaps
#error Please regenerate code without Scaled Bitmaps.

#endif /* ALTIA_DRV_LOADRASTER */



extern AltiaDevInfo *altiaLibGetDevInfo(void);

extern ALTIA_CHAR *altiaMsgNoMemColorDDB;

extern EGL_COLOR altiaLibGetColor(
#ifdef Altiafp
ALTIA_WINDOW devinfo,
ALTIA_COLOR pixel
#endif
);

extern EGL_DIB *altiaLibLoadDIB(
#ifdef Altiafp
Altia_Raster_type *raster,
ALTIA_BOOLEAN forPermRGB888ScalableBits
#endif
);

extern ALTIA_UINT32 altiaLibDIBToBitmap(
#ifdef Altiafp
EGL_DEVICE_ID devId,
EGL_DIB       *ptr,
ALTIA_UINT32  *bits,
EGL_REFERENCE_ID ref
#endif
);

extern void altiaLibLoadMonoDIB(
#ifdef Altiafp
ALTIA_CONST ALTIA_UBYTE  *altiaBitPtr,
ALTIA_UINT32 altiaTotalBits,
ALTIA_SHORT  width,
ALTIA_SHORT  height,
EGL_MDIB     *destDIB
#endif
);

extern ALTIA_UINT32 altiaLibDIBToMonoBitmap(
#ifdef Altiafp
EGL_DEVICE_ID devId,
EGL_MDIB      *ptr,
ALTIA_UINT32  *bits
#endif
);



#if EGL_MIXED_RASTER_TRANSFORMATION
/* Query function to see if EGL code can handle
** the specified transformed raster draw.
*/
extern ALTIA_BOOLEAN egl_RasterTransformDraw(
#ifdef Altiafp
EGL_GC_ID gc,
void * trans,
EGL_DDB_ID srcBitmapId
#endif
);
#endif /* EGL_MIXED_RASTER_TRANSFORMATION */

extern ALTIA_CHAR *altiaMsgNoMemColorDIB;
extern ALTIA_CHAR *altiaMsgNoMemMonoDIB;

extern void altiaLibInitDIB(
#ifdef Altiafp
EGL_DIB    *ptr,
int         width,
int         height,
int         numBits,
EGL_ARGB    **colorPtr,
ALTIA_UBYTE **dataPtr
#endif
);

extern EGL_DIB *altiaLibCreateDIB(
#ifdef Altiafp
int           width,
int           height,
ALTIA_SHORT   rasterType,
ALTIA_INDEX   colors,
ALTIA_SHORT   colorCnt,
ALTIA_BOOLEAN forPermRGB888ScalableBits,
ALTIA_BOOLEAN forTempScaledBits,
ALTIA_BOOLEAN forScaledText,
ALTIA_UBYTE   **dataPtr
#endif
);

extern void altiaTransformBitmap(
#ifdef Altiafp
void                 *origDIB,
int                  origWidth,
int                  origHeight,
void                 *destDIB,
int                  dx,
int                  dy,
int                  destWidth,
int                  destHeight,
Altia_Transform_type *trans,
int                  mask
#endif
);

extern EGL_MDIB *altiaLibCreateMonoDIB(
#ifdef Altiafp
int           width,
int           height,
ALTIA_BOOLEAN forTempScaledBits,
ALTIA_BOOLEAN forScaledText,
ALTIA_UBYTE   **dataPtr
#endif
);

void altiaLibRasterClear(
#ifdef Altiafp
Altia_Raster_type *raster
#endif
);

static void localDeleteBitmaps(
#ifdef Altiafp
ALTIA_WINDOW win,
ALTIA_UINT32 dc,
ALTIA_UINT32 bitmap,
ALTIA_UINT32 mask,
EGL_INT hasAlpha
#endif
);

int scaledRasterCnt = 0;
int scaledSaveBitsCnt = 0;

/*--- localFindScaledRaster() ---------------------------------------------
 *
 * Local utility function that tries to match the attributes of a newly
 * scaled bitmap with the attributes of one that already exists.  If it
 * is possible to match the attributes, reuse the existing scaled bitmap
 * to conserve graphics library memory and improve performance.
 *-------------------------------------------------------------------------*/
static int localFindScaledRaster(Altia_Raster_type *raster,
                                 Altia_Transform_type *total)
{
    int i;
    Altia_Scaled_Raster_type *scaledPtr;

    if (raster->id == 0)
	return -1;

    scaledPtr = test_scaledRaster;
    for (i = 0; i < scaledRasterCnt; i++, scaledPtr++)
    {
	/* If the raster matches and the transform matches,
	 * we have a complete match and we can just increment
	 * the usage count.
	 */
	if (raster->id == scaledPtr->origId
	    && raster->maskId == scaledPtr->origMaskId


	    && total->a00 == scaledPtr->a00
	    && total->a01 == scaledPtr->a01
	    && total->a10 == scaledPtr->a10
	    && total->a11 == scaledPtr->a11)

	{

	    scaledPtr->cnt += 1;

/*
printf("Found EXISTING raster %d, count now %d\n", i, test_scaledRaster[i].cnt);
*/

	    return i;
	}
    }

/*
printf("\nFound NO existing raster match of %d entries\n", scaledRasterCnt);
*/

    return -1;
}


/*--- localDeleteScaledRasterData() ---------------------------------------
 *
 * Local utility function that deletes the scaled raster data from an
 * existing entry in the array of scaled rasters.
 *-------------------------------------------------------------------------*/
static void localDeleteScaledRasterData(Altia_Scaled_Raster_type *scaledPtr)
{
    AltiaDevInfo *devInfo = altiaLibGetDevInfo();

    /* Use a local utility to delete the actual scaled bitmap and
     * mask DDBs if they exist.
     */
    localDeleteBitmaps((ALTIA_WINDOW) devInfo,
                       (ALTIA_UINT32) devInfo->gc,
                       scaledPtr->id, scaledPtr->maskId, scaledPtr->hasAlpha);
    scaledPtr->id = 0;
    scaledPtr->maskId = 0;
    scaledPtr->cnt = 0;
}


/*--- localAddScaledRaster() ----------------------------------------------
 *
 * Local utility function that adds a new scaled bitmap to the array of
 * existing scaled bitmaps (presumably because it does not match any of
 * the existing entries).
 *-------------------------------------------------------------------------*/
static void localAddScaledRaster(Altia_Raster_type *raster,
                                 Altia_Transform_type *total)
{
    Altia_Scaled_Raster_type *scaledPtr = NULL;

#if EGL_NEEDS_SCALEDRASTERREUSE
    /* If scaled raster data is being reused for this target,
     * there may be an entry for the same original raster and mask
     * that is actually available because its use count is 0.
     * Try to find such an entry.  We must delete the data in
     * such an entry because it doesn't match what we need (if an
     * exact matching entry existed, it would've been discovered
     * by an earlier call to localFindScaledRaster()).  This
     * goes against the need to reuse scaled raster data, but
     * what else can we do without keeping around potentially
     * huge amounts of scaled raster data.
     */
    if (scaledRasterCnt > 0)
    {
	int i;
	Altia_Scaled_Raster_type *tempScaledPtr = test_scaledRaster;

	for (i = 0; i < scaledRasterCnt; i++, tempScaledPtr++)
	{
	    if (tempScaledPtr->cnt <= 0
	        && raster->id == tempScaledPtr->origId
	        && raster->maskId == tempScaledPtr->origMaskId)
	    {

/*
printf("Using SIMILAR raster entry %d of %d\n", i, scaledRasterCnt);
*/

		localDeleteScaledRasterData(tempScaledPtr);
		scaledPtr = tempScaledPtr;
		break;
	    }
	}
    }
#endif /* EGL_NEEDS_SCALEDRASTERREUSE */

    /* Entries are available.  Take the next one. */
    if (scaledPtr == NULL && scaledRasterCnt < test_scaled_rasters_count)
    {
	scaledPtr = test_scaledRaster + scaledRasterCnt;

/*
printf("Adding NEW raster entry %d\n", scaledRasterCnt);
*/

	scaledRasterCnt++;
    }

    /* If we are pointing to a valid entry, initialize it. */
    if (scaledPtr != NULL)
    {
	scaledPtr->origId = raster->id;
	scaledPtr->origMaskId = raster->maskId;
       	scaledPtr->id = raster->scaledId;
       	scaledPtr->maskId = raster->scaledMaskId;
	if (raster->apixels != -1)
	    scaledPtr->hasAlpha = 1;
	else
	    scaledPtr->hasAlpha = 0;
       	scaledPtr->cnt = 1;
       	scaledPtr->a00 = total->a00;
       	scaledPtr->a01 = total->a01;
       	scaledPtr->a10 = total->a10;
       	scaledPtr->a11 = total->a11;

    }
    else
    {
	/* Don't error out if out of room
	_altiaErrorMessage(
	    ALT_TEXT("No room to add another scaled raster entry!"));
	 */
    }
}


/*--- localRemoveScaledRaster() --------------------------------------------
 *
 * Local utility function that removes a scaled bitmap from the array
 * of scaled bitmaps.  The use count for the entry associated with the
 * scaled bitmap is decremented.  If the use count goes to 0 and the
 * caller wants the data permanently removed, the bitmap data is
 * completely deleted.
 *-------------------------------------------------------------------------*/
void localRemoveScaledRaster(ALTIA_UINT32 scaledId,
                                    ALTIA_UINT32 scaledMaskId,
                                    ALTIA_BOOLEAN permanently)
{
    int i;
    Altia_Scaled_Raster_type *scaledPtr;

    if (scaledId == 0)
	return;

#if !defined(EGL_NEEDS_SCALEDRASTERREUSE) || (EGL_NEEDS_SCALEDRASTERREUSE < 1)
    /* If the target graphics library does not need to reuse
     * scaled raster data, we will always permanently delete the
     * entry to conserve resources when its use count goes to 0.
     *
     * If a target needs to reuse scaled raster data,
     * EGL_NEEDS_SCALEDRASTERREUSE should be defined as 1 and the
     * course of action to take is determined by the caller's setting
     * of the "permanently" flag.  Reusing scaled raster data does not
     * conserve graphics resources, but it reduces bitmap deletion and
     * creation which improves performance and may be necessary on some
     * targets (e.g., pre X11R6 servers) that have a limited pool of
     * bitmap IDs.
     */
    permanently = true;
#endif /* NOT EGL_NEEDS_SCALEDRASTERREUSE */

    scaledPtr = test_scaledRaster;
    for (i = 0; i < scaledRasterCnt; i++, scaledPtr++)
    {
	/* If we can match scaled bitmap and mask bitmap IDs,
	 * we've found the entry to remove.
	 */
	if (scaledId == scaledPtr->id
	    && scaledMaskId == scaledPtr->maskId)
	{
	    /* Decrement the usage count.  If it goes to 0 and
	     * we are being asked to remove the data permanently,
	     * delete the data and entry completely.
	     */
	    scaledPtr->cnt -= 1;

	    if (scaledPtr->cnt <= 0 && permanently)
	    {
		int cnt;

/*
printf("Removing (PERMANENTLY) raster entry %d\n", i);
*/

		localDeleteScaledRasterData(scaledPtr);

		cnt = scaledRasterCnt - i -1;
		if (cnt > 0)
		{
		    memmove(&test_scaledRaster[i], &test_scaledRaster[i+1],
		            sizeof(Altia_Scaled_Raster_type) *cnt);
		}
		scaledRasterCnt--;
	    }

/*
else
printf("Made AVAILABLE raster entry %d, use count %d, permanently = %d\n",
       i, test_scaledRaster[i].cnt, permanently);
*/

	    break;
	}
    }
}


/*--- localFindScaledSaveBits() -------------------------------------------
 *
 * Local utility function that tries to match the attributes of a bitmap
 * with the attributes of one that already exists.  If it is possible
 * to match the attributes, reuse the existing bitmap's saved DIB data
 * to conserve graphics library memory and improve performance.
 *-------------------------------------------------------------------------*/
static int localFindScaledSaveBits(Altia_Raster_type *raster)
{
    int i;
    Altia_Scaled_Save_Bits_type *saveBitsPtr;

    if (raster->id == 0)
	return -1;

    saveBitsPtr = test_scaledSaveBits;
    for (i = 0; i < scaledSaveBitsCnt; i++, saveBitsPtr++)
    {
	/* If the raster bitmap and mask bitmap match, we have
	 * a complete match.
	 */
	if (raster->id == saveBitsPtr->origId &&
	    raster->maskId == saveBitsPtr->origMaskId)
	{
	    /* We have a match.  If the incoming raster isn't already
	     * referencing the saved DIB data, increment the usage count.
	     */
	    if (raster->saveBits != saveBitsPtr->saveBits
	        || raster->saveMaskBits != saveBitsPtr->saveMaskBits)
	    {
		saveBitsPtr->cnt += 1;
	    }
	    return i;
	}
    }
    return -1;
}


/*--- localAddScaledSaveBits() --------------------------------------------
 *
 * Local utility function that adds a new bitmap's saved DIB data to the
 * array of existing saved DIB data (presumably because it does not match
 * any of the existing entries).
 *-------------------------------------------------------------------------*/
static void localAddScaledSaveBits(Altia_Raster_type *raster)
{
    if (scaledSaveBitsCnt < test_scaled_rasters_count)
    {
	Altia_Scaled_Save_Bits_type *saveBitsPtr
	    = test_scaledSaveBits + scaledSaveBitsCnt;

	saveBitsPtr->origId = raster->id;
	saveBitsPtr->origMaskId = raster->maskId;
	saveBitsPtr->saveBits = raster->saveBits;
	saveBitsPtr->saveMaskBits = raster->saveMaskBits;
       	saveBitsPtr->cnt = 1;
	scaledSaveBitsCnt++;
    }
}


/*--- localRemoveScaledSaveBits() -----------------------------------------
 *
 * Local utility function that removes the usage of an entry in the array
 * of saved DIB data.  The use count for the entry associated with the
 * saved DIB data is decremented.  If the use count goes to 0, the saved
 * DIB data is completely deleted.
 *-------------------------------------------------------------------------*/
static void localRemoveScaledSaveBits(ALTIA_UINT32 saveBits,
                                      ALTIA_UINT32 saveMaskBits)
{
    int i;
    Altia_Scaled_Save_Bits_type *saveBitsPtr;

    if (saveBits == 0)
	return;

    saveBitsPtr = test_scaledSaveBits;
    for (i = 0; i < scaledSaveBitsCnt; i++, saveBitsPtr++)
    {
	/* If there is a match, we found the one to remove. */
	if (saveBits == saveBitsPtr->saveBits
	    && saveMaskBits == saveBitsPtr->saveMaskBits)
	{
	    int cnt;

	    /* Decrement usage count.  If it goes to 0, free
	     * the memory used by the saved DIBs if they exist.
	     */
	    saveBitsPtr->cnt -= 1;
	    if (saveBitsPtr->cnt <= 0)
	    {
		if (saveBitsPtr->saveBits != 0)
		    ALTIA_FREE((EGL_DIB *) saveBitsPtr->saveBits);

		if (saveBitsPtr->saveMaskBits != 0)
		    ALTIA_FREE((EGL_MDIB *) saveBitsPtr->saveMaskBits);

		/* Shift entries after the one just cleared to
		 * occupy it so there are no holes.
		 */
		cnt = scaledSaveBitsCnt - i -1;
		if (cnt > 0)
		{
		    memmove(&test_scaledSaveBits[i],
		            &test_scaledSaveBits[i+1],
		            sizeof(Altia_Scaled_Save_Bits_type) *cnt);
		}
		scaledSaveBitsCnt--;
	    }
	    break;
	}
    }
}




#if !EGL_USE_ALTIABITSFORSCALING



/*--- localReadSaveBits() --------------------------------------------
 * For this target graphics library, we must read from the
 * current DDB to get DIB data that we can use to compute
 * pixels for scaling/stretching/rotating the raster.  When
 * we are through, we want a DIB that is in EGL_RGB888 format
 * which is the easiest for us to use because it is simply
 * triplets of 1-byte red, green, and blue values.
 */
static ALTIA_BOOLEAN localReadSaveBits(ALTIA_WINDOW win,
                                    Altia_Raster_type *raster,
				    EGL_DIB **dib888Ptr)
{
    ALTIA_UBYTE  *image888Ptr;
    ALTIA_COORD width = raster->width;
    ALTIA_COORD height = raster->height;
    EGL_INT hasAlpha = 0;
    ALTIA_SHORT rasType = ALTIA_RASTER_24_RLE;

    if (raster->apixels != -1)
    {
        hasAlpha = 1;
        rasType = ALTIA_RASTER_32_RLE;
    }
    /* If we don't have any DDB data, we are done. */
    if (raster->saveBits == 0)
    {
	_altiaErrorMessage(
	    ALT_TEXT("Missing original DDB for scaling raster!"));
	return false;
    }



    /* First allocate a permanent EGL_RGB888 DIB structure.
     * We don't have a choice.  We must allocate the DIB and its
     * data area because we want to keep them around for future
     * drawing of the scaled/stretched/rotated raster.  The
     * altiaLibRasterDelete() function must free the memory.
     * Note that altiaLibCreateDIB() allocates a continuous block
     * of memory for the DIB structure and the data area that it
     * points to so we only need to do a single free when the time
     * comes to destroy it.
     */
    if (((*dib888Ptr) = altiaLibCreateDIB(width, height,
	                   rasType  /* Only type for perm scalable bits */,
	                   -1    /* Don't make a color table */,
	                   0     /* No color count */,
	                   true  /* For permanent EGL_RGB888 scalable bits */,
	                   false /* Don't use static area for scaled bits */,
	                   false /* Not for scaled text bits */,
	                   &image888Ptr  /* Return ptr to data */ )) == NULL)
        return false;

    /* Copy the whole DDB from (0,0) to (width-1,height-1) into
     * the DIB starting at its (0,0).
     */
    if (egl_BitmapRead(((AltiaDevInfo *) win)->devId,
                       (EGL_DDB_ID) raster->saveBits,
                       0, 0, (EGL_POS) (width - 1), (EGL_POS) (height - 1),
                       *dib888Ptr, 0, 0, hasAlpha) != EGL_STATUS_OK)
    {
        ALTIA_FREE(*dib888Ptr);
        return false;
    }


    return true;
}


#endif



/*--- localGetScaledSaveBits() --------------------------------------------
 *
 * Local utility function that gets bitmap and mask DIBs from DDBs
 * so that we can smooth scaled/stretched/rotated bitmap data.  On this
 * target, the format of the DDB data that we have is not public so
 * we must build a DIB from the DDB data.  On another target, it may
 * be possible to use the DDB data directly if the format is known
 * and fixed.  These extra DIBs were not built at initialization time
 * because they require memory allocation and we don't want to do that
 * unless a raster is actually scaled/stretched/rotated.
 *-------------------------------------------------------------------------*/
ALTIA_BOOLEAN localGetScaledSaveBits(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                                     Altia_Raster_type *raster)
{
    int saveIndex;

    /* If we already have saved bits that can be used for this raster,
     * use them.
     */
    if ((saveIndex = localFindScaledSaveBits(raster)) >= 0)
    {
        raster->saveBits = test_scaledSaveBits[saveIndex].saveBits;
        raster->saveMaskBits = test_scaledSaveBits[saveIndex].saveMaskBits;
        return true;
    }
    else
    {
        EGL_DIB *dib888Ptr;

        ALTIA_COORD width = raster->width;
        ALTIA_COORD height = raster->height;


#if EGL_USE_ALTIABITSFORSCALING
		/* For this target graphics library, we don't need to read
		 * from the current DDB to get DIB data for scaling the raster.
		 * Instead, we can just load a DIB from the original raster
		 * data that is in EGL_RGB888 format which is the easiest for
		 * us to use because it is simply triplets of 1-byte red, green,
		 * and blue values).  Note that altiaLibLoadDIB() allocates a
		 * continuous block of memory for the DIB structure and the data
		 * area that it points to so we only need to do a single free
		 * when the time comes to destroy it.
		 */

        {
            /* Use permanent EGL_RGB888 data */
            if ((dib888Ptr = altiaLibLoadDIB(raster, true)) == NULL)
                return false;
        }

#else /* NOT EGL_USE_ALTIABITSFORSCALING (i.e., do bitmap read for scaling) */
        if (!localReadSaveBits(win, raster, &dib888Ptr))
            return false;
#endif /* EGL_USE_ALTIABITSFORSCALING */


        /* If we have a transparency mask, need to create a MDIB from
         * the raster mask data as well so we have MDIB data that we
         * can scale/stretch/rotate.  On this target, the format of
         * the MDDB data that we already have is not public so we must
         * build an MDIB from the mask data.  On another target, it
         * may be possible to use the MDDB data directly if the format
         * is known and fixed.  We didn't build the extra MDIB at
         * initialization because it requires memory allocation and we
         * don't want to do that unless a raster is actually
         * scaled/stretched/rotated.
         */
        if (raster->maskId != 0)
        {
            EGL_MDIB *mdibPtr;

            /* If we don't have any DDB data, we are done. */
            if (raster->saveMaskBits == 0)
            {
                ALTIA_FREE(dib888Ptr);
                _altiaErrorMessage(
                    ALT_TEXT("Missing original mask DDB for scaling raster!"));
                return false;
            }

            /* First allocate the MDIB structure and its data area.
             * We don't have a choice.  We must allocate the MDIB and its
             * data area because we want to keep them around for future
             * drawing of the scaled/stretched/rotated raster.  The
             * altiaLibRasterDelete() function must free the memory.
             */
            if ((mdibPtr = (EGL_MDIB *) ALTIA_MALLOC((BITSTOBYTES(width)*height
                * sizeof(ALTIA_UBYTE))
                + sizeof(EGL_MDIB))) == NULL)
            {
                ALTIA_FREE(dib888Ptr);
                _altiaErrorMessage(altiaMsgNoMemMonoDIB);
                return false;
            }

            /* Initialize and load the MDIB with original mask data. */
            altiaLibLoadMonoDIB(
#ifndef ALTIA_DRV_LOADASSET_CHUNKS
                altiaImageAddress(raster->mask),
#else
                altiaImageAddressChunk(raster->mask, raster->maskCnt),
#endif /* ALTIA_DRV_LOADASSET_CHUNKS */
                raster->maskCnt, width, height,
                mdibPtr);

#ifdef ALTIA_DRV_LOADASSET_CHUNKS
            /* Get the address of 0 with size 0.  For drivers that are loading
             * assets in chunks this will "commit" the request.
             */
            (void)altiaImageAddressChunk((ALTIA_BINDEX)0, (ALTIA_UINT32)0);
#endif /* ALTIA_DRV_LOADASSET_CHUNKS */

            /* We have an MDIB for the bitmap mask.  We will
             * update the raster's saveMaskBits variable to point
             * to this new entity rather than the original MDDB.
             */
            raster->saveMaskBits = (ALTIA_UINT32) mdibPtr;
        }


        /* OK!  Everything went well and we have a DIB for the bitmap.
         * We will update the raster's saveBits variable to point
         * to this new entity rather than the original DDB.
         */
        raster->saveBits = (ALTIA_UINT32) dib888Ptr;

        /* Now add the saved DIB(s) to the local list.  From here on
         * out, it is up to the local list to keep track of them so
         * they can be shared by other bitmaps if needed.
         */
        localAddScaledSaveBits(raster);
    }

    return true;
}



/*--- localCreateTransBitmap() --------------------------------------------
 *
 * Local utility function that makes target specific graphics library
 * calls to merge a color device dependendent bitmap (DDB) with a monochrome
 * device dependent bitmap (MDDB) to create a transparent device dependent
 * bitmap (TDDB).  This is very target specific code.  If this type of
 * operation is not required on a target, this function is not necessary.
 * Please note that the creation of the TDDB by the target graphics library
 * is likely to require memory allocation on this target and the target
 * specific graphics library call to destroy the TDDB should be made when
 * the TDDB is nolonger needed.
 *-------------------------------------------------------------------------*/
static ALTIA_UINT32 localCreateTransBitmap(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                                    ALTIA_UINT32 bitmap, ALTIA_UINT32 mask,
                                    int width, int height, int hasAlpha)
{

#if EGL_HAS_TRANSBITMAPCREATE
    /* On this target, the graphics library requires that we explicitly
     * make a graphics library function call to create a color DDB that has
     * a transparency mask.  In this case, the pixels in the DDB that are
     * suppose to be transparent must have a value of 0.  Perform an AND
     * of color DDB with mono MDDB and use WHITE and BLACK as the foreground
     * and background colors for the MDDB.  This way, a color pixel that
     * needs to be visible will get ANDed with WHITE and still be visible.
     * A color pixel that needs to be transparent will get ANDed with BLACK
     * and become 0.
     */

    EGL_TDDB_ID transId;

    egl_RasterModeSet((EGL_GC_ID) dc, EGL_RASTER_OP_AND);
    egl_ForegroundColorSet((EGL_GC_ID) dc,
	 altiaLibGetColor(win, EGL_ADD_ALPHA(ALTIA_WHITE, EGL_OPAQUE_ALPHA)));
    egl_BackgroundColorSet((EGL_GC_ID) dc,
         altiaLibGetColor(win, EGL_ADD_ALPHA(ALTIA_BLACK, EGL_OPAQUE_ALPHA)));
    egl_AlphaChannelSet((EGL_GC_ID) dc, EGL_OPAQUE_ALPHA);

#if EGL_HAS_MONOBITMAPBLT
    /* If the target graphics library has a separate function to blit
     * monochrome bitmaps, use it.
     */
    egl_MonoBitmapBlt((EGL_GC_ID) dc, (EGL_MDDB_ID) mask,
                      (EGL_POS)0, (EGL_POS)0,
		      (EGL_POS)(width - 1), (EGL_POS)(height - 1),
                      (EGL_DRAWABLE_ID) (bitmap), (EGL_FLOAT)0, (EGL_FLOAT)0,
		      hasAlpha);
#else
    egl_BitmapBlt((EGL_GC_ID) dc, (EGL_MDDB_ID) mask,
                  (EGL_POS)0, (EGL_POS)0,
		  (EGL_POS)(width - 1), (EGL_POS)(height - 1),
                  (EGL_DRAWABLE_ID) (bitmap), (EGL_FLOAT)0, (EGL_FLOAT)0,
		  hasAlpha);
#endif /* EGL_HAS_MONOBITMAPBLT */

    /* Put the raster mode back to copy which is the standard
     * mode for drawing.
     */
    egl_RasterModeSet((EGL_GC_ID) dc, EGL_RASTER_OP_COPY);

    /* Now create the transparent TDDB from the massaged color DDB
     * and original mono MDDB.  On this target, the resulting TDDB
     * actually references the DDB and MDDB.  We will not ever destroy
     * these individually.  When the TDDB is later destroyed, the
     * graphics library will also destroy the associated DDB and
     * MDDB for us.
     */
    if ((transId
         = egl_TransBitmapCreateFromDDB(((AltiaDevInfo *) win)->devId,
                                       (EGL_DDB_ID) bitmap,
                                       (EGL_MDDB_ID) mask,
                                       NULL, hasAlpha)) == NULL)
    {
	_altiaErrorMessage(altiaMsgNoMemColorDDB);
	return 0;
    }

    /* It worked! Return the transparent bitmap id */
    return (ALTIA_UINT32) transId;

#else /* NOT EGL_HAS_TRANSBITMAPCREATE */
    /* This target has no explicit transparent bitmap type so
     * just return the original bitmap id to the caller.
     */
    return (ALTIA_UINT32) bitmap;

#endif /* EGL_HAS_TRANSBITMAPCREATE */

}


/*--- localDeleteBitmaps() ------------------------------------------------
 *
 * Local utility function that makes target specific graphics library
 * calls to delete device dependent bitmaps (DDBs) if they exist.
 * This is very target specific code.  This function correctly handles
 * being called with a nil bitmap parameter and just a mask.  It will
 * just delete the mask in this case.  If, however, it is called with
 * a bitmap and a mask, it will do a transparent bitmap destroy on this
 * target under the assumption that the mask is the bitmap's partner in
 * a transparent bitmap.
 *-------------------------------------------------------------------------*/
static void localDeleteBitmaps(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                               ALTIA_UINT32 bitmap, ALTIA_UINT32 mask,
                               ALTIA_INT hasAlpha)
{
    /* If the bitmap argument is valid, free it.. */
    if (bitmap != 0)
    {

#if EGL_HAS_TRANSBITMAPCREATE
	/* If there is a mask with the bitmap, then free the bitmap
	 * as a transparent bitmap on this target.  This will
	 * also free the regular bitmap and mask bitmap that were
	 * used to create the transparent bitmap.
	 */
	if (mask != 0)
	    egl_TransBitmapDestroy(((AltiaDevInfo *) win)->devId,
		                   (EGL_TDDB_ID) bitmap, hasAlpha);
	else
	    egl_BitmapDestroy(((AltiaDevInfo *) win)->devId,
	                      (EGL_DDB_ID) bitmap, hasAlpha);

#else /* NOT EGL_HAS_TRANSBITMAPCREATE */
	/* This target has no explicit function to simultaneously
	 * free the regular bitmap and mask bitmap of a transparent
	 * bitmap.  Instead, each element is freed individually.
	 */
	egl_BitmapDestroy(((AltiaDevInfo *) win)->devId,
	                  (EGL_DDB_ID) bitmap, hasAlpha);
	if (mask != 0)
	    egl_MonoBitmapDestroy(((AltiaDevInfo *) win)->devId,
	                          (EGL_MDDB_ID) mask);

#endif /* EGL_HAS_TRANSBITMAPCREATE */

    }
    else if (mask != 0)
    {
	/* We have a mask without a bitmap.  Just free the mask. */
	egl_MonoBitmapDestroy(((AltiaDevInfo *) win)->devId,
	                      (EGL_MDDB_ID) mask);
    }
}


/*--- altiaLibLoadRaster() ------------------------------------------------
 *
 * Function called from generic Altia code to create a device
 * dependent bitmap (DDB) from Altia raster data.  This function has just
 * a slight amount of code that is target specific.  The Altia functions
 * it calls do most of the target specific work.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibLoadRaster(Altia_Raster_type *raster)
{
#ifdef ALTIA_DRV_LOADRASTER
    /* Provide an override for Altia drivers. */
    if (ALTIA_FALSE == driverLoadRaster(raster))
    {
        /* Failed to load the raster */
        return;
    }
#else
    AltiaDevInfo *devInfo = altiaLibGetDevInfo();
    EGL_DIB *dibPtr;
    ALTIA_UINT32 bitData;
    int hasAlpha = 0;

    /* Alpha flag */
    if (raster->apixels != -1)
        hasAlpha = 1;

    /* Don't have a raster yet.  If we cannot make one, IDs will be zero. */
    raster->id = 0;
    raster->maskId = 0;

    /* Start by creating a target specific color device independent
     * bitmap (DIB) from the Altia raster data.  If memory allocation
     * is enabled, the DIB is allocated memory and we must free it
     * before returning.  If memory allocation is NOT enabled,
     * the DIB resides in static variable space.  If a NULL pointer
     * is returned, it is most likely because there isn't enough
     * memory or static variable space to convert the Altia raster
     * data to a DIB.
     */
    if ((dibPtr = altiaLibLoadDIB(raster, false)) == NULL)
        return;

    /* Convert the DIB to a target specific color device dependent
     * bitmap (DDB).  We will get a 0 value id if this fails. Please
     * note that the creation of the DDB by the target graphics library
     * is likely to require memory allocation on this target.  In this
     * case, the target specific graphics library call to destroy the
     * DDB should be made in altiaLibRasterDelete().
     */
    raster->id = altiaLibDIBToBitmap(devInfo->devId, dibPtr, &bitData, raster);


    /* Bitmap scaling is enabled, but we have no scaled data yet so let's
     * initialize the scaled elements to show that.  On this target, it
     * will be necessary to build some DIB data from the raster->id DDB
     * later on if we need to draw a scaled/stretched/rotated version
     * of the bitmap.  On this target, we MUST initially set saveBits
     * to raster->id so it is available later to build DIB data!
     *
     * On other targets, it may be possible to just save a pointer to the
     * data portion of the DDB (returned in the bitData pointer of the
     * previous function call).  On this target, however, the format of
     * the data portion is not public knowledge and we will have to deal
     * with that if the bitmap gets scaled/stretched/rotated.
     */
    raster->saveBits = raster->id;  /* For this target, we must do this! */
    raster->scaledId = 0;


    /*
    * Now we create the transparent mask if there is transparent bit data.
    */
    if (raster->id != 0 && raster->mask != -1)
    {
        /* We must create a target specific monochrome device independent
         * bitmap (MDIB) from the mask data and then we can convert that
         * into a monochrome device dependent bitmap (MDDB).  We can merge
         * this with the color DDB to get a transparent DDB.  That's a
         * lot to digest, but we'll take it in steps.  On this target,
         * creating the MDIB for the transparent mask is just like creating
         * the MDIB for an Altia stencil.  We can call the routine that
         * knows how to initialize and load data into an MDIB.  We will
         * reuse the memory of the DIB data structure, pointed to by dibPtr,
         * for this MDIB.  We know it has to be big enough because it held
         * color data already and monochrome data is always much smaller.
         */
        altiaLibLoadMonoDIB(
#ifndef ALTIA_DRV_LOADASSET_CHUNKS
            altiaImageAddress(raster->mask),
#else
            altiaImageAddressChunk(raster->mask, raster->maskCnt),
#endif /* ALTIA_DRV_LOADASSET_CHUNKS */
            raster->maskCnt, raster->width, raster->height,
            (EGL_MDIB *) dibPtr);

#ifdef ALTIA_DRV_LOADASSET_CHUNKS
        /* Get the address of 0 with size 0.  For drivers that are loading
         * assets in chunks this will "commit" the request.
         */
        (void)altiaImageAddressChunk((ALTIA_BINDEX)0, (ALTIA_UINT32)0);
#endif /* ALTIA_DRV_LOADASSET_CHUNKS */

        /* Convert the MDIB to a MDDB.  If the conversion fails, we will
        * have just a color bitmap with no mask.  Please note that the
        * creation of the MDDB by the target graphics library is likely to
        * require memory allocation on this target.  In this case, the
        * the target specific graphics library call to destroy the MDDB
        * should be made in altiaLibRasterDelete().  We do not need
        * a pointer to the DIB data, so pass NULL for that parameter.
        */
        if ((raster->maskId = altiaLibDIBToMonoBitmap(devInfo->devId,
            (EGL_MDIB *) dibPtr, NULL)) != 0)
        {
            ALTIA_UINT32 transBitmap;

            /* Bitmap scaling is enabled, but we have no scaled data yet
             * so let's initialize the scaled mask elements to show that.
             * On this target, it will be necessary to build some MDIB data
             * from the raster->maskId MDDB later on if we need to draw a
             * scaled/stretched/rotated version of the bitmap.  Set
             * saveMaskBits to raster->maskId so it is available later to
             * build DIB data.
             *
             * On other targets, it may be possible to just save a pointer
             * to the data portion of the MDDB (returned in the bitData
             * pointer of the previous function call).  On this target,
             * however, the format of the data portion is not public
             * knowledge and we will have to deal with that if the bitmap
             * gets scaled/stretched/rotated.
             */
            raster->saveMaskBits = raster->maskId;
            raster->scaledMaskId = 0;

            /* On this target, the graphics library requires that
             * we execute a set of operations to merge the DDB and MDDB
             * to get a transparent device dependent bitmap (TDDB).  Please
             * note that the creation of the TDDB by the target graphics
             * library is likely to require memory allocation on this target.
             * In this case, the target specific graphics library call to
             * destroy the TDDB should be made in altiaLibRasterDelete().
             */
            if ((transBitmap = localCreateTransBitmap((ALTIA_WINDOW) devInfo,
                                  (ALTIA_UINT32) devInfo->gc,
                                  raster->id, raster->maskId,
                                  raster->width, raster->height,
                                  hasAlpha)) == 0)
            {
                /* The creation failed.  We will have just a color bitmap
                 * with no transparency.  Delete the mask (but not the
                 * color bitmap)
                 */
                localDeleteBitmaps((ALTIA_WINDOW) devInfo,
                                   (ALTIA_UINT32) devInfo->gc,
                                   0  /* keeping color bitmap */,
                                   raster->maskId, 0);
                raster->maskId = 0;
            }
            else
            {
                /*  The TDDB becomes our raster id that we use in future
                 *  drawing operations if there is no bitmap scaling/
                 *  stretching/rotating.
                 */
                raster->id = transBitmap;
            }
        }
    }


#endif /* ALTIA_DRV_LOADRASTER */
}


/*--- localCopyRaster() ---------------------------------------------------
 *
 * Local utility function that finally draws a bitmap.  This is very
 * target specific code.  Some targets may require several graphics library
 * operations to draw a bitmap.  For example, it may be necessary to OR the
 * bitmap with some version of the mask to make transparent areas in the
 * bitmap all 1s and then AND the result with the destination area.  Pixels
 * that are in the transparent sections will stay the same when they are
 * ANDed with all 1s.  Other pixels will get the colors from the bitmap.
 * This is just one of many possible approaches.  On some targets, the
 * necessary work can be done at an earlier stage when the regular bitmap
 * and mask are converted to a transparent bitmap (see the local utility
 * localCreateTransBitmap()) and this function can just draw the bitmap
 * in one easy operation.  This is just one of many possible approaches.
 * The approach taken on a particular target is often very target specific.
 *-------------------------------------------------------------------------*/
static void localCopyRaster(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                            ALTIA_UINT32 bitmap, ALTIA_UINT32 mask,
                            ALTIA_FLOAT x, ALTIA_FLOAT y, int width, int height, int hasAlpha)
{

#if EGL_HAS_TRANSBITMAPBLT
    /* This target has one function for performing the blit of
     * a regular color bitmap without a transparency mask.  It has
     * a different function for blitting a color bitmap with a
     * transparency mask.
     */
    if (bitmap != 0)
    {
	if (mask == 0)
	    egl_BitmapBlt((EGL_GC_ID) dc, (EGL_DDB_ID) bitmap,
	                  (EGL_POS)0, (EGL_POS)0,
			  (EGL_POS)(width - 1), (EGL_POS)(height - 1),
			  ((AltiaDevInfo*)win)->drawable,
			  (EGL_FLOAT)x, (EGL_FLOAT)y, hasAlpha);
	else
	    egl_TransBitmapBlt((EGL_GC_ID) dc,
	                       (EGL_DDB_ID) bitmap, (EGL_MDDB_ID) mask,
	                       (EGL_POS)0, (EGL_POS)0,
			       (EGL_POS)(width - 1), (EGL_POS)(height - 1),
	                       ((AltiaDevInfo*)win)->drawable, (EGL_FLOAT)x,
			       (EGL_FLOAT)y, hasAlpha);
    }

#else  /* NOT EGL_HAS_TRANSBITMAPBLT */
    /* This target has just a single function to blit a regular color bitmap
     * or a color bitmap with a transparency mask.
     */
    if (bitmap != 0)
	egl_BitmapBlt((EGL_GC_ID) dc, (EGL_DDB_ID) bitmap,
	              0, 0, width - 1, height - 1,
		      ((AltiaDevInfo*)win)->drawable, (EGL_FLOAT)x,
		      (EGL_FLOAT)y, hasAlpha);

#endif /* EGL_HAS_TRANSBITMAPBLT */
}


/*--- localTransformedDraw() ----------------------------------------------
 *
 * Local utility function that prepares a scaled/stretched/rotated bitmap
 * for drawing and draws it.  It is tricky business.  On most targets, it
 * requires the creation of one or more temporary device independent bitmaps
 * (DIBs) that can be loaded with data that represent the transformed
 * version of the original bitmap.  One or two of the DIBs are finally
 * converted to device dependent bitmaps (DDBs) that can be drawn.  More
 * than one DIB and DDB are needed if the bitmap has a transparent mask.
 * This function returns the DDB ids to the caller so they can choose to
 * save them for future drawing or destroy them if saving them is not
 * appropriate (for example, because they are too big).  This function
 * has just a slight amount of code that is target specific.  The Altia
 * functions it calls do most of the target specific work.
 *-------------------------------------------------------------------------*/
static void localTransformedDraw(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                                 Altia_GraphicState_type *gs,
                                 Altia_Raster_type *raster,
                                 Altia_Transform_type *total,
                                 Altia_Extent_type *wex,
                                 int dx, int dy, int destWidth, int destHeight,
                                 int off, int totalHeight,
                                 ALTIA_UINT32 *bitmap, ALTIA_UINT32 *mask)
{
    EGL_DIB  *colorDest;
    EGL_DEVICE_ID devId = ((AltiaDevInfo *) win)->devId;

    EGL_MDIB *monoDest;

    int xoffset, yoffset;
    ALTIA_COORD xorigin, yorigin;
    int interpType;
    ALTIA_FLOAT mx, my;
    ALTIA_COORD x = raster->x;
    ALTIA_COORD y = raster->y;
    int width = raster->width;
    int height = raster->height;
    int hasAlpha = 0;

    if (raster->apixels != -1)
        hasAlpha = 1;

    /* For drawing the transformed raster into a new bitmap, we want it to
     * draw at 0,0.  Get any rotation offset and then add a translation to
     * the transform to get a 0,0 origin.
     */
    altia_rotation_offset(width, height, total, &xoffset, &yoffset);
    altiaLibTransform(total, 0, 0, &xorigin, &yorigin);
    altiaLibTranslate(total, ALTIA_I2D(-xorigin - xoffset),
                      ALTIA_I2D(-yorigin - yoffset));

    /* If the raster has a mask, we need to transform that as well. */
    if (raster->maskId != 0)
    {
	/* Get a mono DIB to use as a destination. */
	if ((monoDest = altiaLibCreateMonoDIB(destWidth, destHeight,
	                         true  /* Use scaled static area if needed */,
	                         false /* Not for scaled text bits */,
	                         NULL  /* Return ptr not needed */ )) == NULL)
	{
	    *mask = 0;
	    *bitmap = 0;
	    return;
	}

	/* Transform the original mono DIB (raster's saveMaskBits) into the
	 * destination mono DIB.
	 */
	altiaTransformBitmap((void *) raster->saveMaskBits, width, height,
	                     (void *) monoDest, dx, dy, destWidth, destHeight,
	                     total, 1);

	/* Convert the destination mono DIB to a mono DDB.  This is
	 * potentially reusable on future drawing operations so return it
	 * to the caller.
	 */
	*mask = altiaLibDIBToMonoBitmap(devId, monoDest,
	                                NULL /* Return ptr not needed */ );


    }
    else

    *mask = 0;

    /* Determine what type of color interpolation algorithm we should use
     * based on the type of raster data.  If the data is 8-bit color, use
     * nearest neighbor.  With higher resolution color, use the much
     * nicer bilinear algorithm.
     */
    if (raster->rasterType == ALTIA_RASTER_8_RLE)
	interpType = NearestNeighbor;
    else

	interpType = Bilinear;

    /* Get a color DIB to use as a destination.  For this target,
     * always get a DIB for an ALTIA_RASTER_24_RLE raster (which will
     * give us a DIB of type EGL_ARGB8888) type because the saved
     * bits we are using are always in 24-bit format (EGL_RGB888).
     * The altiaTransformRaster() function expects the destination to
     * be EGL_ARGB8888 when the source is EGL_RGB888.  That's how it
     * was architected.
     */
    if ((colorDest = altiaLibCreateDIB(destWidth, destHeight,
                           (ALTIA_SHORT) ((raster->apixels != -1) ?
                           ALTIA_RASTER_32_RLE : ALTIA_RASTER_24_RLE),
                           -1    /* Don't make a color table */,
                           0     /* No color count */,
                           false /* Not for permanent scalable bits */,
                           true  /* Use static area for scaled bits */,
                           false /* Not for scaled text bits */,
 	                       NULL  /* Return ptr not needed */ )) == NULL)
    {
	if (*mask != 0)
	{
	    localDeleteBitmaps(win, dc, 0  /* no color bitmap */, *mask, 0);
	    *mask = 0;
	}
	*bitmap = 0;
	return;
    }

    /* Transform the original color DIB (raster's saveBits) into the
     * destination color DIB.
     */
    altiaTransformRaster((void *) raster->saveBits, width, height,
                         (void *) colorDest, dx, dy, destWidth, destHeight,
                         total, interpType, (void *) raster->saveMaskBits);

#if EGL_HAS_INDEXEDSYSTEMCOLORMAP
    /* NOTE:  For indexed color bitmap support, some targets may require
     * us to add the current system color palette to the color DIB
     * at this point before we convert it to a DDB.
     */
#endif /* EGL_HAS_INDEXEDSYSTEMCOLORMAP */

    /* Convert the destination color DIB to a color DDB.  This is potentially
     * reusable on future drawing operations so return it to the caller.
     */
    *bitmap = altiaLibDIBToBitmap(devId, colorDest, NULL, raster);


    /* If we don't have a color DDB, we are done. */
    if (*bitmap == 0)
    {
	if (*mask != 0)
	{
	    localDeleteBitmaps(win, dc, 0  /* no color bitmap */, *mask, 0);
	    *mask = 0;
	}

	return;
    }

    /* If we have a mask, we need to do some work on this target to
     * merge the mask MDDB with the color DDB to get a transparent
     * device dependent bitmap (TDDB).
     */
    if (*mask != 0)
    {
	ALTIA_UINT32 transBitmap;

	/* On this target, the graphics library requires that we
	 * execute a set of operations to merge the DDB and MDDB to
	 * get a transparent device dependent bitmap (TDDB).  Please
	 * note that the creation of the TDDB by the target graphics
	 * library is likely to require memory allocation on this target.
	 * In this case, the target specific graphics library call to
	 * destroy the TDDB should be made when the TDDB is nolonger
	 * needed.
	 */
	if ((transBitmap = localCreateTransBitmap(win, dc, *bitmap, *mask,
	                                        destWidth, destHeight,
						hasAlpha)) == 0)
	{
	    /* The creation failed.  We will have just a color bitmap */
	    localDeleteBitmaps(win, dc, 0  /* keeping color bitmap */,
	                       *mask, 0);
	    *mask = 0;
	}
	else
	{
	    /* The transparent DDB id is potentially reusable on future
	     * drawing operations so return it to the caller.
	     */
	    *bitmap = transBitmap;
	}
    }

    /* Adjust the translation in the transform so that we copy the bitmap
     * to the correct location in the drawing area.
     */
    altiaLibTranslate(total, ALTIA_I2D(xorigin + xoffset),
                      ALTIA_I2D(yorigin + yoffset));
    altiaLibTransformF(total, ALTIA_I2D(x), ALTIA_I2D(y), &mx, &my);

    /* Altia's coord system has its origin at the lower left corner
     * and this target has its origin at the upper left so flip the
     * y value as we do for every other drawing operation.
     * On this target, the raster is inclusive of its y position
     * so y is exactly (wex->y1 - my).
     */
    /* Ticket 646 */
    /* my = wex->y1 - my - totalHeight; */
    my = (ALTIA_FLOAT) (ALTIA_I2D(wex->y1) - my - ALTIA_I2D(totalHeight) + ALTIA_C2D(1.0));

    /* Build a clipping region for the transformed raster.  This
     * must approximate a polygon if the raster is rotated.
     */
    altia_clip_rotate(gs, wex, dc, total, x, y, width, height);

    egl_AlphaChannelSet((EGL_GC_ID) dc, gs->alpha);

#ifdef ALTIA_USE_BACKGROUND_COLOR_FLAG
    /*
     * Sometimes the driver layer uses the background color to treat
     * raster images differently.  The background color doesn't have
     * any real impact on the drawing and would only be used as a
     * flag.  Here we set it accordingly.
     */
    egl_BackgroundColorSet((EGL_GC_ID) dc, gs->background);
#endif /* ALTIA_USE_BACKGROUND_COLOR_FLAG */

#if EGL_HAS_RASTER_SET
    egl_BitmapRasterSet((EGL_GC_ID)dc, (void *)raster);
#endif
    localCopyRaster(win, dc, *bitmap, *mask,
        (ALTIA_FLOAT) (mx + ALTIA_I2D(xoffset + dx)),
        (ALTIA_FLOAT) (my + ALTIA_I2D(off - yoffset)),
        destWidth, destHeight, hasAlpha);
#if EGL_HAS_RASTER_SET
    egl_BitmapRasterSet((EGL_GC_ID)dc, (void *)NULL);
#endif
    /* Clear clip region now that we are done */
    altia_clip_clear(dc);
}





/* Used to decode a packed set of raster data */
static Altia_Raster_type _localRaster;



/*--- altiaLibRasterFromIndex() -------------------------------------------
 *
 * This function populates a raster structure with data from the raster
 * with the specified index in data.c.
 *-------------------------------------------------------------------------*/
Altia_Raster_type * altiaLibRasterFromIndex(ALTIA_INDEX index)
{




    /* Decode raster from packed data into a usable RAM structure */
    if (index >= 0 && index < test_rasters_count)
    {
        _localRaster.rpixels        = ALTIA_RASTER_RPIXELS(index);
        _localRaster.gpixels        = ALTIA_RASTER_GPIXELS(index);
        _localRaster.bpixels        = ALTIA_RASTER_BPIXELS(index);
        _localRaster.apixels        = ALTIA_RASTER_APIXELS(index);
        _localRaster.mask           = ALTIA_RASTER_MASK(index);
        _localRaster.colors         = ALTIA_RASTER_COLORS(index);
        _localRaster.rCnt           = ALTIA_RASTER_RCNT(index);
        _localRaster.gCnt           = ALTIA_RASTER_GCNT(index);
        _localRaster.bCnt           = ALTIA_RASTER_BCNT(index);
        _localRaster.aCnt           = ALTIA_RASTER_ACNT(index);
        _localRaster.maskCnt        = ALTIA_RASTER_MASKCNT(index);
        _localRaster.colorCnt       = ALTIA_RASTER_COLORCNT(index);
        _localRaster.rasterType     = ALTIA_RASTER_RASTERTYPE(index);
        _localRaster.x              = ALTIA_RASTER_X(index);
        _localRaster.y              = ALTIA_RASTER_Y(index);
        _localRaster.width          = ALTIA_RASTER_WIDTH(index);
        _localRaster.height         = ALTIA_RASTER_HEIGHT(index);

        _localRaster.id             = ALTIA_RASTER_ID(index);
        _localRaster.maskId         = ALTIA_RASTER_MASKID(index);
        _localRaster.saveMaskBits   = ALTIA_RASTER_SAVEMASKBITS(index);

        _localRaster.saveBits       = ALTIA_RASTER_SAVEBITS(index);


        _localRaster.scaledId       = ALTIA_RASTER_SCALEDID(index);
        _localRaster.scaledMaskId   = ALTIA_RASTER_SCALEDMASKID(index);
        _localRaster.scaledMaskBits = ALTIA_RASTER_SCALEDMASKBITS(index);


        return &_localRaster;
    }

    /* Failed */
    return NULL;



}

/*--- altiaLibRasterToIndex() -------------------------------------------
 *
 * This function saves the data from a raster structure into the raster
 * with the specified index in data.c.
 *-------------------------------------------------------------------------*/
void altiaLibRasterToIndex(Altia_Raster_type * raster, ALTIA_INDEX index)
{



    if (index >= 0 && index < test_rasters_count)
    {
        ALTIA_SET_RASTER_ID(index,_localRaster.id);
        ALTIA_SET_RASTER_MASKID(index,_localRaster.maskId);
        ALTIA_SET_RASTER_SAVEMASKBITS(index,_localRaster.saveMaskBits);

        ALTIA_SET_RASTER_SAVEBITS(index,_localRaster.saveBits);


        ALTIA_SET_RASTER_SCALEDID(index, _localRaster.scaledId);
        ALTIA_SET_RASTER_SCALEDMASKID(index,_localRaster.scaledMaskId);
        ALTIA_SET_RASTER_SCALEDMASKBITS(index,_localRaster.scaledMaskBits);

    }



}

/*--- altiaLibRasterDelete() ----------------------------------------------
 *
 * Function called from generic Altia code to delete target
 * specific device dependent bitmaps (DDBs) and clear the raster data
 * elements that they are using.  This function has no target specific
 * code although freeing memory held by any DIBs used for
 * scaling/stretching/rotating might be considered target specific.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibRasterDelete(Altia_Raster_type *raster)
{
#ifdef ALTIA_DRV_LOADRASTER
    /* Provide an override for Altia drivers. */
    driverDeleteRaster(raster);
#else
    int hasAlpha = 0;
    AltiaDevInfo *devInfo = altiaLibGetDevInfo();

    if (raster->apixels != -1)
        hasAlpha = 1;



    /* If the raster has no reference to a scaledId, there still could be
     * a scaled raster that refers to this raster.  For example, because
     * an Image object is getting loaded with a new image immediately
     * after it has animated (e.g., transformed) and before it's had a
     * chance to redraw.  The scaled raster data needs to be deleted if
     * its usage count is 0.
     */
    if (!(raster->scaledId) && scaledRasterCnt && raster->id)
    {
        int i;
        Altia_Scaled_Raster_type *scaledPtr = test_scaledRaster;
        for (i = 0; i < scaledRasterCnt; i++, scaledPtr++)
        {
            /* Check for "orphan" scaled data that refers to current raster.
             * It will have a count of 0, but a non-zero id and an
             * origId that matches the current raster id.
             */
            if (!(scaledPtr->cnt) && scaledPtr->id
                && raster->id == scaledPtr->origId
                && raster->maskId == scaledPtr->origMaskId)
            {
                /* The logic here is equivalent to the logic in
                 * localRemoveScaledRaster() for permanently
                 * removing scaled raster data.
                 */
                int cnt;
                localDeleteScaledRasterData(scaledPtr);
                cnt = scaledRasterCnt - i -1;
                if (cnt > 0)
                {
                    memmove(&test_scaledRaster[i], &test_scaledRaster[i+1],
                            sizeof(Altia_Scaled_Raster_type) *cnt);
                }
                scaledRasterCnt--;

                /* The data was found so the search is done. */
                break;
            }
        }
    }
    else
    {
        /* Use a local utility to decrement the usage count for the
         * scaled bitmap data and mask data (if a mask exists) and
         * permanently remove the data if the usage count goes to 0.
         */
        localRemoveScaledRaster(raster->scaledId, raster->scaledMaskId,
                                true /* permanently remove data */);
        raster->scaledId = 0;
        raster->scaledMaskId = 0;
    }

    /* Use a local utility to decrement the usage count for the
     * saved bits data (if any exists) and also delete the data if the
     * usage count goes to 0.
     */
    localRemoveScaledSaveBits(raster->saveBits, raster->saveMaskBits);
    raster->saveBits = 0;
    raster->saveMaskBits = 0;



    /* Use a local utility to delete the actual bitmap and mask DDBs
     * if they exist.
     */
    localDeleteBitmaps((ALTIA_WINDOW) devInfo, (ALTIA_UINT32) devInfo->gc,
                       raster->id, raster->maskId, hasAlpha);

    /* The bitmap and and mask DDBs are now gone so set the raster's data
     * members to reflect this fact.
     */
    raster->id = 0;
    raster->maskId = 0;
#endif /* ALTIA_DRV_LOADRASTER */
}


/*--- altiaLibRasterDraw() ------------------------------------------------
 *
 * Function called from generic Altia code to draw an image (raster object)
 * that has already been set up as a device dependent bitmap (DDB)
 * for the target.  This function has some target specific code if
 * bitmap scaling/stretching/rotating is enabled.  Otherwise, there's
 * just a small amount of target specific code.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibRasterDraw(ALTIA_WINDOW win, ALTIA_UINT32 dc,
                        Altia_GraphicState_type *gs,
                        Altia_Raster_type *raster,
                        Altia_Transform_type *total)
{
    Altia_Extent_type wex;
    ALTIA_COORD width = raster->width;
    ALTIA_COORD height = raster->height;
    ALTIA_COORD x = raster->x;
    ALTIA_COORD y = raster->y;
    ALTIA_FLOAT fx, fy;
    int hasAlpha = 0;

    ALTIA_BOOLEAN isTrans = altiaLibTransformed(total, width, height);




    if (raster->apixels != -1)
        hasAlpha = 1;

    if (raster->id == 0)
        return;



    altiaLibGetFullWindowExtent(win, &wex);

    /* Consolidated alpha channel set here instead
    ** of in separate cases below.  This is required
    ** so the GC is accurate in the call to
    ** egl_RasterTransformDraw().
    */
    egl_AlphaChannelSet((EGL_GC_ID)dc, gs->alpha);


#if EGL_MIXED_RASTER_TRANSFORMATION
    /* Some targets may have mixed hardware and software rendering.
    ** For those targets, ask if the EGL code will perform the
    ** transformation or if we should perform it here.
    */
    if (egl_RasterTransformDraw((EGL_GC_ID)dc, (void *)total,
                                (EGL_DDB_ID)raster->id))
        isTrans = false;
#endif
    if (isTrans == false)
    {


	altiaLibTransform(total, x, y + height - 1, &x, &y);

	/* Altia's coord system has its origin at the lower left corner
	 * and this target has its origin at the upper left so flip the
	 * y value as we do for every other drawing operation.
	 * On this target, the raster is inclusive of its y position
	 * so y is exactly (wex.y1 - y).
	 */
	y = wex.y1 - y;
	fx = ALTIA_I2F(x);
	fy = ALTIA_I2F(y);

	if (gs->clip.x0 <= gs->clip.x1 && gs->clip.y0 <= gs->clip.y1)
	{ /* Clipping */

	    /* On this target, clip rect is inclusive of the rectangle's
	     * coordinates.
	     */

	    egl_ClipRectSet((EGL_GC_ID) dc,
	                    (EGL_POS) gs->clip.x0,
	                    (EGL_POS) (wex.y1 - gs->clip.y1),
	                    (EGL_POS) gs->clip.x1,
	                    (EGL_POS) (wex.y1 - gs->clip.y0));
	}


#ifdef ALTIA_USE_BACKGROUND_COLOR_FLAG
        /*
         * Sometimes the driver layer uses the background color to treat
         * raster images differently.  The background color doesn't have
         * any real impact on the drawing and would only be used as a
         * flag.  Here we set it accordingly.
         */
        egl_BackgroundColorSet((EGL_GC_ID) dc, gs->background);
#endif /* ALTIA_USE_BACKGROUND_COLOR_FLAG */

#if EGL_HAS_RASTER_SET
        egl_BitmapRasterSet((EGL_GC_ID)dc, (void *)raster);
#endif
        localCopyRaster(win, dc, raster->id, raster->maskId, fx, fy,
                        width, height, hasAlpha);
#if EGL_HAS_RASTER_SET
        egl_BitmapRasterSet((EGL_GC_ID)dc, (void *)NULL);
#endif

	/* Clear the clipping rectangle by setting it to the entire
	 * display area.
	 */
	egl_ClipRectSet((EGL_GC_ID) dc,
	                (EGL_POS) wex.x0,
	                (EGL_POS) wex.y0,
	                (EGL_POS) wex.x1,
	                (EGL_POS) wex.y1);


    }
    else
    {
	int dx, dy, destWidth, destHeight, off, totalWidth, totalHeight;
	ALTIA_BOOLEAN clipped;
	ALTIA_BOOLEAN smallClip = false;
	int tempWidth;
	int tempHeight;
	ALTIA_UINT32 bitmap;
	ALTIA_UINT32 mask;

	/* If we have a valid transformed bitmap already, draw it. */
	if (raster->scaledId != 0)
	{
	    int xoffset, yoffset;
	    ALTIA_COORD mx, my;

	    egl_BitmapSize((EGL_DDB_ID)raster->scaledId, &destWidth,
	                      &destHeight, hasAlpha);

	    /* Get any rotation offset for the new origin of the transformed
	     * raster relative to the original origin.
	     */
	    altia_rotation_offset(width, height, total, &xoffset, &yoffset);

	    /* Build a clipping region for the transformed raster.  This
	     * must approximate a polygon if the raster is rotated.
	     */
	    altia_clip_rotate(gs, &wex, dc, total, x, y, width, height);

	    /* Transform the original raster x,y coords. */
	    altiaLibTransform(total, x, y, &mx, &my);

	    /* Altia's coord system has its origin at the lower left corner
	     * and this target has its origin at the upper left so flip the
	     * y value as we do for every other drawing operation.
	     * On this target, the raster is inclusive of its y position
	     * so y is exactly (wex.y1 - my).
	     */
	    /* TRB - 9/19/03 Raster is drawing one pixel too low
	     * so this must be same as the one below so saved pixels
	     * don't draw too low
	     */
	    /* Ticket 646 */
	    my = wex.y1 - my - destHeight+1;
	    /* my = wex.y1 - my - destHeight; */

#ifdef ALTIA_USE_BACKGROUND_COLOR_FLAG
            /*
             * Sometimes the driver layer uses the background color to treat
             * raster images differently.  The background color doesn't have
             * any real impact on the drawing and would only be used as a
             * flag.  Here we set it accordingly.
             */
            egl_BackgroundColorSet((EGL_GC_ID) dc, gs->background);
#endif /* ALTIA_USE_BACKGROUND_COLOR_FLAG */

#if EGL_HAS_RASTER_SET
            egl_BitmapRasterSet((EGL_GC_ID)dc, (void *)raster);
#endif
            /* Draw the transformed bitmap data associated with the raster. */
            localCopyRaster(win, dc, raster->scaledId, raster->scaledMaskId,
                           ALTIA_I2F(mx + xoffset), ALTIA_I2F(my - yoffset),
                           destWidth, destHeight,
                           hasAlpha);
#if EGL_HAS_RASTER_SET
            egl_BitmapRasterSet((EGL_GC_ID)dc, (void *)NULL);
#endif

	    /* Clear clip region now that we are done. */
	    altia_clip_clear(dc);

	    return;
	}

	/* Not so lucky. We don't have a transformed bitmap already
	 * built.  Let's go about building one.
	 */

	/* We want to try and save what we draw, but if it's not the
	 * whole bitmap, let's not save it.  Just draw it on the screen.
	 */

	altia_get_dest_bounds(gs, total, &wex, x, y, width, height,
			      &dx, &dy, &destWidth, &destHeight, &off,
			      &clipped, &totalWidth, &totalHeight);

	/* If there is nothing to draw, return. */
	if (destWidth <= 0 || destHeight <= 0) {

	    return;
	}

	/* If we are clipped we can still save the raster to a bitmap
	 * if its small enough.
	 */
	tempWidth = destWidth;
	tempHeight = destHeight;
	if (clipped)
	{
	    int totalArea = totalWidth * totalHeight;
	    if (totalArea < RASTERPIXSIZE
	        && (destWidth * destHeight * 10) > totalArea)
	    {
		destWidth = totalWidth;
		destHeight = totalHeight;
		smallClip = true;
	    }
	}

	/* If we are not clipped or clip is small enough, draw transformed
	 * raster to an off screen canvas so we can draw faster next time.
	 */
	if (clipped == false || smallClip)
	{
	    /* If raster is really small enough, create new scaled bitmaps,
	     * draw them, and then save the new bitmaps with the raster.
	     */
	    if ((destWidth * destHeight) < RASTERPIXSIZE)
	    {
		int scaledIndex = localFindScaledRaster(raster, total);
		if (scaledIndex >= 0)
		{
		    int xoffset, yoffset;
		    ALTIA_COORD mx, my;

		    raster->scaledId = test_scaledRaster[scaledIndex].id;
		    raster->scaledMaskId = test_scaledRaster[scaledIndex].maskId;

		    /* We can use our local destination width and height
		     * variables to hold the actual width and height of
		     * the existing scaled bitmap.
		     */
		    egl_BitmapSize((EGL_DDB_ID)raster->scaledId, &destWidth,
				  &destHeight, hasAlpha);

		    /* Get any rotation offset for the new origin of the
		     * transformed raster relative to the original origin.
		     */
		     altia_rotation_offset(width, height, total,
		                           &xoffset, &yoffset);

		    /* Build a clipping region for the transformed raster.
		     * This must approximate a polygon if the raster is
		     * rotated.
		     */
		    altia_clip_rotate(gs, &wex, dc, total,
		                      x, y, width, height);

		    /* Transform the original raster x,y coords. */
		    altiaLibTransform(total, x, y, &mx, &my);

		    /* Altia's coord system has its origin at the lower left
		     * corner and this target has its origin at the upper
		     * left so flip the y value as we do for every other
		     * drawing operation.  On this target, the raster is
		     * inclusive of its y position so y is exactly
		     * (wex.y1 - my).
		     */
		    /* Ticket 646 */
		    /* my = wex.y1 - my - destHeight; */
		    my = wex.y1 - my - destHeight+1;

#ifdef ALTIA_USE_BACKGROUND_COLOR_FLAG
                    /*
                     * Sometimes the driver layer uses the background color to treat
                     * raster images differently.  The background color doesn't have
                     * any real impact on the drawing and would only be used as a
                     * flag.  Here we set it accordingly.
                     */
                    egl_BackgroundColorSet((EGL_GC_ID) dc, gs->background);
#endif /* ALTIA_USE_BACKGROUND_COLOR_FLAG */

#if EGL_HAS_RASTER_SET
                    egl_BitmapRasterSet((EGL_GC_ID)dc, (void *)raster);
#endif
                    /* Draw the transformed bitmap data associated with the
                     * raster.
                     */
                    localCopyRaster(win, dc, raster->scaledId,
                            raster->scaledMaskId,
                            ALTIA_I2F(mx + xoffset), ALTIA_I2F(my - yoffset),
                            destWidth, destHeight,
                            hasAlpha);
#if EGL_HAS_RASTER_SET
                    egl_BitmapRasterSet((EGL_GC_ID)dc, (void *)NULL);
#endif

		    /* Clear clip region now that we are done. */
		    altia_clip_clear(dc);
		}
		else
		{
		    /* If a scaled bitmap already exists, get rid of it.
		     * This should really never happen.
		     */
		    if (raster->scaledId != 0)
			altiaLibRasterClear(raster);

		    /* Get bitmap and mask DIBs from our raster DDBs so that
		     * we can smooth the scaled/stretched/rotated image.
		     * If we are lucky, the DIBs already exist.  Otherwise,
		     * this function takes the necessary action to create them.
		     * If we cannot create them, we are done!
		     */
		    if (!localGetScaledSaveBits(win, dc, raster)) {

			return;
		    }

		    localTransformedDraw(win, dc, gs, raster, total, &wex,
		                         0, 0, destWidth, destHeight,
		                         0, destHeight, &bitmap, &mask);

		    /* Assign the scaled bitmap and its mask to the raster. */
		    raster->scaledId = bitmap;
		    raster->scaledMaskId = mask;

		    /* If we really have a scaled bitmap, add it to our set
		     * of other scaled bitmaps so another raster object can
		     * potentially use it as well.
		     */
		    if (bitmap != 0)
			localAddScaledRaster(raster, total);
		}


		return;
	    }
	}

	if (smallClip)
	{
	    /* The small clip could not be drawn to bitmap
	     * so restore destWidth, destHeight.
	     */
	    destWidth = tempWidth;
	    destHeight = tempHeight;
	}

	/* If a scaled bitmap already exists, get rid of it.
	 * This should really never happen.
	 */
	if (raster->scaledId != 0)
	    altiaLibRasterClear(raster);

	/* Get bitmap and mask DIBs from our raster DDBs so that
	 * we can smooth the scaled/stretched/rotated image.
	 * If we are lucky, the DIBs already exist.  Otherwise,
	 * this function takes the necessary action to create them.
	 * If we cannot create them, we are done!
	 */
	 if (!localGetScaledSaveBits(win, dc, raster)) {

	     return;
	 }

	localTransformedDraw(win, dc, gs, raster, total, &wex, dx, dy,
	                     destWidth, destHeight, off, totalHeight,
	                     &bitmap, &mask);

	/* It was determined that these bitmaps were too large to keep
	 * around and they should be freed after we use them to draw.
	 */
	localDeleteBitmaps(win, dc, bitmap, mask, hasAlpha);
    }



}


/*--- altiaLibRasterClear() -----------------------------------------------
 *
 * Function called from generic Altia code to delete target
 * specific SCALED device dependent bitmaps (DDBs) and clear the raster
 * data elements that they are using.  This function has no target specific
 * code, but calls other functions that do have target specific code.
 *
 * Note:  This function is called each time a raster's transformation
 *        changes due to scaling/stretching/rotating so that any existing
 *        SCALED DDBs can be deleted before new ones are created.  Saved
 *        DIBs of the original raster must NOT be deleted by this function
 *        because they are needed to create the new scaled DDBs.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibRasterClear(Altia_Raster_type *raster)
{
    /* Use a local utility to decrement the usage count for the
     * scaled bitmap data and mask data (if a mask exists).  If the
     * usage count goes to 0, local utility can decide if it wants
     * to remove it or keep it around for possible future use.
     */
    localRemoveScaledRaster(raster->scaledId, raster->scaledMaskId,
                            false /* not a permanent removal */);
    raster->scaledId = 0;
    raster->scaledMaskId = 0;
}

void altiaLibRasterClearPerm(Altia_Raster_type *raster)
{
    localRemoveScaledRaster(raster->scaledId, raster->scaledMaskId, true);
    raster->scaledId = 0;
    raster->scaledMaskId = 0;
}




