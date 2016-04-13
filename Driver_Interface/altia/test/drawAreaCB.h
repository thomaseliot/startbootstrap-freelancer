/*
 * FILE : $Revision: 1.2 $ $Date: 2006-02-06 14:59:22 $
 *
 * This file contains the callback definitions for the 
 * DrawingAreaObject. These must support c as well as c++.
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
 *
 */

#ifndef DRAWAREACB_H
#define DRAWAREACB_H

#ifndef Altiafp
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus) || defined(_MSC_VER)
#define Altiafp
#else
#undef Altiafp
#endif /* Check for function prototyping requirement */
#endif /* Altiafp */

#define ALT_REDRAW_CALLBACK 1
#define ALT_RESIZE_CALLBACK 2
#define ALT_CLOSE_CALLBACK  3
#define ALT_EVENT_CALLBACK  4
#define ALT_SHOW_CALLBACK  5
#define ALT_RUNMODE_CALLBACK  6
#define ALT_ACTIVATE_CALLBACK  7
#define ALT_MOVE_CALLBACK  8

typedef void (*AltiaDARedrawFunc)(
#ifdef Altiafp
int objId,
void *win,
ALTIA_UINT32 dc,
int  left,
int  bottom,
int  right,
int  top,
int  x,
int  y,
int  xoff,
int  yoff,
AltiaEx_Transform_type *trans
#endif /* Altiafp */
);

typedef int (*AltiaDAResizeFunc)(
#ifdef Altiafp
int objId,
int width,
int height
#endif /* Altiafp */
);

typedef int (*AltiaDACloseFunc)(
#ifdef Altiafp
int objId
#endif /* Altiafp */
);

typedef int (*AltiaDARunModeFunc)(
#ifdef Altiafp
int mode
#endif /* Altiafp */
);

typedef int (*AltiaDAActivateFunc)(
#ifdef Altiafp
void *win,
int active
#endif /* Altiafp */
);

#ifndef ALTIADACALLBACKFUNC
#define ALTIADACALLBACKFUNC
typedef int (*AltiaDACallBkFunc)(
#ifdef Altiafp
int objId
#endif /* Altiafp */
);
#endif

typedef void (*AltiaDAEventFunc)(
#ifdef Altiafp
int objId,
void *win,
AltiaEx_InputEvent_type *event
#endif /* Altiafp */
);

typedef void (*AltiaDAShowFunc)(
#ifdef Altiafp
int objId,
int showFlag
#endif /* Altiafp */
);

typedef void (*AltiaDAMoveFunc)(
#ifdef Altiafp
int objId,
int x,
int y
#endif /* Altiafp */
);
/*
 * If this function changes change the typedef below
 */
extern int altiaExRegCallback(
#ifdef Altiafp
int objId,
int callbkType,
AltiaDACallBkFunc func
#endif /* Altiafp */
);

typedef int (*AltiaExRegCallbackFunc)(
#ifdef Altiafp
int objId,
int callbkType,
AltiaDACallBkFunc func
#endif /* Altiafp */
);

/*
 * If this function changes change the typedef below
 */
extern int altiaExGetInfo(
#ifdef Altiafp
int objId,
void *win,
int *x,
int *y,
int *width,
int *height
#endif /* Altiafp */
);

typedef int (*AltiaExGetInfoFunc)(
#ifdef Altiafp
int objId,
void *win,
int *x,
int *y,
int *width,
int *height
#endif /* Altiafp */
);

/*
 * If this function changes change the typedef below
 */
extern int altiaExUpdate(
#ifdef Altiafp
int objId,
int left,
int bottom,
int right,
int top 
#endif /* Altiafp */
);

typedef int (*AltiaExUpdateFunc)(
#ifdef Altiafp
int objId,
int left,
int bottom,
int right,
int top 
#endif /* Altiafp */
);

extern int altiaExRouteValue(
#ifdef Altiafp
ALTIA_CHAR *eventName,
AltiaEventType value
#endif /* Altiafp */
);

typedef int (*AltiaExRouteValueFunc)(
#ifdef Altiafp
ALTIA_CHAR *eventName,
AltiaEventType value
#endif /* Altiafp */
);

extern int altiaExRouteText(
#ifdef Altiafp
ALTIA_CHAR *eventName,
const ALTIA_CHAR *string 
#endif /* Altiafp */
);

typedef int (*AltiaExRouteTextFunc)(
#ifdef Altiafp
ALTIA_CHAR *eventName,
const ALTIA_CHAR *string
#endif /* Altiafp */
);

extern int altiaExRouteConnectionValue(
#ifdef Altiafp
int objId,
ALTIA_CHAR *eventName,
AltiaEventType value
#endif /* Altiafp */
);

typedef int (*AltiaExRouteConnectionValueFunc)(
#ifdef Altiafp
int objId,
ALTIA_CHAR *eventName,
AltiaEventType value
#endif /* Altiafp */
);

extern int altiaExRouteConnectionText(
#ifdef Altiafp
int objId,
ALTIA_CHAR *eventName,
const ALTIA_CHAR *string 
#endif /* Altiafp */
);

typedef int (*AltiaExRouteConnectionTextFunc)(
#ifdef Altiafp
int objId,
ALTIA_CHAR *eventName,
const ALTIA_CHAR *string
#endif /* Altiafp */
);

#endif /* DRAWAREACB_H */
