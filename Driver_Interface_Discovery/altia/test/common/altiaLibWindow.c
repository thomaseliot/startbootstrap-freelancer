/* $Revision: 1.6 $    $Date: 2009-05-06 00:04:10 $
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

/** FILE:  altiaLibWindow.c *************************************************
 **
 ** This file contains routines that any Altia code can call to get
 ** the complete extent (i.e., width and height) of the drawing area.
 ** One function gives the extent of the region containing Altia objects
 ** (referred to as the window extent).  Another function provides the
 ** total extent of the available drawing surface (referred to as the
 ** full window extent).
 **
 ** If the target does not support windowing, then the extent
 ** containing Altia objects (window extent) may be much smaller than
 ** the total drawing area (full window extent).  In this case, the
 ** Altia code can avoid any drawing into the other regions of the
 ** display which leaves them available for other applications to use.
 **
 ** If the target supports windowing or code was generated for the full
 ** screen, then the extent containing Altia objects (window extent) is
 ** the same as the total drawing area (full window extent).
 ***************************************************************************/

#include "../altiaBase.h"
#include "../altiaTypes.h"
#include "../altiaExtern.h"
#include "altiaLibDraw.h"
#include "egl_Wrapper.h"



static Altia_Extent_type windowExtent = { 0, 0, 0, 0 };
static Altia_Extent_type fullExtent = { 0, 0, 0, 0 };

static ALTIA_BOOLEAN _bufferOverride = ALTIA_FALSE;

/*--- altiaLibSetBufferOverride() -------------------------------------------
 *
 * Function called from Altia Engine code during execution to override the
 * current draw buffer.
 *
 * Used when the draw destination is no longer the frame buffer for the
 * display.
 *-------------------------------------------------------------------------*/
void altiaLibSetBufferOverride(ALTIA_BOOLEAN override_flag)
{
    if (override_flag != ALTIA_FALSE)
    {
        _bufferOverride = ALTIA_TRUE;
    }
    else
    {
        _bufferOverride = ALTIA_FALSE;
    }
}


/*--- altiaLibGetBufferOverride() -------------------------------------------
 *
 * Function called from Altia Engine code during execution to check the
 * buffer override status
 *
 * Used when the draw destination is no longer the frame buffer for the
 * display.
 *-------------------------------------------------------------------------*/
ALTIA_BOOLEAN altiaLibGetBufferOverride(void)
{
    return _bufferOverride;
}


/*--- altiaLibSetWindowExtent() -------------------------------------------
 *
 * Function called from target specific Altia code during initialization
 * to set the window extent.
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
 *-------------------------------------------------------------------------*/
void altiaLibSetWindowExtent(ALTIA_WINDOW win, Altia_Extent_type * extent)
{
    windowExtent.x0 = extent->x0;
    windowExtent.y0 = extent->y0;
    windowExtent.x1 = extent->x1;
    windowExtent.y1 = extent->y1;
}


/*--- altiaLibGetWindowExtent() -------------------------------------------
 *
 * Function called from generic and target specific Altia code to get the
 * window extent.
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
 * WARNING:  The name for this function cannot change because this
 *           function is called from generic Altia code.
 *-------------------------------------------------------------------------*/
void altiaLibGetWindowExtent(ALTIA_WINDOW win, Altia_Extent_type *extent)
{


    extent->x0 = windowExtent.x0;
    extent->y0 = windowExtent.y0;
    extent->x1 = windowExtent.x1;
    extent->y1 = windowExtent.y1;
}


/*--- altiaLibSetFullWindowExtent() ---------------------------------------
 *
 * Function called from target specific Altia code during initialization
 * to set the full window extent.
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
void altiaLibSetFullWindowExtent(ALTIA_WINDOW win, Altia_Extent_type *extent)
{
    fullExtent.x0 = extent->x0;
    fullExtent.y0 = extent->y0;
    fullExtent.x1 = extent->x1;
    fullExtent.y1 = extent->y1;
}


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
 * WARNING:  The name for this function should not change because this
 *           function may get called from generic Altia code.  At the
 *           time of this writing, it is not called from generic Altia
 *           code, but this could change in the future.
 *-------------------------------------------------------------------------*/
void altiaLibGetFullWindowExtent(ALTIA_WINDOW win, Altia_Extent_type *extent)
{


    extent->x0 = fullExtent.x0;
    extent->y0 = fullExtent.y0;
    extent->x1 = fullExtent.x1;
    extent->y1 = fullExtent.y1;
}
