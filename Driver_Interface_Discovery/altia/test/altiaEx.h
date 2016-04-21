/* $Revision: 1.4 $    $Date: 2008-08-05 15:47:37 $
 * Copyright (c) 2001-2004 Altia Inc.
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


#ifndef ALTIAEX_H
#define ALTIAEX_H

/* This version of altiaEx.h is for use with DeepScreen.
 * Make sure the DeepScreen generated code folder is listed
 * in the include paths for the compilation.
 */
#include "altiaBase.h"

/* version 4 */

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


/* Using fixed point math */
#ifndef ALTIA_DOUBLE
#define ALTIA_DOUBLE long
#endif

/* Define the min and max value used by the plot */
#include <limits.h>
#ifndef DBL_MIN
    #ifndef LONG_MIN
        #define DBL_MIN ((ALTIA_DOUBLE)0x80000000L)
    #else
        #define DBL_MIN ((ALTIA_DOUBLE)LONG_MIN)
    #endif
#endif
#ifndef DBL_MAX
    #ifndef LONG_MAX
        #define DBL_MAX ((ALTIA_DOUBLE)0x7FFFFFFFL)
    #else
        #define DBL_MAX ((ALTIA_DOUBLE)LONG_MAX)
    #endif
#endif

#ifndef ALTIA_FLOAT
#define ALTIA_FLOAT long
#endif



#ifndef ALTIA_CHAR
#define ALTIA_CHAR char
#endif

#ifndef ALT_TEXT
#define ALT_TEXT
#endif

#ifndef ALTIA_SHORT
#define ALTIA_SHORT short
#endif

#ifndef ALTIA_BYTE
#define ALTIA_BYTE char
#endif

#ifndef ALTIA_UINT32
#define ALTIA_UINT32 unsigned long
#endif

#ifndef ALTIA_COLOR
#define ALTIA_COLOR unsigned long
#endif

#ifndef ALTIA_WINDOW
#define ALTIA_WINDOW void *
#endif

#ifndef ALTIA_COORD
#define ALTIA_COORD int
#endif

#ifndef ALTIA_UBYTE
#define ALTIA_UBYTE unsigned char
#endif

#ifndef ALTIA_INT
#define ALTIA_INT int
#endif

#ifndef ALTIA_BOOLEAN
#define ALTIA_BOOLEAN char
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

typedef struct
{
    ALTIA_CHAR  *name;
    ALTIA_CHAR  *font;
    ALTIA_UINT32 id;
    ALTIA_BOOLEAN wide;
} AltiaEx_Label_type;

typedef struct
{
    ALTIA_FLOAT a00, a01, a10, a11, a20, a21;
    ALTIA_FLOAT angle;
    ALTIA_FLOAT xoffset;
    ALTIA_FLOAT yoffset;     
} AltiaEx_Transform_type;

typedef struct
{
    ALTIA_BYTE   eventType;
    ALTIA_SHORT  x;
    ALTIA_SHORT  y;
    ALTIA_UINT32 button;
    ALTIA_CHAR   keyString[8];
} AltiaEx_InputEvent_type;

typedef struct
{
    ALTIA_COORD x0;
    ALTIA_COORD y0;
    ALTIA_COORD x1;
    ALTIA_COORD y1;
} AltiaEx_Extent_type;

typedef struct
{
    ALTIA_SHORT  lineWidth;
    ALTIA_SHORT  count;
    ALTIA_UINT32 pattern;
    ALTIA_UINT32 id;
} AltiaEx_Brush_type;

typedef struct
{
    ALTIA_SHORT data[16];
    ALTIA_BYTE patType; /* 0 = solid, 1 = clear, 2 = bitmap */
    ALTIA_UINT32 id;
} AltiaEx_Pattern_type;

typedef struct
{
    ALTIA_COLOR      foreground;
    ALTIA_COLOR      background;
    AltiaEx_Brush_type brush;
    AltiaEx_Pattern_type pattern;
    AltiaEx_Extent_type clip;
    ALTIA_COORD      tile_x_origin;
    ALTIA_COORD      tile_y_origin;
    ALTIA_SHORT      alpha; 
} AltiaEx_GraphicState_type;

typedef struct
{
    ALTIA_COORD   x;
    ALTIA_COORD   y;
    ALTIA_BYTE    filled;
    ALTIA_SHORT   r1;
    ALTIA_SHORT   r2;
} AltiaEx_Ellipse_type;

typedef struct
{
    ALTIA_COORD x0;
    ALTIA_COORD y0;
    ALTIA_COORD x1;
    ALTIA_COORD y1;
    ALTIA_BYTE  filled;
} AltiaEx_Rect_type;

typedef struct
{
    ALTIA_COORD x;
    ALTIA_COORD y;
} AltiaEx_Coord_type;

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif
#include "drawEx.h"
#include "plotEx.h"
#include "drawAreaCB.h"

typedef enum
{
    ALTIA_EX_INIT_LIB_NOT_COMPATIBLE=-1,
    ALTIA_EX_INIT_ERROR,
    ALTIA_EX_NOT_INITIALIZED,
    ALTIA_EX_INITIALIZED
}AltiaExInitStatus;
#define ALTIA_EX_INIT_STATUS_T_DEFINED 1

typedef struct
{
    AltiaExGetInfoFunc     getInfo;
    AltiaExUpdateFunc      update;
    AltiaExRegCallbackFunc regCallback;
    AltiaExRouteValueFunc  routeValue;
    AltiaExRouteTextFunc   routeText;
    int version;
    AltiaExRouteConnectionValueFunc routeConnValue;
    AltiaExRouteConnectionTextFunc routeConnText;
    void *unused3;
    void *unused4;
    AltiaDrawFuncs_t       drawfuncs;
    AltiaPlotFuncs_t       plotfuncs;
} AltiaExInit_t;

extern ALTIA_CHAR *altiaExGetErrorString(
    void
);

typedef AltiaExInitStatus (*INIT_EX_FUNC)(
    AltiaExInit_t *initinfo,
    int size
);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* ALTIAEX_H */
