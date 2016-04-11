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

#ifndef ALTIA_IMAGE_ACCESSOR_H
#define ALTIA_IMAGE_ACCESSOR_H

#include "altiaBase.h"
#include "altiaTypes.h"

/******************************************************************************
 * Definitions
 *****************************************************************************/

#define ALTIA_IMAGE_STORAGE_DDB         0x00000001
#define ALTIA_IMAGE_STORAGE_EXT_FILES   0x00000002
#define ALTIA_IMAGE_STORAGE_COMPRESSED  0x00000004
#define ALTIA_IMAGE_STORAGE_CUSTOM      0x00000008
#define ALTIA_IMAGE_STORAGE_ALPHA_FONTS 0x00000010
#define ALTIA_IMAGE_STORAGE_FILE_SYSTEM 0x00000020
#define ALTIA_IMAGE_STORAGE_DYNAMIC_MEM 0x00000040
#define ALTIA_IMAGE_STORAGE_CUSTOM_LOAD 0x00000080


/******************************************************************************
 * Types
 *****************************************************************************/

typedef struct
{
    ALTIA_UBYTE depth;
    ALTIA_UBYTE shift;
} Altia_ImageChannel_type;

typedef struct
{
    ALTIA_USHORT            alignment;
    ALTIA_BOOLEAN           isBigEndian;
    ALTIA_UBYTE             depth;
    Altia_ImageChannel_type red;
    Altia_ImageChannel_type green;
    Altia_ImageChannel_type blue;
    Altia_ImageChannel_type alpha;
} Altia_ImageFormat_type;


/******************************************************************************
 * Public API
 *****************************************************************************/

extern ALTIA_BOOLEAN altiaImageInitialize(void);
extern ALTIA_BOOLEAN altiaImageLoadComplete(void);
extern ALTIA_BOOLEAN altiaImageTerminate(void);
extern ALTIA_CONST ALTIA_UBYTE * altiaImageAddress(ALTIA_BINDEX index);
extern ALTIA_CONST ALTIA_UBYTE * altiaImageFontAddress(ALTIA_FBINDEX index);
extern ALTIA_CONST ALTIA_UBYTE * altiaImageStencilAddress(ALTIA_BINDEX index);
extern ALTIA_CONST ALTIA_UBYTE * altiaImagePatternAddress(ALTIA_BINDEX index);

#ifdef ALTIA_DRV_LOADASSET_CHUNKS
extern ALTIA_CONST ALTIA_UBYTE * altiaImageAddressChunk(ALTIA_BINDEX index, ALTIA_UINT32 bytes);
extern ALTIA_CONST ALTIA_UBYTE * altiaImageFontAddressChunk(ALTIA_FBINDEX index, ALTIA_UINT32 bytes);
extern ALTIA_CONST ALTIA_UBYTE * altiaImageStencilAddressChunk(ALTIA_BINDEX index, ALTIA_UINT32 bytes);
extern ALTIA_CONST ALTIA_UBYTE * altiaImagePatternAddressChunk(ALTIA_BINDEX index, ALTIA_UINT32 bytes);
#endif /* ALTIA_DRV_LOADASSET_CHUNKS */

extern ALTIA_UINT32 altiaImageMode(void);
extern ALTIA_CONST Altia_ImageFormat_type * altiaImageFormat(ALTIA_USHORT type);


#endif /* ALTIA_IMAGE_ACCESSOR_H */
