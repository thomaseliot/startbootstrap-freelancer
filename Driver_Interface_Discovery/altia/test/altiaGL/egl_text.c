/* %OSNAME% = ALTIAGL_STMICRO_STM32 */
/* Copyright (c) 2011 Altia Inc.
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
#include <stdio.h>
#include <string.h>

#include "egl_Wrapper.h"
#include "altiaBase.h"
#include "altiaTypes.h"
#include "altiaExtern.h"
#include "altiaImageAccess.h"

#include "Xprotostr.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "fb.h"




/* Begin NEW 12/3: Static font image initialization */
#ifndef ALTIA_FONTCHAR_TYPE
#define ALTIA_FONTCHAR_TYPE     Altia_FontChar_type
#endif
/* End NEW 12/3: Static font image initialization */

#define DRAWABLE(win) ((DrawableRec*)((AltiaDevInfo*)win)->drawable)



#ifdef UNICODE
    #error UNICODE compile time definition not allowed because this code was generated using the ASCII Altia Editor
#endif

extern ALTIA_CONST ALTIA_INT test_fontHeaders_count;
extern ALTIA_CONST Altia_FontHeader_type test_fontHeaders[];


extern ALTIA_CONST ALTIA_INT test_fontChars_count;


#ifdef ALTIA_SAVE_FONT_BITMAP
extern Altia_FontChar_type test_fontChars[];
#else
extern ALTIA_CONST Altia_FontChar_type test_fontChars[];
#endif



#ifdef ALTIA_DEFAULT_FONT_CHARACTER
extern Altia_FontChar_type * test_fontCharDefaults[];
#endif

extern AltiaDevInfo *altiaLibGetDevInfo(void);



extern EGL_DIB *altiaLibCreateDIB(
#ifdef Altiafp
int width,
int height,
ALTIA_SHORT rasterType,
ALTIA_INDEX colors,
ALTIA_SHORT colorCnt,
ALTIA_BOOLEAN forPermRGB888ScalableBits,
ALTIA_BOOLEAN forTempScaledBits,
ALTIA_BOOLEAN forScaledText,
ALTIA_UBYTE **dataPtr
#endif
);

extern void altiaLibInitDIB(
#ifdef Altiafp
EGL_DIB *dibPtr,
int width,
int height,
int numBits,
EGL_ARGB **colorPtr,
ALTIA_UBYTE **dataPtr
#endif
);

extern ALTIA_UBYTE altiaLibDecodePixel(
#ifdef Altiafp
ALTIA_CONST ALTIA_UBYTE *pixels,
ALTIA_INT *count,
ALTIA_INT *index,
ALTIA_INT *indexCnt
#endif
);


/* Macros for Font Character Bitmap */



#define EGL_TEXT_DIB    EGL_DIB
#define EGL_TEXT_DDB    EGL_DDB_ID

#define EGL_TEXT_CREATE_DIB(w,h,s) \
    altiaLibCreateDIB(w,h,ALTIA_RASTER_8_RLE,-1,0,false,false,s,NULL)

#define EGL_TEXT_LOAD_DIB(bits,cnt,w,h,dib) \
    LocalLoadAlpha8DIB((ALTIA_UBYTE *)(bits),cnt,w,h,dib)

#define EGL_TEXT_CREATE_DDB(bits,cnt,w,h,dib,ddb) \
    LocalLoadAlpha8DIB((ALTIA_UBYTE *)(bits),cnt,w,h,dib); \
    ddb = egl_BitmapCreate((altiaLibGetDevInfo())->devId, dib, \
        EGL_DIB_INIT_DATA, 0, NULL)

#define EGL_TEXT_BLIT(gc,src,left,top,right,bottom,dst,x,y) \
    egl_BitmapBlt(gc, src, (EGL_POS)(left), (EGL_POS)(top), \
        (EGL_POS)(right), (EGL_POS)(bottom), \
        dst, (EGL_FLOAT)(x), (EGL_FLOAT)(y), 0)

#define EGL_TEXT_DESTROY_DDB(ddb) \
    egl_BitmapDestroy((altiaLibGetDevInfo())->devId, ddb, 0)

extern void altiaLibInitDIB(
#ifdef Altiafp
EGL_DIB *dibPtr,
int width,
int height,
int numBits,
EGL_ARGB **colorPtr,
ALTIA_UBYTE **dataPtr
#endif
);



extern ALTIA_UBYTE altiaLibDecodePixel(
#ifdef Altiafp
ALTIA_CONST ALTIA_UBYTE *pixels,
ALTIA_INT *count,
ALTIA_INT *index, ALTIA_INT *indexCnt
#endif
);





#ifndef ALTIA_CHAR_CAST
#ifdef UNICODE
    #define ALTIA_CHAR_CAST     ALTIA_USHORT
#else
    #define ALTIA_CHAR_CAST     ALTIA_UBYTE
#endif /* UNICODE */
#endif /* !ALTIA_CHAR_CAST */

/* Begin NEW 12/3: Static font image initialization */

/**************************************************************************
 * Begin Static Font Image Forward Declarations.
 ***************************************************************************/
/* End NEW 12/3: Static font image initialization */
static Altia_FontChar_type * binarySearch(int start, int count, ALTIA_CHAR code);
/* Begin NEW 12/3: Static font image initialization */
/**************************************************************************
 * End Static Font Image Forward Declarations.
 ***************************************************************************/

/* End NEW 12/3: Static font image initialization */



/**************************************************************************
 * Font management and text drawing functions
 ***************************************************************************/

/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_FontOpen(void)
 *
 * Initializes the target-specific font engine
 *
 * Returns EGL_STATUS_OK upon success, EGL_STATUS_ERROR otherwise
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_FontOpen(void)
{

    return EGL_STATUS_OK;
}


/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_FontClose(void)
 *
 * Terminates the target-specific font engine
 *
 * Returns EGL_STATUS_OK upon success, EGL_STATUS_ERROR otherwise
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_FontClose(void)
{

    return EGL_STATUS_OK;
}

#if EGL_HAS_FONTCREATEBYNAME
/*-------------------------------------------------------------------------
 *  EGL_FONT_ID egl_FontCreateByName (EGL_FONT_DRIVER_ID fontDriverId,
 *                                    EGL_CHAR *name)
 *
 * If "egl_Wrapper.h" has EGL_HAS_FONTCREATEBYNAME defined as non-zero (1),
 * it means that the target graphics library can create a font directly from
 * a string name.
 *
 * In this case, the Altia code calls egl_FontCreateByName() with the string
 * name of the font as provided by the Altia "data.c" file.  The Altia
 * "data.c" file gets the target string names for fonts from the Altia
 * software file "usercode/<TARGET_DIR>/fonts.ali" at code generation time.
 * By customizing the "fonts.ali" file for a given target, all standard Altia
 * fonts can be mapped to target specific string names.
 *
 * If creating a font directly from a string name is not feasible for a
 * the target, define EGL_HAS_FONTCREATEBYNAME as 0 in "egl_Wrapper.h".
 * See the next function egl_FontCreate() which can create a font from
 * detailed information provided in a structure.
 *
 * If successful, this function returns the identification value for
 * a font.  The identification value can be used to set the font in a gc
 * using egl_FontSet().
 *
 * Depending on the graphics library's implementation of this function, an
 * EGL_FONT_ID may be a simple data type such as int or a pointer to
 * some data structure whose contents are private to the graphics library.
 *
 * If this function cannot create a font from the given string name, it
 * returns (EGL_FONT_ID) 0.
 *
 *-------------------------------------------------------------------------*/
EGL_FONT_ID egl_FontCreateByName (EGL_FONT_DRIVER_ID fontDriverId,
                                  EGL_CHAR *name)
{

    ALTIA_INDEX i;

    for (i = 0; i < test_fontHeaders_count; i++)
    {
        ALTIA_INDEX fidx = test_fontHeaders[i].fontIdx;

        if (ALT_STRCMP((ALTIA_CHAR*)name, test_fonts[fidx].name) == 0)
        {
/* Begin NEW 12/3: Static font image initialization */

/* End NEW 12/3: Static font image initialization */

#ifdef ALTIA_DEFAULT_FONT_CHARACTER
            Altia_FontHeader_type *header;
            Altia_FontChar_type *cptr;
#endif

#ifdef ALTIA_SAVE_FONT_BITMAP
            /* We need to insure that the bitmap is cleared for all chars */
            int start = test_fontHeaders[i].firstChar;
            ALTIA_INDEX j;
            for (j = 0; j < test_fontHeaders[i].charCnt; j++)
            {
                test_fontChars[start + j].bitmap = 0;
            }
#endif

#ifdef ALTIA_DEFAULT_FONT_CHARACTER
            /* find the default character for this font */
            header = (Altia_FontHeader_type *)&test_fontHeaders[i];
            cptr = NULL;

            cptr = binarySearch(header->firstChar, header->charCnt,
                (EGL_CHAR)(ALTIA_DEFAULT_FONT_CHARACTER));

            test_fontCharDefaults[i] = cptr;
#endif /* ALTIA_DEFAULT_FONT_CHARACTER */

/* Begin NEW 12/3: Static font image initialization */

/* End NEW 12/3: Static font image initialization */


            return (EGL_FONT_ID)&test_fontHeaders[i];
        }
    }

    return (EGL_FONT_ID) 0;
}
#endif /* EGL_HAS_FONTCREATEBYNAME */


/*-------------------------------------------------------------------------
 *  EGL_FONT_ID egl_FontCreate (EGL_DEVICE_ID devId,
 *                              EGL_FONT_DEF *pFontDefinition)
 *
 * Gets the identification value for a font based on the specifications in
 * the structure pointed to by pFontDefinition.  The identification
 * value can be used to set the font in a gc using egl_FontSet().
 *
 * Depending on the graphics library's implementation of this function, an
 * EGL_FONT_ID may be a simple data type such as int or a pointer to
 * some data structure whose contents are private to the graphics library.
 *
 * NOTE:
 * -----
 * This function can be empty if EGL_HAS_FONTCREATEBYNAME is defined as
 * non-zero (1) in "egl_Wrapper.h" because the Altia code will call
 * egl_FontCreateByName() to get a font id directly from a a string
 * identifying the font.  See egl_FontCreateByName() for more details.
 * -----
 *
 * The definitions for the EGL_FONT_DEF structure look like:
 *
 * #define EGL_FONT_FACE_NAME_MAX_LENGTH   80
 * #define EGL_FONT_FAMILY_NAME_MAX_LENGTH 40
 *
 * typedef struct egl_range
 * {
 *   int min;
 *   int max;
 * } EGL_RANGE;
 *
 * typedef struct egl_font_def
 * {
 *     EGL_RANGE pixelSize;        // average size of font in pixels
 *     EGL_RANGE weight;           // weight is a bold setting from 0 - 100
 *     int     italic;             // Italic is usually either on or off
 *     int     spacing;            // Mono spaced or proportional
 *     int     charSet;            // ISO 8859-1, Unicode, etc
 *     char familyName[EGL_FONT_FAMILY_NAME_MAX_LENGTH]; // e.g., "Helvetica"
 * } EGL_FONT_DEF;
 *
 * For Altia's usage, pixelSize.min and pixelSize.max are the same
 * value with a range between 8 and 24 for mapping with standard Altia fonts.
 *
 * For Altia's usage, weight.min and weight.max are the same value as one
 * of the following defines:
 *    #define EGL_FONT_BOLD_OFF        0
 *    #define EGL_FONT_BOLD_LIGHT      25
 *    #define EGL_FONT_BOLD            50
 *
 * For Altia's usage, italic is passed as one of the following defines:
 *    #define EGL_FONT_UPRIGHT         1
 *    #define EGL_FONT_ITALIC          2
 *
 * For Altia's usage, spacing is passed as one of the following defines:
 *    #define EGL_FONT_MONO_SPACED     3
 *    #define EGL_FONT_PROPORTIONAL    4
 *
 * For Altia's usage, charSet is passed as one of the following defines:
 *    #define EGL_FONT_ISO_8859_1      5
 *    #define EGL_FONT_UNICODE         6
 *
 * For Altia's usage, familyName is set to something like "Lucida",
 *   "Helvetica", "Times", or "Courier".
 *
 * This function is not required to provide nearest matching.  That
 * is done instead by egl_FontFind().  To create a font, code should
 * first call egl_FontFind() to find a suitable match and use the
 * EGL_FONT_DEF data returned by egl_FontFind() to call egl_FontCreate().
 *
 * This function returns a fond identification value if it succeeds.
 * Otherwise, it returns (EGL_FONT_ID) 0.
 *
 *-------------------------------------------------------------------------*/
EGL_FONT_ID egl_FontCreate (EGL_FONT_DRIVER_ID fontDriverId,
                            EGL_FONT_DEF * pFontDefinition)
{
    /* This function is empty for X11 and can probably be empty for
     * interfacing to most graphics libraries.  Always return an error.
     * See egl_FontCreateByName() instead.
     */
    return (EGL_FONT_ID) 0;
}


/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_FontDestroy (EGL_FONT_ID fontId)
 *
 * Destroys all graphics library internal data structures associated with
 * the font identified by fontId.
 *
 * The calling code should not use the value of fontId in any other graphics
 * library calls that require an EGL_FONT_ID.  To do so may cause a memory
 * access error.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_FontDestroy (EGL_FONT_ID fontId)
{

/* Begin NEW 12/3: Static font image initialization */

/* End NEW 12/3: Static font image initialization */
#ifdef ALTIA_SAVE_FONT_BITMAP
    /* We have to remove all the bitmaps we have created for the font */
    ALTIA_INDEX i;
    Altia_FontHeader_type *hptr = (Altia_FontHeader_type*)fontId;
    ALTIA_INDEX start = hptr->firstChar;

    for (i = 0; i < hptr->charCnt; i++)
    {
        void* bitmap = (void *)test_fontChars[start + i].bitmap;
        if (bitmap != NULL)
        {
            EGL_TEXT_DESTROY_DDB(bitmap);
            test_fontChars[start + i].bitmap = 0;
        }
    }
#endif
/* Begin NEW 12/3: Static font image initialization */

/* End NEW 12/3: Static font image initialization */


    return EGL_STATUS_OK;
}


/*-------------------------------------------------------------------------
 * EGL_STATUS egl_FontFind (EGL_DEVICE_ID devId,
 *                          EGL_FONT_DESC *pFontDescriptor,
 *                          EGL_FONT_DESC_PRIORITY *pFontDescPriority,
 *                          EGL_FONT_DEF *pFontDefinition)
 *
 * This function takes a set of font description items from the data
 * structure pointed to by pFontDescriptor and it takes priority levels
 * for the items from the data structure pointed to by pFontDescPriority.
 * It finds a font that most closely matches the description given the
 * priority levels for each item in the description.
 *
 * It returns the font's definition in the data structure pointed to by
 * pFontDefinition and this definition is usually passed to egl_FontCreate()
 * to create a reference to the font (i.e., a font identification value).
 *
 * NOTE:
 * -----
 * This function can be empty if EGL_HAS_FONTCREATEBYNAME is defined as
 * non-zero (1) in "egl_Wrapper.h" because the Altia code will call
 * egl_FontCreateByName() to get a font id directly from a string
 * identifying the font.  See egl_FontCreateByName() for more details.
 * -----
 *
 * There would be very few instances where this function might fail
 * except perhaps if the device has no support for drawing text in
 * even a single font style.  In general, the graphics library should
 * always return a match even if it only has a single font.
 *
 * The definitions for the EGL_FONT_DESC structure look like:
 *
 * #define EGL_FONT_FACE_NAME_MAX_LENGTH   80
 * #define EGL_FONT_FAMILY_NAME_MAX_LENGTH 40
 *
 * typedef struct egl_range
 * {
 *   int min;
 *   int max;
 * } EGL_RANGE;
 *
 * typedef struct egl_font_desc
 * {
 *     EGL_RANGE pixelSize;        // average size of font in pixels
 *     EGL_RANGE weight;           // weight is a bold setting from 0 - 100
 *     int     italic;             // Italic is usually either on or off
 *     int     spacing;            // Mono spaced or proportional
 *     int     charSet;            // ISO 8859-1, Unicode, etc
 *     char familyName[EGL_FONT_FAMILY_NAME_MAX_LENGTH]; // e.g., "Helvetica"
 * } EGL_FONT_DESC;
 *
 * For Altia's usage, pixelSize.min and pixelSize.max are the same
 * value with a range between 8 and 24 for mapping with standard Altia fonts.
 *
 * For Altia's usage, weight.min and weight.max are the same value as one
 * of the following defines:
 *    #define EGL_FONT_BOLD_OFF        0
 *    #define EGL_FONT_BOLD_LIGHT      25
 *    #define EGL_FONT_BOLD            50
 *
 * For Altia's usage, italic is passed as one of the following defines:
 *    #define EGL_FONT_UPRIGHT         1
 *    #define EGL_FONT_ITALIC          2
 *
 * For Altia's usage, spacing is passed as one of the following defines:
 *    #define EGL_FONT_MONO_SPACED     3
 *    #define EGL_FONT_PROPORTIONAL    4
 *
 * For Altia's usage, charSet is passed as one of the following defines:
 *    #define EGL_FONT_ISO_8859_1      5
 *    #define EGL_FONT_UNICODE         6
 *
 * For Altia's usage, familyName is set to something like "Lucida",
 *   "Helvetica", "Times", or "Courier".
 *
 * Each item in the EGL_FONT_DESC structure has a priority value in the
 * EGL_FONT_DESC_PRIORITY structure.  The higher the priority value, the
 * more priority egl_FindFont() should place on matching the value for
 * that item as found in the EGL_FONT_DESC structure.
 *
 * The definition for the EGL_FONT_DESC_PRIORITY structure looks like:
 *
 * typedef struct egl_font_desc_priority
 * {
 *     int pixelSize;
 *     int weight;
 *     int italic;
 *     int spacing;
 *     int charSet;
 *     int familyName;
 * } EGL_FONT_DESC_PRIORITY;
 *
 * For Altia's usage, pixelSize has a priority of 6 or a priority
 * of EGL_FONT_DONT_CARE (which should be defined as 0).
 *
 * For Altia's usage, weight always has a priority of 4.
 *
 * For Altia's usage, italic always has a priority of 3.
 *
 * For Altia's usage, spacing has a priority of 5 or a priority
 * of EGL_FONT_DONT_CARE (which should be defined as 0).
 *
 * For Altia's usage, charSet has a priority of 1 or a priority
 * of EGL_FONT_DONT_CARE (which should be defined as 0).
 *
 * For Altia's usage, familyName has a priority of 2 or a priority
 * of EGL_FONT_DONT_CARE (which should be defined as 0).
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_FontFind (EGL_FONT_DRIVER_ID fontDriverId,
                         EGL_FONT_DESC * pFontDescriptor,
                         EGL_FONT_DESC_PRIORITY * pFontDescPriority,
                         EGL_FONT_DEF * pFontDefinition)
{
    /*
     * See egl_FontCreateByName() instead.
     */
    return ~(EGL_STATUS_OK);
}


/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_FontGet (EGL_GC_ID gc, EGL_FONT_ID *pFontId)
 *
 * The caller passes a pointer to a variable of type EGL_FONT_ID and this
 * function assigns the variable the font identification value that is
 * currently being referenced by the given gc.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_FontGet (EGL_GC_ID gc, EGL_FONT_ID *pFontId)
{
    GCPtr pGC = (GCPtr)gc;
    *pFontId = (EGL_FONT_ID)pGC->font;

    return EGL_STATUS_OK;
}


/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_FontMetricsGet (EGL_FONT_ID fontId,
 *                                 EGL_FONT_METRICS *pFontMetrics)
 *
 * This function returns metrics on the font identified by fontId.  The
 * caller passes a pointer, pFontMetrics, to a data structure of type
 * EGL_FONT_METRICS and this function assigns values to the elements of the
 * data structure.
 *
 * The data structure definition looks like:
 *
 * typedef struct egl_font_metrics
 * {
 *   int  height;           // Maximum height of characters in the font
 *   int  maxDescent;       // Maximum descent below baseline
 *   int  spacing;          // Mono spaced or proportional
 * } EGL_FONT_METRICS;
 *
 * Altia expects maxDescent to be a pixel value for the maximum descent
 * of the font's character set below baseline.
 *
 * The spacing element should have a value from one of the following defines:
 *     #define EGL_FONT_MONO_SPACED     3
 *     #define EGL_FONT_PROPORTIONAL    4
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_FontMetricsGet(EGL_FONT_ID fontId,
                              EGL_FONT_METRICS * pFontMetrics)
{
    Altia_FontHeader_type  *header = (Altia_FontHeader_type*)fontId;
    int fixedWidth = 1;


    ALTIA_SHORT maxWidth;
    int i;



    if (fontId == (EGL_FONT_ID) 0)
        return ~(EGL_STATUS_OK);

    /* Altia only needs the "maxDescent" and "spacing" elements of the
     * EGL_FONT_METRICS data structure.
     */
    pFontMetrics->maxDescent = 0;

    /* As of 4/10/2003, Altia code also needs height of characters. */
    pFontMetrics->height = header->height;


/* Begin NEW 12/3: Static font metric data */

    maxWidth = header->maxWidth;
/* End NEW 12/3: Static font metric data */
    for (i = 0; i < header->charCnt; i++)
    {
        if (maxWidth != test_fontChars[header->firstChar+i].width)
        {
            fixedWidth = 0;
            break;
        }
    }
/* Begin NEW 12/3: Static font metric data */

/* End NEW 12/3: Static font metric data */


    if (fixedWidth)
        pFontMetrics->spacing = EGL_FONT_MONO_SPACED;
    else
        pFontMetrics->spacing = EGL_FONT_PROPORTIONAL;

    return EGL_STATUS_OK;
}


/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_FontSet (EGL_GC_ID gc, EGL_FONT_ID fontId)
 *
 * Sets the current font for the given gc using the font identification
 * value given by fontId which was presumably returned by an earlier
 * call to egl_FontCreateByName() or egl_FontCreate().
 *
 * Future text drawing operations using the given gc will render text
 * in the current font for the gc.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_FontSet (EGL_GC_ID gc, EGL_FONT_ID fontId)
{
    /* For better performance on this target, don't set the font
     * just yet.  Instead, just save the font for later inspection.
     */
    GCPtr pGC = (GCPtr)gc;
    pGC->font = fontId;

    return EGL_STATUS_OK;
}

/* Begin NEW 12/3: Static font image data binary search */

/**************************************************************************
 * Begin Static Font Image Binary Search Utility.
 ***************************************************************************/
/* End NEW 12/3: Static font image data binary search */
/* For speed we use binary searches of the font character array.
 * The array is always sorted in order of character codes.
 */
static Altia_FontChar_type * binarySearch(int start, int count, ALTIA_CHAR code)
{

    ALTIA_INT32 low = (ALTIA_INT32)start;
    ALTIA_INT32 high = (ALTIA_INT32)(start + count - 1);
    ALTIA_INT32 index = (high + low) >> 1;
    ALTIA_UINT32 val = (ALTIA_UINT32)((ALTIA_CHAR_CAST)code);

    while (1)
    {
        ALTIA_UINT32 encoding = (ALTIA_UINT32)((ALTIA_CHAR_CAST)(test_fontChars[index].encoding));
        if (encoding == val)
        {
            /* Success -- found the character */
            return &test_fontChars[index];
        }
        else if (encoding > val)
        {
            high = index - 1;
        }
        else
        {
            low = index + 1;
        }

        if (low > high)
            break;
        else
            index = (high + low) >> 1;
    }

    /* Fail -- this character is not in the specified font */
    return NULL;
}
/* Begin NEW 12/3: Static font image data binary search */
/**************************************************************************
 * End Static Font Image Binary Search Utility.
 ***************************************************************************/

/* End NEW 12/3: Static font image data binary search */





/* This function saves a bit of code when using the DDB image storage
 * option.  It's called from egl_FontDdbGet() and egl_TextDraw().
 */
static ALTIA_FONTCHAR_TYPE * egl_FontCharGet(EGL_FONT_ID fontId, EGL_CHAR text)
{
/* Begin NEW 12/3: Static font image initialization */
    static int textErr = 0;

/* End NEW 12/3: Static font image initialization */
    Altia_FontHeader_type *header = (Altia_FontHeader_type*)fontId;
    Altia_FontChar_type *cptr = NULL;

    cptr = binarySearch(header->firstChar, header->charCnt, text);
    if (cptr == NULL)
    {
        if (textErr == 0)
        {
            ALTIA_CHAR buf[256];
            textErr = 1;
            ALT_SPRINTF(buf, ALT_TEXT("char %#x missing from font %s"),
                        text, test_fonts[header->fontIdx].name);
            _altiaErrorMessage(buf);
        }

#ifdef ALTIA_DEFAULT_FONT_CHARACTER
        /* use the default character */
        cptr = test_fontCharDefaults[header->fontIdx];
#endif
    }

    return cptr;
/* Begin NEW 12/3: Static font image initialization */

/* End NEW 12/3: Static font image initialization */



}



/*-------------------------------------------------------------------------
 * void LocalLoadAlpha8DIB (ALTIA_UBYTE * pixels, long pixelcnt,
 *                          ALTIA_SHORT width, ALTIA_SHORT height,
 *                          EGL_DIB * dibPtr)
 *
 * This function loads an EGL_ALPHA8 DIB to support anti-aliased fonts.
 *
 *-------------------------------------------------------------------------*/
static void LocalLoadAlpha8DIB(ALTIA_UBYTE * pixels, long pixelcnt,
                               ALTIA_SHORT width, ALTIA_SHORT height,
                               EGL_DIB * dibPtr)
{
    int widthCnt = 0;
    long total;
    ALTIA_INT count = 0;
    ALTIA_INT index = 0;
    ALTIA_INT indexCnt = 0;

    ALTIA_UBYTE * dataPtr;
    ALTIA_UBYTE * minPtr;

    /*
     * initialize our device independent bitmap (DIB) data structure.
     * it returns a pointer to the data section of the DIB for us.
     */
    altiaLibInitDIB(dibPtr, width, height, 8, NULL, &dataPtr);

    dibPtr->colorFormat = EGL_ALPHA8;
    dibPtr->imageFormat = EGL_DIB_DIRECT;

    minPtr = dataPtr;
    dataPtr += (height - 1) * dibPtr->stride;
    total = pixelcnt;

    while (index < total && dataPtr >= minPtr)
    {
        /*
         * the pixel data is compressed so decode the next element
         */


            *dataPtr = altiaLibDecodePixel(pixels, &count,
                                           &index, &indexCnt);


        /*
         * increment our byte pointer and count by the number of bytes
         * used by this latest index value.
         */
        dataPtr++;
        widthCnt++;

        /*
         * if we are finished with a row...
         */
        if(widthCnt == dibPtr->stride)
        {
            widthCnt = 0;

            /*
             * remember we are copying the rows from last to first for this
             * target.  so, to get to the first pointer of the previous row when
             * we just finished the current row, jump backwards 2 rows.
             */
            dataPtr -= 2 * dibPtr->stride;
        }
    }
}






/*-------------------------------------------------------------------------
 *  EGL_STATUS egl_TextDraw (EGL_GC_ID gc, EGL_POS left, EGL_POS baseline,
 *                           EGL_SIZE length, const EGL_CHAR *text)
 *
 * Draws a string of text characters starting at the designated left
 * pixel position.  The baseline of the text (i.e., the bottom of the
 * text NOT taking into account any descending character parts) will
 * be at the given baseline pixel position.
 *
 * If length is -1, the string pointed to by text is drawn up to its
 * null (0) terminator.  If length is not -1, only length characters
 * of the string pointed to by text are drawn.
 *
 * The text is drawn onto the bitmap for the given gc as previously
 * set by egl_DefaultBitmapSet().  If the bitmap was never set, then
 * the text is drawn directly to the display.
 *
 * The color of the text is determined by the current foreground color
 * for the gc.
 *
 * The color around the text is determined by the current background
 * color for the gc.  If the background color is EGL_COLOR_TRANSPARENT,
 * the color around the text is transparent (i.e., the text appears
 * to float over whatever color is currently behind it and this is
 * specifically how Altia wants to draw text).
 *
 * Any portion of the text that falls outside of the gc's current clipping
 * rectangle is not drawn.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 * NOTE:  THIS FUNCTION DRAWS SINGLE BYTE CHARACTERS.  TO SUPPORT WIDE
 *        UNICODE CHARACTERS, ANOTHER FUNCTION MUST BE IMPLEMENTED AND THE
 *        ALTIA GENERATED CODE FOR THIS TARGET MUST BE MODIFIED TO SUPPORT
 *        WIDE CHARACTERS.  IN THIS CASE, THE OPERATING SYSTEM MUST SUPPORT
 *        WIDE CHARACTER VERSIONS OF THE STANDARD C STRING FUNCTIONS TO COPY
 *        STRINGS, COMPARE STRINGS, GET THE LENGTH OF STRINGS, ETC.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_TextDraw(ALTIA_WINDOW win, EGL_GC_ID gc, EGL_POS x, EGL_POS y,
                        EGL_SIZE length, const EGL_CHAR *text)
{
    int i;
    GCPtr pGC = (GCPtr)gc;
    Altia_FontHeader_type *header = (Altia_FontHeader_type*)pGC->font;

    EGL_TEXT_DIB * dib = NULL;

    EGL_TEXT_DDB ddb;
    int maxwidth = header->maxWidth;

    /* Note:  If ALTIA_DRV_LOADMONO is defined, the task of
     * creating, populating, and deleting a mono DDB is left up to
     * special GCops functions DrvLoadMono() and DrvDeleteMono().
     */

    /* Validate parameters */
    if (text == NULL)
        return ~(EGL_STATUS_OK);

    /* Validate length */
    if (length < 0)
        length = ALT_STRLEN((EGL_CHAR *)text);

    if (length == 0)
        return ~(EGL_STATUS_OK);

    /* Loop through string and draw each character */
    for (i = 0; i < length; i++)
    {
        ALTIA_FONTCHAR_TYPE *cptr = NULL;

        /* Get font character information */
        cptr = egl_FontCharGet(pGC->font, text[i]);

        /* If the character was not found (encoded), skip it */
        if (NULL == cptr)
            continue;

#ifdef ALTIA_SAVE_FONT_BITMAP
        if (cptr->bitmap == 0)
        {
#endif
            if (maxwidth < cptr->bitWidth)
            {
                /* The font lied about what its max width is so we
                 * have to create a new DIB
                 */


                dib = NULL;

                maxwidth = cptr->bitWidth;
            }

            if (NULL == dib)
                dib = EGL_TEXT_CREATE_DIB(maxwidth, header->height, FALSE);

            if (NULL == dib)
                 return ~(EGL_STATUS_OK);

            /* Load the DIB with the character image data */
            EGL_TEXT_CREATE_DDB(altiaImageFontAddress(cptr->bits), cptr->bitCnt,
                 cptr->bitWidth,
                 header->height,
                 dib, ddb);



            if ((ALTIA_UINT32)ddb == 0)
            {



                return EGL_STATUS_ERROR;
            }

#ifdef ALTIA_SAVE_FONT_BITMAP
            cptr->bitmap = (ALTIA_UINT32)ddb;
        }
        else
            ddb = (EGL_TEXT_DDB)cptr->bitmap;
#endif /* ALTIA_SAVE_FONT_BITMAP */

        /* Blit the text character to the screen */
        EGL_TEXT_BLIT(gc, ddb, 0, 0,
                      (cptr->bitWidth -1),
                      (header->height -1),
                      ((AltiaDevInfo*)win)->drawable,
                      ALTIA_I2F(x + cptr->offsetx),
                      ALTIA_I2F(y - header->height + 1));

#ifndef ALTIA_SAVE_FONT_BITMAP

        /* Destroy the temporary font character bitmap */
        EGL_TEXT_DESTROY_DDB(ddb);


#endif /* ALTIA_SAVE_FONT_BITMAP */

        /* Advance to next character position */
        x += cptr->width;
    }



    return EGL_STATUS_OK;
}


/*-------------------------------------------------------------------------
 * EGL_STATUS egl_TextWidthGet(EGL_FONT_ID fontId, EGL_SIZE *width,
 *                             EGL_SIZE length, const EGL_CHAR *text)
 *
 * For the string of text pointed to by text, this function gives the pixel
 * width for the string if it were rendered using the font associated with
 * fontId.
 *
 * If length is -1, the dimensions are computed for the string up to its
 * null (0) terminator.  If length is not -1, only the dimensions for
 * length characters of the string are computed.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 * NOTE:  THIS FUNCTION IS FOR SINGLE BYTE CHARACTERS.  TO SUPPORT WIDE
 *        UNICODE CHARACTERS, ANOTHER FUNCTION MUST BE IMPLEMENTED AND THE
 *        ALTIA GENERATED CODE FOR THIS TARGET MUST BE MODIFIED TO SUPPORT
 *        WIDE CHARACTERS.  IN THIS CASE, THE OPERATING SYSTEM MUST SUPPORT
 *        WIDE CHARACTER VERSIONS OF THE STANDARD C STRING FUNCTIONS TO COPY
 *        STRINGS, COMPARE STRINGS, GET THE LENGTH OF STRINGS, ETC.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_TextWidthGet(EGL_FONT_ID fontId, EGL_SIZE *width,
                            EGL_SIZE length, const EGL_CHAR *text)
{
    Altia_FontHeader_type *header = (Altia_FontHeader_type*)fontId;
    EGL_FONT_METRICS fontMetrics;

    /* Default return values */
    *width = 0;

    /* Validate parameters */
    if (fontId == (EGL_FONT_ID)0 || text == NULL)
        return ~(EGL_STATUS_OK);

    /* Validate length */
    if (length < 0)
        length = ALT_STRLEN((EGL_CHAR *)text);

    if (length == 0)
        return ~(EGL_STATUS_OK);

    /* Font Metrics */
    egl_FontMetricsGet(fontId, &fontMetrics);

    /* Width and offset calculations */
    if (fontMetrics.spacing == EGL_FONT_MONO_SPACED)
    {
        /* Font is fixed width -- it is easy to figure the size */
        *width = header->maxWidth * length;
    }
    else
    {
        ALTIA_CONST ALTIA_FONTCHAR_TYPE * cptr;
        int i;

        for (i = 0; i < length; i++)
        {
            /* Get font character information */
            cptr = egl_FontCharGet(fontId, text[i]);

            /* If the character was not found (encoded), skip it */
            if (NULL == cptr)
                continue;

            *width += cptr->width;
        }
    }

    return EGL_STATUS_OK;
}


/*-------------------------------------------------------------------------
 * #if EGL_HAS_TEXTSIZEGETWITHOFFSET
 * EGL_STATUS egl_TextSizeGet (EGL_FONT_ID fontId,
 *                             EGL_SIZE *width, EGL_SIZE *height,
 *                             EGL_SIZE *offset,
 *                             EGL_SIZE length, const EGL_CHAR *text)
 * #else
 * EGL_STATUS egl_TextSizeGet (EGL_FONT_ID fontId,
 *                             EGL_SIZE *width, EGL_SIZE *height,
 *                             EGL_SIZE length, const EGL_CHAR *text)
 * #endif
 *
 * For the string of text pointed to by text, this function gives the pixel
 * width and height for the string if it were rendered using the font
 * associated with fontId.  In addition, it gives the pixel offset for
 * the string if EGL_HAS_TEXTSIZEGETWITHOFFSET is defined as non-zero (1)
 * in "egl_Wrapper.h".
 *
 * Offset is the pixel distance from the left edge of the string to the
 * true left edge of the most left character of the string.  For many
 * simple targets, offset is always 0 and EGL_HAS_TEXTSIZEGETWITHOFFSET
 * can be defined as 0.  However, some targets incorporate empty pixels
 * at the beginning of a string before drawing the left most pixels of
 * the first character.  This space can vary for each character of a
 * variable width font.  The offset should report this spacing.  The
 * Altia code can better position the left side of the text if it knows
 * the text offset.
 *
 * If length is -1, the dimensions are computed for the string up to its
 * null (0) terminator.  If length is not -1, only the dimensions for
 * length characters of the string are computed.
 *
 * This function returns EGL_STATUS_OK (0) if successful, non-zero otherwise.
 *
 * NOTE:  THIS FUNCTION IS FOR SINGLE BYTE CHARACTERS.  TO SUPPORT WIDE
 *        UNICODE CHARACTERS, ANOTHER FUNCTION MUST BE IMPLEMENTED AND THE
 *        ALTIA GENERATED CODE FOR THIS TARGET MUST BE MODIFIED TO SUPPORT
 *        WIDE CHARACTERS.  IN THIS CASE, THE OPERATING SYSTEM MUST SUPPORT
 *        WIDE CHARACTER VERSIONS OF THE STANDARD C STRING FUNCTIONS TO COPY
 *        STRINGS, COMPARE STRINGS, GET THE LENGTH OF STRINGS, ETC.
 *
 *-------------------------------------------------------------------------*/
EGL_STATUS egl_TextSizeGet(EGL_FONT_ID fontId,
                           EGL_SIZE *width, EGL_SIZE *height,
#if EGL_HAS_TEXTSIZEGETWITHOFFSET
                           EGL_SIZE *offset,
#endif
                           EGL_SIZE length, const EGL_CHAR *text)
{
    Altia_FontHeader_type *header = (Altia_FontHeader_type*)fontId;
    EGL_FONT_METRICS fontMetrics;

    /* Default return values */
    *width = 0;
    *height = 0;
#if EGL_HAS_TEXTSIZEGETWITHOFFSET
    *offset = 0;
#endif

    /* Validate parameters */
    if (fontId == (EGL_FONT_ID)0 || text == NULL)
        return ~(EGL_STATUS_OK);

    /* Validate length */
    if (length < 0)
        length = ALT_STRLEN((EGL_CHAR *)text);

    if (length == 0)
        return ~(EGL_STATUS_OK);

    /* Font Metrics */
    egl_FontMetricsGet(fontId, &fontMetrics);

    /* Width and offset calculations */
    if (fontMetrics.spacing == EGL_FONT_MONO_SPACED)
    {
        /* Font is fixed width -- it is easy to figure the size */
        *width = header->maxWidth * length;
    }
    else
    {
        int max_width = 0;
        ALTIA_CONST ALTIA_FONTCHAR_TYPE * cptr;
        char status = 0;
        int i;

        /* Process characters in string */
        for (i = 0; i < length; i++)
        {
            /* Get font character information */
            cptr = egl_FontCharGet(fontId, text[i]);

            /* If the character was not found (encoded), skip it */
            if (NULL == cptr)
                continue;

            /* Special for just one character */
            if (1 == length)
            {
                /* Start with Pixel-width */
                *width = cptr->bitWidth;

                /* Single character should use the character's pixel width, but this
                ** causes differing results for right and center justified text.
                ** Therefore take the larger of either the advance or the pixel width.
                */
                if (*width < cptr->width)
                    *width = cptr->width;
#if EGL_HAS_TEXTSIZEGETWITHOFFSET
                /* Use character offset */
                *offset = cptr->offsetx;
#endif
            }
            else
            {
                int last_width;

                /* Capture offset if this is the first character */
                if (!status)
                {
#if EGL_HAS_TEXTSIZEGETWITHOFFSET
                    /* Save the offset */
                    *offset = cptr->offsetx;
#endif
                    /* Since this is the first character we need to remove the offset
                    ** from the total width.  A negative offset is actually adding
                    ** to our total width.  A positive offset is ignored.
                    */
                    if (cptr->offsetx < 0)
                        *width -= cptr->offsetx;
                }

                /* Calculate impact of character on length if the character were the last.
                ** This is used for the last character but also for checking if a previous
                ** character was exceptionally wide and overhaning the last character.
                */
                last_width = cptr->bitWidth + cptr->offsetx;

                /* Update max width -- this will catch if we have an exceptionally wide
                ** character with a small advance in the middle of the string.
                */
                if (max_width < *width)
                {
                    if (last_width < cptr->width)
                        max_width = *width + cptr->width;
                    else
                        max_width = *width + last_width;
                }

                /* Adjust width */
                if (i < (length-1))
                {
                    /* Not last character -- use advance */
                    *width += cptr->width;
                }
                else
                {
                    /* Last character -- we should use the width adjusted by the bearing.
                    ** However some characters may be very narrow (like the space character)
                    ** which makes those characters hard to select with a cursor.
                    */
                    if (last_width < cptr->width)
                        *width += cptr->width;
                    else
                        *width += last_width;
                }

                /* Valid character */
                status = 1;
            }
        }

        /* Use the larger of either the calculated width, or the max width */
        if (*width < max_width)
            *width = max_width;
    }

    /* Height calculation */
    *height = header->height;

    return EGL_STATUS_OK;
}




