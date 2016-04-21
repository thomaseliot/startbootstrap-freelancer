/* $Revision: 1.4 $    $Date: 2004-06-02 17:43:59 $
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



#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"

/*----------------------------------------------------------------------*/
void altiaLibStringExtent(Altia_Font_type *font, ALTIA_CHAR *str,
                          int len, Altia_Transform_type *trans,
                          Altia_Extent_type *extent)
{
    Altia_Extent_type temp;
    int width, height, offset;

    altiaLibFontExtent(font, str, len, &width, &height, &offset);
    temp.x0 = (ALTIA_COORD)offset;
    temp.x1 = (ALTIA_COORD)(width + offset);
    temp.y0 = (ALTIA_COORD)0;
    temp.y1 = (ALTIA_COORD)height;
    altiaLibTransformExtent(trans, &temp, extent);
}


