/* $Revision: 1.5 $    $Date: 2008-08-05 15:47:37 $
 * Copyright (c) 2001-2003 Altia Inc.
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

#ifndef DRAWEX_H
#define DRAWEX_H
/******************************************************************
 * Determine if function prototyping is enabled.
 ******************************************************************/
#ifndef Altiafp
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus) || defined(_MSC_VER)
#define Altiafp
#else
#undef Altiafp
#endif /* Check for function prototyping requirement */
#endif /* Altiafp */

/* If compiling for Unicode and the target does not have its
 * own wide character string manipulation functions (such as
 * with VxWorks), we can reference our own extern declarations
 * for these functions.  The target .gen should then include
 * a line to copy wCharStrings.c and compiling should be done
 * with the additional flags:  -DUNICODE -DALTIA_WCHAR_SUPPORT
 */
#if defined(UNICODE) && defined(ALTIA_WCHAR_SUPPORT)
#include "wCharStrings.h"
#endif

/*
 * Draw functions
 */

extern int altiaExEllipseDraw(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Ellipse_type *ellipse,
    ALTIA_BYTE fill,
    AltiaEx_Transform_type *trans
);

typedef int (*altiaExEllipseDrawFunc)(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Ellipse_type *ellipse,
    ALTIA_BYTE fill,
    AltiaEx_Transform_type *trans
);

extern int altiaExRectDraw(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Rect_type *rect,
    ALTIA_BYTE fill,
    AltiaEx_Transform_type *trans
);

typedef int (*altiaExRectDrawFunc)(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Rect_type *rect,
    ALTIA_BYTE fill,
    AltiaEx_Transform_type *trans
);

extern int altiaExLineDraw(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Coord_type *coords,
    ALTIA_SHORT count,
    AltiaEx_Transform_type *trans
);

typedef int (*altiaExLineDrawFunc)(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Coord_type *coords,
    ALTIA_SHORT count,
    AltiaEx_Transform_type *trans
);

extern int altiaExArcDraw(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Coord_type *center,
    ALTIA_SHORT r1,
    ALTIA_SHORT r2,
    ALTIA_DOUBLE angleStart,
    ALTIA_DOUBLE angleEnd,
    ALTIA_BYTE fill,
    AltiaEx_Transform_type *trans
);

extern int altiaExArcDraw2(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Coord_type *center,
    ALTIA_SHORT r1,
    ALTIA_SHORT r2,
    ALTIA_DOUBLE angleStart,
    ALTIA_DOUBLE angleEnd,
    ALTIA_BYTE fill,
    AltiaEx_Transform_type *trans,
    ALTIA_FLOAT angleInc
);

typedef int (*altiaExArcDrawFunc)(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Coord_type *center,
    ALTIA_SHORT r1,
    ALTIA_SHORT r2,
    ALTIA_DOUBLE angleStart,
    ALTIA_DOUBLE angleEnd,
    ALTIA_BYTE fill,
    AltiaEx_Transform_type *trans
);

extern int altiaExPolygonDraw(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Coord_type *coords,
    ALTIA_SHORT count,
    ALTIA_BYTE fill,
    AltiaEx_Transform_type *trans
);

typedef int (*altiaExPolygonDrawFunc)(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Coord_type *coords,
    ALTIA_SHORT count,
    ALTIA_BYTE fill,
    AltiaEx_Transform_type *trans
);

extern int altiaExLabelDraw(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Coord_type *loc,
    AltiaEx_Label_type *label,
    AltiaEx_Transform_type *trans
);

typedef int (*altiaExLabelDrawFunc)(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    AltiaEx_Coord_type *loc,
    AltiaEx_Label_type *label,
    AltiaEx_Transform_type *trans
);

extern ALTIA_UINT32 altiaExCreateRaster(
    ALTIA_INT width,
    ALTIA_INT height,
    ALTIA_INT bitsPerPixel,
    ALTIA_INT bytesPerScan,
    ALTIA_UINT32 *ctable,
    ALTIA_INT ctableSize,
    ALTIA_UINT32 rmask,
    ALTIA_UINT32 gmask,
    ALTIA_UINT32 bmask,
    ALTIA_UINT32 amask,
    ALTIA_UBYTE *data,
    ALTIA_UBYTE *transmask
);

typedef ALTIA_UINT32 (*altiaExCreateRasterFunc)(
    ALTIA_INT width,
    ALTIA_INT height,
    ALTIA_INT bitsPerPixel,
    ALTIA_INT bytesPerScan,
    ALTIA_UINT32 *ctable,
    ALTIA_INT ctableSize,
    ALTIA_UINT32 rmask,
    ALTIA_UINT32 gmask,
    ALTIA_UINT32 bmask,
    ALTIA_UINT32 amask,
    ALTIA_UBYTE *data,
    ALTIA_UBYTE *transmask
);

extern int altiaExRasterDraw(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    ALTIA_INT x,
    ALTIA_INT y,
    ALTIA_UINT32 ras,
    AltiaEx_Transform_type *trans
);

typedef int (*altiaExRasterDrawFunc)(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    ALTIA_INT x,
    ALTIA_INT y,
    ALTIA_UINT32 ras,
    AltiaEx_Transform_type *trans
);

extern int altiaExDeleteRaster(
    ALTIA_UINT32 rasPtr
);

typedef int (*altiaExDeleteRasterFunc)(
    ALTIA_UINT32 rasPtr
);

extern ALTIA_UINT32 altiaExGetRasterBitmap(
    ALTIA_UINT32 rasPtr
);

typedef ALTIA_UINT32 (*altiaExGetRasterBitmapFunc)(
    ALTIA_UINT32 rasPtr
);

/****************************************************************************/
/* RTC-1597:
 * CUSTOM TARGET FEATURE!  This next function is custom for targets that
 * must draw a Stencil into a DAO.  THIS IS NON-STANDARD!  There are no
 * altiaExCreateStencil() and altiaExDeleteStencil() functions for initially
 * creating the Stencil or deleting it.  Custom target code must manage the
 * creation, initialization, and deletion of the stencil.  This function
 * only provides an interface to drawing the Stencil into a DAO.
 *
 * The design must contain at least one Stencil object otherwise this function
 * is not generated.  The code generator will not generate any code for
 * Stencil support unless the design contains at least one Stencil!
 *
 * Altia Design and Runtime versions of the DAO do NOT support this function!
 */
extern int altiaExStencilDraw(
    ALTIA_UINT32 objId,
    ALTIA_WINDOW rwin,
    ALTIA_UINT32 dc,
    AltiaEx_GraphicState_type *gs,
    ALTIA_INT x,
    ALTIA_INT y,
    ALTIA_UINT32 sten,
    ALTIA_UINT32 stenID,
    AltiaEx_Transform_type *trans
);

extern int altiaExRepostEvent(
    ALTIA_UINT32 objId,
    ALTIA_UINT32 rwin,
    AltiaEx_InputEvent_type*event
);

typedef int (*altiaExRepostEventFunc)(
    ALTIA_UINT32 objId,
    ALTIA_UINT32 rwin,
    AltiaEx_InputEvent_type*event
);

extern int altiaExGetDrawView(
    ALTIA_UINT32 objId,
    ALTIA_UINT32 *win,
    ALTIA_INT *x,
    ALTIA_INT *y
);

typedef int (*altiaExGetDrawViewFunc)(
    ALTIA_UINT32 objId,
    ALTIA_UINT32 *win,
    ALTIA_INT *x,
    ALTIA_INT *y
);

/* RTC-806 */
extern void altiaExSetRasterTexture(
    ALTIA_UINT32 rasPtr,
    ALTIA_UINT32 texId
);

extern ALTIA_UINT32 altiaExGetRasterTexture(
    ALTIA_UINT32 rasPtr
);

typedef struct
{
    altiaExEllipseDrawFunc altiaExEllipseDraw;
    altiaExRectDrawFunc altiaExRectDraw;
    altiaExLineDrawFunc altiaExLineDraw;
    altiaExArcDrawFunc altiaExArcDraw;
    altiaExPolygonDrawFunc altiaExPolygonDraw;
    altiaExLabelDrawFunc altiaExLabelDraw;
    altiaExCreateRasterFunc altiaExCreateRaster;
    altiaExDeleteRasterFunc altiaExDeleteRaster;
    altiaExRasterDrawFunc altiaExRasterDraw;
    altiaExRepostEventFunc altiaExRepostEvent;
    altiaExGetRasterBitmapFunc  altiaExGetRasterBitmap;
    altiaExGetDrawViewFunc  altiaExGetDrawView;
    void*  unused4;
    void*  unused5;
    void*  unused6;
}AltiaDrawFuncs_t;

#endif /* DRAWEX_H */
