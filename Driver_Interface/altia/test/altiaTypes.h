/* $Revision: 1.82 $    $Date: 2010-12-13 00:34:39 $
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

/******************************************************************
 * Determine if function prototyping is enabled.
 ******************************************************************/
#if !defined(ALTIATYPES_H)
#define ALTIATYPES_H

#ifndef Altiafp
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus) || defined(_MSC_VER)
#define Altiafp
#else
#undef Altiafp
#endif /* Check for function prototyping requirement */
#endif /* Altiafp */


/* Code generated with ASCII character support */




/* The different drawing types */
#define AltiaNoDraw              0
#define AltiaEllipseDraw         1
#define AltiaLineDraw            2
#define AltiaRectDraw            3
#define AltiaSoundObjDraw        4
#define AltiaPolygonDraw         5
#define AltiaFillPolygonDraw     6
#define AltiaSplineDraw          7
#define AltiaCSplineDraw         8
#define AltiaFillSplineDraw      9
#define AltiaLabelDraw          10
#define AltiaStencilDraw        11
#define AltiaRasterDraw         12
#define AltiaClipDraw           13
#define AltiaChildDraw          14
#define AltiaLPlotDraw          15
#define AltiaTextioDraw         16
#define AltiaFPPlotDraw         17
#define AltiaStripChartDraw     18
#define AltiaTickDraw           19
#define AltiaRect3dDraw         20
#define AltiaMLineTextDraw      21
#define AltiaPieDraw            22
#define AltiaDrawingAreaDraw    23
#define AltiaImageObjDraw       24
#define AltiaDPolyDraw          25
#define AltiaDLineDraw          26
#define AltiaOpenGLDraw         27
#define AltiaSnapshotDraw       28
#define AltiaSkinDraw           29
#define AltiaLanguageDraw       30
#define AltiaPathObjectDraw     31
#define AltiaFlashPlayerDraw    32
#define AltiaBlurFilterDraw     33
#define AltiaDisplayDraw        34
#define AltiaTDSceneDraw        35
#define AltiaMultiPlotDraw      36
#define AltiaAlphaMaskDraw      37

/* The different animation types */
#define AltiaNoAnimation        0
#define AltiaNormalAnimation  	1
#define AltiaDeckAnimation      2
#define AltiaCustomSAnimation	3
#define AltiaCustomFAnimation	4
#define AltiaCustomIAnimation	5

/* The different object types */
#define AltiaNoObject           0
#define AltiaDynamicObject      1
#define AltiaStaticObject       2
#define AltiaCloneObject        3

/* The different animation function types */
#define AltiaAnimationFunc      0
#define AltiaControlFunc        1
#define AltiaConnectionFunc     2
#define AltiaTimerStartFunc     3
#define AltiaTimerStopFunc      4
#define AltiaCustomFunc         5

/* The different skin command types (bits) */
#define AltiaSkinColorFg        (ALTIA_UBYTE)(1)
#define AltiaSkinColorBg        (ALTIA_UBYTE)(2)
#define AltiaSkinBrush          (ALTIA_UBYTE)(4)
#define AltiaSkinFont           (ALTIA_UBYTE)(8)
#define AltiaSkinImage          (ALTIA_UBYTE)(16)
#define AltiaSkinPosition       (ALTIA_UBYTE)(32)

/*
 *  These are the bit definitions for the Altia_StateEntry_t mask
 */
#define ALTIA_STATE_NOSAVE_FOREGROUND 1
#define ALTIA_STATE_NOSAVE_BACKGROUND 2
#define ALTIA_STATE_NOSAVE_BRUSH 4
#define ALTIA_STATE_NOSAVE_PATTERN 8
#define ALTIA_STATE_OVERFLOW_STATE 16
#define ALTIA_STATE_NO_MAP 32
#define ALTIA_STATE_NO_ALPHA 64

/*
 * These are the bit definitions for the Altia_CustomI_t flags
 */
#define ALTIA_CUSTI_SAME_BEHAVIOR 1
#define ALTIA_CUSTI_MODIFIED_VALUE 2
#define ALTIA_CUSTI_CLEAR_MODIFIED 0xfd
#define ALTIA_CUSTI_RANGE_CHECK 4
#define ALTIA_CUSTI_MIN_MAX_SET 8
#define ALTIA_CUSTI_RANGE_ZERO 16

/*
 * These are the bit definitions for the Altia_CustomF_t flags
 */
#define ALTIA_CUSTF_SAME_BEHAVIOR 1
#define ALTIA_CUSTF_MODIFIED_VALUE 2
#define ALTIA_CUSTF_CLEAR_MODIFIED 0xfd
#define ALTIA_CUSTF_RANGE_CHECK 4
#define ALTIA_CUSTF_MIN_MAX_SET 8
#define ALTIA_CUSTF_RANGE_ZERO 16

/*
 * These are the bit definitions for the Altia_CustomS_t flags
 */
#define ALTIA_CUSTS_ADD 1
#define ALTIA_CUSTS_CLEAR_ADD 0xfe
#define ALTIA_CUSTS_MODIFIED_VALUE 2
#define ALTIA_CUSTS_CLEAR_MODIFIED 0xfd

/*
 * These are the different raster types
 */
#define ALTIA_RASTER_8_RLE          0x0001
#define ALTIA_RASTER_24_RLE         0x0002
#define ALTIA_RASTER_32_RLE         0x0003 /* With alpha channel */

/*
 * Custom raster types -- used with DDB image formats
 */
#define ALTIA_RASTER_CUSTOM_FLAG    0x8000
#define ALTIA_RASTER_CUSTOM_LAYER   0x0100
#define ALTIA_RASTER_CUSTOM_CHROMA  0x0080
#define ALTIA_RASTER_CUSTOM_MDDB    0x0001
#define ALTIA_RASTER_CUSTOM_DDB     0x0002
#define ALTIA_RASTER_CUSTOM_TDDB    0x0003
#define ALTIA_RASTER_CUSTOM_PAL     0x0001 /* Same as MDDB */

/*
 * Layer color formats
 */
#define ALTIA_LAYER_RGB888          0
#define ALTIA_LAYER_ARGB8888        1
#define ALTIA_LAYER_RGB565          2
#define ALTIA_LAYER_ARGB4444        3
#define ALTIA_LAYER_ALPHA           4
#define ALTIA_LAYER_PAL             5
#define ALTIA_LAYER_APAL            6

/*
 * Layer blend modes
 */
#define ALTIA_LAYER_BLEND_NORMAL    0
#define ALTIA_LAYER_BLEND_VIDEO     1
#define ALTIA_LAYER_BLEND_CHILD     2

/*
 * These are the different Input Command Types
 */

#define ALTIA_NOOP_INPUT_CMD 0
#define ALTIA_INC_INPUT_CMD 1
#define ALTIA_DEC_INPUT_CMD 2
#define ALTIA_LESS_INPUT_CMD 3
#define ALTIA_GREAT_INPUT_CMD 4
#define ALTIA_NOT_EQUAL_INPUT_CMD 5
#define ALTIA_LESS_EQUAL_INPUT_CMD 6
#define ALTIA_GREAT_EQUAL_INPUT_CMD 7

/*
 * These are the different Input Area Types
 */

#define ALTIA_GRAPHIC_INPUT_AREA 0
#define ALTIA_RECT_INPUT_AREA 1
#define ALTIA_POLAR_INPUT_AREA 2

/*
 * These are the different Altia_InputEvent_types
 */
#ifndef ALTIA_MOTION_EVENT_TYPE
#define ALTIA_MOTION_EVENT_TYPE 0
#endif
#ifndef ALTIA_DOWN_EVENT_TYPE
#define ALTIA_DOWN_EVENT_TYPE 1
#endif
#ifndef ALTIA_UP_EVENT_TYPE
#define ALTIA_UP_EVENT_TYPE 2
#endif
#ifndef ALTIA_KEY_EVENT_TYPE
#define ALTIA_KEY_EVENT_TYPE 3
#endif
#ifndef ALTIA_KEY_UP_EVENT_TYPE
#define ALTIA_KEY_UP_EVENT_TYPE 4
#endif
#ifndef ALTIA_ENTER_EVENT_TYPE
#define ALTIA_ENTER_EVENT_TYPE 5
#endif
#ifndef ALTIA_LEAVE_EVENT_TYPE
#define ALTIA_LEAVE_EVENT_TYPE 6
#endif
#ifndef ALTIA_CHANNEL_EVENT_TYPE
#define ALTIA_CHANNEL_EVENT_TYPE 7
#endif
#ifndef ALTIA_TIMER_EVENT_TYPE
#define ALTIA_TIMER_EVENT_TYPE 8
#endif

/*
 * These are the different button types
 */
#ifndef ALTIA_LEFTMOUSE_BUTTON
#define ALTIA_LEFTMOUSE_BUTTON 1
#endif
#ifndef ALTIA_MIDDLEMOUSE_BUTTON
#define ALTIA_MIDDLEMOUSE_BUTTON 2
#endif
#ifndef ALTIA_RIGHTMOUSE_BUTTON
#define ALTIA_RIGHTMOUSE_BUTTON 4
#endif

/*
 * These are the different button masks
 */
#define ALTIA_LEFTMOUSE_DOWN_MASK 0
#define ALTIA_MIDDLEMOUSE_DOWN_MASK 1
#define ALTIA_RIGHTMOUSE_DOWN_MASK (1<<1)
#define ALTIA_LEFTMOUSE_UP_MASK (1<<2)
#define ALTIA_MIDDLEMOUSE_UP_MASK (1<<3)
#define ALTIA_RIGHTMOUSE_UP_MASK (1<<4)

/*
 * These are the different event mask values
 */
#define ALTIA_KEY_MASK (1<<0)
#define ALTIA_KEY_UP_MASK (1<<1)
#define ALTIA_ENTER_MASK (1<<4)
#define ALTIA_LEAVE_MASK (1<<5)
#define ALTIA_MOTION_MASK (1<<6)
#define ALTIA_UP_MASK ((1<<2)|(1<<3))
#define ALTIA_DOWN_MASK ((1<<2)|(1<<3))

/*
 * These are the different types functions can be
 */
#define ALTIA_INTEGER_TYPE  1
#define ALTIA_STRING_TYPE  2
#define ALTIA_FLOAT_TYPE  4

/*
 * Task types for the ALTIA_TASKING feature
 */
#define ALTIA_TASK_NOP          0
#define ALTIA_TASK_EVENT        1
#define ALTIA_TASK_FLUSH        2
#define ALTIA_TASK_COUNT        3

/*
 * These are the connection types
 */
#define ALTIA_INPUT_CONNECTION_TYPE 0
#define ALTIA_OUTPUT_CONNECTION_TYPE 1

/*
 * These are the different alpha mask operation types
 */
#define ALTIA_ALPHA_MASK_COLOR_DRAW     0x01
#define ALTIA_ALPHA_MASK_ALPHA_DRAW     0x02

/*
 * These are the different alpha mask operation modes
 */
#define ALTIA_ALPHA_MASK_MODE_NORMAL    0
#define ALTIA_ALPHA_MASK_MODE_INVERT    1

#define A_PI	3.14159265358979323846
#define RADPERDEG ALTIA_C2D(A_PI/180.0)
#define DEGPERRAD ALTIA_C2D(180.0/A_PI)

#define NearestNeighbor 1
#define Bilinear 2

/* Undef out unfriendly macros */
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

/* Character type used in file names for fopen */
#ifndef ALTIA_FILE_CHAR
#define ALTIA_FILE_CHAR ALTIA_BYTE
#endif

typedef void (*AltiaReportFuncType)(
    ALTIA_CHAR  *name,
    AltiaEventType value
);

typedef void (*AltiaReportFuncIdType)(
    int nameId,
    AltiaEventType value
);

typedef void (*AltiaInternalTimerFunc)(
    ALTIA_UINT32 parm
);

typedef int (*AltiaOpenGLAnimationFunc)(
    int objid,
    unsigned long animateID,
    int vtype,
    AltiaEventType value
);

typedef int (*AltiaFlashPlayerAnimationFunc)(
    int objid,
    unsigned long animateID,
    int vtype,
    AltiaEventType value
);

/* Altia Draw Types */
typedef struct
{
    ALTIA_COORD x;
    ALTIA_COORD y;
} Altia_Coord_type;

typedef struct
{
    Altia_Coord_type p1, p2;
} AltiaLineObj;

typedef struct
{
    ALTIA_COORD left, right;
    ALTIA_COORD bottom, top;
} AltiaBoxObj;





typedef struct
{
    ALTIA_VARFX a02, a12, a22;
    ALTIA_FIXPT _a00, _a01;
    ALTIA_VARFX _a02;
    ALTIA_FIXPT _a10, _a11;
    ALTIA_VARFX _a12, _a22;
} Altia_Nonaffine_type, *ALTIA_NONAFFINE_PTR;

typedef struct
{
    ALTIA_FIXPT a00, a01, a10, a11, a20, a21;

    ALTIA_FIXPT angle;
    ALTIA_FIXPT xoffset;
    ALTIA_FIXPT yoffset;


} Altia_Transform_type;


typedef struct Altia_NonaffineHdr_struct
{
    ALTIA_INT           count;
    ALTIA_INDEX         first;
    ALTIA_INDEX         last;
    ALTIA_INDEX         free;
    ALTIA_NONAFFINE_PTR array;
    struct Altia_NonaffineHdr_struct *next;
} Altia_NonaffineHdr_type;




typedef struct
{
    ALTIA_FIXPT a00, a01, a10, a11;
    ALTIA_FIXPT angle;
} Altia_BaseTrans_type;





typedef struct
{
    ALTIA_FIXPT a20, a21;

    ALTIA_FIXPT xoffset;
    ALTIA_FIXPT yoffset;


} Altia_OffsetTrans_type;


#ifdef UNMODIFIED_TRANSFORM_DATA

#endif /* UNMODIFIED_TRANSFORM_DATA */

typedef struct
{
    ALTIA_SHORT  lineWidth;
    ALTIA_SHORT  count;
    ALTIA_UINT32 pattern;
    ALTIA_UINT32 id;
} Altia_Brush_type;

typedef struct
{
    ALTIA_BINDEX index;
    ALTIA_BYTE   patType;  /* 0 = solid, 1 = clear, 2 = bitmap */
    ALTIA_UINT32 id;
} Altia_Pattern_type;

typedef struct
{
    ALTIA_CHAR  *name;
    ALTIA_USHORT ascent;
    ALTIA_USHORT descent;
    ALTIA_UINT32 id;
} Altia_Font_type;

typedef struct
{
    ALTIA_COORD x0;
    ALTIA_COORD y0;
    ALTIA_COORD x1;
    ALTIA_COORD y1;
} Altia_Extent_type;

typedef struct
{
    ALTIA_CHAR  *name;
    ALTIA_INDEX  font;
    Altia_Extent_type extent;
} Altia_Label_type;

typedef ALTIA_UBYTE Altia_AlphaMaskBSS_type;

typedef struct
{
    ALTIA_COORD x;
    ALTIA_COORD y;
    ALTIA_SHORT width;
    ALTIA_SHORT height;
} Altia_Clip_type;

typedef struct
{
    ALTIA_INT8  firstColor:2;
    ALTIA_INT8  colorCnt:2;
    ALTIA_INT8  firstAlpha:2;
    ALTIA_INT8  alphaCnt:2;
} Altia_AlphaMaskROM_type;

typedef struct
{
    ALTIA_USHORT width;
    ALTIA_USHORT height;
    ALTIA_UBYTE  mode;
} Altia_AlphaMaskRAM_type;

typedef struct
{
    ALTIA_COLOR      foreground;
    ALTIA_COLOR      background;
    ALTIA_INDEX      brush;
    ALTIA_INDEX      pattern;
    ALTIA_INDEX      font;
    Altia_Extent_type clip;
    ALTIA_COORD      tile_x_origin;
    ALTIA_COORD      tile_y_origin;
    ALTIA_SHORT      alpha;

} Altia_GraphicState_type;

typedef struct
{
    AltiaEventType             state;
    ALTIA_UBYTE                mask;
    ALTIA_INDEX                basetrans;
    ALTIA_INDEX                offtrans;
    ALTIA_COLOR                foreground;
    ALTIA_COLOR                background;
    ALTIA_INDEX                pattern;
    ALTIA_INDEX                brush;
    ALTIA_SHORT                alpha;
} Altia_StateEntry_type;

typedef struct
{
    ALTIA_INT16 state:2;
    ALTIA_INT16 mask:2;
    ALTIA_INT16 basetrans:2;
    ALTIA_INT16 offtrans:2;
    ALTIA_INT16 foreground:2;
    ALTIA_INT16 background:2;
    ALTIA_INT16 pattern:2;
    ALTIA_INT16 brush:2;
    ALTIA_INT8  alpha;
} Altia_StateEntryROM_type;

/* Altia_FunctionName_type array is for animations (normal, deck, custom).
 * It is indexed from the dynamic obj array and FunctionFIndex_type records.
 * It indexes either the state, deck, or one of the custom animation arrays.
 * There is an array entry for each unique time a function name is
 * used for a normal animation, deck card animation, or special object custom
 * animation. The animateType can be AltiaNoAnimation (can happen for a
 * deck card animation when the deck has no cards), AltiaNormalAnimation,
 * etc. as defined earlier in this file.
 */
typedef struct
{
    ALTIA_INDEX        first;  /* StateEntry or Deck or Custom[SFI] index */
    ALTIA_SHORT        count;
    ALTIA_INDEX        object;
    ALTIA_SHORT        animateType;
} Altia_FunctionName_type;

typedef struct
{
    ALTIA_INT16 first:5;
    ALTIA_INT16 count:3;
    ALTIA_INT16 object:4;
    ALTIA_INT16 animateType:4;
} Altia_FunctionNameROM_type;

/* Altia_FunctionIndex_type array lists all the function names.  Just
 * one entry for each function name in the design (even if it is used in
 * several different ways such as an animation name and timer start/stop).
 * Entries are in sorted order by the name.  This array is searched to
 * determine if a new event name is of interest.  The firstIndex element
 * for an entry refers to a record in the Altia_FunctionFIndex_type array.
 * The count indicates how many sequential records of the
 * Altia_FunctionFIndex_type array are associated with the function name
 * (in other words, the number of different ways the function name is used).
 */
typedef struct
{
    ALTIA_CHAR        *name;
    ALTIA_INDEX        firstIndex; /* Index to Altia_FunctionFIndex_type */
    ALTIA_INDEX        count;
} Altia_FunctionIndex_type;

/* Altia_FunctionFIndex_type array has an entry for each different way
 * a function name is used (e.g., normal/deck/custom animation, control when,
 * connection, timer start, timer stop).  The entries related to a specific
 * function name are in sequence.  Entries then index into various arrays
 * based upon the funcType.  For example, if the funcType for an entry is
 * AltiaAnimationFunc (the function name is used as a normal/deck/custom
 * animation), the index refers to an Altia_FunctionName_type record.
 * The Altia_FunctionFIndex_type array is itself indexed by records in
 * the Altia_FunctionIndex_type array.
 */
typedef struct
{
    ALTIA_BYTE           funcType;
    ALTIA_INDEX          index; /* example: Altia_FunctionName_type index */
} Altia_FunctionFIndex_type;

typedef struct
{
    ALTIA_INT16 funcType:4;
    ALTIA_INT16 index:6;
} Altia_FunctionFIndexROM_type;

typedef struct
{
    ALTIA_UINT32     id;
    ALTIA_UBYTE      flag;
    ALTIA_DOUBLE     value;
    ALTIA_FLOAT      min;
    ALTIA_FLOAT      max;
} Altia_CustomFValue_type;

typedef struct
{
    ALTIA_INT16 id;
    ALTIA_INT8  min:2;
    ALTIA_INT8  max:2;
} Altia_CustomFValueROM_type;

typedef struct
{
    ALTIA_INT8  flag;
    ALTIA_INT32 value;
} Altia_CustomFValueRAM_type;

typedef struct
{
    ALTIA_UINT32     id;
    ALTIA_UBYTE      flag;



/* Pie and/or Text I/O objects are present. */
#define ALTIA_USING_strsize 1


/* The size of the value element of a Altia_CustomSValue_type
 * must be as large as the largest size expected by the objects
 * that use it.
 */
#if defined(ALTIA_USING_strsize) && defined(ALTIA_USING_mlinestrsize)
    #if (64 > 96)
        #define ALTIA_CUSTOMSVALUE_SIZE  64
    #else
        #define ALTIA_CUSTOMSVALUE_SIZE  96
    #endif
#elif defined(ALTIA_USING_mlinestrsize)
    #define ALTIA_CUSTOMSVALUE_SIZE  96
#elif defined(ALTIA_USING_strsize)
    #define ALTIA_CUSTOMSVALUE_SIZE  64
#else
    #define ALTIA_CUSTOMSVALUE_SIZE  1
#endif

    ALTIA_CHAR     value[ALTIA_CUSTOMSVALUE_SIZE];



    ALTIA_SHORT    index;

} Altia_CustomSValue_type;

typedef struct
{
    ALTIA_UINT32     id;
    ALTIA_UBYTE      flag;
    ALTIA_INT        value;
    ALTIA_INT        min;
    ALTIA_INT        max;
} Altia_CustomIValue_type;

typedef struct
{
    ALTIA_INT8  flag;
    ALTIA_INT32 value;
} Altia_CustomIValueRAM_type;

typedef struct
{
    ALTIA_INT32 id:16;
    ALTIA_INT32 min:2;
    ALTIA_INT32 max:10;
} Altia_CustomIValueROM_type;

typedef struct
{
    ALTIA_SHORT    type;
    ALTIA_INDEX    child;
} Altia_ChildEntry_type;

typedef struct
{
    ALTIA_INT8  type:3;
    ALTIA_INT8  child:4;
} Altia_ChildEntryROM_type;


#ifdef MONO_DISPLAY
#define PACK_MONO_COLOR_DATA(source, target)    \
{                                               \
    if(source == (ALTIA_COLOR)LONG_MINUS_ONE)   \
        target = ALTIA_NO_COLOR_SPECIFIED;      \
    else if(source == (ALTIA_COLOR)ALTIA_BLACK) \
        target = ALTIA_PIXEL_OFF;               \
    else                                        \
        target == ALTIA_PIXEL_ON;               \
}

#define UNPACK_MONO_COLOR_DATA(source, target)  \
{                                               \
    if(source == ALTIA_NO_COLOR_SPECIFIED)      \
        target = (ALTIA_COLOR)LONG_MINUS_ONE;   \
    else if(source == ALTIA_PIXEL_OFF)          \
        target = (ALTIA_COLOR)ALTIA_BLACK;      \
    else                                        \
        target = (ALTIA_COLOR)ALTIA_WHITE;      \
}
#endif /* MONO_DISPLAY */

typedef struct
{
    ALTIA_UINT32                   id;
    ALTIA_SHORT                    drawType;
    ALTIA_INDEX                    drawIndex;
    ALTIA_COLOR                    foreground;
    ALTIA_COLOR                    background;
    ALTIA_BYTE                     brush;
    ALTIA_BYTE                     pattern;
    Altia_Transform_type           total;
    ALTIA_INDEX                    parent;
    Altia_Extent_type              extent;
    Altia_Transform_type           trans;
    ALTIA_BYTE                     mapped;
    ALTIA_INDEX                    firstChild;
    ALTIA_SHORT                    childCnt;
    ALTIA_INDEX                    firstFunc; /* FunctionName */
    ALTIA_SHORT                    funcCnt;
    Altia_Transform_type           lastTrans;
    ALTIA_FLOAT                    behave_rptx;
    ALTIA_FLOAT                    behave_rpty;
    ALTIA_INDEX                    currentFunc;
    AltiaEventType                 currentValue;
    ALTIA_SHORT                    alpha;



} Altia_DynamicObject_type;

typedef struct
{
    ALTIA_COLOR          foreground;
    ALTIA_COLOR          background;
    ALTIA_BYTE           brush;
    ALTIA_BYTE           pattern;
    ALTIA_SHORT          mapped : 2;
    ALTIA_SHORT          alpha  :14;
    Altia_Transform_type lastTrans;
} Altia_DynamicState_type;

typedef struct
{
    ALTIA_COLOR          foreground;
    ALTIA_COLOR          background;
    ALTIA_BYTE           brush;
    ALTIA_BYTE           pattern;
    ALTIA_SHORT          mapped : 2;
    ALTIA_SHORT          alpha  :14;
} Altia_DynamicStateC_type;

typedef struct
{
    ALTIA_INT32 foreground:25;
    ALTIA_INT32 brush:2;
    ALTIA_INT32 pattern:2;
    ALTIA_INT32 mapped:2;
    ALTIA_INT32 background:25;
    ALTIA_INT32 alpha:2;
} Altia_DynamicStateROM_type;

typedef struct
{
    ALTIA_INDEX          firstChild;
    ALTIA_SHORT          childCnt;
    Altia_Extent_type    extent;
    Altia_Transform_type total;
    Altia_Transform_type trans;
} Altia_DynamicObjectRAM_type;

typedef struct
{
    ALTIA_INT32 id:5;
    ALTIA_INT32 drawType:6;
    ALTIA_INT32 drawIndex:2;
    ALTIA_INT32 parent:4;
    ALTIA_INT32 firstFunc:6;
    ALTIA_INT32 funcCnt:5;
    ALTIA_INT32 stateIndex:3;
    ALTIA_INT8  behave_rptx:2;
    ALTIA_INT8  behave_rpty:2;
    ALTIA_INT8  custom:2;
} Altia_DynamicObjectROM_type;

typedef struct
{
    ALTIA_COLOR                    foreground;
    ALTIA_COLOR                    background;
    ALTIA_BYTE                     brush;
    ALTIA_BYTE                     pattern;
    Altia_Transform_type           total;
    ALTIA_INDEX                    parent;
    Altia_Extent_type              extent;
    /* can't use a index for otrans since move obj can change it */
    Altia_OffsetTrans_type         otrans;
    ALTIA_SHORT                    alpha;
} Altia_StaticObject_type;

typedef struct
{
    ALTIA_UINT32                   id;
    ALTIA_SHORT                    drawType;
    ALTIA_INDEX                    drawIndex;
    ALTIA_INDEX                    basetrans;
    ALTIA_BYTE                     mapped;

} Altia_StaticObjectC_type;

typedef struct
{
    Altia_Extent_type      extent;
    Altia_OffsetTrans_type otrans;
    Altia_Transform_type   total;
} Altia_StaticObjectRAM_type;

typedef struct
{
    ALTIA_INT32 foreground:2;
    ALTIA_INT32 background:25;
    ALTIA_INT32 id:5;
    ALTIA_INT32 brush:2;
    ALTIA_INT32 pattern:2;
    ALTIA_INT32 parent:4;
    ALTIA_INT32 alpha:2;
    ALTIA_INT32 drawType:5;
    ALTIA_INT32 drawIndex:3;
    ALTIA_INT32 basetrans:3;
    ALTIA_INT32 mapped:2;
} Altia_StaticObjectROM_type;

typedef struct
{
    ALTIA_BYTE        objType;
    ALTIA_INDEX       object;

} Altia_Sequence_type;

typedef struct
{
    ALTIA_INT8  objType:3;
    ALTIA_INT8  object:3;
} Altia_SequenceROM_type;

/**************************************************************************
 * Coords in the object structs types are assumed to be in obj coords.
 * This allows for transforms to change and we can comput new coords.
 * The extents in the dobjs and sobjs are assumed to be screen coords so
 * computing drawing areas are done real fast.
 **************************************************************************/

typedef struct
{
    ALTIA_COORD   x;
    ALTIA_COORD   y;
    ALTIA_BYTE    filled;
    ALTIA_SHORT   r1;
    ALTIA_SHORT   r2;
} Altia_Ellipse_type;

typedef struct
{
    ALTIA_INT16 x:2;
    ALTIA_INT16 y:2;
    ALTIA_INT16 filled:2;
    ALTIA_INT16 r1:2;
    ALTIA_INT16 r2:2;
} Altia_EllipseROM_type;

typedef struct
{
    ALTIA_COORD x0;
    ALTIA_COORD y0;
    ALTIA_COORD x1;
    ALTIA_COORD y1;
    ALTIA_BYTE  filled;
} Altia_Rect_type;

typedef struct
{
    ALTIA_INT32 x0:10;
    ALTIA_INT32 y0:11;
    ALTIA_INT32 y1:11;
    ALTIA_INT16 x1:10;
    ALTIA_INT16 filled:2;
} Altia_RectROM_type;

typedef struct
{
    ALTIA_SHORT   count;
    ALTIA_INDEX   first;
} Altia_NCoord_type;

typedef struct
{
    AltiaEventType  card;
    ALTIA_INDEX  firstChild;
    ALTIA_SHORT  childCnt;

} Altia_Deck_type;

typedef struct
{
    ALTIA_INT8  card:2;
    ALTIA_INT8  firstChild:3;
    ALTIA_INT8  childCnt:2;
} Altia_DeckROM_type;

typedef struct
{
    ALTIA_INT    id;
    ALTIA_USHORT width;
    ALTIA_USHORT height;
    ALTIA_INDEX  firstLayer;
    ALTIA_SHORT  layerCnt;
    ALTIA_INDEX  firstScreen;
    ALTIA_SHORT  screenCnt;
} Altia_DisplayC_type;

typedef struct
{
    ALTIA_UBYTE  enabled;
    ALTIA_COLOR  background;
    ALTIA_INT    activeLayer;
} Altia_Display_type;

#define LAYER_FLAGS_SINGLE_BUFFERED     ((ALTIA_UBYTE)0x01)
#define LAYER_FLAGS_INTERNAL_RAM        ((ALTIA_UBYTE)0x02)
#define LAYER_FLAGS_RLE                 ((ALTIA_UBYTE)0x04)

typedef struct
{

    ALTIA_INT16 id:2;
    ALTIA_INT16 format:2;
    ALTIA_INT16 flags:2;
    ALTIA_INT16 bufferWidth:2;
    ALTIA_INT16 bufferHeight:2;
    ALTIA_INT16 blendMode:2;
    ALTIA_INT16 palette:2;
    ALTIA_INT16 firstChild:2;
    ALTIA_INT8  childCnt;


} Altia_LayerC_type;

typedef struct
{
    ALTIA_UBYTE  visible   :1;
    ALTIA_UBYTE  useChroma :1;
    ALTIA_UBYTE  alpha;
    ALTIA_COORD  x;
    ALTIA_COORD  y;
    ALTIA_USHORT width;
    ALTIA_USHORT height;
    ALTIA_COLOR  background;
    ALTIA_COLOR  chroma;
    ALTIA_COORD  bufferX;
    ALTIA_COORD  bufferY;
    ALTIA_INT    blendTarget;
} Altia_Layer_type;

typedef struct
{
    ALTIA_INT    id;
    ALTIA_INDEX  firstLayer;
    ALTIA_SHORT  layerCnt;
} Altia_Screen_type;

typedef struct
{
    ALTIA_INDEX       layer;
    Altia_Layer_type  data;
} Altia_ScreenLayer_type;

typedef struct
{
    ALTIA_UBYTE  depth;
    ALTIA_INDEX  firstColor;
    ALTIA_SHORT  colorCnt;
} Altia_Palette_type;

typedef struct
{
    ALTIA_UBYTE  red;
    ALTIA_UBYTE  green;
    ALTIA_UBYTE  blue;
} Altia_ColorEntry_type;

/* Define data structure that can hold attributes of a
 * scaled bitmap.  This allows for comparing attributes
 * of a new scaled bitmap with those of an existing
 * scaled bitmaps and reusing the existing scaled bitmap
 * if there is an exact match.
 */
typedef struct
{
    ALTIA_UINT32          id;
    ALTIA_UINT32          maskId;
    ALTIA_UINT32          origId;
    ALTIA_UINT32          origMaskId;
    ALTIA_INT             hasAlpha;


    ALTIA_FIXPT           a00;
    ALTIA_FIXPT           a01;
    ALTIA_FIXPT           a10;
    ALTIA_FIXPT           a11;


    ALTIA_INT             cnt;
}  Altia_Scaled_Raster_type;

typedef struct
{
    ALTIA_UINT32          saveBits;
    ALTIA_UINT32          saveMaskBits;
    ALTIA_UINT32          origId;
    ALTIA_UINT32          origMaskId;
    ALTIA_INT             cnt;
}  Altia_Scaled_Save_Bits_type;

typedef struct
{
    ALTIA_BINDEX rpixels;
    ALTIA_BINDEX gpixels;
    ALTIA_BINDEX bpixels;
    ALTIA_BINDEX apixels;
    ALTIA_BINDEX mask;
    ALTIA_INDEX colors;
    ALTIA_UINT32 rCnt;
    ALTIA_UINT32 gCnt;
    ALTIA_UINT32 bCnt;
    ALTIA_UINT32 aCnt;
    ALTIA_UINT32 maskCnt;
    ALTIA_SHORT colorCnt;
    ALTIA_SHORT rasterType;
    ALTIA_COORD x;
    ALTIA_COORD y;
    ALTIA_SHORT width;
    ALTIA_SHORT height;
    ALTIA_UINT32 id;
    ALTIA_UINT32 maskId;
    ALTIA_UINT32 saveMaskBits;

    ALTIA_UINT32 saveBits;


    ALTIA_UINT32 scaledId;
    ALTIA_UINT32 scaledMaskId;
    ALTIA_UINT32 scaledMaskBits;


} Altia_Raster_type;

typedef struct
{
    ALTIA_UINT32 id;
    ALTIA_UINT32 maskId;
    ALTIA_UINT32 saveMaskBits;

    ALTIA_UINT32 saveBits;


    ALTIA_UINT32 scaledId;
    ALTIA_UINT32 scaledMaskId;
    ALTIA_UINT32 scaledMaskBits;

} Altia_RasterID_type;
typedef struct
{
    ALTIA_INT32 rpixels:14;
    ALTIA_INT32 gpixels:14;
    ALTIA_INT32 rasterType:3;
    ALTIA_INT32 bpixels:14;
    ALTIA_INT32 apixels:14;
    ALTIA_INT32 mask:2;
    ALTIA_INT32 colors:2;
    ALTIA_INT32 rCnt:12;
    ALTIA_INT32 gCnt:12;
    ALTIA_INT32 maskCnt:2;
    ALTIA_INT32 colorCnt:2;
    ALTIA_INT32 x:2;
    ALTIA_INT32 y:2;
    ALTIA_INT32 bCnt:12;
    ALTIA_INT32 aCnt:12;
    ALTIA_INT32 width:10;
    ALTIA_INT32 height:9;
} Altia_RasterROM_type;

typedef struct
{
    ALTIA_BINDEX  bits;
    ALTIA_UINT32 bitCnt;
    ALTIA_COORD  x;
    ALTIA_COORD  y;
    ALTIA_SHORT  width;
    ALTIA_SHORT  height;
    ALTIA_UINT32 id;
    ALTIA_UINT32 saveBits;
} Altia_Stencil_type;

typedef struct
{
    ALTIA_UINT32 id;
    ALTIA_UINT32 saveBits;
} Altia_StencilID_type;

typedef struct
{
    ALTIA_BOOLEAN routing;
    ALTIA_BOOLEAN append;
    ALTIA_BOOLEAN clearNext;
    ALTIA_BYTE    justify;
    ALTIA_BYTE    base;
    ALTIA_BYTE    decPts;
    ALTIA_SHORT   max_count;
    ALTIA_SHORT   max_pixel_count;
    ALTIA_INDEX   labelIndex;
    ALTIA_SHORT   lengthMode;


    ALTIA_INDEX   inputIndex;

} Altia_Textio_type;

typedef struct
{
    ALTIA_INT32 routing:2;
    ALTIA_INT32 append:2;
    ALTIA_INT32 clearNext:2;
    ALTIA_INT32 justify:4;
    ALTIA_INT32 base:4;
    ALTIA_INT32 decPts:8;
    ALTIA_INT32 max_count:16;
    ALTIA_INT32 max_pixel_count:16;
    ALTIA_INT32 lengthMode:4;
} Altia_TextioRAM_type;

typedef struct
{
    ALTIA_INT8  labelIndex:2;
    ALTIA_INT8  inputIndex:2;
} Altia_TextioROM_type;

typedef struct
{
    /* Only variables that are initialized to 0  */

    ALTIA_CHAR  dispStr[64 + 1];


    Altia_CustomSValue_type *custS;

} Altia_TextioBSS_type;

typedef struct
{

    /* Bit-packed input data */
    ALTIA_USHORT clip:1;
    ALTIA_USHORT scroll_on:1;
    ALTIA_USHORT cursor:6;
    ALTIA_USHORT actual_scroll_on:1;
    ALTIA_USHORT selectOn:1;
    ALTIA_USHORT shortcutOn:1;
    ALTIA_USHORT jumpOn:1;
    ALTIA_SHORT  start;
    ALTIA_SHORT  selectStart;
    ALTIA_SHORT  selectEnd;
    ALTIA_COLOR  color;


} Altia_TextioInput_type;

typedef struct
{
    /* Only variables that are initialized to 0  */
    ALTIA_CHAR    hlcolor[25];
} Altia_TextioInputBSS_type;

typedef struct
{
    ALTIA_SHORT         count;
    ALTIA_SHORT         width;
    ALTIA_SHORT         height;
    ALTIA_SHORT         index;
    Altia_Extent_type   extent;
    Altia_Extent_type   drawExtent;

    Altia_Coord_type    coord[256];


    ALTIA_INT		numMallocd;
} Altia_LinePlot_type;

typedef struct
{
    ALTIA_SHORT         count;
    ALTIA_SHORT         index;
    Altia_Extent_type   extent;
    Altia_Extent_type   drawExtent;

    Altia_Coord_type    coord[256];


    ALTIA_INT		numMallocd;
} Altia_DynamicPoly_type;

typedef struct
{
    ALTIA_SHORT         count;
    ALTIA_SHORT         index;
    Altia_Extent_type   extent;
    Altia_Extent_type   drawExtent;

    Altia_Coord_type    coord[256];


    ALTIA_INT		numMallocd;
} Altia_DynamicLine_type;

typedef struct
{
    ALTIA_SHORT         count;
    ALTIA_SHORT         width;
    ALTIA_SHORT         height;
    ALTIA_SHORT         index;
    Altia_Extent_type   extent;
    Altia_Extent_type   drawExtent;

    Altia_Coord_type    coord[256];


    ALTIA_INT		numMallocd;
} Altia_FillPolyPlot_type;



typedef struct
{
    ALTIA_COORD  deltaX;
    ALTIA_COORD  deltaY;
    ALTIA_SHORT  width;
    ALTIA_SHORT  height;
} Altia_RectInput_type;

typedef struct
{
    ALTIA_BYTE   direction;
    ALTIA_SHORT  radius;
    ALTIA_SHORT  in_radius;
    ALTIA_FLOAT  langle;
    ALTIA_FLOAT  tangle;
} Altia_PolarInput_type;

typedef struct
{
    ALTIA_BOOLEAN allKeys;
    ALTIA_BYTE    buttonMask;
    ALTIA_UINT32  mask;
    ALTIA_UINT32  keyCode;
    ALTIA_CHAR    keyString[8]; /* must use ascii instead of keycode to be
                                 * portable */
} Altia_SensorInput_type;


typedef struct
{
    ALTIA_USHORT    enableCmd  : 3; /* an input command type */
    ALTIA_USHORT    executeCmd : 3; /* an input command type */
    ALTIA_USHORT    areaType   : 2; /* The Type the area index is */
    ALTIA_USHORT    outside    : 1;
    ALTIA_USHORT    ivalid     : 1;
    ALTIA_INDEX     sensor;         /* index to Altia_SensorInput_type */
    ALTIA_INDEX     area;
    ALTIA_CHAR  *   outputFunc;
    ALTIA_CHAR  *   enableFunc;
    AltiaEventType  multiple;       /* the multiple to interpolate by */
    AltiaEventType  outputValue;
    AltiaEventType  enableValue;
    AltiaEventType  ivalue;
} Altia_IStateObj_type;

/* Each object with input will have an entry in the sequence array.
 * The sequence array is processed back to front since input is in
 * the reverse order to the order in the universe.
 */
typedef struct
{
    ALTIA_INDEX   gIndex;       /* index into obj array based on objType */
    ALTIA_INDEX   first;        /* index to the first Altia_Input_type */
    ALTIA_UBYTE   count;        /* number of input types */
    ALTIA_BYTE    objType;
    ALTIA_BOOLEAN enabled;      /* If the input is enabled */
    ALTIA_BOOLEAN enabledOnly;  /* If the input is enabled via enable only */
} Altia_InputSequence_type;

typedef struct
{
    ALTIA_BYTE   eventType;

    ALTIA_COORD  x;
    ALTIA_COORD  y;
    ALTIA_UINT32 button;
    ALTIA_CHAR   keyString[8]; /* must use ascii instead of keycode to be */
} Altia_InputEvent_type;

typedef struct
{
    ALTIA_BYTE    type;
    ALTIA_CHAR    *name;
} Altia_LinkConnection_type;

typedef struct
{
    ALTIA_BOOLEAN inRoute;
    ALTIA_INDEX  firstLink;
    ALTIA_SHORT  count;

} Altia_Connection_type;

typedef struct
{
    ALTIA_BYTE    enableCmd;
    ALTIA_BYTE    executeCmd;
    AltiaEventType enableVal;
    AltiaEventType outputVal;
    ALTIA_CHAR    *outputFunc;
    ALTIA_CHAR    *enableFunc;
} Altia_TimerState_type;

typedef struct
{
    ALTIA_SHORT         pointCount;
    ALTIA_SHORT         width;
    ALTIA_SHORT         height;
    ALTIA_SHORT         xinc;
    ALTIA_SHORT         right;
    ALTIA_SHORT         size;
    ALTIA_SHORT         ymax;

    Altia_Coord_type    coord[256];


    ALTIA_INT		numMallocd;
} Altia_StripChart_type;

typedef struct
{
    ALTIA_INDEX             index;
    ALTIA_UINT32            startTime;
    ALTIA_BOOLEAN           internal;
} AltiaTimerTime_t;

typedef struct
{
    ALTIA_UINT32            parm;
    AltiaInternalTimerFunc  funcptr;
    ALTIA_UINT32            msecs;
} AltiaInternalTimer_t;

typedef struct
{
    ALTIA_BYTE    startCmd;
    ALTIA_BYTE    stopCmd;
    ALTIA_BOOLEAN stopped;
    AltiaEventType startVal;
    AltiaEventType stopVal;
    ALTIA_UINT32  msecs;
    ALTIA_INDEX   firstState;
    ALTIA_SHORT   count;

} Altia_Timer_type;

typedef struct
{
    ALTIA_INT32 startCmd:2;
    ALTIA_INT32 stopCmd:2;
    ALTIA_INT32 startVal:2;
    ALTIA_INT32 stopVal:2;
    ALTIA_INT32 msecs:9;
    ALTIA_INT32 firstState:2;
    ALTIA_INT32 count:2;
} Altia_TimerROM_type;

typedef struct
{
    ALTIA_INT8  stopped;
} Altia_TimerRAM_type;

typedef struct
{
    ALTIA_INT         arcRadius;
    ALTIA_INT         tickRadius;
    ALTIA_INT         startAngle;
    ALTIA_INT         span;
    ALTIA_INT         majorNum;
    ALTIA_INT         minorNum;
    ALTIA_INT         subMinorNum;
    ALTIA_INT         majorLen;
    ALTIA_INT         minorLen;
    ALTIA_INT         subMinorLen;
    ALTIA_INT         majorWidth;
    ALTIA_INT         minorWidth;
    ALTIA_INT         subMinorWidth;
    ALTIA_INT         arcWidth;
    ALTIA_INT         majorShape;
    ALTIA_INT         minorShape;
    ALTIA_INT         subMinorShape;
    ALTIA_INT         tickPos;
    ALTIA_INT         arcPos;
    ALTIA_INT         labelPos;
    ALTIA_FLOAT       initLabel;
    ALTIA_FLOAT       labelInc;
    ALTIA_INDEX       majorBrush;
    ALTIA_INDEX       minorBrush;
    ALTIA_INDEX       subMinorBrush;
    ALTIA_INDEX       font;
    Altia_Extent_type	arcExtent;

    Altia_Coord_type  majorCoord[2* 36];
    Altia_Coord_type  minorCoord[2* (8)*(36 -1)];
    Altia_Coord_type  subMinorCoord[2* (4)*(8 +1)*(36 -1)];
    Altia_Label_type       label[36];
    Altia_Transform_type   labeltr[36];
    ALTIA_CHAR	      labelString[36][8];


    ALTIA_INT	      numMajorMallocd;
    ALTIA_INT	      numMinorMallocd;
    ALTIA_INT	      numSubMinorMallocd;
    ALTIA_INT	      numLabelStringMallocd;
} Altia_Tick_type;

typedef ALTIA_BOOLEAN (*Altia_WhenFunc_type)(
    AltiaEventType value
);

typedef struct
{
    ALTIA_COLOR      bgcolor;
    ALTIA_COLOR      shadow;
    ALTIA_COLOR      highlight;
    ALTIA_INT        activate;
    Altia_Coord_type p0;
    Altia_Coord_type p1;
} Altia_Rect3d_type;

// structure for holding each font tag's information
typedef struct
{
    ALTIA_INT   startline; // Line where tag begins
    ALTIA_INT   startpos;  // Character position of tag in startline
    ALTIA_INT   endline;   // Line where tag ends
    ALTIA_COLOR color;     // font tag color
    ALTIA_INT   size;      // Size of font-tagged text. In the case where
                           // a tag ends on a different line, size is set
                           // to number of chars remaining on the endline
}Altia_FontTagArray_type;

// Enumeration for state machine that identifies a font tag as
//  each character is appended to MLTO
typedef enum {
    FONT_TAG_IDLE_ST,
    FONT_START_TAG_START_ST,
    FONT_START_TAG_COLOR_VALUE_ST,
    FONT_START_TAG_ENDQUOTE_ST,
    FONT_START_TAG_END_ST,
    FONT_START_TAG_DETECTED_ST,
    FONT_END_TAG_START_ST,
    FONT_END_TAG_DETECTED_ST
} FontTagDetectionStateType;

typedef struct
{
    Altia_CustomSValue_type *custS;
} Altia_MLineTextBSS_type;

typedef struct
{
    ALTIA_BOOLEAN routing;
    ALTIA_BOOLEAN append;
    ALTIA_BOOLEAN clearNext;
    ALTIA_BOOLEAN whDirty;
    ALTIA_BYTE    justify;
    ALTIA_BYTE    vertstyle;
    ALTIA_SHORT   max_count;

    ALTIA_SHORT   hilight;
    ALTIA_SHORT   prevSelection;
    ALTIA_SHORT   leftpad;
    ALTIA_SHORT   rightpad;
    ALTIA_SHORT   vertpad;
    ALTIA_INT     hlwidth;
    ALTIA_COLOR   _color;
    ALTIA_INDEX   labelIndex;
    ALTIA_BOOLEAN insert;
    ALTIA_BOOLEAN del;
    ALTIA_SHORT   max_pixels;
    ALTIA_SHORT   setline;
    ALTIA_INT     width;
    ALTIA_INT     height;
    ALTIA_INT     insertIndex;
    ALTIA_INT     insertMallocSize;
    ALTIA_SHORT   firstLetter;


#ifdef ALTIA_USING_mlinestrsize
    ALTIA_CHAR  insertString[96 + 1];
#else
    ALTIA_CHAR  insertString[ALTIA_CUSTOMSVALUE_SIZE + 1];
#endif



#ifdef ALTIA_USING_mlinestrsize
    ALTIA_CHAR    dispStr[96 + 1];
#else
    ALTIA_CHAR    dispStr[ALTIA_CUSTOMSVALUE_SIZE + 1];
#endif


    ALTIA_CHAR    _hlcolor[25];
} Altia_MLineText_type;

typedef struct
{
    ALTIA_COLOR color;
    ALTIA_UBYTE percent;
    ALTIA_UBYTE num;

    ALTIA_CHAR    labelStr[64];


} Altia_PieSlice_type;

typedef struct
{
    ALTIA_INDEX sliceIdx;
    ALTIA_INDEX font;
    ALTIA_SHORT sliceCnt;
    ALTIA_SHORT maxCnt;
    ALTIA_SHORT curSlice;
    ALTIA_COORD radius;

} Altia_Pie_type;

typedef struct
{
    ALTIA_CHAR  encoding;
    ALTIA_UBYTE width;
    ALTIA_UBYTE bitWidth;
    ALTIA_BYTE offsetx;
    ALTIA_SHORT bitCnt;
    ALTIA_FBINDEX bits;
#ifdef ALTIA_SAVE_FONT_BITMAP
    ALTIA_UINT32 bitmap;
#endif
} Altia_FontChar_type;

typedef struct
{
    ALTIA_INT32 encoding:9;
    ALTIA_INT32 width:5;
    ALTIA_INT32 offsetx:2;
    ALTIA_INT32 bits:16;
    ALTIA_INT16 bitWidth:5;
    ALTIA_INT16 bitCnt:8;
} Altia_FontCharROM_type;

typedef struct
{
    ALTIA_USHORT charCnt;
    ALTIA_INDEX firstChar;
    ALTIA_SHORT maxWidth;
    ALTIA_SHORT height;
    ALTIA_INDEX fontIdx;
} Altia_FontHeader_type;

typedef struct
{
    ALTIA_SHORT          width;
    ALTIA_SHORT          height;
    ALTIA_INDEX          objIndex;
    ALTIA_COORD          x;
    ALTIA_COORD          y;

} Altia_DrawingArea_type;


typedef struct
{
    ALTIA_CHAR  *name;
    ALTIA_INDEX specIndex;
    ALTIA_SHORT specCnt;
} Altia_Property_type;

typedef struct
{
    ALTIA_UBYTE useType;
    ALTIA_UBYTE specType;
    ALTIA_UBYTE valueType;
    /* flag bits, bit number 1 = bg color */
    ALTIA_UBYTE flag;
    /* name does not get changed so it can be a pointer
     * to a string literal.
     */
    ALTIA_CHAR  *name;
    AltiaEventType value;
} Altia_Specifier_type;


typedef struct
{
    ALTIA_BOOLEAN show;
    ALTIA_BOOLEAN wait;
    ALTIA_INDEX   labelIndex;
    ALTIA_BOOLEAN routing;


    ALTIA_CHAR    snd_string[96];



} Altia_SoundObj_type;

typedef struct
{

    ALTIA_CHAR rasterName[64];


    ALTIA_INDEX rasterIdx;
} Altia_ImageObj_type;

typedef struct
{
    ALTIA_CHAR * name;
    ALTIA_INDEX child;
    ALTIA_SHORT count;
} Altia_NameData_type;

typedef struct
{
    ALTIA_INT    idx;
    ALTIA_SHORT  count;
    ALTIA_UBYTE  cmd;
    ALTIA_FLOAT  brush;
    Altia_Coord_type pos;
    ALTIA_BYTE   anchor;
    ALTIA_INDEX  fontidx;
    ALTIA_UINT32 fg;
    ALTIA_UINT32 bg;
    ALTIA_CHAR * image;
} Altia_SkinData_type;

typedef struct
{

    ALTIA_FILE_CHAR skinName[64];
    ALTIA_FILE_CHAR skinPath[64];


    ALTIA_INDEX id;
    ALTIA_INT count;
    Altia_SkinData_type * data;
} Altia_SkinObj_type;

typedef struct
{
    ALTIA_INDEX idx;
    ALTIA_CHAR * str;
} Altia_LangData_type;

typedef struct
{

    ALTIA_FILE_CHAR langName[64];


    ALTIA_INDEX idx;
    ALTIA_INT count;
    Altia_LangData_type * data;
} Altia_LangObj_type;

typedef struct
{
    ALTIA_INDEX rasterIdx;
    ALTIA_COORD srcX;
    ALTIA_COORD srcY;
    ALTIA_INT srcW;
    ALTIA_INT srcH;
    ALTIA_INT32 srcObj;
    ALTIA_INT srcLayer;
    ALTIA_INT aMode;
    ALTIA_BOOLEAN clrBm;
    ALTIA_BOOLEAN routing;
} Altia_Snapshot_type;

typedef struct
{
    ALTIA_BOOLEAN routing;
    ALTIA_INDEX rasterIdx;
    ALTIA_INT32 fileIdx; /* ALTIA_INDEX is unsigned short? */
    ALTIA_UINT32 fileLen;
    ALTIA_INT32 compIdx; /* Companion file */
    ALTIA_UINT32 compLen;
    void *tdSceneOSG;

    ALTIA_INT render;
    ALTIA_INT width;
    ALTIA_INT height;
    ALTIA_CHAR *meshFn;

    ALTIA_CHAR *ltName;
    ALTIA_INT ltId;
    ALTIA_INT ltType;
    ALTIA_INT ltOn;
    ALTIA_DOUBLE ltPosX;
    ALTIA_DOUBLE ltPosY;
    ALTIA_DOUBLE ltPosZ;
    ALTIA_DOUBLE ltDirX;
    ALTIA_DOUBLE ltDirY;
    ALTIA_DOUBLE ltDirZ;
    ALTIA_DOUBLE ltIntensity;
    ALTIA_COLOR ltColorDiff;
    ALTIA_COLOR ltColorSpec;
    ALTIA_COLOR ltColorAmb;
    ALTIA_DOUBLE ltAttenConst;
    ALTIA_DOUBLE ltAttenLinear;
    ALTIA_DOUBLE ltAttenQuad;
    ALTIA_DOUBLE ltSpotCutoff;
    ALTIA_DOUBLE ltSpotFade;

    ALTIA_CHAR *camName;
    ALTIA_INT camId;
    ALTIA_DOUBLE camFovy;
    ALTIA_DOUBLE camAspect;
    ALTIA_INT camMode;
    ALTIA_DOUBLE camPosX;
    ALTIA_DOUBLE camPosY;
    ALTIA_DOUBLE camPosZ;
    ALTIA_DOUBLE camTgtX;
    ALTIA_DOUBLE camTgtY;
    ALTIA_DOUBLE camTgtZ;
    ALTIA_DOUBLE camUpX;
    ALTIA_DOUBLE camUpY;
    ALTIA_DOUBLE camUpZ;

    ALTIA_CHAR *animName;
    ALTIA_INT animId;
    ALTIA_DOUBLE animVal;
    ALTIA_DOUBLE animSpd;
    ALTIA_INT animCmd;
    ALTIA_INT animDone;
} Altia_TDScene_type;

typedef struct
{
    ALTIA_CHAR *libName;
}Altia_ExInitialize_type;

/* Transparent Work Data Definition */
typedef struct
{
    ALTIA_INT transColor;
    Altia_Extent_type transLimit;
    ALTIA_UINT32 transRgn;
    ALTIA_UBYTE* transBits;
} Altia_Transparent_Data;

/* The first part of this structure must match the drawing area
 * structure since this structure will be passed around as a
 * drawing area structure.
 */
typedef struct
{
    ALTIA_SHORT          width;
    ALTIA_SHORT          height;
    ALTIA_INDEX          objIndex;
    ALTIA_COORD          x;
    ALTIA_COORD          y;

    ALTIA_CHAR  *name;
    ALTIA_CHAR  *cameraName;
    ALTIA_CHAR  *animationName;
    ALTIA_INT    rate;
    ALTIA_INT    flags;
    ALTIA_INT    lightType;
    ALTIA_FLOAT  lightRadius;
    ALTIA_INT    lightAmbientColor;
    ALTIA_INT    lightDiffuseColor;
    ALTIA_INT    lightSpecularColor;
    ALTIA_BOOLEAN animate;
    ALTIA_FLOAT  cdirX;
    ALTIA_FLOAT  cdirY;
    ALTIA_FLOAT  cdirZ;
    ALTIA_FLOAT  clocX;
    ALTIA_FLOAT  clocY;
    ALTIA_FLOAT  clocZ;
    ALTIA_FLOAT  lightX;
    ALTIA_FLOAT  lightY;
    ALTIA_FLOAT  lightZ;
    ALTIA_INT    timerId;
#ifdef RUNGLDATA
    ALTIA_UINT32 glasetIdx;
    ALTIA_INT    glasetCnt;
    ALTIA_UINT32 glatrackIdx;
    ALTIA_INT    glatrackCnt;
    ALTIA_UINT32 glquatIdx;
    ALTIA_INT    glquatCnt;
    ALTIA_UINT32 glvectIdx;
    ALTIA_INT    glvectCnt;
    ALTIA_UINT32 glmatrixIdx;
    ALTIA_INT    glmatrixCnt;
    ALTIA_UINT32 glatimeIdx;
    ALTIA_INT    glatimeCnt;
    ALTIA_UINT32 glvertIdx;
    ALTIA_INT    glvertCnt;
    ALTIA_UINT32 glvertArrIdx;
    ALTIA_INT    glvertArrCnt;
    ALTIA_UINT32 glweightIdx;
    ALTIA_INT    glweightCnt;
    ALTIA_UINT32 gljointIdx;
    ALTIA_INT    gljointCnt;
    ALTIA_UINT32 glvert3dIdx;
    ALTIA_INT    glvert3dCnt;
    ALTIA_UINT32 glindicesIdx;
    ALTIA_INT    glindicesCnt;
    ALTIA_UINT32 glmeshbuffIdx;
    ALTIA_INT    glmeshbuffCnt;
    ALTIA_UINT32 glmatseqIdx;
    ALTIA_INT    glmatseqCnt;
    ALTIA_UINT32 glseqIdx;
    ALTIA_INT    glseqCnt;
    ALTIA_UINT32 gltypesIdx;
    ALTIA_INT    gltypesCnt;
#endif
} Altia_OpenGLObj_type;

typedef struct
{
    /* DAO specifics */
    ALTIA_SHORT         width;
    ALTIA_SHORT         height;
    ALTIA_INDEX         objIndex;
    ALTIA_COORD         x;
    ALTIA_COORD         y;

    /* FlashPlayer specifics */

    ALTIA_BYTE filename[64];


    ALTIA_BOOLEAN initialized;
    ALTIA_BOOLEAN routing;
} Altia_FlashPlayer_type;

typedef short ALTIA_TOKEN_CHAR;

/* For token value */
typedef enum
{
    ALTIA_TOKEN_VAL_INT,
    ALTIA_TOKEN_VAL_STR,
    ALTIA_TOKEN_VAL_FLOAT
} Altia_TokenValue_type;

/* Token structure */
typedef struct
{
    Altia_TokenValue_type val;
    ALTIA_CHAR * str;
} Altia_Token_type;

typedef struct
{
    ALTIA_SHORT     count;
    Altia_Token_type * data;
} Altia_TokenList_type;

typedef struct {
    ALTIA_INT cnt;
    ALTIA_INDEX cmdIdx;
    ALTIA_INDEX paintIdx;

} Altia_PathObject_type;

typedef struct {
    ALTIA_CHAR cmd;
    ALTIA_INDEX paramIdx;
} Altia_PathCmd_type;

typedef struct {
    ALTIA_FLOAT param;
} Altia_PathParam_type;

typedef struct {
    ALTIA_BOOLEAN stroke;
    ALTIA_COLOR strokeColor;
    ALTIA_FLOAT strokeWidth;
    ALTIA_INT endCap;
    ALTIA_INT jointStyle;
    ALTIA_INDEX dashIdx;
    ALTIA_INT dashCnt;
    ALTIA_INT fillType;
    ALTIA_COLOR fillColor;
    ALTIA_INDEX linearIdx;
    ALTIA_INDEX radialIdx;
    ALTIA_INDEX imageIdx;
} Altia_VectorPaint_type;

typedef struct {
    ALTIA_FLOAT directionX1;
    ALTIA_FLOAT directionY1;
    ALTIA_FLOAT directionX2;
    ALTIA_FLOAT directionY2;
    ALTIA_INDEX stopIdx;
    ALTIA_INT   stopCnt;
    ALTIA_FLOAT gradTrans[6];
} Altia_LinearGradient_type;

typedef struct {
    ALTIA_FLOAT centerX;
    ALTIA_FLOAT centerY;
    ALTIA_FLOAT radius;
    ALTIA_FLOAT focusX;
    ALTIA_FLOAT focusY;
    ALTIA_INDEX stopIdx;
    ALTIA_INT   stopCnt;
    ALTIA_FLOAT gradTrans[6];
} Altia_RadialGradient_type;

typedef struct {
    ALTIA_COLOR color;
    ALTIA_FLOAT offset;
} Altia_GradientStop_type;

typedef struct {
    ALTIA_FLOAT dash;
} Altia_Dash_type;

typedef struct {
    ALTIA_FLOAT stdDevX;
    ALTIA_FLOAT stdDevY;
    ALTIA_BOOLEAN stdDevLinked;
    ALTIA_BOOLEAN routing;
} Altia_BlurFilter_type;

/* Altia Tasking Types */
typedef struct
{
    ALTIA_UINT32 type :8;
    ALTIA_UINT32 anim :24;
    AltiaEventType event;
} AltiaTaskElementType;

typedef struct
{
    ALTIA_UINT32 type :8;
    ALTIA_UINT32 idx  :24;
    Altia_GraphicState_type gs;
} AltiaObjectElementType;

typedef struct
{
    ALTIA_INT32 size;
    ALTIA_INT32 current;
    ALTIA_INT32 peak;
    ALTIA_INT32 error;
    ALTIA_INT32 time;
    ALTIA_INT32 id;
} AltiaQueueStatusType;

#endif /* ALTIATYPES_H */
