/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* $Revision: 1.5 $    $Date: 2009-05-06 00:03:00 $
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
#include "egl_Wrapper.h"
#include "Xprotostr.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "mi.h"

extern GCPtr   lastClipGc;
extern EGL_POS lastClipLeft;
extern EGL_POS lastClipTop;
extern EGL_POS lastClipRight;
extern EGL_POS lastClipBottom;
/*****************************************************************
 * Clip region management functions required for rendering
 * scaled/rotated Rasters (color bitmaps), Stencils (monochrome
 * bitmaps), and text.  If these features are not supported in
 * the target, just leave these functions empty.
 *****************************************************************/

#define ADDRECT(reg,r,fr,rx1,ry1,rx2,ry2)			\
if (((rx1) < (rx2)) && ((ry1) < (ry2)) &&			\
    (!((reg)->data->numRects &&					\
       ((r-1)->y1 == (ry1)) &&					\
       ((r-1)->y2 == (ry2)) &&					\
       ((r-1)->x1 <= (rx1)) &&					\
       ((r-1)->x2 >= (rx2)))))					\
{								\
    if ((reg)->data->numRects == (reg)->data->size)		\
    {								\
	miRectAlloc(reg, 1);					\
	fr = REGION_BOXPTR(reg);				\
	r = fr + (reg)->data->numRects;				\
    }								\
    r->x1 = (rx1);						\
    r->y1 = (ry1);						\
    r->x2 = (rx2);						\
    r->y2 = (ry2);						\
    (reg)->data->numRects++;					\
    if(r->x1 < (reg)->extents.x1)				\
	(reg)->extents.x1 = r->x1;				\
    if(r->x2 > (reg)->extents.x2)				\
	(reg)->extents.x2 = r->x2;				\
    r++;							\
}

/***************************************************************************/
EGL_STATUS egl_ClipRegionGet (EGL_GC_ID gc,
                              EGL_REGION_ID *pClipRegionId)
{
    *pClipRegionId = ((GCPtr)gc)->clientClip;
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_ClipRegionSet (EGL_GC_ID gc, EGL_REGION_ID clipRegionId)
{
    lastClipLeft = -1234;
    lastClipTop = -1234;
    lastClipRight = -1234;
    lastClipBottom = -1234;
    (*((GCPtr)gc)->funcs->ChangeClip)((GCPtr)gc, CT_REGION, clipRegionId, 0);
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_RegionClipToRect (EGL_REGION_ID regionId,
                                 const EGL_RECT * pClipRect)
{
    BoxRec box;
    RegionRec region;
    box.x1 = pClipRect->left;
    box.y1 = pClipRect->top;
    box.x2 = pClipRect->right+1;
    box.y2 = pClipRect->bottom+1;
    miRegionInit(&region, &box, 1);
    miIntersect(regionId, regionId, &region);
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_REGION_ID egl_RegionCreate (void)
{

    RegionPtr region =  miRegionCreate(NULL, 64);
    region->extents.x1 = 16384;
    region->extents.x2 = 0;
    return (EGL_REGION_ID) region;
}

/***************************************************************************/
EGL_STATUS egl_RegionDestroy (EGL_REGION_ID regionId)
{
    miRegionDestroy(regionId);
    return EGL_STATUS_OK;
}

/***************************************************************************/
EGL_STATUS egl_RegionRectInclude (EGL_REGION_ID regionId,
                                  const EGL_RECT * rect)
{
    BoxPtr  firstRect, rects;

    /* Lets first see if this rect has the same y as the last rectangle.
     * If it does then just adjust the x values and don't add a 
     * new rectangle.  This is reduces the number of clip rectangles and
     * also keeps the frame buffer code from drawing things twice which
     * is bad if we are doing an xor.
     */
    if (REGION_NUM_RECTS((RegionPtr)regionId) > 1)
    {
        BoxPtr lastRect = REGION_END((RegionPtr)regionId);
	if (lastRect->y1 == rect->top && (lastRect->y2 == (rect->bottom+1)))
	{ /* This rect has same y1 and y2 as last rect we added */
	    lastRect->x1 = min(lastRect->x1, rect->left);
	    lastRect->x2 = max(lastRect->x2, rect->right);
	    if (lastRect->x1 < ((RegionPtr)regionId)->extents.x1)
		((RegionPtr)regionId)->extents.x1 = lastRect->x1;
	    if (lastRect->x2 > ((RegionPtr)regionId)->extents.x2)
		((RegionPtr)regionId)->extents.x2 = lastRect->x2;
	    return EGL_STATUS_OK;
	}
    }
    firstRect = REGION_BOXPTR((RegionPtr)regionId);
    rects = REGION_TOP((RegionPtr)regionId);
    ADDRECT((RegionPtr)regionId, rects, firstRect, rect->left, rect->top,
		rect->right+1, rect->bottom+1);
    ((RegionPtr)regionId)->extents.y1 = 
	                   REGION_BOXPTR((RegionPtr)regionId)->y1;
    ((RegionPtr)regionId)->extents.y2 = 
	                   REGION_END((RegionPtr)regionId)->y2;

    return EGL_STATUS_OK;
}
