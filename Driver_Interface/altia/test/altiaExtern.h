/* $Revision: 1.61 $    $Date: 2010-04-12 21:59:42 $
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

#ifndef ALTIAEXTERN_H
#define ALTIAEXTERN_H

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

#ifdef ALTIAGL
/* For embedded target types like EGL_POS, include the
 * embedded target header file.
 */
#include "egl_Wrapper.h"
#endif /* ALTIAGL */

#include "altiaDataPtr.h"

extern ALTIA_CONST ALTIA_INT test_window_width;
extern ALTIA_CONST ALTIA_INT test_window_height;
extern ALTIA_CONST ALTIA_INT test_window_xoffset;
extern ALTIA_CONST ALTIA_INT test_window_yoffset;
extern ALTIA_COLOR test_window_background;

#ifdef ALTIA_TASKING
extern ALTIA_CONST ALTIA_INT altiaTaskQueue_count;
extern AltiaTaskElementType altiaTaskQueue[];

extern ALTIA_CONST ALTIA_INT altiaObjectQueue_count;
extern AltiaObjectElementType altiaObjectQueue[];
#endif /* ALTIA_TASKING */

extern ALTIA_COLOR _altiaGetBackgroundColor(
    void
);

extern void _altiaGetWindowSize(
    ALTIA_INT *width,
    ALTIA_INT *height
);

extern void _altiaSetClipCheck(ALTIA_BOOLEAN checkClip);
extern ALTIA_BOOLEAN _altiaGetClipCheck(void);

extern ALTIA_WINDOW altiaGetMainWin(void);

extern ALTIA_CONST ALTIA_INT test_layers_count;







extern ALTIA_CONST ALTIA_INT test_dobjs_count;

extern ALTIA_INDEX test_dobjsCurFunc[6];
extern AltiaEventType test_dobjsCurValue[];
extern Altia_DynamicObjectRAM_type test_dobjsRAM[];
extern ALTIA_CONST Altia_DynamicObjectROM_type test_dobjsROM[];

extern Altia_DynamicState_type test_dstates[];
extern ALTIA_CONST Altia_DynamicStateC_type test_dstatesROM[];






extern ALTIA_CONST ALTIA_INT test_sobjs_count;

extern Altia_StaticObjectRAM_type test_sobjsRAM[];
extern ALTIA_CONST Altia_StaticObjectROM_type test_sobjsROM[];





extern ALTIA_CONST ALTIA_INT test_states_count;

extern ALTIA_CONST Altia_StateEntryROM_type test_states[];





extern ALTIA_CONST ALTIA_INT test_basetrans_count;
extern ALTIA_CONST Altia_BaseTrans_type test_basetrans[];



extern ALTIA_CONST ALTIA_INT test_offtrans_count;
extern ALTIA_CONST Altia_OffsetTrans_type test_offtrans[];





extern ALTIA_CONST ALTIA_INT test_funcs_count;

extern ALTIA_CONST Altia_FunctionNameROM_type test_funcs[];





extern ALTIA_CONST ALTIA_INT test_funcIndex_count;
extern ALTIA_CONST Altia_FunctionIndex_type test_funcIndex[];
extern AltiaEventType test_curValue[];



extern ALTIA_CONST ALTIA_INT test_funcFIndexs_count;

extern ALTIA_CONST Altia_FunctionFIndexROM_type test_funcFIndexs[];





extern ALTIA_CONST ALTIA_INT test_children_count;



extern ALTIA_CONST Altia_ChildEntryROM_type test_children[];






extern ALTIA_CONST ALTIA_INT test_sequence_count;



extern ALTIA_CONST Altia_SequenceROM_type test_sequence[];










extern ALTIA_INT test_patterns_count;
extern Altia_Pattern_type test_patterns[];



extern ALTIA_CONST ALTIA_INT test_brushes_count;
extern Altia_Brush_type test_brushes[];



extern ALTIA_CONST ALTIA_INT test_fonts_count;
extern Altia_Font_type test_fonts[];







extern ALTIA_CONST ALTIA_INT test_rects_count;
extern ALTIA_CONST Altia_Rect_type test_rects[];






extern ALTIA_CONST ALTIA_INT test_altiaDrawPtCount;
extern EGL_POS test_altiaDrawPts[];










extern ALTIA_CONST ALTIA_INT test_labels_count;
extern Altia_Label_type test_labels[];





#if !defined(ALTIA_DIB_MALLOC) || (0 == ALTIA_DIB_MALLOC)
/*
 * Extern declarations for one or more temp arrays used to build device
 * independent bitmaps (DIBs) when memory allocation is NOT allowed.
 */
extern ALTIA_CONST ALTIA_INT test_altiaMaxBitmap;
extern ALTIA_UBYTE test_altiaBitmapData[];

extern ALTIA_CONST ALTIA_INT test_altiaMaxBitmap2;
extern ALTIA_UBYTE test_altiaBitmapData2[];

#endif /* !ALTIA_DIB_MALLOC */




extern ALTIA_CONST ALTIA_INT test_scaled_rasters_count;
extern Altia_Scaled_Raster_type test_scaledRaster[];
extern Altia_Scaled_Save_Bits_type test_scaledSaveBits[];




extern ALTIA_CONST ALTIA_INT test_rasters_count;





extern Altia_RasterID_type test_rasterid[];
extern ALTIA_CONST Altia_RasterROM_type test_rastersROM[];








extern ALTIA_CONST ALTIA_INT test_decks_count;

extern ALTIA_CONST Altia_DeckROM_type test_decks[];
























extern ALTIA_INT test_extents_count;
extern ALTIA_CONST ALTIA_INT test_extents_max;
extern Altia_Extent_type test_extents[];
extern AltiaDevInfo test_layerDevInfo[];



extern ALTIA_CONST ALTIA_INT test_custI_count;

extern Altia_CustomIValueRAM_type test_custIRAM[];
extern ALTIA_CONST Altia_CustomIValueROM_type test_custIROM[];





extern ALTIA_CONST ALTIA_INT test_custF_count;

extern Altia_CustomFValueRAM_type test_custFRAM[];
extern ALTIA_CONST Altia_CustomFValueROM_type test_custFROM[];





extern ALTIA_CONST ALTIA_INT test_custS_count;
extern Altia_CustomSValue_type test_custS[];







extern ALTIA_CONST ALTIA_INT test_textios_count;
extern Altia_TextioBSS_type test_textiosBSS[];

extern Altia_TextioRAM_type test_textiosRAM[];
extern ALTIA_CONST Altia_TextioROM_type test_textiosROM[];



extern ALTIA_CONST ALTIA_INT test_textioInputsBSS_count;
extern Altia_TextioInputBSS_type test_textioInputsBSS[];
extern Altia_TextioInput_type test_textioInputs[];






















extern ALTIA_CONST ALTIA_INT test_timerState_count;
extern ALTIA_CONST Altia_TimerState_type test_timerState[];



extern ALTIA_CONST ALTIA_INT test_localTimers_count;
extern AltiaTimerTime_t test_localTimers[];



extern ALTIA_CONST ALTIA_INT test_timers_count;

extern Altia_TimerRAM_type test_timersRAM[];
extern ALTIA_CONST Altia_TimerROM_type test_timersROM[];












































/*
 * General purpose functions
 */

extern ALTIA_BOOLEAN _altiaInit(
    ALTIA_WINDOW win
);

extern void _altiaClose(
    void
);

extern AltiaReportFuncType AltiaReportFuncPtr;

extern AltiaReportFuncType AltiaInvalidAnimationNamePtr;

/* altiaLibUtils.c */
extern ALTIA_SHORT altiaGetDrawType(
    ALTIA_INDEX objIdx,
    ALTIA_SHORT objType
);

extern void _altiaDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_SHORT objType,
    ALTIA_INDEX obj
);

extern void _altiaDrawExtent(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_Extent_type *extent
);

extern void AltiaUpdate(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    ALTIA_INT copy
);

/* 
 * Tasking functions 
 */
#ifdef ALTIA_TASKING
extern ALTIA_BOOLEAN AltiaPrepareBuild(
    ALTIA_WINDOW win,
    ALTIA_INT idx,
    Altia_Extent_type *extent
);

extern ALTIA_INT AltiaUpdateBuild(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_Extent_type *extent
);

extern void AltiaPrepareDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_Extent_type *extent
);

extern void AltiaUpdateDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    ALTIA_INT idx
);

extern void AltiaFinishDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_Extent_type *extent,
    ALTIA_INT copy
);
#endif /* ALTIA_TASKING */

/* 
 * Animation functions 
 */

extern ALTIA_BOOLEAN AltiaAnimate(
    ALTIA_CHAR *funcName,
    AltiaEventType value
);

extern ALTIA_BOOLEAN AltiaAnimateId(
    int nameId,
    AltiaEventType value
);

extern ALTIA_CHAR * AltiaAnimationIdToName(
    int nameId
);

extern int AltiaAnimationNameToId(
    ALTIA_CHAR *funcName
);

extern ALTIA_BOOLEAN AltiaAnimateText(
    ALTIA_CHAR *funcName,
    ALTIA_CHAR *value
);

extern ALTIA_INDEX _altiaFindAnimationIdx(
    ALTIA_CHAR *funcName
);

extern AltiaEventType _altiaFindCurVal(
    ALTIA_CHAR *funcName
);

extern ALTIA_CHAR *_altiaFindCurString(
    ALTIA_CHAR *funcName
);

extern ALTIA_INDEX _altiaGetCustomIIndex(
    ALTIA_UINT32 id,
    ALTIA_INDEX first,
    ALTIA_SHORT count
);

/* 
 * Clipboard and Shaping functions 
 */

extern ALTIA_BOOLEAN altiaShapeText(
    ALTIA_CHAR *text
);

extern ALTIA_CHAR *altiaLibGetTextFromClipBoard(
    void
);

extern void altiaLibCopyTextToClipBoard(
    ALTIA_CHAR *text
);

/*
 * Display and Layer functions 
 */

extern ALTIA_BOOLEAN altiaLibDisplayInit(
     ALTIA_WINDOW win
);

extern ALTIA_INDEX altiaDisplayIdx(
     ALTIA_INDEX layerIdx
);

ALTIA_SHORT altiaGetLayer(
     void
);

/*
 * Transform functions
 */


#define altiaLibEndTrans(t)
#define altiaLibOffEndTrans(t)
#define altiaLibInitTrans(t)
#define altiaLibCopyTrans(dest, src) (*(dest) = *(src))
#define altiaLibCopyOffTrans(dest, src) (*(dest) = *(src))






#define ALTIA_ROTATED(trans) ((trans)->a01 != 0 || (trans)->a10 != 0 \
     \
)

#define ALTIA_ROTATED90(trans) (ALTIA_ROTATED(trans) && (trans)->a00 == 0 && (trans)->a11 == 0)

#define ALTIA_STRETCHED(trans) (((trans)->a00-(trans)->a11) != 0)

#define ALTIA_SCALED(trans) ((trans)->a00 != ALT_I2FX(1) || (trans)->a11 != ALT_I2FX(1))





extern Altia_Transform_type *altiaGetTotalTrans(
    ALTIA_INDEX objIdx,
    ALTIA_SHORT objType
);

extern void _altiaBldTrans(
    ALTIA_INDEX,
    ALTIA_INDEX,
    Altia_Transform_type *
);

extern void _altiaPushTrans(
    ALTIA_SHORT objType,
    ALTIA_INDEX obj
);

extern void _altiaTotalTrans(
    ALTIA_SHORT objType,
    ALTIA_INDEX obj
);

extern void altiaLibTransformExtent(
    ALTIA_CONST Altia_Transform_type *trans,
    ALTIA_CONST Altia_Extent_type *olde,
    Altia_Extent_type *newe
);

extern void altiaLibPostMultiplyTrans(
    Altia_Transform_type *trans,
    ALTIA_CONST Altia_Transform_type *trans2
);

extern void altiaLibPreMultiplyTrans(
    Altia_Transform_type *trans,
    ALTIA_CONST Altia_Transform_type *trans2
);

extern void altiaLibPostMultiplyRel(
    Altia_Transform_type *trans,
    ALTIA_CONST Altia_Transform_type *trans2
);

extern void altiaLibDoRelTrans(
    Altia_Transform_type *current,
    ALTIA_CONST Altia_Transform_type *behave,
    ALTIA_CONST Altia_Transform_type *last,
    ALTIA_CONST Altia_Transform_type *group,
    ALTIA_SHORT preMult
);

extern void altiaLibTransformF(
    ALTIA_CONST Altia_Transform_type *trans,
    ALTIA_DOUBLE x,
    ALTIA_DOUBLE y,
    ALTIA_FLOAT *newx,
    ALTIA_FLOAT *newy
);

extern ALTIA_BOOLEAN altiaLibTransformed(
    ALTIA_CONST Altia_Transform_type *trans,
    ALTIA_INT x,
    ALTIA_INT y
);

extern void altiaLibTransform(
    ALTIA_CONST Altia_Transform_type *trans,
    ALTIA_INT x,
    ALTIA_INT y,
    ALTIA_COORD *newx,
    ALTIA_COORD *newy
);

extern void altiaLibTranslate(
    Altia_Transform_type *trans,
    ALTIA_DOUBLE x,
    ALTIA_DOUBLE y
);

extern void altiaLibRotate(
    Altia_Transform_type *trans,
    ALTIA_DOUBLE angle
);

extern void altiaLibScale(
    Altia_Transform_type *trans,
    ALTIA_DOUBLE sx,
    ALTIA_DOUBLE sy
);



extern void altiaLibInterpolate(
    Altia_Transform_type *trans,
    ALTIA_FIXPT percent,
    ALTIA_CONST Altia_Transform_type *trans2
);


extern ALTIA_BOOLEAN altiaLibTransformInvertable(
    ALTIA_CONST Altia_Transform_type *trans
);

extern void altiaLibInvertRel(
    Altia_Transform_type *trans
);

extern void altiaLibInvertTrans(
    Altia_Transform_type *trans
);

extern void altiaLibTransformDist(
    ALTIA_CONST Altia_Transform_type *trans,
    ALTIA_INT x,
    ALTIA_INT y,
    ALTIA_COORD *newx,
    ALTIA_COORD *newy
);

extern void altiaLibTransformDistF(
    ALTIA_CONST Altia_Transform_type *trans,
    ALTIA_FLOAT x,
    ALTIA_FLOAT y,
    ALTIA_FLOAT *newx,
    ALTIA_FLOAT *newy
);

extern void altiaLibInvTransformDistF(
    ALTIA_CONST Altia_Transform_type *trans,
    ALTIA_DOUBLE x,
    ALTIA_DOUBLE y,
    ALTIA_FLOAT *newx,
    ALTIA_FLOAT *newy
);

extern void altiaLibInvTransformF(
    ALTIA_CONST Altia_Transform_type *trans,
    ALTIA_DOUBLE x,
    ALTIA_DOUBLE y,
    ALTIA_FLOAT *newx,
    ALTIA_FLOAT *newy
);

extern void altiaLibSubExtent(
    Altia_Extent_type *e1,
    Altia_Extent_type *e2,
    Altia_Extent_type *extent
);

extern ALTIA_BOOLEAN altiaLibIntersect(
    Altia_Extent_type *e1,
    Altia_Extent_type *e2
);

extern void _altiaErrorMessage(
    ALTIA_CONST ALTIA_CHAR *msg
);

extern void altia_get_dest_bounds(
    Altia_GraphicState_type *gs,
    Altia_Transform_type    *trans,
    Altia_Extent_type       *wex,
    ALTIA_COORD             x,
    ALTIA_COORD             y,
    int                     width,
    int                     height,
    int                     *dx,
    int                     *dy,
    int                     *destWidth,
    int                     *destHeight,
    int                     *yoff,
    ALTIA_BOOLEAN           *clipped,
    int                     *totalWidth,
    int                     *totalHeight
);

extern void altia_rotation_offset(
    int                  width,
    int                  height,
    Altia_Transform_type *trans,
    int                  *xoffset,
    int                  *yoffset
);

extern void altiaTransformRaster(
    void                 *origDIB,
    int                  origWidth,
    int                  origHeight,
    void                 *destDIB,
    int                  dx,
    int                  dy,
    int                  destWidth,
    int                  destHeight,
    Altia_Transform_type *trans,
    int                  interType,
    void                 *mask
);

extern void altia_clip_rotate(
    Altia_GraphicState_type *gs,
    Altia_Extent_type       *wex,
    ALTIA_UINT32            dc,
    Altia_Transform_type    *trans,
    ALTIA_COORD             x,
    ALTIA_COORD             y,
    int                     width,
    int                     height
);

extern void altia_clip_clear(
    ALTIA_UINT32 dc
);


/*
 * Extent functions
 */

extern ALTIA_BOOLEAN _altiaIsHidden(
    ALTIA_INDEX,
    ALTIA_SHORT
);

extern Altia_Extent_type *altiaGetCurrentExtent(
    ALTIA_INDEX objIdx,
    ALTIA_SHORT objType
);

extern void _altiaUpdateParentExtent(
    ALTIA_SHORT objType,
    ALTIA_INDEX obj
);

extern void _altiaAddExtent(
    ALTIA_SHORT objType,
    ALTIA_INDEX obj
);

extern void _altiaFindExtent(
    ALTIA_SHORT objType,
    ALTIA_INDEX obj,
    Altia_Extent_type *extent
);

extern void _altiaFindExtentFloat(
    ALTIA_SHORT objType,
    ALTIA_INDEX obj,
    Altia_Extent_type *extent,
    ALTIA_FLOAT *fx0,
    ALTIA_FLOAT *fy0,
    ALTIA_FLOAT *fx1,
    ALTIA_FLOAT *fy1
);

extern void altiaLibRoundExtent(
    ALTIA_FLOAT x0,
    ALTIA_FLOAT y0,
    ALTIA_FLOAT x1,
    ALTIA_FLOAT y1,
    Altia_Extent_type *
);

extern void altiaLibEllipseExtent(
    ALTIA_CONST Altia_Ellipse_type *ellipse,
    ALTIA_SHORT bw,
    Altia_Transform_type *total,
    Altia_Extent_type *extent
);

extern void altiaLibRectExtent(
    Altia_Extent_type *olde,
    ALTIA_SHORT bwidth,
    Altia_Transform_type *trans,
    Altia_Extent_type *newe
);

extern void altiaLibCoordExtent(
    ALTIA_CONST Altia_Coord_type *coords,
    ALTIA_SHORT cnt,
    ALTIA_SHORT bwidth,
    Altia_Transform_type *trans,
    Altia_Extent_type *newe
);

extern void altiaLibClipExtent(
    Altia_Clip_type *clip,
    ALTIA_INDEX obj,
    Altia_Transform_type *total,
    Altia_Extent_type *extent
);

extern void altiaLibUpdateClipExtent(
    ALTIA_INDEX obj
);

extern void altiaLibUpdateAlphaMaskExtent(
    ALTIA_INDEX dobj,
    ALTIA_SHORT childType,
    ALTIA_INDEX child
);

extern void _altiaGetExtent(
    ALTIA_SHORT objType,
    ALTIA_INDEX obj,
    Altia_Extent_type *extent
);

extern void _altiaAddMyExtent(
    ALTIA_SHORT objType,
    ALTIA_INDEX obj,
    Altia_Extent_type *extent
);

/*
 * Draw functions
 */

extern void altiaLibInitDrawLib(
    void
);

extern void altiaLibCloseDrawLib(
    void
);

extern void altiaLibCopy(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_Extent_type *extent
);

#if !defined(ALTIA_NO_BACKGROUND_FILL) || (0 == ALTIA_NO_BACKGROUND_FILL)
extern void altiaLibClearRect(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    ALTIA_COLOR color,
    Altia_Extent_type *extent
);
#endif /* !ALTIA_NO_BACKGROUND_FILL */

extern void altiaLibEllipseDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_CONST Altia_Ellipse_type *ellipse,
    ALTIA_BYTE filled,
    Altia_Transform_type *total
);

extern void altiaLibLineDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_CONST Altia_Coord_type *coords,
    ALTIA_SHORT count,
    Altia_Transform_type *total
);

#ifdef ANTIALIAS
extern void altiaLibWuLineDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Coord_type *coords,
    ALTIA_SHORT count,
    Altia_Transform_type *total
);
#endif

extern void altiaLibLineSegDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Coord_type *coords,
    ALTIA_SHORT count,
    Altia_Transform_type *total
);

extern void altiaLibArcDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Coord_type *center,
    ALTIA_SHORT radius1,
    ALTIA_SHORT radius2,
    ALTIA_DOUBLE astart,
    ALTIA_DOUBLE aend,
    Altia_Transform_type *total
);
extern void altiaLibArcDraw2(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Coord_type *center,
    ALTIA_SHORT radius1,
    ALTIA_SHORT radius2,
    ALTIA_DOUBLE astart,
    ALTIA_DOUBLE aend,
    Altia_Transform_type *total,
    ALTIA_FLOAT ainc
);

extern void altiaLibTickExtent(
    Altia_Tick_type *tick,
    ALTIA_INDEX obj,
    Altia_Transform_type *total,
    Altia_Extent_type *extent
);

extern void altiaLibFillArcDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Coord_type *center,
    ALTIA_SHORT radius1,
    ALTIA_SHORT radius2,
    ALTIA_DOUBLE astart,
    ALTIA_DOUBLE aend,
    Altia_Transform_type *total
);

extern void altiaLibFillArcDraw2(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Coord_type *center,
    ALTIA_SHORT radius1,
    ALTIA_SHORT radius2,
    ALTIA_DOUBLE astart,
    ALTIA_DOUBLE aend,
    Altia_Transform_type *total,
    ALTIA_FLOAT ainc
);

extern void altiaLibRectDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_CONST Altia_Rect_type *rect,
    ALTIA_BYTE filled,
    Altia_Transform_type *total
);

extern void altiaLibPolygonDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_CONST Altia_Coord_type *coords,
    ALTIA_SHORT count,
    Altia_Transform_type *total
);

extern void altiaLibFillPolygonDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_CONST Altia_Coord_type *coords,
    ALTIA_SHORT count,
    Altia_Transform_type *total
);

extern void altiaLibSplineDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_CONST Altia_Coord_type *coords,
    ALTIA_SHORT count,
    Altia_Transform_type *total
);

extern void altiaLibCSplineDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_CONST Altia_Coord_type *coords,
    ALTIA_SHORT count,
    Altia_Transform_type *total
);

extern void altiaLibFillSplineDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_CONST Altia_Coord_type *coords,
    ALTIA_SHORT count,
    Altia_Transform_type *total
);

extern void altiaLibLabelDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Label_type *label,
    Altia_Transform_type *total
);

extern void altiaLibPieDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Pie_type *pie,
    Altia_Transform_type *total
);

extern void altiaLibRasterDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Raster_type *raster,
    Altia_Transform_type *total
);

extern void altiaLibRasterDrawId(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_INDEX obj,
    Altia_Transform_type *total
);

extern Altia_ColorEntry_type *_altiaRasterColors(
    ALTIA_INDEX index
);

extern Altia_Raster_type * altiaLibRasterFromIndex(
    ALTIA_INDEX index
);

extern void altiaLibRasterToIndex(
    Altia_Raster_type *raster,
    ALTIA_INDEX index
);

extern void altiaLibLoadRaster(
    Altia_Raster_type *raster
);

extern void altiaLibRasterDelete(
    Altia_Raster_type *raster
);

extern void altiaLibRasterClear(
    Altia_Raster_type *raster
);

extern void altiaLibRasterClearPerm(
    Altia_Raster_type *raster
);

extern void altiaLibStencilDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_CONST Altia_Stencil_type *stencil,
    Altia_StencilID_type *id,
    Altia_Transform_type *total
);

extern void altiaLibLoadStencil(
    ALTIA_CONST Altia_Stencil_type *stencil,
    Altia_StencilID_type *id
);

extern void altiaLibClipDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Clip_type *clip,
    ALTIA_INDEX obj,
    Altia_Transform_type *total
);

extern void altiaLibLPlotDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_LinePlot_type *plot,
    ALTIA_INDEX obj,
    Altia_Transform_type *total
);

extern void altiaLibDLineDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_DynamicLine_type *dline,
    ALTIA_INDEX obj,
    Altia_Transform_type *total
);

extern void altiaLibSSChartDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_StripChart_type *sschart,
    ALTIA_INDEX obj,
    Altia_Transform_type *total
);

extern void altiaLibTickDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Tick_type *tick,
    ALTIA_INDEX obj,
    Altia_Transform_type *total
);

extern void altiaLibRect3dDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_Rect3d_type *rect3d,
    Altia_Transform_type *total
);

extern void altiaLibTextioDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_INDEX textio,
    ALTIA_INDEX ddata,
    Altia_Transform_type *total
);

extern ALTIA_BOOLEAN altiaLibTextioProcess(
    ALTIA_INDEX textio,
    ALTIA_INDEX dobj,
    ALTIA_BOOLEAN pushExtent
);

void altiaLibMLineTextDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_MLineText_type *mlto,
    ALTIA_INDEX ddata,
    Altia_Transform_type *total
);

extern ALTIA_BOOLEAN altiaLibMLineTextProcess(
    ALTIA_INDEX mline,
    ALTIA_INDEX dobj,
    ALTIA_BOOLEAN useSubExtent,
    Altia_Extent_type * extent,
    ALTIA_INT totLines,
    ALTIA_BOOLEAN pushExtent
);

void altiaLibSoundObjDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    Altia_SoundObj_type *soundobj,
    ALTIA_INDEX ddata,
    Altia_Transform_type *total
);

extern void altiaLibAlphaMaskInitialize(
    void
);

extern void altiaLibAlphaMaskClose(
    void
);

extern void altiaLibAlphaMaskDraw(
    ALTIA_WINDOW win,
    ALTIA_UINT32 dc,
    Altia_GraphicState_type *gs,
    ALTIA_INDEX aobj,
    ALTIA_INDEX dobj,
    Altia_Transform_type *total
);

extern void altiaLibAlphaMaskClear(
    ALTIA_INDEX aobj
);

/* 
 * Font functions
 */

extern ALTIA_UINT32 _altiaFindFont(
    ALTIA_CHAR *name
);

extern void _altiaLoadFonts(
    void
);

extern ALTIA_UINT32  altiaLibLoadFont(
    ALTIA_CHAR *name
);

extern ALTIA_UINT32 _altiaGetFont(
    ALTIA_INDEX font
);

extern Altia_Brush_type *_altiaGetBrushEntry(
    ALTIA_INDEX brush
);

extern ALTIA_UINT32 altiaLibCreateBrush(
    Altia_Brush_type *brush
);

extern ALTIA_INDEX _altiaGetBrush(
    ALTIA_SHORT objType,
    ALTIA_INDEX obj
);

extern void altiaLibLoadPattern(
    Altia_Pattern_type *pat
);

extern ALTIA_UINT32 altiaLibGetPattern(
    Altia_GraphicState_type *gs
);

extern Altia_Pattern_type *_altiaGetPatternEntry(
    ALTIA_INDEX pat
);

extern void altiaLibStringExtent(
    Altia_Font_type *font,
    ALTIA_CHAR *str,
    int len,
    Altia_Transform_type *trans,
    Altia_Extent_type *extent
);

extern void altiaLibFontExtent(
    Altia_Font_type *font,
    ALTIA_CHAR      *s,
    int              len,
    int             *width,
    int             *height,
    int             *offset 
);

extern ALTIA_BOOLEAN altiaLibIsFixedWidthFont(
    Altia_Font_type *font
);

/* utility functions */

ALTIA_BOOLEAN altiaLibIsNumber(
    ALTIA_CHAR *string,
    AltiaEventType *dval
);

ALTIA_BOOLEAN altiaLibIsInteger(
    ALTIA_CHAR *string,
    int *val
);

ALTIA_INDEX altiaFindObj(
    AltiaEventType id,
    ALTIA_SHORT *otype
);

/* Utility geometry functions */

extern ALTIA_BOOLEAN altiaLibBoxContains(
    AltiaBoxObj      *b,
    Altia_Coord_type *p 
);

extern ALTIA_BOOLEAN altiaLibRectContains(
    ALTIA_CONST Altia_Rect_type *rect,
    ALTIA_SHORT bw,
    ALTIA_COORD x,
    ALTIA_COORD y,
    Altia_Transform_type *total
);

extern ALTIA_BOOLEAN altiaLibEllipseContains(
    ALTIA_CONST Altia_Ellipse_type *ellipse,
    ALTIA_SHORT bw,
    ALTIA_COORD x,
    ALTIA_COORD y,
    Altia_Transform_type *total
);

extern ALTIA_BOOLEAN altiaLibBoxIntersectsBox(
    AltiaBoxObj      *b1,
    AltiaBoxObj      *b2
);

extern ALTIA_BOOLEAN altiaLibBoxWithinBox(
    AltiaBoxObj      *b1,
    AltiaBoxObj      *b2
);

extern ALTIA_BOOLEAN altiaLibLineIntersects(
    AltiaLineObj     *l1,
    AltiaLineObj     *l2
);

extern ALTIA_BOOLEAN altiaLibLineIntersectsBox(
    AltiaLineObj     *l1,
    AltiaBoxObj      *b
);

extern void altiaLibGetPolyBox(
    ALTIA_CONST Altia_Coord_type *coords,
    ALTIA_INT        count,
    AltiaBoxObj      *b
);

extern ALTIA_BOOLEAN altiaLibPolyContains(
    ALTIA_CONST Altia_Coord_type *coords,
    ALTIA_INT        count,
    Altia_Coord_type *p
);

extern ALTIA_BOOLEAN altiaLibPolyIntersectsLine(
    ALTIA_CONST Altia_Coord_type *coords,
    ALTIA_INT        count,
    AltiaLineObj     *l 
);

extern ALTIA_BOOLEAN altiaLibPolyIntersectsBox(
    ALTIA_CONST Altia_Coord_type *coords,
    ALTIA_INT        count,
    AltiaBoxObj      *b 
);

extern ALTIA_BOOLEAN altiaHandleInputEvent(
    Altia_InputEvent_type *event
);

extern ALTIA_BOOLEAN builtinSetFont(
    ALTIA_INDEX fontidx,
    ALTIA_SHORT otype,
    ALTIA_INDEX idx
);


/******************************************************************
 * DeepScreen engine funcs/globals referenced by DeepScreen API.
 ******************************************************************/
extern int _altiaAPICaching;     /* For caching state, defined in draw.c */
extern int _altiaAPINeedUpdate;  /* Need to update state, defined in draw.c */
extern int _altiaBreakLoop;      /* For event error, defined in altiaAPI.c */

extern int TargetCheckEvent(
    int *status
);

extern ALTIA_BOOLEAN TargetGetEvent(
    int *retVal);

extern void TargetSleep(
    unsigned long msecs
);

extern ALTIA_BOOLEAN _altiaConnectionToFloat(
    ALTIA_CHAR *funcName
);

extern ALTIA_BOOLEAN _altiaConnectionToFloatId(
    int index
);

extern ALTIA_BOOLEAN _altiaConnectionFromFloat(
    ALTIA_CHAR *funcName
);

extern int TargetGetTextWidth(
    ALTIA_CONST ALTIA_CHAR *name,
    ALTIA_CONST ALTIA_CHAR *text
);

extern int TargetGetTextWidthId(
    int nameId, ALTIA_CONST ALTIA_CHAR *text
);

extern int TargetGetFont(
    ALTIA_CONST ALTIA_CHAR *name
);

extern int TargetGetFontId(
    int nameId
);

extern int TargetSetFont(
    ALTIA_CONST ALTIA_CHAR *name, int fontId
);

extern int TargetSetFontId(int nameId,
    int fontId
); 

#ifdef ALTIA_TASKING
extern int TargetAltiaTaskRun(
    ALTIA_UINT32 milliSeconds
);

extern void TargetAltiaTaskStatus(
    AltiaQueueStatusType * status
);

extern void TargetAltiaObjectStatus(
    AltiaQueueStatusType * status
);
#endif /* ALTIA_TASKING */


#endif /* ALTIAEXTERN_H */
