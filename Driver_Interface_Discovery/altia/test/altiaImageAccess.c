/* Copyright (c) 2010 Altia Inc.
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

/***************************************************************************
 * FILE         : altiaImageAccess.c
 *
 * AUTHOR       : Altia, Incorporated
 *                Copyright 2010, Altia, Incorporated
 *                All rights reserved.
 *
 * ONELINER     : Interface for Image Access Engine
 *
 * DESCRIPTION  : This file contains the interface for accessing images
 *                for Altia DeepScreen.  The images may be in generated
 *                source code files or in external binary files.
 *
 ***************************************************************************/

#include <stdio.h>

#include "altiaImageAccess.h"
#include "altiaImageData.h"


/******************************************************************************
 * External Interfaces
 *****************************************************************************/
#ifdef ALTIA_DRV_LOADASSET
extern void * driverLoadAsset(
#ifdef Altiafp
char * filename, 
int size
#endif
);
extern void * driverFinishAsset(
#ifdef Altiafp
void * asset,
int size
#endif
);
extern void driverFreeAsset(
#ifdef Altiafp
void * asset,
int size
#endif
);
#endif /* ALTIA_DRV_LOADASSET */

#ifdef ALTIA_DRV_LOADASSET_CHUNKS
extern void * driverLoadAssetChunk(
#ifdef Altiafp
void * address,
int size
#endif
);
#endif /* ALTIA_DRV_LOADASSET_CHUNKS */


/******************************************************************************
 * Definitions
 *****************************************************************************/


/******************************************************************************
 * Types
 *****************************************************************************/


/******************************************************************************
 * Private variables
 *****************************************************************************/


/******************************************************************************
 * Public functions
 *****************************************************************************/

ALTIA_BOOLEAN altiaImageInitialize(void)
{
    /* Called during _altiaInit() in order to allocate
    ** memory and load assets into memory.
    */
#if IMAGE_USE_EXTERNAL_FILES && ALTIA_IMAGE_BANK_COUNT
    int i;

    /* Loop through asset file list and load each data bank */
    for (i = 0; i < ALTIA_IMAGE_BANK_COUNT; i++)
    {
#ifdef ALTIA_DRV_LOADASSET
        /* Driver will load the assets */
        pAltiaImageBanks[i] = (ALTIA_UBYTE *)driverLoadAsset((char *)pAltiaImageFiles[i], (int)uAltiaImageFileSizes[i]);
#else
        FILE * fp;

        /* If partition size is 0, skip it.  This is an empty reflash
         * partition.  It can happen when reflash partitioning is enabled
         * and a runtime font engine is also enabled.
         */
        if (0 == uAltiaImageFileSizes[i])
        {
            pAltiaImageBanks[i] = NULL;
            continue;
        }

        /* Open the asset file */
        fp = fopen(pAltiaImageFiles[i], "rb");
        if (!fp)
        {
            return ALTIA_FALSE;
        }

        /* Load the asset into a memory block */
        pAltiaImageBanks[i] = (ALTIA_UBYTE *)ALTIA_MALLOC(uAltiaImageFileSizes[i]);
        if (NULL != pAltiaImageBanks[i])
        {
            fread(pAltiaImageBanks[i], 1, uAltiaImageFileSizes[i], fp);
        }

        fclose(fp);
#endif /* ALTIA_DRV_LOADASSET */
        /* Verify the asset file was loaded */
        if (NULL == pAltiaImageBanks[i])
        {
            return ALTIA_FALSE;
        }
    }
#endif /* ALTIA_IMAGE_BANK_COUNT && IMAGE_USE_EXTERNAL_FILES */
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN altiaImageLoadComplete(void)
{
    /* Called after pixels and stencils are
    ** decompressed.  Can be used to optionally
    ** free that memory if no longer used.
    **
    ** NOTE:  The memory is still required when:
    **
    ** 1. Using uncompressed image storage
    ** 2. Using compressed images with NO_PRELOAD
    ** 3. Using software based scaling/rotation
    */
#if IMAGE_USE_EXTERNAL_FILES && ALTIA_IMAGE_BANK_COUNT
#ifdef ALTIA_DRV_LOADASSET
    int i;

    /* Loop through asset file list and 'finish' each data bank */
    for (i = 0; i < ALTIA_IMAGE_BANK_COUNT; i++)
    {
        /* Driver may do something with the asset */
        pAltiaImageBanks[i] = driverFinishAsset((void *)pAltiaImageBanks[i], (int)uAltiaImageFileSizes[i]);
    }
#endif /* ALTIA_DRV_LOADASSET */
#endif /* ALTIA_IMAGE_BANK_COUNT && IMAGE_USE_EXTERNAL_FILES */
    return ALTIA_TRUE;
}

ALTIA_BOOLEAN altiaImageTerminate(void)
{
    /* Called during _altiaClose() in order to free
    ** asset memory.
    */
#if IMAGE_USE_EXTERNAL_FILES && ALTIA_IMAGE_BANK_COUNT
    int i;

    /* Loop through asset file list and free each data bank */
    for (i = 0; i < ALTIA_IMAGE_BANK_COUNT; i++)
    {
#ifdef ALTIA_DRV_LOADASSET
        /* Driver will free the assets */
        driverFreeAsset((void *)pAltiaImageBanks[i], (int)uAltiaImageFileSizes[i]);
#else
        /* A NULL address is an empty reflash partition.  This can happen
         * when reflash partitioning is enabled and a runtime font engine
         * is also enabled.
         */
        if (NULL != pAltiaImageBanks[i])
        {
            ALTIA_FREE(pAltiaImageBanks[i]);
        }
#endif /* ALTIA_DRV_LOADASSET */
        /* Reset the asset pointer */
        pAltiaImageBanks[i] = NULL;
    }
#endif /* ALTIA_IMAGE_BANK_COUNT && IMAGE_USE_EXTERNAL_FILES */
    return ALTIA_TRUE;
}

ALTIA_CONST ALTIA_UBYTE * altiaImageAddress(ALTIA_BINDEX index)
{
    ALTIA_UBYTE * address = (ALTIA_UBYTE *)NULL;
#if ALTIA_IMAGE_BANK_COUNT
    ALTIA_BINDEX bank = (ALTIA_BINDEX)((index >> ALTIA_IMAGE_BANK_SHIFT) & ALTIA_IMAGE_BANK_MASK);

    if ((bank >= (ALTIA_BINDEX)0) && (bank < (ALTIA_BINDEX)ALTIA_IMAGE_BANK_COUNT))
    {
        address = (ALTIA_UBYTE *)pAltiaImageBanks[bank];
        address += (index & ALTIA_IMAGE_INDEX_MASK);
    }
#endif
    return (ALTIA_CONST ALTIA_UBYTE *)address;
}

ALTIA_CONST ALTIA_UBYTE * altiaImageFontAddress(ALTIA_FBINDEX index)
{
    return altiaImageAddress((ALTIA_BINDEX)index);
}

ALTIA_CONST ALTIA_UBYTE * altiaImageStencilAddress(ALTIA_BINDEX index)
{
    return altiaImageAddress(index);
}

ALTIA_CONST ALTIA_UBYTE * altiaImagePatternAddress(ALTIA_BINDEX index)
{
    return altiaImageAddress(index);
}

#ifdef ALTIA_DRV_LOADASSET_CHUNKS
ALTIA_CONST ALTIA_UBYTE * altiaImageAddressChunk(ALTIA_BINDEX index, ALTIA_UINT32 bytes)
{
    ALTIA_UBYTE * address = (ALTIA_UBYTE * )altiaImageAddress(index);

    /* Tell the driver to load the asset chunk */
    address = driverLoadAssetChunk((void *)address, (int)bytes);

    return (ALTIA_CONST ALTIA_UBYTE *)address;
}

ALTIA_CONST ALTIA_UBYTE * altiaImageFontAddressChunk(ALTIA_FBINDEX index, ALTIA_UINT32 bytes)
{
    ALTIA_UBYTE * address = (ALTIA_UBYTE * )altiaImageAddressChunk((ALTIA_BINDEX)index, bytes);

    /* Get the address of 0 with size 0.  For drivers that are loading assets in
     * chunks this will "commit" the request.
     */
    (void)altiaImageAddressChunk((ALTIA_BINDEX)0, (ALTIA_UINT32)0);

    return address;
}

ALTIA_CONST ALTIA_UBYTE * altiaImageStencilAddressChunk(ALTIA_BINDEX index, ALTIA_UINT32 bytes)
{
    ALTIA_UBYTE * address = (ALTIA_UBYTE * )altiaImageAddressChunk(index, bytes);

    /* Get the address of 0 with size 0.  For drivers that are loading assets in
     * chunks this will "commit" the request.
     */
    (void)altiaImageAddressChunk((ALTIA_BINDEX)0, (ALTIA_UINT32)0);

    return address;
}

ALTIA_CONST ALTIA_UBYTE * altiaImagePatternAddressChunk(ALTIA_BINDEX index, ALTIA_UINT32 bytes)
{
    ALTIA_UBYTE * address = (ALTIA_UBYTE * )altiaImageAddressChunk(index, bytes);

    /* Get the address of 0 with size 0.  For drivers that are loading assets in
     * chunks this will "commit" the request.
     */
    (void)altiaImageAddressChunk((ALTIA_BINDEX)0, (ALTIA_UINT32)0);

    return address;
}
#endif /* ALTIA_DRV_LOADASSET_CHUNKS */

ALTIA_UINT32 altiaImageMode(void)
{
    ALTIA_UINT32 mode = 0x00000000;

#if IMAGE_USE_EXTERNAL_FILES
    mode |= ALTIA_IMAGE_STORAGE_EXT_FILES;
#endif
#if IMAGE_USE_COMPRESSION
    mode |= ALTIA_IMAGE_STORAGE_COMPRESSED;
#endif
#if IMAGE_USE_ALPHA_FONTS
    mode |= ALTIA_IMAGE_STORAGE_ALPHA_FONTS;
#endif
#if IMAGE_USE_FILE_SYSTEM
    mode |= ALTIA_IMAGE_STORAGE_FILE_SYSTEM;
#endif
#if IMAGE_USE_DYNAMIC_MEMORY
    mode |= ALTIA_IMAGE_STORAGE_DYNAMIC_MEM;
#endif
#if IMAGE_USE_CUSTOM_LOAD
    mode |= ALTIA_IMAGE_STORAGE_CUSTOM_LOAD;
#endif
#if IMAGE_USE_DDB
    mode |= ALTIA_IMAGE_STORAGE_DDB;
#endif

    return mode;
}

ALTIA_CONST Altia_ImageFormat_type * altiaImageFormat(ALTIA_USHORT type)
{
#if IMAGE_USE_DDB
    ALTIA_USHORT format = type & 0x000f;
    if (ALTIA_RASTER_CUSTOM_MDDB == format)
    {
        return &altiaImageFormatMDDB;
    }
    else if (ALTIA_RASTER_CUSTOM_DDB == format)
    {
        return &altiaImageFormatDDB;
    }
    else if (ALTIA_RASTER_CUSTOM_TDDB == format)
    {
        return &altiaImageFormatTDDB;
    }
#endif
    return NULL;
}
