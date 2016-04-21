/* $Revision: 1.8 $    $Date: 2009-05-06 00:04:10 $
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

/** FILE:  altiaLibPattern.c ***********************************************
 **
 ** This file contains the routines to create patterns in case something
 ** special needs to be done on this target to create patterns.  Typically
 ** the solid and clear patterns are available with little or no special
 ** effort for almost every graphics library.
 ** If no objects in a design use any patterns (this would imply there are
 ** no closed vector objects in the design, just lines, splines, text,
 ** rasters, stencils, etc.), this file contains no code.
 ***************************************************************************/



#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "../altiaImageAccess.h"
#include "altiaLibDraw.h"
#include "egl_Wrapper.h"

extern AltiaDevInfo *altiaLibGetDevInfo(void);

/*--- altiaLibLoadPattern() -----------------------------------------------
 *
 * Function called by other Altia code to load the given pattern.
 * If the pattern is solid or clear, it can be set on the fly and
 * there is nothing to create ahead of time.  Otherwise, create
 * the necessary bitmap for the pattern and set the pattern structure's
 * id to the bitmap so the bitmap can be used as needed to draw filled
 * objects with the pattern.
 *
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibLoadPattern(Altia_Pattern_type *pat)
{
    if (CLEAR_PATTERN == pat->patType || SOLID_PATTERN == pat->patType)
    {
        /* Nothing to do for this pattern */
        pat->id = 0;
    }
    else
    {
        ALTIA_CONST ALTIA_UBYTE * address;

        /* The pattern is stored in the image data */
#ifndef ALTIA_DRV_LOADASSET_CHUNKS
        address = altiaImagePatternAddress(pat->index);
#else
        address = altiaImagePatternAddressChunk(pat->index, 8);
#endif /* ALTIA_DRV_LOADASSET_CHUNKS */

        if (NULL == address)
        {
            pat->patType = CLEAR_PATTERN;
            pat->id = 0;
        }
        else
        {
            EGL_MDIB monoDIB;
            EGL_MDDB_ID monoDDB;

            monoDIB.pImage = (void *)address;

            /* Altia Patterns are always 8x8 */
            monoDIB.width = 8;
            monoDIB.height = 8;

            /* The stride defines how many bits make up 1 row of image data.
            ** Our rows are aligned on byte boundaries so the answer is 8.
            */
            monoDIB.stride = 8;

            /* Create the pattern DDB */
            if ((monoDDB = egl_MonoBitmapCreate((altiaLibGetDevInfo())->devId,
                &monoDIB, EGL_DIB_INIT_DATA, 1,
                NULL)) == NULL)
            {
                pat->patType = CLEAR_PATTERN;
                pat->id = 0;
                return;
            }

            pat->patType = BITMAP_PATTERN;
            pat->id = (ALTIA_UINT32)monoDDB;
        }
    }
}

void altiaLibUnloadPattern(Altia_Pattern_type *pat)
{
    if (pat->id != 0)
    {
        egl_MonoBitmapDestroy((altiaLibGetDevInfo())->devId,
                              (EGL_MDDB_ID)pat->id);
        pat->id = 0;
    }
}

