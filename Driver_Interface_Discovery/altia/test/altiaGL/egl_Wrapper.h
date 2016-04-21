/* $Revision: 1.24 $    $Date: 2010-04-13 14:34:06 $
 * Copyright (c) 2007 Altia Inc.
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

/** FILE:  egl_Wrapper.h ****************************************************
 **
 ** This file contains declarations for all of the graphics
 ** library data types, macros, and functions, used in the Altia
 ** target specific code.  The complexity of this file depends
 ** on the compatability between the actual target graphics library
 ** elements and the graphics library functions, data types, and
 ** macros used in the Altia target specific code.  There are
 ** three levels of compatability:
 **
 ** Level A
 ** -------
 ** If the graphics functions, data types, and macros used in the Altia
 ** target specific code directly map to actual elements of the target
 ** graphics library, this file might only have to list one or more
 ** target graphics library include files.
 **
 ** Level B
 ** -------
 ** At this level of compatibility, the graphics functions, data types,
 ** and macros used in the Altia target specific code are very close to
 ** the actual elements of the target graphics library.  In this case,
 ** only some additional declarations are needed to "map" elements used
 ** in the Altia code to actual elements.
 **
 ** Level C
 ** -------
 ** At this lowest level of compatibility, the graphics functions, data
 ** types, and macros used in the Altia target specific code don't map
 ** well at all to the real elements of the target graphics library.
 ** In this case, this file declares in detail the functions, data
 ** types, and macros used in the Altia target specific code and
 ** there is an associated "egl_Wrapper.c" file that contains the
 ** definitions for these functions.  These are often referred to as
 ** "Wrapper" functions and that's how this file and an "egl_Wrapper.c"
 ** file get their names.  The bodies of the functions must translate
 ** the data types coming from the Altia code and make calls to the
 ** actual target graphics library functions as needed.  They are the
 ** "wrapping" around target graphics library functions so that the
 ** specifics of the target graphics library are hidden from the Altia
 ** code.
 **
 ** Of course Level A compatability is ideal, but unlikely.
 **
 ** Level B compatability is also unlikely.
 **
 ** The solution for dealing with Level C compatibility is not ideal
 ** because it adds an extra layer of calls between the Altia target
 ** specific code and the actual target graphics library, but it may be
 ** relatively easy to implement for many different graphics libraries.
 ** If there is not a need for color bitmaps or scaled/stretched/rotated
 ** monochrome bitmaps, it isn't terribly difficult to write functions
 ** that map most of the vector drawing, font, and untransformed (i.e.,
 ** not scaled, stretched, or rotated) monochrome bitmap operations
 ** to functions available in another graphics library.
 **
 ** For a port of all Altia features:
 **
 **     Vector drawing
 **     Color bitmap drawing w/ transparent bits and scale/stretch/rotate
 **     Monochrome bitmap drawing with scale/stretch/rotate
 **     Text drawing with scale/stretch/rotate
 **     Bitmap fill patterns for vector objects
 **     Dashed line styles for vector objects
 **
 ** to another graphics library, it is more work, but very doable.  The
 ** Altia target specific code for this target graphics library is very
 ** well commented and organized so that it can serve as the best starting
 ** point for such an effort.
 **
 ***************************************************************************/
#ifndef _EGL_WRAPPER_H
#define _EGL_WRAPPER_H

#include <egl_md.h>
#include <pixmap.h>
#include <pixmapstr.h>
#include <gc.h>


/*****************************************************************
 * Define some very generic macros that are used by the Altia
 * target specific code, but possibly not defined elsewhere.
 *****************************************************************/

#ifndef NULL
#define NULL 0
#endif

#if !defined(FALSE) || (FALSE!=0)
#define FALSE 0
#endif

#if !defined(TRUE) || (TRUE!=1)
#define TRUE 1
#endif

/*****************************************************************
 * Define the graphics library macros used by the Altia target
 * specific code.
 *****************************************************************/


#define ALTIA_PIXMAPS 1


/* If the target graphics library has a single function for opening
 * and initializing the display device, define the next macro as 1
 * (which is the default).  The Altia code will just call egl_DeviceOpen()
 * at initialization time and egl_DeviceClose() at the end.  Otherwise,
 * define it as 0 and at initialization time, the Altia code will call
 * egl_Initialize(), egl_DriverFind() to get a device, event service,
 * and font driver, and egl_EventQCreate() to get an event queue.
 * At the end, the Altia code will call egl_EventQDestroy() and
 * egl_Deinitialize().  As is apparent, defining it as 0 just creates
 * a lot more work for typically no gain with most simple target graphics
 * libraries.
 */
#define EGL_HAS_DEVICEOPEN 1


/* If the target graphics library supports the egl_FontCreateByName()
 * function to create a font directly from a string name, define the
 * next macro as 1 (which is the default).  Otherwise, define it as 0
 * and the Altia code will only call egl_FontCreate() to create a font
 * from a structure of detailed information.
 */
#define EGL_HAS_FONTCREATEBYNAME 1


/* The blit functions (egl_BitmapBlt(), egl_MonoBitmapBlt(), and
 * egl_TransBitmapBlt()) take EGL_FLOAT types for the destX and destY
 * parameters in newer versions of these functions instead of
 * EGL_POS types in earlier versions.  To write driver layer versions
 * of these functions that support both calling styles, use the following
 * define to determine the type for destX and destY parameters.  If the
 * define is non-zero, the type is EGL_FLOAT.  Otherwise, EGL_POS.  If
 * the type is EGL_FLOAT and EGL_POS is desired, use the ALTIA_ROUND
 * macro to copy each parameter into a local variable of type EGL_POS.
 */
#define EGL_HAS_BLT_FLOAT_XY 1


/* If the target graphics library supports drawing multiple connected
 * line segments with the function call egl_Lines(), define the next
 * macro as 1 (which is the default).  Otherwise, define it as 0 and
 * the Altia code will simply call egl_Line() multiple times.  This
 * may not look very good if the line end cap style on the target
 * is too severe (e.g., squared corners).
 */
#define EGL_HAS_LINES 1


/* If the target graphics library needs the egl_MonoBitmapBlt() function
 * to blit monochrome bitmaps (because egl_BitmapBlt() can only blit
 * color bitmaps), define the next macro as 1 (this is the default).
 * Otherwise, define it as 0 and the Altia code will only call
 * egl_BitmapBlt() to blit both monochrome and color bitmaps.
 * In Altia, monochrome bitmaps are used for Stencil objects and the
 * transparent pixels of color bitmaps (Raster objects in Altia).
 */
#ifndef EGL_HAS_MONOBITMAPBLT
#define EGL_HAS_MONOBITMAPBLT 1
#endif


/* For text scaling/stretching/rotating, the job is easiest if
 * the target graphics library properly supports drawing text into
 * a monochrome bitmap (by temporarily setting the bitmap for the GC
 * to the monochrome bitmap and doing a text draw) and reading
 * the monochrome data back using egl_MonoBitmapRead().  If this
 * is the case, define the next macro as 1 (this is the default).
 * Otherwise, define it as 0 and the Altia code will do everything
 * with full color which requires the implementation of the graphics
 * library functions egl_BitmapRead() and egl_ColorConvert().
 * Being able to do a monochrome bitmap read also allows us to reuse
 * an existing monochrome bitmap in the process.  On some targets,
 * this can improve performance.
 *
 * NOTE:  On some targets (e.g., pre X11R6 servers), it is ABSOLUTELY
 *        NECESSARY to reuse the monochrome bitmap because there is a
 *        limited pool of monochrome bitmap IDs available and we can
 *        easily exhaust the pool by frequently creating and destroying
 *        a temporary monochrome bitmap.
 */


#define EGL_HAS_MONOBITMAPREAD 1



/* For Altia Stencil object scaling/stretching/rotating, the job is
 * easiest if the target graphics library properly supports writing
 * monochrome data into an existing monochrome bitmap (this is in
 * addition to being able to read data from an existing monochrome
 * bitmap - see EGL_HAS_MONOBITMAPREAD for more on this).  If the target
 * graphics library supports egl_MonoBitmapWrite(), define the next
 * macro as 1 (this is the default).  Otherwise, define it as 0 and the
 * Altia code will not use the function.  This may also result in the
 * Altia code not using egl_MonoBitmapRead() to its full potential.
 * Being able to do a monochrome bitmap write also allows us to reuse
 * an existing monochrome bitmap in the process.  On some targets,
 * this can improve performance.
 *
 * NOTE:  On some targets (e.g., pre X11R6 servers), it is ABSOLUTELY
 *        NECESSARY to reuse the monochrome bitmap because there is a
 *        limited pool of monochrome bitmap IDs available and we can
 *        easily exhaust the pool by frequently creating and destroying
 *        a temporary monochrome bitmap.
 */


#define EGL_HAS_MONOBITMAPWRITE 1



/* If the target graphics library has its own functions to create
 * a polygon clip region, define the next macro as 1 (the default
 * is 0) and the Altia code will call egl_PolyRegionCreate() and its
 * associated functions.  These are only needed for rendering scaled/
 * stretched/rotated color bitmaps (Raster objects in Altia).  If the
 * macro is defined as 0 (which is the default), the Altia code will
 * call egl_RegionCreate() and its associated functions to approximate
 * a polygon clip region using multiple rectangular regions.  Again,
 * these calls are only needed for rendering scaled/stretched/rotated
 * color bitmaps (Raster objects in Altia).
 */
#define EGL_HAS_POLYREGIONCREATE 0


/* If the target graphics library function egl_TextSizeGet() supports
 * an argument to get the offset of the text, define the next macro as 1
 * (this is the default).  Otherwise, define it as 0 and the Altia code
 * will not call the function with the additional argument.  See the
 * declaration for egl_TextSizeGet() later in this file to study its
 * argument syntax.
 */
#ifndef EGL_HAS_TEXTSIZEGETWITHOFFSET
#define EGL_HAS_TEXTSIZEGETWITHOFFSET 1
#endif


/* If the target graphics library needs the egl_TransBitmapBlt() function
 * to blit a color bitmap with a monochrome bitmap transparency mask
 * (because egl_BitmapBlt() can only blit regular color bitmaps), define
 * the next macro as 1 (this is the default).  Otherwise, define it as
 * 0 and the Altia code will only call egl_BitmapBlt() to blit both
 * regular color bitmaps (Raster objects in Altia) and color bitmaps
 * with monochrome bitmap transparency masks (Raster objects with
 * transparent bits in Altia).
 */
#ifndef EGL_HAS_TRANSBITMAPBLT
#define EGL_HAS_TRANSBITMAPBLT 1
#endif


/* If the target graphics library requires an expicit call to
 * egl_TransBitmapCreate() to create a color bitmap with transparency
 * (Altia Raster object with transparent bits), define the next macro as 1
 * (0 is the default).  With this macro defined as 0, no call is made to
 * create a special transparent bitmap.  Instead, the color bitmap and
 * monochrome bitmap mask are kept as individual entities.
 *
 * Typically, if EGL_HAS_TRANSBITMAPCREATE is defined as 0, then
 * EGL_HAS_TRANSBITMAPBLT is defined as 1 to allow the target graphics
 * library the opportunity to blit the color bitmap and its associated
 * monochrome bitmap mask in whatever special way is necessary for the
 * target to achieve the desired tranparency result.
 *
 * If the next macro is defined as 1, calls are also made to
 * egl_TransBitmapDestroy() to destroy a color bitmap with transparency.
 * With this macro defined as 0, egl_BitmapDestroy() is called to
 * destroy the color bitmap and egl_MonoBitmapDestroy() is called to
 * destroy the monochrome bitmap mask.
 */
#ifndef EGL_HAS_TRANSBITMAPCREATE
#define EGL_HAS_TRANSBITMAPCREATE 0
#endif


/* If the target graphics library has a windowing system and needs to
 * create a window as a drawing destination, define the next macro as 1
 * (0 is the default) and the Altia code will call egl_WindowCreate() to
 * create a window and egl_WindowDestroy() to destroy the window when it
 * is no longer needed.  Otherwise, define it as 0 (which is the default)
 * and the Altia code will not call this function.
 */
#define EGL_HAS_WINDOWCREATE 0

/* If this is set then the user must modify altiaUtils.c and add
 * the SampleSetupPalette function for his hardware.  This allows the
 * user to make palette changes to AltiaGL to work with the hardware
 * OS or other programs.
 */
#define EGL_HAS_INDEXEDSYSTEMCOLORMAP 0


/* Altia draws its own dashed lines for this target using small line
 * segments and polygons.  Define the next macro as 1 (which is the default)
 * and the Altia code in "altiaLibDash.c" will trim off a pixel on the
 * right and bottom edges of any horizontal and vertical line segments
 * used to draw dashed lines.  To tell if this is needed for a particular
 * target graphics library, study the thickness of the small line segments
 * in dashed lines being drawn and if they are too short, define the next
 * macro as 0 instead of 1.
 *
 * miZeroLine and fbZeroLine require a set value of 1
 * fbPolyLine requires a set value of 0
 */
#define EGL_NEEDS_DASHLINESLINETRIM 1


/* Altia draws its own dashed lines for this target using small line
 * segments and polygons.  Define the next macro as 1 (which is the default)
 * and the Altia code in "altiaLibDash.c" will trim off a pixel on the
 * right and bottom edges of any rectangular shaped polygons used in
 * drawing the dashed lines.  To tell if this is needed for a particular
 * target graphics library, study the thickness of the small polygon
 * segments in dashed lines being drawn and if they are too short, define
 * the next macro as 0 instead of 1.
 */
#define EGL_NEEDS_DASHLINESPOLYTRIM 0


/* If the target graphics library needs to reuse scaled raster data,
 * define the next macro as 1 (default is 0).  Reusing scaled raster data
 * does not conserve graphics resources which is a typical goal on
 * targets with limited memory.  However, it does reduce bitmap deletion
 * and creation.  This improves performance.  In addition, it may be
 * necessary on some targets (e.g., pre X11R6 servers) that have a
 * limited pool of bitmap IDs.
 */
#define EGL_NEEDS_SCALEDRASTERREUSE 1


/* When Altia renders scaled/stretched/rotated Raster objects, it first
 * uses device independent bitmap (DIB) data to calculate colors for the
 * transformed version of a Raster.  Ideally, we want to get the DIB without
 * using any graphics calls to read the current DDB data.  This can be
 * done by reading the data from the Altia raster data structure and putting
 * it in a DIB format that makes it easy to access the color information
 * for each pixel.  There is Altia code to do this and if the next macro
 * is defined as 1 (which is the default), the code is used.
 *
 * There is also Altia code available that uses the egl_BitmapRead() and
 * egl_ColorConvert() target graphics library calls to read pixel values
 * from the current DDB and convert them into a DIB format that makes it
 * easy to access the color information for each pixel.  This code is
 * enabled if the next macro is defined as 0 (1 is the default).
 *
 * NOTE:  It should be a rare case that the macro needs to be defined as 0.
 *        The code to get the DIB from the Altia raster data structure
 *        should work on any target.
 */
#define EGL_USE_ALTIABITSFORSCALING 1

/* To enable target graphics library function calls for handling/managing
 * the screen cursor, define the next macro as 1 (0 is the default).  Most
 * graphics libraries manage the cursor on their own by default and this
 * is what is expected except for VxWorks WindML UGL.  If cursor
 * handling/managing is enabled for a graphics library other than VxWorks
 * WindML UGL, it will most likely cause compile time and link errors
 * because the cursor handling functions and data types are not
 * declared or defined except in the generated code for VxWorks WindML
 * UGL.  See "altiaUtils.c" and "altiaInput.c" for examples of how
 * the egl_Cursor*() functions and data types are used.
 */
#ifndef EGL_USE_CURSOR_HANDLING
#define EGL_USE_CURSOR_HANDLING 0
#endif /* !EGL_USE_CURSOR_HANDLING */

/* To enable the transfer of the working raster transform data to the EGL
 * layer define the next macro as 1.  This will override Altia's default
 * pipeline to handle the scaling and rotation of rasters.  Define this
 * in the driver's egl_md.h if it is desired.  Please note that the 
 * code generation option Scale Bitmap/Text must be disabled for this to
 * work.
 */
#ifndef EGL_HAS_RASTER_TRANSFORMATION_LIB
#define EGL_HAS_RASTER_TRANSFORMATION_LIB 0
#endif

/* To force always drawng just a single character at a time for text strings
 * (perhaps because this is the best approach for hardware acceleration),
 * define the next macro as 1.  Defining this macro as 1 is usually done
 * only when the EGL_HAS_RASTER_TRANSFORMATION_LIB macro is defined as 1.
 * Define this in the driver's egl_md.h if it is desired.  Please note that
 * the code generation option Scale Bitmap/Text must be disabled for this
 * to work.
 */
#ifndef EGL_USE_SINGLE_CHAR_DRAW
#define EGL_USE_SINGLE_CHAR_DRAW 0
#endif

#if ALTIA_ALPHA_BLENDING == 1
    #ifdef DRIVER_ALPHA
        #define EGL_DO_ALPHA_BLENDING 0
    #else
        #define EGL_DO_ALPHA_BLENDING 1
    #endif
#else
    #define EGL_DO_ALPHA_BLENDING 0
#endif

/*
 * Macro to merge the alpha value into the color
 */
#ifndef EGL_ADD_ALPHA


    #ifdef DRIVER_ALPHA
        #define EGL_ADD_ALPHA(c,a) (((a) << 24)|(c))
    #else
        /* If alpha is not needed, use this macro to save a shift for speed */
        #define EGL_ADD_ALPHA(c,a) (c)
    #endif /* DRIVER_ALPHA */

#endif /* EGL_ADD_ALPHA */

#if ALTIA_DOUBLE_BUFFERING || ALTIA_PIXMAPS || EGL_DO_ALPHA_BLENDING
    #ifndef USE_FB
        #define USE_FB
    #endif
#endif

/*
 * Simple macros for general use
 */
#define EGL_STATUS_OK            0
#define EGL_STATUS_ERROR         (-1)
#define EGL_STATUS_QUIT          (-2)
#define EGL_STATUS_Q_DATA        (-4)
#define EGL_STATUS_Q_EMPTY       (-5)
#define EGL_NO_WAIT              0
#define EGL_WAIT_FOREVER         (-1)


/*
 * Simple macro defining a type of driver information request.
 */
#define EGL_MODE_INFO_REQ        4



/*
 * Simple macros that define various types of sub-system drivers.
 */
#define EGL_DISPLAY_TYPE         4
#define EGL_EVENT_SERVICE_TYPE   1
#define EGL_FONT_ENGINE_TYPE     8



/*
 * Simple macros for fundamental line styles and colors.
 */
#define EGL_LINE_STYLE_SOLID     0xffffffff

/*
 * To be sure that we don't get the transparent color mixed up with
 * a possible color in a bitmap we make sure the signed bit is set.
 */
#define EGL_COLOR_TRANSPARENT    ((EGL_COLOR)0x80010101L)

/*
 * Alpha channel values range is from 0-255 (transparent -> opaque).
 */
#define EGL_TRANSPARENT_ALPHA 0x0
#define EGL_OPAQUE_ALPHA      0xff

/*
 * Simple macro for making EGL_ARGB from alpha, red, green, and blue
 * components.  The format of the EGL_ARGB is not meant to be public.
 */
#ifndef EGL_MAKE_ARGB
    #define EGL_MAKE_ARGB(alpha, red, green, blue)  \
             (((EGL_ARGB)(alpha) << 24)             \
            | ((EGL_ARGB)(red) << 16)               \
            | ((EGL_ARGB)(green) << 8)              \
            | (EGL_ARGB)(blue))

    #define EGL_ARGB_ALPHA(argb) (EGL_ARGB)(((argb) >> 24) & 0xFF)
    #define EGL_ARGB_RED(argb) (EGL_ARGB)(((argb) >> 16) & 0xFF)
    #define EGL_ARGB_GREEN(argb) (EGL_ARGB)(((argb) >> 8) & 0xFF)
    #define EGL_ARGB_BLUE(argb) (EGL_ARGB)((argb) & 0xFF)
#endif

/*
 * Transparent indexed color definition for cursor
 */
#define EGL_CURSOR_COLOR_TRANSPARENT 0xff

/*
 * Simple macros for identifying the attributes of a font.
 */
#define EGL_FONT_FACE_NAME_MAX_LENGTH   80
#define EGL_FONT_FAMILY_NAME_MAX_LENGTH 40
#define EGL_FONT_DONT_CARE       0
#define EGL_FONT_BOLD_OFF        0
#define EGL_FONT_BOLD_LIGHT      25
#define EGL_FONT_BOLD            50
#define EGL_FONT_UPRIGHT         1
#define EGL_FONT_ITALIC          2
#define EGL_FONT_MONO_SPACED     3
#define EGL_FONT_PROPORTIONAL    4
#define EGL_FONT_ISO_8859_1      5
#define EGL_FONT_UNICODE         6

/*
 * Simple macros for identifying the attributes of a pointer or key event.
 */
#define EGL_MAX_EVENT_SIZE          64  /* maximum event size in bytes */
#define EGL_EVENT_CATEGORY_INPUT    1
#define EGL_EVENT_TYPE_KEYBOARD     1
#define EGL_EVENT_TYPE_POINTER      2
#define EGL_EVENT_TYPE_EXPOSE       3
#define EGL_EVENT_TYPE_RESIZE       4
#define EGL_EVENT_TYPE_CLOSE        5

#define EGL_POINTER_BUTTON1      0x00000001
#define EGL_POINTER_BUTTON2      0x00000002
#define EGL_POINTER_BUTTON3      0x00000004
#define EGL_KEYBOARD_KEY_MASK    0xffff0000
#define EGL_KEYBOARD_KEYDOWN     0x00010000
#define EGL_KEYBOARD_KEYUP       0x00020000

#define EGL_UNI_PRIVATE          0xE000
#define EGL_UNI_DELETE           0x007F
#define EGL_UNI_HOME             (EGL_UNI_PRIVATE + 0)
#define EGL_UNI_END              (EGL_UNI_PRIVATE + 1)
#define EGL_UNI_INSERT           (EGL_UNI_PRIVATE + 2)
#define EGL_UNI_PAGE_UP          (EGL_UNI_PRIVATE + 3)
#define EGL_UNI_PAGE_DOWN        (EGL_UNI_PRIVATE + 4)
#define EGL_UNI_LEFT_ARROW       (EGL_UNI_PRIVATE + 5)
#define EGL_UNI_RIGHT_ARROW      (EGL_UNI_PRIVATE + 6)
#define EGL_UNI_UP_ARROW         (EGL_UNI_PRIVATE + 7)
#define EGL_UNI_DOWN_ARROW       (EGL_UNI_PRIVATE + 8)
#define EGL_UNI_CAPS_LOCK        (EGL_UNI_PRIVATE + 11)
#define EGL_UNI_NUM_LOCK         (EGL_UNI_PRIVATE + 12)
#define EGL_UNI_SCROLL_LOCK      (EGL_UNI_PRIVATE + 13)
#define EGL_UNI_LEFT_SHIFT       (EGL_UNI_PRIVATE + 14)
#define EGL_UNI_RIGHT_SHIFT      (EGL_UNI_PRIVATE + 15)
#define EGL_UNI_LEFT_CTRL        (EGL_UNI_PRIVATE + 16)
#define EGL_UNI_RIGHT_CTRL       (EGL_UNI_PRIVATE + 17)
#define EGL_UNI_LEFT_ALT         (EGL_UNI_PRIVATE + 18)
#define EGL_UNI_RIGHT_ALT        (EGL_UNI_PRIVATE + 19)

#define EGL_UNI_FUNC_BASE        (EGL_UNI_PRIVATE + 0x0f00)
#define EGL_UNI_F1               (EGL_UNI_FUNC_BASE + 1)
#define EGL_UNI_F2               (EGL_UNI_FUNC_BASE + 2)
#define EGL_UNI_F3               (EGL_UNI_FUNC_BASE + 3)
#define EGL_UNI_F4               (EGL_UNI_FUNC_BASE + 4)
#define EGL_UNI_F5               (EGL_UNI_FUNC_BASE + 5)
#define EGL_UNI_F6               (EGL_UNI_FUNC_BASE + 6)
#define EGL_UNI_F7               (EGL_UNI_FUNC_BASE + 7)
#define EGL_UNI_F8               (EGL_UNI_FUNC_BASE + 8)
#define EGL_UNI_F9               (EGL_UNI_FUNC_BASE + 9)
#define EGL_UNI_F10              (EGL_UNI_FUNC_BASE + 10)
#define EGL_UNI_F11              (EGL_UNI_FUNC_BASE + 11)
#define EGL_UNI_F12              (EGL_UNI_FUNC_BASE + 12)



/*
 * Simple macros for setting the creation modes for bitmaps.
 * Used in parameters of type EGL_DIB_CREATE_MODE.
 */
#define EGL_DIB_INIT_DATA        0 /* Initialize the bitmap with i
                                    * the data present within the DIB
                                    */
#define EGL_DIB_INIT_VALUE       1 /* Initialize the bitmap to black */
#define EGL_DIB_INIT_TRANS       2 /* Initialize the bitmap to transparent -
                                      for use with Snapshot object */



/*
 * Simple macros for bitmap image formats.
 */
#define EGL_INDEXED_1            1 /* Image data is 1 bit index into CLUT */
#define EGL_INDEXED_8            8 /* Image data is 8 bit index into CLUT */
#define EGL_DIRECT               0 /* Image data includeds color data */
#define EGL_DIB_DIRECT           EGL_DIRECT



/*
 * Simple macros for bitmap color formats
 * NOTE:  These must match the layer formats from altiaTypes.h!!!!
 */
#define EGL_RGB888               0
#define EGL_ARGB8888             1
#define EGL_RGB565               2
#define EGL_ARGB4444             3
#define EGL_ALPHA8               4
#define EGL_PAL                  5
#define EGL_APAL                 6
#define EGL_FORMAT_COUNT         7

#define EGL_DEVICE_COLOR_32      EGL_ARGB8888 /* The internal color representation. */

/*
 * Simple macros for raster drawing modes.
 * Used in parameters of type EGL_RASTER_OP.
 */
#define EGL_RASTER_OP_COPY       0x02020101L
#define EGL_RASTER_OP_AND        0x00020002L
#define EGL_RASTER_OP_OR         0x00020003L
#define EGL_RASTER_OP_XOR        0x00020004L



/*
 * Simple macros for bitmap drawing areas that are always valid.
 */
#define EGL_DEFAULT_ID           ((EGL_DDB_ID)(NULL))
#define EGL_DISPLAY_ID           ((EGL_DDB_ID)(-1))


/*****************************************************************
 * Define the graphics library data types used by the Altia
 * target specific code.
 *****************************************************************/


/*
 * Simple data types passed to the graphics library and returned by it.
 */

typedef unsigned long  EGL_UINT32;
typedef unsigned short EGL_UINT16;
typedef unsigned char  EGL_UINT8;
typedef signed long    EGL_INT32;
typedef signed short   EGL_INT16;
typedef signed char    EGL_INT8;
typedef signed char    EGL_POS8;
typedef signed short   EGL_POS16;
typedef signed int     EGL_INT;
typedef unsigned int   EGL_UINT;

#ifndef EGL_SIZE
#define EGL_SIZE   EGL_INT32
#endif

#ifndef EGL_ORD
#define EGL_ORD    EGL_INT32
#endif

#ifndef EGL_BOOL
#define EGL_BOOL   EGL_UINT8
#endif

#ifndef EGL_FALSE
#define EGL_FALSE   (EGL_BOOL)0
#endif

#ifndef EGL_TRUE
#define EGL_TRUE    (EGL_BOOL)1
#endif

/* We want the array of EGL_POS to look like any array of EGL_POINT
 * so we set EGL_POS size to be 16 bits.
 */
#ifndef EGL_POS
#define EGL_POS   EGL_POS16
#endif

/* Until code generator auto-adjusts char for
 * different UNICODE targets, just set ALTIA_CHAR
 * based on the definition of preprocessor macros.
 */
#ifndef UNICODE
typedef char EGL_CHAR;
#elif defined(WCHAR_IS_WCHAR_T)
typedef wchar_t EGL_CHAR;
#elif defined(WIN32) || defined(WCHAR_IS_USHORT)
typedef unsigned short EGL_CHAR;
#elif defined(WCHAR_IS_UINT)
typedef unsigned int EGL_CHAR;
#elif defined(WCHAR_IS_ULONG)
typedef unsigned long EGL_CHAR;
#elif defined(WCHAR_IS_SHORT)
typedef short EGL_CHAR;
#elif defined(WCHAR_IS_LONG)
typedef long EGL_CHAR;
#else
typedef int EGL_CHAR;
#endif

typedef unsigned short EGL_WCHAR;

typedef EGL_ORD        EGL_STATUS;
typedef EGL_UINT32     EGL_TIMEOUT;
typedef EGL_UINT32     EGL_TIMESTAMP;
typedef EGL_UINT32     EGL_LINE_STYLE;
typedef EGL_UINT32     EGL_COLOR_FORMAT;


typedef signed long EGL_FLOAT;



typedef struct egl_range
{
    EGL_ORD eglMin;
    EGL_ORD eglMax;
} EGL_RANGE;

typedef struct egl_rect
{
    EGL_POS left;
    EGL_POS top;
    EGL_POS right;
    EGL_POS bottom;
} EGL_RECT;

typedef struct egl_point
{
    EGL_POS16 x;
    EGL_POS16 y;
} EGL_POINT;

typedef struct
{
    EGL_UINT8 red;
    EGL_UINT8 green;
    EGL_UINT8 blue;
} EGL_ARGB_COLOR;

#if EGL_HAS_RASTER_TRANSFORMATION_LIB
typedef struct
{
    EGL_FLOAT a00;
    EGL_FLOAT a01;
    EGL_FLOAT a10;
    EGL_FLOAT a11;
    EGL_FLOAT a20;
    EGL_FLOAT a21;

} EGL_TRANSFORM;
#endif /* EGL_HAS_RASTER_TRANSFORMATION_LIB */

/*
 * Graphics device information data types passed to the graphics library
 * or returned by it.  Note that the EGL_MODE_INFO structure has mostly
 * elements that are not used by Altia.
 */
typedef EGL_UINT32     EGL_INFO_REQ;

/*
 * Various data types returned by the graphics library that have no public
 * content (e.g., device specific color types and subsystem identifiers).
 * They are typically used as-is in other graphics library calls.
 */
typedef EGL_UINT32     EGL_ARGB;
typedef EGL_UINT32     EGL_COLOR;
typedef void          *EGL_ID;
typedef void          *EGL_REGION_ID;
typedef void          *EGL_POLYREGION_ID;
typedef void          *EGL_DEVICE_ID;
typedef void          *EGL_GC_ID;
typedef void          *EGL_FONT_DRIVER_ID;
typedef void          *EGL_EVENT_SERVICE_ID;
typedef void          *EGL_EVENT_Q_ID;
typedef void          *EGL_WINDOW_ID;
typedef void          *EGL_DRAWABLE_ID;
typedef void *         EGL_CHAR_ID;
typedef void *         EGL_FONT_ID;

/*
 * Font data types passed to the graphics library and returned by it.
 * Some have public elements, some have elements not used by Altia,
 * some have no public elements.
 */
typedef struct egl_font_desc
{
    EGL_RANGE pixelSize;        /* average size of font in pixels */
    EGL_RANGE weight;           /* weight is a bold setting from 0 - 100 */
    EGL_ORD italic;             /* Italic is usually either on or off */
    EGL_ORD spacing;            /* Mono spaced or proportional */
    EGL_ORD charSet;            /* ISO 8859-1, Unicode, etc */
    EGL_CHAR faceName[EGL_FONT_FACE_NAME_MAX_LENGTH];    /* face name of font */
    EGL_CHAR familyName[EGL_FONT_FAMILY_NAME_MAX_LENGTH];
} EGL_FONT_DESC;


typedef struct egl_font_desc_priority
{
    EGL_ORD pixelSize;
    EGL_ORD weight;
    EGL_ORD italic;
    EGL_ORD spacing;
    EGL_ORD charSet;
    EGL_ORD faceName;
    EGL_ORD familyName;
} EGL_FONT_DESC_PRIORITY;


typedef struct egl_font_def
{
    EGL_SIZE structSize;  /* normally will equal sizeof(EGL_FONT_DEF) */
    EGL_SIZE pixelSize;   /* average size of font in pixels */
    EGL_SIZE weight;      /* weight is a bold setting from 0 - 100 */
    EGL_ORD  italic;      /* italic is usually either on or off */
    EGL_ORD  charSet;     /* ISO 8859-1, Unicode, etc */
    char     faceName[EGL_FONT_FACE_NAME_MAX_LENGTH];
    char     familyName[EGL_FONT_FAMILY_NAME_MAX_LENGTH];
} EGL_FONT_DEF;


typedef struct egl_font_metrics
{
    EGL_SIZE unusedPixelSize;  /* Average pixel size of font */
    EGL_SIZE unusedWeight;     /* Can be anywhere between 0 - 100 */
    EGL_ORD  unusedItalic;     /* Italicized or not */
    EGL_SIZE height;           /* Average height of characters */
    EGL_SIZE unusedMaxAscent;  /* Maximum ascent above baseline */
    EGL_SIZE maxDescent;       /* Maximum descent below baseline */
    EGL_SIZE unusedMaxAdvance; /* Max char width plus interchar spacing */
    EGL_SIZE unusedLeading;    /* Interline spacing */
    EGL_ORD  spacing;          /* Mono spaced or proportional */
    EGL_ORD  unusedFontType;   /* TrueType, MicroType, PostScript, Bitmapped */
    EGL_ORD  unusedCharSet;    /* ISO 8859-1, Unicode, etc */
    EGL_BOOL unusedScalable;   /* Scalable or not */
    char     unusedFaceName[EGL_FONT_FACE_NAME_MAX_LENGTH];
    char     unusedFamilyName[EGL_FONT_FAMILY_NAME_MAX_LENGTH];
} EGL_FONT_METRICS;



/*
 * Event (key/mouse) data types passed to the graphics library and returned
 * by it. Some have public elements, some have elements not used by Altia,
 * some have no public elements.
 */
typedef struct egl_event_header
{
    EGL_UINT32    type;             /* type of event */
    EGL_UINT32    category;         /* category of event */
    EGL_ID        unusedObjectId;   /* ID used internally for routing */
} EGL_EVENT_HEADER;


typedef struct
{
    EGL_UINT16          unusedScanCode;
    EGL_WCHAR           key;
} EGL_EVENT_KEYBOARD;


typedef struct
{
    EGL_POS             unusedDY;       /* change in Y position */
    EGL_POS             unusedDX;       /* change in X position */
    EGL_UINT16          buttonChange;
    EGL_UINT16          buttonState;
} EGL_EVENT_POINTER;

typedef struct
{
    EGL_UINT16          width;
    EGL_UINT16          height;
    EGL_UINT16          count;
} EGL_EVENT_EXPOSE;

typedef struct
{
    EGL_UINT16          width;
    EGL_UINT16          height;
} EGL_EVENT_RESIZE;

typedef struct egl_event
{
    EGL_EVENT_HEADER  header;           /* event header */
    char              reserved [EGL_MAX_EVENT_SIZE - sizeof (EGL_EVENT_HEADER)];
} EGL_EVENT;


typedef struct egl_input_event
{
    EGL_EVENT_HEADER    header;         /* event identifier */
    EGL_TIMESTAMP       unusedTimeStamp; /* time event queued */
    EGL_POS             x;              /* current X position */
    EGL_POS             y;              /* current Y position */
    EGL_UINT32          window;         /* Window id if there is one */
    EGL_UINT32          modifiers;      /* keyboard modifier keys and
                                         * pointer button states
                                         */
    union
    {
        EGL_EVENT_KEYBOARD  keyboard;   /* keyboard data */
        EGL_EVENT_POINTER   pointer;    /* pointer data */
        EGL_EVENT_EXPOSE    expose;     /* window expose data */
        EGL_EVENT_RESIZE    resize;     /* window resize data */
    } type;
} EGL_INPUT_EVENT;


/*
 * Bitmap data types passed to the graphics library and returned by it.
 * Some have public elements, some have elements not unused by Altia,
 * some have no public elements.
 */
typedef void         *EGL_BITMAP_ID;          /* No public content */
typedef void         *EGL_REFERENCE_ID;       /* reference to source graphic */
typedef EGL_UINT32    EGL_DIB_CREATE_MODE;    /* DIB creation modes */
typedef EGL_UINT32    EGL_RASTER_OP;          /* Raster operation modes */

typedef struct egl_dib
{
    EGL_SIZE          width;       /* width of bitmap image in pixels */
    EGL_SIZE          height;      /* height of bitmap image in pixels */
    EGL_SIZE          stride;      /* distance in pixels between adjacent
                                    * scan lines of the image */
    EGL_POS8          imageFormat; /* format of the index in bitmap image */
    EGL_COLOR_FORMAT  colorFormat; /* format of the image */
    EGL_INT16         clutSize;    /* size of clut in number of elements */
    void             *pClut;       /* Pointer to CLUT */
    void             *pImage;      /* Pointer to image */
} EGL_DIB;


typedef struct egl_mdib
{
    EGL_SIZE        width;     /* width of bitmap image in pixels */
    EGL_SIZE        height;    /* height of bitmap image in pixels */
    EGL_SIZE        stride;    /* distance in pixels between image data rows */
    EGL_UINT8      *pImage;    /* Pointer to image data */
} EGL_MDIB;

typedef struct egl_cdib
{
    EGL_SIZE        width;      /* width of cursor in pixels */
    EGL_SIZE        stride;     /* distance in pixels between adjacent
                                 * scan lines of the cursor */
    EGL_SIZE        height;     /* height of cursor in pixels */
    EGL_POINT       hotSpot;    /* cursor hot spot point */
    EGL_SIZE        clutSize;   /* size of clut in number of elements */
    void            *pClut;     /* Pointer to CLUT */
    void            *pImage;    /* Pointer to image */
} EGL_CDIB;

typedef void *EGL_DDB_ID;
typedef void *EGL_MDDB_ID;
typedef void *EGL_TDDB_ID;
typedef void *EGL_CDDB_ID;

/* 
 * Layer, Display, and Draw management types used in egl_layer.c and
 * egl_common.c.  These structures are target specific and should
 * not be directly reused across different hardware platforms.
 */
typedef struct
{
    EGL_INT16 xoff;
    EGL_INT16 yoff;
    EGL_UINT8 display;
    EGL_UINT8 RSV      : 3;
    EGL_UINT8 drawn    : 1;  /* 1 == layer buffer was redrawn       */
    EGL_UINT8 visible  : 1;  /* 1 == layer is visible (enabled)     */
    EGL_UINT8 copy     : 1;  /* 1 == layer should be copied to GRAM */
    EGL_UINT8 autosize : 1;  /* 1 == autosize and redraw layer      */
    EGL_UINT8 update   : 1;  /* 1 == update the layer               */

    /* animation flags */
    union
    {
        struct
        {
            EGL_UINT16 RSV    :5;
            EGL_UINT16 enable :1;
            EGL_UINT16 alpha  :1;
            EGL_UINT16 x      :1;
            EGL_UINT16 y      :1;
            EGL_UINT16 width  :1;
            EGL_UINT16 height :1;
            EGL_UINT16 xoff   :1;
            EGL_UINT16 yoff   :1;
            EGL_UINT16 color  :1;
            EGL_UINT16 chroma :1;
            EGL_UINT16 target :1;
        } bits;
        EGL_UINT16 value;
    } animated;
} EGL_LAYER_T;

#define EGL_DRAWABLE DrawablePtr

/*****************************************************************
 * Define a data structure specific to Altia that contains
 * identifiers for the various subsystems of the graphics library.
 * The Altia target specific code can pass around a pointer to
 * an instance of this data structure to have access to the
 * various subsystems.
 *****************************************************************/
typedef struct
{
    EGL_DEVICE_ID        devId;
    EGL_FONT_DRIVER_ID   fontDrvId;
    EGL_EVENT_SERVICE_ID eventServiceId;
    EGL_GC_ID            gc;
    EGL_BITMAP_ID        memBitmap;
    EGL_DRAWABLE         display;
    EGL_EVENT_Q_ID       qId;
    EGL_SIZE             colorDepth;
    EGL_DRAWABLE         drawable;
} AltiaDevInfo;


/*****************************************************************
 * This file finishes with declarations for the required graphics
 * library functions.  These are the only graphics library functions
 * used by the Altia target specific code.
 *****************************************************************/


/*****************************************************************
 * General initialization and management functions
 *****************************************************************/
extern EGL_BOOL egl_UpdateBlocked(void);

extern EGL_STATUS egl_UpdateEnd(void);

extern EGL_STATUS egl_UpdateStart(void);

extern EGL_STATUS egl_BatchEnd(EGL_GC_ID gc);

extern EGL_STATUS egl_BatchStart(EGL_GC_ID gc);

#if EGL_HAS_DEVICEOPEN
extern EGL_STATUS    egl_DeviceClose(EGL_DEVICE_ID devId);

extern EGL_DEVICE_ID egl_DeviceOpen(EGL_CHAR *name);
#else /* NOT EGL_HAS_DEVICEOPEN */
extern EGL_STATUS egl_Deinitialize(void);

extern EGL_STATUS egl_DriverFind (EGL_UINT32 devType, EGL_UINT32 instance,
                                  EGL_UINT32 *pDeviceId);

extern EGL_EVENT_Q_ID egl_EventQCreate (EGL_EVENT_SERVICE_ID eventServiceId,
                                        EGL_SIZE queueSize);

extern EGL_STATUS egl_EventQDestroy (EGL_EVENT_SERVICE_ID eventServiceId,
                                     EGL_EVENT_Q_ID qId);

extern EGL_STATUS egl_Initialize(void);
#endif /* EGL_HAS_DEVICE_OPEN */


extern EGL_GC_ID egl_GcCreate(EGL_DEVICE_ID devId, int depth);

extern EGL_STATUS egl_GcDestroy(EGL_GC_ID gc);

extern EGL_SIZE egl_GetDepth(EGL_DEVICE_ID devId);

extern EGL_STATUS egl_Info (EGL_DEVICE_ID devId, EGL_INFO_REQ infoRequest,
                            void *pInfo, int layerNo);

extern EGL_STATUS egl_FinishInit (EGL_DEVICE_ID devId);

extern void local_GcAttributesSet(ALTIA_WINDOW win, GCPtr pGC, int filled,
                                  int text);

#if EGL_HAS_WINDOWCREATE
extern EGL_WINDOW_ID egl_WindowCreate(EGL_DEVICE_ID devId,
                                      EGL_WINDOW_ID parent,
                                      EGL_SIZE *winWidth, EGL_SIZE *winHeight,
                                      EGL_BOOL fullScreen);

extern EGL_STATUS egl_WindowDestroy(EGL_DEVICE_ID devId, EGL_WINDOW_ID winId);
#endif  /* EGL_HAS_WINDOWCREATE */

#if EGL_DO_ALPHA_BLENDING
extern void MergeAlphaPixmaps (PixmapPtr src, PixmapPtr dst);

extern PixmapPtr GetScratchPixmap (int width, int height);

extern PixmapPtr GetScratchAlphaPixmap (int width, int height, int initValue);

extern void CleanupScratchPixmaps (void);
#endif /* EGL_DO_ALPHA_BLENDING */


/*****************************************************************
 * Clip rectangle management functions to limit the redrawing area to
 * only what is necessary.
 *****************************************************************/

extern EGL_STATUS egl_ClipRectSet(EGL_GC_ID gc, EGL_POS left, EGL_POS top,
                                  EGL_POS right, EGL_POS bottom);


/*****************************************************************
 * Vector object drawing functions
 *****************************************************************/

extern EGL_STATUS egl_BackgroundColorSet(EGL_GC_ID gc, EGL_COLOR colorRef);

extern EGL_STATUS egl_FillPatternSet(EGL_GC_ID gc, EGL_MDDB_ID patternBitmap);

extern EGL_STATUS egl_ForegroundColorSet(EGL_GC_ID gc, EGL_COLOR colorRef);

extern EGL_STATUS egl_AlphaChannelSet(EGL_GC_ID gc, EGL_INT16 alpha);

extern EGL_STATUS egl_ClearDC(EGL_GC_ID gc);

extern EGL_STATUS egl_Line(ALTIA_WINDOW win, EGL_GC_ID gc, EGL_POS x1,
                           EGL_POS y1, EGL_POS x2,
                           EGL_POS y2);


#if EGL_HAS_LINES
extern EGL_STATUS egl_Lines(ALTIA_WINDOW win, EGL_GC_ID gc,
                            EGL_ORD numPoints, EGL_POS *data);
#endif

extern EGL_STATUS egl_LineStyleSet(EGL_GC_ID gc, EGL_LINE_STYLE lineStyle);

extern EGL_STATUS egl_LineWidthSet(EGL_GC_ID gc, EGL_SIZE lineWidth);

extern EGL_STATUS egl_Polygon(ALTIA_WINDOW win, EGL_GC_ID gc,
                              EGL_ORD numPoints, EGL_POS *data, EGL_BOOL fill);

extern EGL_STATUS egl_Rectangle(ALTIA_WINDOW win, EGL_GC_ID gc,
                                EGL_POS left, EGL_POS top, EGL_POS right, 
                                EGL_POS bottom, EGL_BOOL fill, EGL_BOOL clear);

/*****************************************************************
 * Dashed line drawing functions
 *****************************************************************/

extern EGL_STATUS egl_PixelSet(ALTIA_WINDOW win, EGL_GC_ID gc,
                               EGL_POS x, EGL_POS y,
                               EGL_COLOR colorRef);

/*****************************************************************
 * Required for MonoBitmapRead
 *****************************************************************/
extern EGL_COLOR egl_PixelGet(ALTIA_WINDOW win, EGL_GC_ID gc,
                               EGL_POS x, EGL_POS y);


/*****************************************************************
 * Color management functions
 *****************************************************************/

extern EGL_STATUS egl_ColorAlloc(EGL_DEVICE_ID devId, EGL_ARGB *pAllocColors,
                                 EGL_ORD *pIndex, EGL_COLOR *pEglColors,
                                 EGL_SIZE numColors);


#if !EGL_HAS_MONOBITMAPREAD || !EGL_USE_ALTIABITSFORSCALING
extern EGL_STATUS egl_ColorConvert (EGL_DEVICE_ID devId, void *sourceArray,
                                    EGL_COLOR_FORMAT sourceFormat,
                                    void *destArray,
                                    EGL_COLOR_FORMAT destFormat,
                                    EGL_SIZE arraySize);
#endif


/*****************************************************************
 * Font management and text drawing functions
 *****************************************************************/


extern EGL_STATUS egl_FontOpen(void);

extern EGL_STATUS egl_FontClose(void);

extern EGL_FONT_ID egl_FontCreate (EGL_FONT_DRIVER_ID fontDriverId,
                                   EGL_FONT_DEF *pFontDefinition);

#if EGL_HAS_FONTCREATEBYNAME
extern EGL_FONT_ID egl_FontCreateByName (EGL_FONT_DRIVER_ID fontDriverId,
                                         EGL_CHAR *name);
#endif

extern EGL_STATUS egl_FontDestroy (EGL_FONT_ID fontID);

extern EGL_STATUS egl_FontFind (EGL_FONT_DRIVER_ID fontDriverId,
                                EGL_FONT_DESC *pFontDescriptor,
                                EGL_FONT_DESC_PRIORITY *pFontDescPriority,
                                EGL_FONT_DEF *pFontDefinition);

extern EGL_STATUS egl_FontGet (EGL_GC_ID gc, EGL_FONT_ID *pFontId);

extern EGL_STATUS egl_FontMetricsGet (EGL_FONT_ID fontId,
                                      EGL_FONT_METRICS *pFontMetrics);

extern EGL_STATUS egl_FontSet (EGL_GC_ID gc, EGL_FONT_ID fontId);

extern EGL_STATUS egl_FontPreload(EGL_INT font, EGL_CHAR code);

extern EGL_STATUS egl_TextDraw (ALTIA_WINDOW win, EGL_GC_ID gc, EGL_POS x,
                                EGL_POS y,
                                EGL_SIZE length, const EGL_CHAR *text);

#if EGL_HAS_TEXTSIZEGETWITHOFFSET
extern EGL_STATUS egl_TextSizeGet (EGL_FONT_ID fontId, EGL_SIZE *width,
                                   EGL_SIZE *height, EGL_SIZE *offset,
                                   EGL_SIZE length, const EGL_CHAR *text);
#else
extern EGL_STATUS egl_TextSizeGet (EGL_FONT_ID fontId, EGL_SIZE *width,
                                   EGL_SIZE *height, EGL_SIZE length,
                                   const EGL_CHAR *text);
#endif

extern EGL_STATUS egl_TextWidthGet (EGL_FONT_ID fontId, EGL_SIZE *width,
                                   EGL_SIZE length, const EGL_CHAR *text);

extern void * egl_FontDdbGet(EGL_FONT_ID fontId, EGL_CHAR text, 
                             void * charinfo);



/****************************************************************
 * Alpha Mask APIs
 *****************************************************************/

extern EGL_BOOL egl_AlphaMaskOpen(void);

extern EGL_BOOL egl_AlphaMaskClose(void);

extern EGL_BOOL egl_AlphaMaskResize(EGL_INT obj);

extern EGL_INT egl_AlphaMaskSetMode(EGL_INT obj);

extern EGL_BOOL egl_AlphaMaskStartColorDraw(ALTIA_WINDOW win, EGL_GC_ID gc, 
                                            EGL_INT obj);

extern EGL_BOOL egl_AlphaMaskEndColorDraw(ALTIA_WINDOW win, EGL_GC_ID gc, 
                                          EGL_INT obj);

extern EGL_BOOL egl_AlphaMaskStartAlphaDraw(ALTIA_WINDOW win, EGL_GC_ID gc, 
                                            EGL_INT obj);

extern EGL_BOOL egl_AlphaMaskEndAlphaDraw(ALTIA_WINDOW win, EGL_GC_ID gc, 
                                          EGL_INT obj);

extern EGL_BOOL egl_AlphaMaskClear(EGL_INT obj);

extern EGL_BOOL egl_AlphaMaskTransformSet(ALTIA_WINDOW win, EGL_GC_ID gc, 
                                          EGL_INT obj, void * total);

extern EGL_BOOL egl_AlphaMaskDraw(void * gs, ALTIA_WINDOW win, EGL_GC_ID gc,
                                  EGL_INT obj, EGL_FLOAT destX, EGL_FLOAT destY);


/****************************************************************
 * Event handling functions for receiving keyboard and mouse events
 *****************************************************************/

extern EGL_STATUS egl_EventGet (EGL_EVENT_Q_ID eventQId, EGL_EVENT *pEvent,
                                EGL_SIZE eventSize, EGL_TIMEOUT timeout);


#if EGL_HAS_WINDOWCREATE
extern EGL_STATUS egl_EventGetNextExpose (EGL_EVENT_Q_ID eventQId,
                                          EGL_EVENT *pEvent,
                                          EGL_SIZE eventSize);
#endif



/*****************************************************************
 * Color bitmap (Altia Raster object), color bitmap with transparency
 * (Altia Raster object with transparent bits), and monochrome bitmap
 * (Altia Stencil object) rendering functions.
 *
 * In addition, an offscreen bitmap is used for double-buffering.  This
 * requires the use of egl_BitmapBlt(), egl_BitmapCreate(),
 * egl_BitmapDestroy(),  and egl_DefaultBitmapSet().
 *****************************************************************/

extern EGL_STATUS egl_BitmapBlt (EGL_GC_ID gc,
                                 EGL_DDB_ID srcBitmapId,
                                 EGL_POS sourceLeft, EGL_POS sourceTop,
                                 EGL_POS sourceRight, EGL_POS sourceBottom,
                                 EGL_DDB_ID dstBitmapId,
                                 EGL_FLOAT destX, EGL_FLOAT destY,
                                 EGL_INT hasAlpha);

extern EGL_DDB_ID egl_BitmapCreate (EGL_DEVICE_ID devId,EGL_DIB *pDib,
                                    EGL_DIB_CREATE_MODE createMode,
                                    EGL_UINT32 initValue,
                                    EGL_REFERENCE_ID refId);

extern EGL_STATUS egl_BitmapDestroy (EGL_DEVICE_ID devId,EGL_DDB_ID ddbId,
                                     EGL_INT hasAlpha);




#if !EGL_HAS_MONOBITMAPREAD || !EGL_USE_ALTIABITSFORSCALING


extern EGL_STATUS egl_BitmapRead (EGL_DEVICE_ID devId, EGL_DDB_ID ddbId,
                                  EGL_POS srcLeft, EGL_POS srcTop,
                                  EGL_POS srcRight, EGL_POS srcBottom,
                                  EGL_DIB *pDib, EGL_POS dstX, EGL_POS dstY,
                                  EGL_INT hasAlpha);


#endif  /* NOT EGL_HAS_MONOBITMAPREAD || NOT EGL_USE_ALTIABITSFORSCALING */




#if EGL_HAS_MONOBITMAPREAD
extern EGL_STATUS egl_DefaultBitmapForNextGc(EGL_DDB_ID destBitmap);
#endif

extern EGL_STATUS egl_BitmapSize (EGL_DDB_ID bitmap, int *width, int *height,
                                  EGL_INT hasAlpha);


#if EGL_HAS_MONOBITMAPBLT
extern EGL_STATUS egl_MonoBitmapBlt (EGL_GC_ID gc,
                                     EGL_MDDB_ID srcBitmapId,
                                     EGL_POS sourceLeft, EGL_POS sourceTop,
                                     EGL_POS sourceRight, EGL_POS sourceBottom,
                                     EGL_DRAWABLE_ID dstBitmapId,
                                     EGL_FLOAT destX, EGL_FLOAT destY,
                                     EGL_INT hasAlpha);
#endif


extern EGL_MDDB_ID egl_MonoBitmapCreate (EGL_DEVICE_ID devId,EGL_MDIB *mDib,
                                         EGL_DIB_CREATE_MODE createMode,
                                         EGL_UINT32 initValue,
                                         EGL_REFERENCE_ID refId);

extern EGL_STATUS egl_MonoBitmapDestroy (EGL_DEVICE_ID devId,
                                         EGL_MDDB_ID mDdbId);


#if EGL_HAS_MONOBITMAPREAD
extern EGL_STATUS egl_MonoBitmapRead(EGL_GC_ID gc, EGL_MDDB_ID bitmap,
                                     EGL_POS srcLeft, EGL_POS srcTop,
                                     EGL_POS srcRight, EGL_POS srcBottom,
                                     EGL_MDIB *pMDib,
                                     EGL_POS dstX, EGL_POS dstY);
#endif


#if EGL_HAS_MONOBITMAPWRITE
extern EGL_STATUS egl_MonoBitmapWrite(EGL_GC_ID gc, EGL_MDIB *pMdib,
                                      EGL_POS srcLeft, EGL_POS srcTop,
                                      EGL_POS srcRight, EGL_POS srcBottom,
                                      EGL_MDDB_ID mDdbId,
                                      EGL_POS dstX, EGL_POS dstY);
#endif


extern EGL_STATUS egl_RasterModeSet(EGL_GC_ID gc, EGL_RASTER_OP rasterOp);


#if EGL_HAS_TRANSBITMAPBLT
extern EGL_STATUS egl_TransBitmapBlt(EGL_GC_ID gc,
                                     EGL_DDB_ID srcBitmapId,
                                     EGL_MDDB_ID srcMaskId,
                                     EGL_POS sourceLeft, EGL_POS sourceTop,
                                     EGL_POS sourceRight, EGL_POS sourceBottom,
                                     EGL_DRAWABLE_ID dstBitmapId,
                                     EGL_FLOAT destX, EGL_FLOAT destY,
                                     EGL_INT hasAlpha);
#endif


#if EGL_HAS_TRANSBITMAPCREATE
extern EGL_TDDB_ID egl_TransBitmapCreateFromDDB(EGL_DEVICE_ID devId,
                                          EGL_DDB_ID ddbId,
                                          EGL_MDDB_ID mDdbId,
                                          EGL_REFERENCE_ID refId,
                                          EGL_INT hasAlpha);

extern EGL_STATUS egl_TransBitmapDestroy (EGL_DEVICE_ID devId,
                                          EGL_TDDB_ID tDdbId, EGL_INT hasAlpha);
#endif

#if EGL_HAS_RASTER_TRANSFORMATION_LIB
extern EGL_STATUS egl_RasterTransformSet(EGL_GC_ID gc, void *trans);
extern EGL_STATUS egl_RasterTransformClear(EGL_GC_ID gc);
#endif



/*****************************************************************
 * Clip region management functions required for rendering scaled/rotated
 * color bitmaps, monochrome bitmaps, and text.
 *****************************************************************/

#if EGL_HAS_POLYREGIONCREATE
extern EGL_POLYREGION_ID egl_PolyRegionCreate(void);

extern EGL_STATUS egl_PolyRegionDestroy(EGL_POLYREGION_ID polyRegionId);

extern EGL_STATUS egl_PolyRegionGet(EGL_GC_ID gc,
                                    EGL_POLYREGION_ID *polyRegionId);

extern EGL_STATUS egl_PolyRegionInclude(EGL_POLYREGION_ID polyRegionId,
                                        EGL_ORD numPoints, EGL_POS *data);

extern EGL_STATUS egl_PolyRegionIntersectRect(EGL_POLYREGION_ID regionId,
                                              const EGL_RECT *pClipRect);

extern EGL_STATUS egl_PolyRegionSet(EGL_GC_ID gc,
                                    EGL_POLYREGION_ID polyRegionId);

#else  /* NO EGL_HAS_POLYREGIONCREATE */
extern EGL_STATUS egl_ClipRegionGet (EGL_GC_ID gc,
                                     EGL_REGION_ID *pClipRegionId);

extern EGL_STATUS egl_ClipRegionSet (EGL_GC_ID gc, EGL_REGION_ID clipRegionId);

extern EGL_STATUS egl_RegionClipToRect (EGL_REGION_ID regionId,
                                        const EGL_RECT *pClipRect);

extern EGL_REGION_ID egl_RegionCreate (void);

extern EGL_STATUS egl_RegionDestroy (EGL_REGION_ID regionId);

extern EGL_STATUS egl_RegionRectInclude (EGL_REGION_ID regionId,
                                         const EGL_RECT *pRect);

#endif /* EGL_HAS_POLYREGIONCREATE */

extern EGL_STATUS egl_CursorInit (EGL_DEVICE_ID devId, EGL_POS w, EGL_POS h,
                                  EGL_POS cursorX, EGL_POS cursorY);

extern EGL_CDDB_ID egl_CursorBitmapCreate (EGL_DEVICE_ID devId,
                                           EGL_CDIB * pCDib);

extern EGL_STATUS egl_CursorImageSet (EGL_DEVICE_ID devId, EGL_CDDB_ID pCImage);

extern EGL_CDDB_ID egl_CursorImageGet (EGL_DEVICE_ID devId);

extern EGL_STATUS egl_CursorOn (EGL_DEVICE_ID devId);

extern EGL_STATUS egl_CursorDeinit (EGL_DEVICE_ID devId);

extern EGL_STATUS egl_CursorBitmapDestroy (EGL_DEVICE_ID devId,
                                           EGL_CDDB_ID pCImage);

extern void egl_CursorMove (EGL_DEVICE_ID devId, EGL_POS x, EGL_POS y);

extern void egl_CursorRepaint (EGL_DEVICE_ID devId);


/*****************************************************************
 * Data used throughout the pipeline and target files
 *****************************************************************/


#endif /* !_EGL_WRAPPER_H */
