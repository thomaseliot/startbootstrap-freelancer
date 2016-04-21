/* $Revision: 1.4 $    $Date: 2004-06-02 23:59:09 $
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

/** FILE:  altiaLibDraw.h ***************************************************
 **
 ** This file contains some declarations that may be unique to the
 ** implementation of Altia code for this target and are used throughout
 ** the target specific portion of the Altia code, but they usually
 ** don't depend on or reference any target specific data types,
 ** macros, etc.
 ***************************************************************************/


/* Altia's definition of white is red = 0xff, blue = 0xff,
 * and green = 0xff packed into 24-bits (the low order 24-bits
 * if it is being assigned to a 32-bit or larger data type).
 * Altia's definition of black is red = 0x00, blue = 0x00,
 * green = 0x00 also packed into 24-bits.
 */
#ifndef ALTIA_WHITE
#define ALTIA_WHITE 0x0ffffffU
#endif
#ifndef ALTIA_BLACK
#define ALTIA_BLACK 0x0U
#endif


/* The principal types of Altia fill patterns are solid (uses the
 * current foreground color), clear (uses the current background
 * color and the object can have an outline in the current foreground
 * color), or a bitmap (defined by a bit (0/1) pattern where the
 * current foreground and background colors are both used).
 */
#define SOLID_PATTERN 0
#define CLEAR_PATTERN 1
#define BITMAP_PATTERN 2


/* Some Altia constants used for drawing ellipses. */
#define AXIS ALTIA_C2F(0.42f)
#define SEEN ALTIA_C2F(1.025f)


/*--- altiaLibGetFullWindowExtent() ---------------------------------------
 *
 * Function called from generic and target specific Altia code to get the
 * full window extent.
 *
 * If the target does not support windowing, then the extent
 * containing Altia objects (window extent) may be much smaller than
 * the total drawing area (full window extent).  In this case, the
 * Altia code can avoid any drawing into the other regions of the
 * display which leaves them available for other applications to use.
 *
 * If the target supports windowing or code was generated for the full
 * screen, then the extent containing Altia objects (window extent) is
 * the same as the total drawing area (full window extent).
 *
 *-------------------------------------------------------------------------*/
extern void altiaLibGetFullWindowExtent(
    ALTIA_WINDOW win,
    Altia_Extent_type *extent
);

extern void altiaLibSetFullWindowExtent(
    ALTIA_WINDOW win,
    Altia_Extent_type *extent
);

extern void altiaLibGetWindowExtent(
    ALTIA_WINDOW win,
    Altia_Extent_type *extent
);

extern void altiaLibSetWindowExtent(
    ALTIA_WINDOW win,
    Altia_Extent_type * extent
);

extern void altiaLibSetBufferOverride(
    ALTIA_BOOLEAN override_flag
);

extern ALTIA_BOOLEAN altiaLibGetBufferOverride(
    void
);


